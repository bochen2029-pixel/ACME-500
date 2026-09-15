// ============================================================================
//  acme/license.h — THE LICENCE TABLE: per class-band, keyed per judge   [C1, E3]
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
//  E3 · THE LADDER v2 (F21, F23; ARCHITECTURE v4 §28). The rule as built had
//  three defects the reviews named and O31 measured: the null was a plug-in
//  baseline that moved with every incumbent outcome; the process reset on
//  every promotion, so the error compounded across rungs; and n0 stood as a
//  condition inside a test it was never part of, paid once and never per rung.
//  Under a judge at its own null the rule promoted 17.7 percent of bands
//  against a stated 5 (the E3a receipt). The rule here:
//
//    THE BAR IS FROZEN PER TERM. For a class-band, the paired arm's good rate
//    (the incumbent's, in the same band) is measured from at least m_min
//    outcomes and frozen for the term at its upper confidence bound, so that
//    estimation error only raises the bar. A band whose bar is unmeasured runs
//    no process: it cannot widen, and it does not lapse.
//
//    ONE PROCESS PER CLASS-BAND-TERM, NEVER RESET WITHIN IT. The null is
//    g <= g0 = 1 - (1 - bar)(1 + delta): the machine is worse than the bar by
//    more than the writ's tolerance on the failure side. Over the wager's
//    outcomes X_i in arrival order, E = prod (bar/g0)^X ((1-bar)/(1-g0))^(1-X)
//    is a nonnegative supermartingale under every rate in the null, so by
//    Ville's inequality the chance it ever crosses 1/alpha_k is at most alpha_k
//    at every look. Rung k is reached at E >= 1/alpha_k with alpha_k = alpha
//    2^-k, so the chance of ever reaching ANY rung falsely in a term is at most
//    alpha in total. n0 is the planning figure, printed beside every rung
//    claim; it is no longer a condition.
//
//    DEMOTION IS MIRRORED AND EASIER. A second process over every machine
//    outcome in the band, null g >= bar, alternative g0, threshold 1/alpha_d
//    at alpha_d = 1/5.5; when it fires the band drops one rung the same day
//    and the process restarts. Trust rebuilds asymmetrically by construction.
//
//    TERM AND LAPSE. At the end of a term an EARNED rung is kept only if the
//    term's process reached the rung's threshold; otherwise the band drops one
//    rung, never below the rung it was admitted at (history, the bootstrap
//    rule, a test: rung 1, the canary-only rung), which only the mirrored
//    process or kappa can take away. A term in which fewer than m_term
//    outcomes arrived is a term without evidence, not a term of non-attainment,
//    and the rung stands. (The rule as the paper states it re-demands even the
//    admitted rung every term at 1/alpha_1, which no band of modest volume can
//    meet; E3b read that as a lapse of every band at the first boundary.)
//
//    EACH OUTCOME IS GRADED AGAINST THE BAR IN FORCE WHEN ITS ACT WAS MADE
//    (F24, found in E3b). An outcome arrives one verdict latency after the
//    act, and the bar moves between terms because the paired arm's rate is not
//    stationary under the machine's presence (at the battery's scale the frozen
//    g0 swung by a sixth between thirty-day terms). Graded against the arrival
//    term's bar, a null judge promotes whenever the bar falls between the act
//    and the verdict, and the plant showed it: a judge three points inside every
//    term's g0 promoted on half the bands. So the ladder keeps the bars it froze,
//    term by term, and each outcome's factor uses the bar of its act's term. A
//    product of factors each with expectation at most one under its own null,
//    given the past, is still a nonnegative supermartingale, so the bound stands.
//
//  NBAND and band_of() live in firm.h beside the writ, so both arms band alike.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"

namespace acme {

// How a rung came to be, on the LICENSE row's via byte (v4 §26.6, the
// admission kind, extended with the causes of a narrowing so a fold can grade
// every change): 0 the wager widened it · 1 history admitted it · 2 the
// bootstrap rule · 3 pooled across tenants · 4 grafted · 5 a test admission
// (O31's harness) · 6 the mirrored process fired · 7 kappa · 8 the term lapsed
// · 9 a regime change.
enum LicCause : uint8_t { LC_WAGER = 0, LC_HISTORY, LC_BOOTSTRAP, LC_POOLED, LC_GRAFTED, LC_TEST, LC_DEMOTED, LC_KAPPA, LC_LAPSED, LC_REGIME, LC_N };
inline const char* lic_cause_name(int c) {
  static const char* n[] = { "wager", "history", "bootstrap", "pooled", "grafted", "test", "demoted", "kappa", "lapsed", "regime" };
  return (c >= 0 && c < LC_N) ? n[c] : "?";
}

struct Lic {
  int   rung = 0;
  int   expiry_day = 0;             // E3: the end of the current term
  double logE = 0.0;                // E3: THE TERM'S EVIDENCE PROCESS. Never reset within a term.
  double logE_demote = 0.0;         // the mirrored process; restarts when it fires
  long   n_machine = 0, n_incumbent = 0;        // n_machine: the WAGER — unattended acts only (via 1 and 3)
  long   good_machine = 0, good_incumbent = 0;
  long   n_assisted = 0, good_assisted = 0;     // drafts a person keyed and warrants a signer executed (via 2 and 4):
                                                // graded, printed, never fed to the e-process
  // kappa
  double sup_created = 0, sup_removed = 0;
  long   n_agree = 0, n_disagree = 0, n_replayed = 0;
  long   agree_good = 0, agree_total = 0;
  bool   history_licensed = false;
  bool   unlicensable = false;      // n0 unreachable inside a term at any tolerable rate (a label; the ladder never reads it)
  int    n0 = 0;                    // the planning figure (F21, F23): printed, never a condition
  // E3: the ladder v2
  double bar = 0.0;                 // the paired arm's good rate in this band, frozen for the term at its upper bound; 0 = unmeasured
  double bar_g0 = 0.0;              // the null's boundary: 1 - (1 - bar)(1 + delta)
  double bar_pt = 0.0;              // the same rate at its point estimate: the mirrored process's null (E3b: at the upper bound a machine as
                                    // good as the incumbent looked worse than the bar and was demoted; conservative for promotion is the
                                    // upper bound, conservative for demotion is not)
  int    bar_n = 0;                 // the outcomes the bar was frozen from
  int    term_rung = 0;             // the highest rung whose threshold the term's process has crossed
  int    cause = 0;                 // LicCause of the last change
  long   n_term = 0;                // the wager's outcomes that entered this term's process
  double g0_min = 9.0, g0_max = 0.0;// the lowest and highest null boundary the band has run under (a reading: the bar moves between terms)
  int    floor_rung = 0;            // E3: the admitted rung (history, test, bootstrap: 1). A lapse never drops below it; the mirrored process can.
  double kappa() const { return sup_removed > 1e-9 ? sup_created / sup_removed : 9.99; }
};

struct Ladder {
  int NC = 0;
  std::vector<Lic> lic;             // [NC * NBAND]
  double alpha_promote = 1.0 / 20.0;
  double alpha_demote  = 1.0 / 5.5;   // demotion an order easier than promotion; trust rebuilds asymmetrically
  int    term_days = 30;
  // E3: the writ's tolerance and the bar's rules, copied at init so a fold and the live table agree
  float  delta = 0.30f;
  int    m_min = 200;               // outcomes of the paired arm before a bar is measured
  int    m_term = 10;               // outcomes in a term below which the rung stands (no evidence is not non-attainment)
  float  bar_z = 1.2816f;           // the one-sided 90 percent point: the bar is frozen at its upper bound
  // the key
  uint32_t schema_pin = 0;          // alphabet_hash() at init
  uint32_t judge_hash = 0;          // the judge these rungs were earned under
  std::vector<uint32_t> template_hash;   // [NC] the template each class was read with
  // E3 (F24): the bars as frozen, term by term, so an outcome is graded against
  // the bar of its act's term: the days terms opened, and per band per term the
  // bar and its g0 (0 = unmeasured that term)
  enum { MAXT = 64 };
  std::vector<uint32_t> opened;     // the days on which terms opened, ascending
  std::vector<float>    hist_bar, hist_g0, hist_pt;   // [(c*NBAND+b) * MAXT + t]: the upper-bound bar, its g0, and the point bar

  void init(int nc, uint32_t schema = 0, uint32_t judge = 0) {
    NC = nc; lic.assign((size_t)nc * NBAND, Lic{});
    schema_pin = schema; judge_hash = judge; template_hash.assign(nc, 0u);
    for (int c = 0; c < nc; ++c) for (int b = 0; b < NBAND; ++b) lic[(size_t)c * NBAND + b].n0 = n0_for(c);
    opened.clear(); hist_bar.assign((size_t)nc * NBAND * MAXT, 0.f); hist_g0.assign((size_t)nc * NBAND * MAXT, 0.f); hist_pt.assign((size_t)nc * NBAND * MAXT, 0.f);
  }
  // the index of the term in force on a day (-1 before the first term opened)
  int term_index_of(uint32_t day) const { int t = -1; for (uint32_t d : opened) { if (d <= day) ++t; else break; } return std::min(t, (int)MAXT - 1); }
  Lic& at(int c, int b) { return lic[(size_t)c * NBAND + (b < 0 ? 0 : (b >= NBAND ? NBAND - 1 : b))]; }
  const Lic& at(int c, int b) const { return lic[(size_t)c * NBAND + (b < 0 ? 0 : (b >= NBAND ? NBAND - 1 : b))]; }
  // The rung a judge may act under. A judge the table was not earned under
  // reads rung 0 everywhere: the licence is keyed, not named.
  int rung_for(int c, int b, uint32_t judge) const { return judge == judge_hash ? at(c, b).rung : 0; }

  // E3: the evidence the term's process must show to reach rung k: log(2^k / alpha)
  double threshold(int k) const { return std::log(1.0 / alpha_promote) + (double)k * std::log(2.0); }

  // E3: freeze the bar for the term from the paired arm's outcomes to date.
  // The upper one-sided bound, so estimation error only makes the bar harder.
  void freeze_bar(int c, int b) {
    Lic& L = at(c, b);
    L.bar_n = (int)L.n_incumbent;
    if (L.n_incumbent >= m_min) {
      const double p = (double)L.good_incumbent / (double)L.n_incumbent;
      const double se = std::sqrt(std::max(0.0, p * (1.0 - p)) / (double)L.n_incumbent);
      L.bar = std::min(0.98, std::max(0.02, p + (double)bar_z * se));
      L.bar_g0 = std::max(0.02, 1.0 - (1.0 - L.bar) * (1.0 + (double)delta));
      L.bar_pt = std::min(0.98, std::max(0.02, p));
      L.g0_min = std::min(L.g0_min, L.bar_g0); L.g0_max = std::max(L.g0_max, L.bar_g0);
    } else { L.bar = 0.0; L.bar_g0 = 0.0; L.bar_pt = 0.0; }
  }
  // E3: a term opens: every bar is frozen, every evidence process starts at one,
  // and the frozen bar joins the band's history under the term's index
  void open_term_band(int c, int b, uint32_t day, int t) {
    Lic& L = at(c, b);
    freeze_bar(c, b);
    L.logE = 0.0; L.n_term = 0; L.term_rung = 0; L.expiry_day = (int)day + term_days;
    if (t >= 0 && t < (int)MAXT) { const size_t h = ((size_t)c * NBAND + b) * MAXT + t; hist_bar[h] = (float)L.bar; hist_g0[h] = (float)L.bar_g0; hist_pt[h] = (float)L.bar_pt; }
  }
  // opens the term for every band; returns the term's index. A second opening
  // on the same day (the first term's day may also be a boundary) refreezes in
  // place under the same index.
  int open_term(uint32_t day, int skip_band = -1) {
    if (opened.empty() || opened.back() != day) opened.push_back(day);
    const int t = term_index_of(day);
    for (int c = 0; c < NC; ++c) for (int b = 0; b < NBAND; ++b) { if ((int)((size_t)c * NBAND + b) == skip_band) continue; open_term_band(c, b, day, t); }
    return t;
  }
  static bool term_boundary(uint32_t day, int term_days_) { return day > 0 && (int)(day % (uint32_t)term_days_) == 0; }

  // An arrived outcome. THE ONLY THING THAT WIDENS A LICENCE is an outcome of
  // an unattended act (via 1 or 3), and only against a measured bar: the bar
  // frozen for the term in which the act was made (F24). A draft or a warrant
  // is graded as assisted and feeds nothing.
  void observe(int c, int b, int via, bool good, uint32_t act_day = 0) {
    Lic& L = at(c, b);
    if (via == 1 || via == 3) {
      ++L.n_machine; L.good_machine += good ? 1 : 0;
      const int t = term_index_of(act_day);
      if (t < 0) return;                                       // no term had opened when the act was made: no process
      const size_t h = ((size_t)c * NBAND + b) * MAXT + t;
      const double p1 = hist_bar[h], p0 = hist_g0[h];
      if (p1 <= 0.0 || p0 <= 0.0) return;                      // no bar in the act's term: the band can neither widen nor lapse on it
      L.logE        += good ? std::log(p1 / p0) : std::log((1.0 - p1) / (1.0 - p0));
      // the mirrored process: null "as good as the incumbent", at the point bar; alternative a delta worse than it
      const double q1 = hist_pt[h], q0 = std::max(0.02, 1.0 - (1.0 - q1) * (1.0 + (double)delta));
      L.logE_demote += good ? std::log(q0 / q1) : std::log((1.0 - q0) / (1.0 - q1));
      ++L.n_term;
      // the term's high-water mark: the highest rung whose threshold the process has crossed
      while (L.term_rung < 5 && L.logE >= threshold(L.term_rung + 1)) ++L.term_rung;
    } else if (via == 2 || via == 4) {
      ++L.n_assisted; L.good_assisted += good ? 1 : 0;
    } else {
      ++L.n_incumbent; L.good_incumbent += good ? 1 : 0;
    }
  }
};

// O15 · THE LADDER IS A FOLD. Rebuild every licence's evidence counts and both
// e-processes from OUTCOME rows alone, in tape order, and compare to the live
// ladder. The rows carry the band and the via, so nothing else is joined. E3:
// the processes depend on the term (the TICK rows name the day) and on the
// frozen bar; fold_ladder_v2 below folds those too, and O45 compares the
// processes, the bars and every LICENSE row's justification.  [v2, step B3; E3]
inline Ladder fold_ladder(const Tape& tape, int NC, uint32_t from_day) {
  Ladder L; L.init(NC);
  tape.fold([&](const Rec& r) {
    if (r.type != R_OUTCOME || r.day < from_day || r.cls >= NC || r.a == OK_UNRESOLVED) return;
    L.observe(r.cls, r.band, (int)r.via, r.a == OK_GOOD, 0);     // counts only: no term ever opens in this fold
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

// O45 · THE LADDER v2 IS A FOLD. The evidence and demotion processes, the
// frozen bars and the term's high-water marks rebuilt from OUTCOME rows in
// arrival order with the terms named by the TICK rows; the rungs taken from
// LICENSE rows as O30 takes them, and every LICENSE row graded as it is
// folded: a widening by the wager must find the process past the rung's
// threshold on the row before it; a lapse must sit on a term boundary with the
// term's mark below the rung and at least m_term outcomes in the process; a
// demotion by the mirrored process must find that process past its threshold.
// The governor's own grade/step order is: outcomes of the day, then the step
// (which closes a term on its boundary). The fold replays exactly that.
struct LadderFoldOut { Ladder lad; long license_rows = 0, unjustified = 0; char first[160] = {0}; };
// `lie_band` is O45's lie: a fold that skips one class-band's term boundaries, so an
// outcome of the closed term is counted into the next (-1 = no lie).
inline LadderFoldOut fold_ladder_v2(const Tape& tape, int NC, const Ladder& like, int lie_band = -1) {
  LadderFoldOut out; Ladder& L = out.lad;
  L.init(NC, like.schema_pin, like.judge_hash);
  L.alpha_promote = like.alpha_promote; L.alpha_demote = like.alpha_demote; L.term_days = like.term_days;
  L.delta = like.delta; L.m_min = like.m_min; L.m_term = like.m_term; L.bar_z = like.bar_z;
  auto miss = [&](const char* why, const Rec& r) { if (out.unjustified++ == 0) snprintf(out.first, sizeof out.first, "%s: LICENSE cls %u band %d rung %d cause %s day %u", why, r.cls, r.b, r.a, lic_cause_name(r.via), r.day); };
  bool opened = false; uint32_t day = 0;
  std::vector<uint32_t> act_day;                                   // [oid] the day of the wager's act on the cell (F24: the bar of that term grades it)
  for (size_t i = 0; i < tape.rec.size(); ++i) {
    const Rec& r = tape.rec[i];
    if (r.type == R_TICK) { day = (uint32_t)r.a; continue; }
    if (r.type == R_EFFECT && !(r.flags & RF_SHADOW) && (r.via == 1 || r.via == 3)) {
      if (r.oid >= act_day.size()) act_day.resize((size_t)r.oid + 1024, 0);
      act_day[r.oid] = r.day; continue;
    }
    if (r.type == R_OUTCOME) {
      if (r.cls >= NC || r.a == OK_UNRESOLVED) continue;
      L.observe(r.cls, r.band, (int)r.via, r.a == OK_GOOD, r.oid < act_day.size() ? act_day[r.oid] : 0);
      continue;
    }
    if (r.type == R_LICENSE && r.cls < NC) {
      ++out.license_rows;
      Lic& x = L.at(r.cls, r.b);
      switch (r.via) {
        case LC_WAGER:   if (r.a > x.rung && x.logE < L.threshold(r.a) - 1e-9) miss("widening below threshold", r); break;
        case LC_DEMOTED: {
          if (!(x.logE_demote > std::log(1.0 / L.alpha_demote))) { miss("demotion without the process", r); }
          else { x.logE_demote = 0.0; }
          break;
        }
        case LC_LAPSED:  if (!Ladder::term_boundary(day, L.term_days) || x.n_term < L.m_term || x.term_rung >= x.rung || x.rung <= x.floor_rung) miss("lapse off its rule", r); break;
        case LC_HISTORY: case LC_TEST: case LC_KAPPA: case LC_REGIME: case LC_BOOTSTRAP: case LC_POOLED: case LC_GRAFTED: break;
        default: miss("unknown cause", r); break;
      }
      if (r.via == LC_HISTORY) x.history_licensed = true;
      if (r.via == LC_HISTORY || r.via == LC_TEST || r.via == LC_BOOTSTRAP) x.floor_rung = std::max(x.floor_rung, r.a);
      x.rung = r.a; x.cause = r.via;
      if (r.via == LC_REGIME || r.via == LC_KAPPA) { /* the live resets nothing on these but the rung */ }
      continue;
    }
    // the governor's term-open, as the live does it: at init (the first LICENSE row
    // of the run marks that the governor exists) and on every boundary at the step
    if (r.type == R_NOTE && r.a == 3) {                            // E3: the governor's NOTE: a term opened (b = the day)
      L.open_term((uint32_t)r.b, lie_band);                        // THE LIE (O45): one band's boundaries skipped
      opened = true;
    }
  }
  (void)opened;
  return out;
}
inline long ladder_diff_v2(const Ladder& a, const Ladder& b, char* first, size_t n) {
  long bad = 0;
  auto near = [](double x, double y) { return std::fabs(x - y) <= 1e-9 * std::max(1.0, std::max(std::fabs(x), std::fabs(y))); };
  for (size_t i = 0; i < a.lic.size() && i < b.lic.size(); ++i) {
    const Lic& x = a.lic[i]; const Lic& y = b.lic[i];
    const char* why = nullptr;
    if (x.n_machine != y.n_machine || x.good_machine != y.good_machine) { why = "wager counts"; }
    else if (x.n_incumbent != y.n_incumbent || x.good_incumbent != y.good_incumbent) { why = "incumbent counts"; }
    else if (!near(x.bar, y.bar) || x.bar_n != y.bar_n) { why = "bar"; }
    else if (!near(x.logE, y.logE)) { why = "logE"; }
    else if (!near(x.logE_demote, y.logE_demote)) { why = "logE_demote"; }
    else if (x.n_term != y.n_term || x.term_rung != y.term_rung) { why = "term"; }
    else if (x.rung != y.rung) { why = "rung"; }
    if (why != nullptr) {
      if (bad++ == 0 && first) snprintf(first, n, "%s at class-band %zu (fold %.4f/%.4f/%.4f/%d vs live %.4f/%.4f/%.4f/%d)", why, i, x.bar, x.logE, x.logE_demote, x.rung, y.bar, y.logE, y.logE_demote, y.rung);
    }
  }
  return bad + (long)(a.lic.size() != b.lic.size());
}

} // namespace acme
