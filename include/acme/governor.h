// ============================================================================
//  acme/governor.h — THE GOVERNOR: how much                              [C1]
//
//  Its own translation unit, never included by the machine's. It holds the
//  salt; before the machine's period it draws, for every open cell, canary /
//  audit / retained-control and writes STRATUM rows; after settlement it
//  consumes OUTCOME rows, runs the ladder, and writes LICENSE and KAPPA rows.
//  The machine reads the strata and the rungs from rows and never touches the
//  salt: a lineage that can pick its own arena picks an easy one, and v1's
//  resident held the salt in the writ it carried (F14's sibling, fixed here).
//
//  In the sim it runs in-process from this header; in INTELLECT it is a
//  separate process with the same file contract as fusord's switch.
// ============================================================================
#pragma once
#ifdef ACME_NO_GOVERNOR
#error "governor.h is the GOVERNOR: the salt, the strata and the ladder. The machine's translation unit must not include it (oracle O17)."
#endif
#include "core.h"
#include "firm.h"
#include "ledger.h"
#include "license.h"
#include "machine.h"   // ReplayOut, for history admission; SupervisionMeter, for kappa

namespace acme {

enum Stratum : uint8_t { ST_NONE = 0, ST_CANARY = 1, ST_AUDIT = 2, ST_RETAINED = 3 };

struct Governor {
  uint64_t salt = 0xAC3E5A17ULL;   // THE ARENA'S SALT. The operator's, once; read here and nowhere else.
  Ladder   lad;
  Writ     wr;
  int      NC = 0;
  std::vector<float> per_term;     // arrivals per class per term, MEASURED from ARRIVE rows by the compile step
  bool     lie_band_dependent = false;   // O21's lie: a draw that depends on the band. Never set outside the battery.

  void init(int nc, const Writ& writ, const std::vector<float>& arrivals_per_day,
            uint32_t schema_pin, uint32_t judge_hash, const std::vector<uint32_t>& template_hash) {
    NC = nc; wr = writ;
    lad.init(nc, schema_pin, judge_hash);
    lad.template_hash = template_hash;
    per_term.assign(nc, 1.f);
    // n0 per class-band, and mark the ones the world will never answer fast
    // enough to license. The honest account prints them as human rather than
    // holding them in shadow forever.
    for (int c = 0; c < nc; ++c) {
      per_term[c] = std::max(1.f, (c < (int)arrivals_per_day.size() ? arrivals_per_day[c] : 0.f) * (float)lad.term_days);
      for (int b = 0; b < NBAND; ++b) {
        Lic& L = lad.at(c, b);
        L.n0 = n0_for(c, wr.canary_delta);
        if (per_term[c] * 0.25 < L.n0 / 4.0) L.unlicensable = true;   // even a 25% canary cannot clear it
      }
    }
  }

  // The canary rate. The writ's eps_floor is the uniform component: a floor the
  // kernel may raise and never lower; canary_cap is the most of a class the
  // wager may take in a term. At rungs 1 to 3 the rate is the class's, n0 over
  // the term's arrivals; a band at rung 4 or above explores at eps* = sqrt(3/F)
  // on its own outcome count. A cell whose band is not yet known (never read)
  // is drawn at the class rate.
  float canary_rate(int c, int b) const {
    if (b >= 0) { const Lic& L = lad.at(c, b);
      if (L.rung >= 4) return std::max(wr.eps_floor, 1.0f / std::sqrt(3.0f * std::max(1.f, (float)L.n_machine))); }
    return (float)std::min((double)wr.canary_cap, std::max((double)wr.eps_floor, lad.at(c, 0).n0 / (double)per_term[c]));
  }
  // The audit rate: the sampled review fraction that never reaches zero, at
  // the class's own wager count. (v1 drew it at the band's count, which no
  // governor can do before the judge has named the band.)
  float audit_rate(int c) const {
    long n = 0; for (int b = 0; b < NBAND; ++b) n += lad.at(c, b).n_machine;
    return std::max(0.01f, 1.0f / std::sqrt(3.0f * std::max(1.f, (float)n)));
  }

  // THE STRATA. Drawn per open cell, per period, from the salt, before the
  // machine reads anything. The band of a cell is the band of its last
  // proposal, if it has one; the draw itself is keyed on the cell, so a cell's
  // stratum persists across periods while its rate stands. Audit takes
  // precedence over canary, as it does in the gate's published order. The row
  // carries the kind, the canary rate and the audit rate the draw was made
  // against, so a fold can grade the draw (O21).
  void draw_strata(Ledger& L, Tape& tape, uint32_t day) {
    for (uint32_t i : L.open_idx) {
      const Obligation& o = L.ob[i];
      if (o.state == OB_DECIDED || o.state == OB_SETTLED) continue;
      const int c = o.cls;
      int b = -1;
      if (const Ledger::LastProp* p = L.memo(o.id)) b = p->band;
      const float rc = canary_rate(c, b), ra = audit_rate(c);
      int kind = ST_NONE;
      if (u01(salt, 7100 + c, o.id) < ra)      kind = ST_AUDIT;
      else if (u01(salt, 7000 + c, o.id) < rc) kind = ST_CANARY;
      if (lie_band_dependent && kind == ST_CANARY && b != 2) kind = ST_NONE;   // THE LIE (O21): the wide band alone is drawn
      put_fold(tape, L, R_STRATUM, day, o.id, c, -3, ARM_GOVERNOR, kind, (int)(rc * 1e6f), 0.f, ra, b);
    }
  }

  // Outcomes arrive. The governor consumes the OUTCOME rows of the period just
  // settled: this is the only place a licence can widen, and it is a fold.
  void grade(const Tape& tape, size_t from_row) {
    for (size_t i = from_row; i < tape.rec.size(); ++i) {
      const Rec& r = tape.rec[i];
      if (r.type != R_OUTCOME || r.cls >= NC) continue;
      lad.observe(r.cls, r.band, (int)r.via, r.a == OK_GOOD, lad.p_incumbent(r.cls, r.band));
    }
  }

  // Climb. Widening needs the world; narrowing needs nothing but evidence.
  // The supervision meter is the machine's own account of minutes it caused
  // and removed, per class-band; kappa is judged on it.
  void step(uint32_t day, const SupervisionMeter& sup, Tape& tape) {
    for (int c = 0; c < NC; ++c) for (int b = 0; b < NBAND; ++b) {
      Lic& L = lad.at(c, b);
      L.sup_created = sup.created(c, b); L.sup_removed = sup.removed(c, b);
      const int before = L.rung;
      // --- narrow first, always, and with no signature
      if (L.logE_demote > std::log(1.0 / lad.alpha_demote) && L.rung > 0) { L.rung -= 1; L.logE_demote = 0; L.logE = 0; }
      if (L.kappa() > wr.kappa_max && L.rung > 1 && L.sup_removed > 60.0) { L.rung = 1; L.logE = 0; }
      if (L.expiry_day > 0 && (int)day > L.expiry_day && L.rung > 1) { L.rung -= 1; L.logE = 0; }
      // --- widen only inside the rule, and only on arrivals past the floor
      const bool enough = L.n_machine >= L.n0 || (L.history_licensed && b == 2 && L.n_machine >= std::max(20, L.n0 / 8));
      if (enough && L.logE > std::log(1.0 / lad.alpha_promote) && L.rung < 5) {
        L.rung += 1; L.logE = 0; L.expiry_day = (int)day + 3 * lad.term_days;
      }
      if (L.rung != before) tape.put(R_LICENSE, day, 0, c, -3, ARM_GOVERNOR, L.rung, b, (float)L.logE, (float)L.kappa(), b);
      // v2: kappa is a row once a term, per class-band that has removed anything
      if (day > 0 && (int)(day % (uint32_t)lad.term_days) == 0 && L.sup_removed > 0.0)
        tape.put(R_KAPPA, day, 0, c, -3, ARM_GOVERNOR, (int)L.sup_created, (int)L.sup_removed, 0.f, (float)L.kappa(), b);
    }
  }

  // Licence the agreement bands from history: the band is admitted to unattended
  // action where the resident's coinciding choice beat the firm's own baseline on
  // arrived outcomes, with both sides above the sample floor, AND the judge's
  // agreement with the firm on the class is beyond chance (C1: Cohen's kappa
  // >= 0.2). Without the second clause a constant judge or a coin is admitted
  // on the half of history it happened to agree with, at the firm's own good
  // rate, which is F22 and what oracle O16 catches.
#ifndef ACME_KAPPA_ADMIT
#define ACME_KAPPA_ADMIT 0.2     // overridable only to MEASURE F22 (the receipt's experiment), never in a build that ships
#endif
  static constexpr double KAPPA_ADMIT = ACME_KAPPA_ADMIT;
  int license_from_history(const ReplayOut& R, uint32_t day, Tape& tape) {
    int promoted = 0;
    for (int c = 0; c < lad.NC; ++c) for (int b = 0; b < NBAND; ++b) {
      Lic& L = lad.at(c, b);
      if (L.agree_total < 40) continue;
      if (R.kappa_agree(c) < KAPPA_ADMIT) continue;
      const double p_agree = (double)L.agree_good / (double)L.agree_total;
      const double firm_base = (R.n[c] > 0) ? (double)(R.agree_good[c] + R.disagree_firm_good[c]) / (double)R.n[c] : 0.0;
      if (p_agree >= firm_base - 0.01 && b >= 1) {
        L.history_licensed = true;
        if (L.rung < 1) { L.rung = 1; ++promoted;
          tape.put(R_LICENSE, day, 0, c, -3, ARM_GOVERNOR, L.rung, b, (float)p_agree, (float)firm_base, b, RF_HISTORY); }
      }
    }
    return promoted;
  }
};

} // namespace acme
