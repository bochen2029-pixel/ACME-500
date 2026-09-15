// ============================================================================
//  acme/license.h — THE LICENCE TABLE: per class-band, keyed per judge   [C1]
//
//  The table the gate reads every period and the governor writes. It is data:
//  a fold of LICENSE rows for the rungs, of OUTCOME rows for the evidence. The
//  machine's translation unit includes this (it must read rungs); it never
//  includes the governor, which is the only thing that moves a rung.
//
//  THE KEY. A licence is a statement about a population read by a particular
//  judge over a particular template: (schema pin, template pin, class, band,
//  judge hash). Swap the judge and the rungs are re-earned on the same tape;
//  the table refuses to be read by a judge it was not earned under.
//
//  NBAND and band_of() live in firm.h beside the writ, so both arms band alike.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"

namespace acme {

struct Lic {
  int   rung = 0;
  int   expiry_day = 0;
  // the e-process: an anytime-valid sequential test. The operator may peek
  // forever without invalidating it, which is how supervision actually happens.
  double logE = 0.0;
  double logE_demote = 0.0;
  long   n_machine = 0, n_incumbent = 0;        // n_machine: the WAGER — unattended acts only (via 1 and 3)
  long   good_machine = 0, good_incumbent = 0;
  long   n_assisted = 0, good_assisted = 0;     // drafts a person keyed and warrants a signer executed (via 2 and 4):
                                                // graded, printed, never fed to the e-process
  // kappa
  double sup_created = 0, sup_removed = 0;
  long   n_agree = 0, n_disagree = 0, n_replayed = 0;
  long   agree_good = 0, agree_total = 0;
  bool   history_licensed = false;
  bool   unlicensable = false;      // n0 unreachable inside a term at any tolerable rate
  int    n0 = 0;
  double kappa() const { return sup_removed > 1e-9 ? sup_created / sup_removed : 9.99; }
};

struct Ladder {
  int NC = 0;
  std::vector<Lic> lic;             // [NC * NBAND]
  double alpha_promote = 1.0 / 20.0;
  double alpha_demote  = 1.0 / 5.5;   // demotion an order easier than promotion; trust rebuilds asymmetrically
  int    term_days = 30;
  // the key
  uint32_t schema_pin = 0;          // alphabet_hash() at init
  uint32_t judge_hash = 0;          // the judge these rungs were earned under
  std::vector<uint32_t> template_hash;   // [NC] the template each class was read with

  void init(int nc, uint32_t schema = 0, uint32_t judge = 0) {
    NC = nc; lic.assign((size_t)nc * NBAND, Lic{});
    schema_pin = schema; judge_hash = judge; template_hash.assign(nc, 0u);
    for (int c = 0; c < nc; ++c) for (int b = 0; b < NBAND; ++b) lic[(size_t)c * NBAND + b].n0 = n0_for(c);
  }
  Lic& at(int c, int b) { return lic[(size_t)c * NBAND + (b < 0 ? 0 : (b >= NBAND ? NBAND - 1 : b))]; }
  const Lic& at(int c, int b) const { return lic[(size_t)c * NBAND + (b < 0 ? 0 : (b >= NBAND ? NBAND - 1 : b))]; }
  // The rung a judge may act under. A judge the table was not earned under
  // reads rung 0 everywhere: the licence is keyed, not named.
  int rung_for(int c, int b, uint32_t judge) const { return judge == judge_hash ? at(c, b).rung : 0; }

  // An arrived outcome. THE ONLY THING THAT WIDENS A LICENCE is an outcome of
  // an unattended act (via 1 or 3). A forecast, a replay, a green battery: those
  // admit. A draft or a warrant is graded as assisted and feeds nothing.
  void observe(int c, int b, int via, bool good, double p_incumbent) {
    Lic& L = at(c, b);
    if (via == 1 || via == 3) {
      ++L.n_machine; L.good_machine += good ? 1 : 0;
      const double p0 = std::min(0.98, std::max(0.02, p_incumbent));
      const double p1 = std::min(0.98, p0 + 0.05);           // the wager: at least non-inferior
      L.logE      += good ? std::log(p1 / p0) : std::log((1 - p1) / (1 - p0));
      // the mirrored process, for demotion
      const double q1 = std::max(0.02, p0 - 0.08);
      L.logE_demote += good ? std::log(q1 / p0) : std::log((1 - q1) / (1 - p0));
    } else if (via == 2 || via == 4) {
      ++L.n_assisted; L.good_assisted += good ? 1 : 0;
    } else {
      ++L.n_incumbent; L.good_incumbent += good ? 1 : 0;
    }
  }
  double p_incumbent(int c, int b) const {
    const Lic& L = at(c, b);
    return (L.n_incumbent >= 12) ? (double)L.good_incumbent / (double)L.n_incumbent : 0.72;
  }
};

// O15 · THE LADDER IS A FOLD. Rebuild every licence's evidence counts and both
// e-processes from OUTCOME rows alone, in tape order, from the day the resident
// first graded, and compare to the live ladder. The rows carry the band and the
// via, so nothing else is joined. [v2, step B3]
inline Ladder fold_ladder(const Tape& tape, int NC, uint32_t from_day) {
  Ladder L; L.init(NC);
  tape.fold([&](const Rec& r) {
    if (r.type != R_OUTCOME || r.day < from_day || r.cls >= NC) return;
    L.observe(r.cls, r.band, (int)r.via, r.a == OK_GOOD, L.p_incumbent(r.cls, r.band));
  });
  return L;
}
inline long ladder_diff(const Ladder& a, const Ladder& b) {
  long bad = 0;
  for (size_t i = 0; i < a.lic.size() && i < b.lic.size(); ++i) {
    const Lic& x = a.lic[i]; const Lic& y = b.lic[i];
    if (x.n_machine != y.n_machine || x.good_machine != y.good_machine) ++bad;
    if (x.n_incumbent != y.n_incumbent || x.good_incumbent != y.good_incumbent) ++bad;
    if (x.n_assisted != y.n_assisted || x.good_assisted != y.good_assisted) ++bad;
  }
  return bad + (long)(a.lic.size() != b.lic.size());
}

} // namespace acme
