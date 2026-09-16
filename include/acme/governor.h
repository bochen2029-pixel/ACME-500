// ============================================================================
//  acme/governor.h — THE GOVERNOR: how much                          [C1, E3]
//
//  Its own translation unit, never included by the machine's. It holds the
//  salt; before the machine's period it draws, for every open cell, retained /
//  audit / canary and writes STRATUM rows; after settlement it consumes OUTCOME
//  rows, runs the ladder, and writes LICENSE, KAPPA and REGIME rows. The
//  machine reads the strata and the rungs from rows and never touches the
//  salt: a lineage that can pick its own arena picks an easy one, and v1's
//  resident held the salt in the writ it carried (F14's sibling, fixed here).
//
//  E3 · THE RETAINED STRATUM (F14, O20). At every rung, a uniform fraction of
//  every class's open cells, drawn on the salt at the writ's eps_floor, is
//  routed to the incumbent and never acted: the gate holds it with reason
//  `retained`, a person decides it, and its outcome is the paired baseline in
//  its band. The control arm never reaches zero, the bar of the ladder v2 has
//  something to be frozen from at every rung, and the people stay practised.
//  v1's uniform floor could not bind because the gate consulted the canary
//  flag at rung 1 only; this is its structural half.
//
//  E3 · THE LADDER v2 lives in license.h; the governor opens the terms, seeds
//  the paired arm's counts from the shadow record (the warm history), and
//  writes every rung change as its own LICENSE row with its cause on the via
//  byte, so a fold can grade each (O45).
//
//  E3 · THE REGIME DETECTOR. On every term boundary the arrivals of each class
//  over the closing term are compared with the term before; a change past the
//  writ's tolerance that is also past three standard errors is a REGIME row,
//  and the class's bands above rung 1 drop to watching at once, because
//  outcomes arrive too late to be the first line of defence.
//
//  In the sim it runs in-process from this header; in INTELLECT it is a
//  separate process with the same file contract as fusord's switch (v4 §27).
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
  std::vector<long>  arr_prev;     // E3: each class's arrivals over the previous term, for the regime detector
  bool     lie_band_dependent = false;   // O21's lie: a draw that depends on the band. Never set outside the battery.
  bool     lie_regime_deaf = false;      // O59's lie: a detector that never fires. Never set outside the battery.

  void init(int nc, const Writ& writ, const std::vector<float>& arrivals_per_day,
            uint32_t schema_pin, uint32_t judge_hash, const std::vector<uint32_t>& template_hash) {
    NC = nc; wr = writ;
    lad.init(nc, schema_pin, judge_hash);
    lad.template_hash = template_hash;
    lad.delta = writ.canary_delta; lad.m_min = writ.m_min; lad.m_term = writ.m_term; lad.bar_z = writ.bar_z; lad.term_days = writ.term_days;
    per_term.assign(nc, 1.f); arr_prev.assign(nc, -1);
    // n0 per class-band, and mark the ones the world will never answer fast
    // enough to license. The honest account prints them as human rather than
    // holding them in shadow forever. n0 is a label and a planning figure; the
    // ladder v2 never reads it (F21, F23).
    for (int c = 0; c < nc; ++c) {
      per_term[c] = std::max(1.f, (c < (int)arrivals_per_day.size() ? arrivals_per_day[c] : 0.f) * (float)lad.term_days);
      for (int b = 0; b < NBAND; ++b) {
        Lic& L = lad.at(c, b);
        L.n0 = n0_for(c, wr.canary_delta);
        if (per_term[c] * 0.25 < L.n0 / 4.0) L.unlicensable = true;   // even a 25% canary cannot clear it
      }
    }
  }
  // E3: the shadow record. The incumbent's outcomes before the machine existed
  // are exterior verdicts on executed human choices in their bands: the paired
  // arm's counts the first term's bars are frozen from.
  void seed_history(const Tape& tape) {
    tape.fold([&](const Rec& r) {
      if (r.type != R_OUTCOME || r.cls >= NC || r.a == OK_UNRESOLVED || r.via != 0) return;
      lad.observe(r.cls, r.band, 0, r.a == OK_GOOD, 0);
    });
  }
  // E3: a term opens: the bars freeze, the processes start at one, and the
  // NOTE (code 3, b = the day) says so, so a fold opens its terms where the
  // governor did (O45).
  void open_term(uint32_t day, Tape& tape) {
    lad.open_term(day);
    tape.put(R_NOTE, day, 0, 0, -3, ARM_GOVERNOR, 3, (int)day);
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
  // stratum persists across periods while its rate stands. E3: the retained
  // stratum is drawn first at the writ's eps_floor and at every rung; audit
  // takes precedence over canary, as in the gate's published order. The row
  // carries the kind, the canary rate (b), the audit rate (value) and the
  // retained rate (margin) the draw was made against, so a fold can grade the
  // draw (O21). D0: the row is written when the draw in force changes, and
  // stands until then; the day is the ledger's, folded from the TICK row.
  void draw_strata(Ledger& L, Tape& tape) {
    const uint32_t day = L.day;
    for (uint32_t i : L.open_idx) {
      const Obligation& o = L.ob[i];
      if (o.state == OB_DECIDED || o.state == OB_SETTLED) continue;
      const int c = o.cls;
      int b = -1;
      if (const Ledger::LastProp* p = L.memo(o.id)) b = p->band;
      const float rc = canary_rate(c, b), ra = audit_rate(c), rr = wr.eps_floor;
      int kind = ST_NONE;
      if (u01(salt, 7050 + c, o.id) < rr)           kind = ST_RETAINED;     // E3: the control arm, at every rung
      else if (u01(salt, 7100 + c, o.id) < ra)      kind = ST_AUDIT;
      else if (u01(salt, 7000 + c, o.id) < rc)      kind = ST_CANARY;
      if (lie_band_dependent && kind == ST_CANARY && b != 2) kind = ST_NONE;   // THE LIE (O21): the wide band alone is drawn
      const int rc_i = (int)(rc * 1e6f);
      if (L.stratum_drawn(o.id) && L.stratum_of(o.id) == kind
          && L.stratum_rate_of(o.id) == (float)rc_i * 1e-6f && L.stratum_audit_of(o.id) == ra) continue;
      put_fold(tape, L, R_STRATUM, day, o.id, c, -3, ARM_GOVERNOR, kind, rc_i, rr, ra, b);
    }
  }

  // Outcomes arrive. The governor consumes the OUTCOME rows of the period just
  // settled: this is the only place a licence can widen, and it is a fold. E3
  // (F24): the outcome is graded against the bar of the term its act was made
  // in; the act's day is the cell's, from the ledger.
  void grade(const Tape& tape, size_t from_row, const Ledger& L) {
    for (size_t i = from_row; i < tape.rec.size(); ++i) {
      const Rec& r = tape.rec[i];
      if (r.type != R_OUTCOME || r.cls >= NC) continue;
      if (r.a == OK_UNRESOLVED) continue;                        // E0: the world never answered; a write-off licenses nothing and demotes nothing
      const Obligation* o = L.at_oid(r.oid);
      lad.observe(r.cls, r.band, (int)r.via, r.a == OK_GOOD, o ? o->day_decided : 0);
    }
  }

  // every rung change is its own LICENSE row, with its cause on the via byte
  void license_row(Tape& tape, uint32_t day, int c, int b, int cause) {
    Lic& L = lad.at(c, b); L.cause = cause;
    tape.put(R_LICENSE, day, 0, c, -3, ARM_GOVERNOR, L.rung, b, (float)L.logE, (float)L.kappa(), b,
             cause == LC_HISTORY ? RF_HISTORY : 0, cause);
  }

  // E3: THE REGIME DETECTOR, at a term boundary. The closing term's arrivals
  // per class against the term before it: a change past the writ's tolerance
  // AND past three standard errors of a Poisson count is a regime change the
  // tape can identify; the class's bands above rung 1 drop to watching at once.
  // A class too thin to show the change stays as it is: the detector says what
  // the rows can say and nothing more.
  int regime_check(const Ledger& L, uint32_t day, Tape& tape) {
    std::vector<long> cur(NC, 0);
    const uint32_t from = day >= (uint32_t)lad.term_days ? day - (uint32_t)lad.term_days : 0;
    for (const Obligation& o : L.ob) if (o.cls < NC && o.day_open >= from && o.day_open < day) ++cur[o.cls];
    int fired = 0;
    for (int c = 0; c < NC; ++c) {
      const long prev = arr_prev[c];
      arr_prev[c] = cur[c];
      if (prev < 0 || lie_regime_deaf) continue;                 // THE LIE (O59): a detector that never hears
      const double dev = std::fabs((double)cur[c] - (double)prev);
      if (prev > 0 && dev > wr.regime_tol * (double)prev && dev > 3.0 * std::sqrt((double)prev)) {
        ++fired;
        tape.put(R_REGIME, day, 0, c, -3, ARM_GOVERNOR, 1, (int)cur[c], (float)prev, (float)((double)cur[c] / (double)prev));
        for (int b = 0; b < NBAND; ++b) { Lic& x = lad.at(c, b); if (x.rung > 1) { x.rung = 1; license_row(tape, day, c, b, LC_REGIME); } }
      }
    }
    return fired;
  }

  // Climb. Widening needs the world; narrowing needs nothing but evidence.
  // The supervision meter is the machine's own account of minutes it caused
  // and removed, per class-band; kappa is judged on it. E3: the order within a
  // step is the ladder v2's: on a term boundary, the regime check and the
  // lapses first; then the mirrored process and kappa; then the term's process
  // widens as far as its evidence reaches; then, on the boundary, the new term
  // opens with its bars frozen.
  void step(const Ledger& L, const SupervisionMeter& sup, Tape& tape) {
    const uint32_t day = L.day;
    const bool boundary = Ladder::term_boundary(day, lad.term_days);
    if (boundary) regime_check(L, day, tape);
    for (int c = 0; c < NC; ++c) for (int b = 0; b < NBAND; ++b) {
      Lic& x = lad.at(c, b);
      x.sup_created = sup.created(c, b); x.sup_removed = sup.removed(c, b);
      // --- the term closes: an EARNED rung the term's process did not re-earn lapses one step, never below the admitted floor
      if (boundary && x.bar > 0.0 && x.n_term >= lad.m_term && x.term_rung < x.rung && x.rung > x.floor_rung) {
        x.rung -= 1; license_row(tape, day, c, b, LC_LAPSED);
      }
      // --- narrow, always, and with no signature
      if (x.logE_demote > std::log(1.0 / lad.alpha_demote) && x.rung > 0) { x.rung -= 1; x.logE_demote = 0.0; license_row(tape, day, c, b, LC_DEMOTED); }
      if (x.kappa() > wr.kappa_max && x.rung > 1 && x.sup_removed > 60.0) { x.rung = 1; license_row(tape, day, c, b, LC_KAPPA); }
      // --- widen only inside the rule: the term's process past the next rung's threshold, against a measured bar
      while (x.rung < 5 && x.bar > 0.0 && x.logE >= lad.threshold(x.rung + 1)) { x.rung += 1; license_row(tape, day, c, b, LC_WAGER); }
      // v2: kappa is a row once a term, per class-band that has removed anything
      if (day > 0 && (int)(day % (uint32_t)lad.term_days) == 0 && x.sup_removed > 0.0)
        tape.put(R_KAPPA, day, 0, c, -3, ARM_GOVERNOR, (int)x.sup_created, (int)x.sup_removed, 0.f, (float)x.kappa(), b);
    }
    if (boundary) open_term(day, tape);
  }

  // E3 (O31's harness): every band past the thin band admitted to rung 1 as a
  // TEST admission, so the wager runs on every class at once and the
  // false-promotion rate can be counted over all of them. Never outside --o31.
  // Z0 · THE BOOTSTRAP ADMISSION, the founding act of a firm with no history.
  // On the empty planet there is nobody to shadow: the agreement band cannot be
  // licensed from a settled history because there is no settled history, and a
  // class that is never admitted never acts, never produces an outcome, and never
  // licenses — the deadlock a zero-seat firm starts in. The constitution breaks it
  // and nothing learned does: a class whose effect carries a usable inverse, whose
  // signature no law reserves and whose counterparty does not demand a person, is
  // admitted at founding to the CANARY RUNG ONLY, where the machine acts on the
  // drawn sample and drafts nothing (there is nobody to key a draft). Everything
  // above rung 1 is still earned from the world's outcomes, on the licensor's
  // salt, exactly as before. This is the authored line, written once, pinned in
  // the writ; it is the founding instance's single point of failure and it is
  // named as such. A class the schema flags `warrant` or `counterparty` is NOT
  // admitted: a zero-seat firm does not sell a promise that needs a person.
  int bootstrap_admit(uint32_t day, Tape& tape, long& refused_warrant, long& refused_counterparty, int rung = 1) {
    int n = 0; refused_warrant = refused_counterparty = 0;
    rung = rung < 1 ? 1 : (rung > 4 ? 4 : rung);
    for (int c = 0; c < lad.NC; ++c) {
      const ClassSpec& s = cls_spec(c);
      if (s.warrant)      { ++refused_warrant;      continue; }
      if (s.counterparty) { ++refused_counterparty; continue; }
      if (!s.reversible)  { ++refused_warrant;      continue; }   // an irreversible act with no signer holds, by law 2
      for (int b = 1; b < NBAND; ++b) {
        Lic& L = lad.at(c, b);
        if (L.rung >= 1) continue;
        // THE FOUNDING GRANT. How much authority the constitution hands a class
        // before any evidence exists. At rung 1 the machine acts on the drawn
        // sample only and holds the rest, which is safe and discharges almost
        // nothing; above it the machine acts and the EXPOSURE CAP is the brake
        // while the mirrored process demotes on the world's first evidence. The
        // asymmetry is unchanged either way: evidence narrows at any moment,
        // and only a ratified rule widens. This is the one dial that says what
        // a firm with no history is allowed to assume about itself.
        L.rung = rung; L.floor_rung = 1; ++n; license_row(tape, day, c, b, LC_BOOTSTRAP);
      }
    }
    return n;
  }

  int admit_all_for_test(uint32_t day, Tape& tape) {
    int n = 0;
    for (int c = 0; c < lad.NC; ++c) for (int b = 1; b < NBAND; ++b) {
      Lic& L = lad.at(c, b);
      if (L.rung >= 1) continue;
      L.rung = 1; L.floor_rung = 1; ++n; license_row(tape, day, c, b, LC_TEST);
    }
    return n;
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
        if (L.rung < 1) { L.rung = 1; L.floor_rung = 1; ++promoted; license_row(tape, day, c, b, LC_HISTORY); }
      }
    }
    return promoted;
  }
};

} // namespace acme
