// ============================================================================
//  acme/report.h — the cascade, the residual, and the counters
//
//  THE CASCADE.  Headcount is not the sum of the work. It is
//
//        N = E / (1 − α) + F
//
//  E is external work: orders fulfilled, claims settled, filings filed. α is
//  the support coefficient — the seats whose headcount tracks HEADCOUNT rather
//  than volume. F is the fixed floor: one IT person exists at five people.
//
//  Two consequences the industry's arithmetic misses.
//
//  First, automation does not remove the fraction of tasks it automates. It
//  reduces E, and the multiplier removes everything standing on top. At α = 0.2
//  the firm is 1.25× its external work; the α-functions leave whether or not
//  anyone ever automates what they do.
//
//  Second, and this is the correction: a within-firm regression CANNOT see F.
//  You fit α across a range of N the firm has never left, then use it to predict
//  a size the firm has never been. The estimator omits the intercept because the
//  intercept is invisible over a narrow range, and it therefore promises a
//  smaller company than physics allows. This file fits it BOTH ways and prints
//  the error, because the honest version of this argument shows its own bias.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "world.h"
#include "human.h"
#include "machine.h"

namespace acme {

// ----------------------------------------------------------------------------
// §1 · THE α/E REGRESSION — an afternoon against payroll and the order book
// ----------------------------------------------------------------------------
struct Cascade {
  double alpha_hat = 0, F_hat = 0;        // fit WITH an intercept
  double alpha_naive = 0;                 // fit through the origin, the usual mistake
  double alpha_true = 0, F_true = 0;
  int    n_points = 0;
  double predict(double E) const              { return E / std::max(0.05, 1.0 - alpha_hat) + F_hat; }
  double predict_naive(double E) const        { return E / std::max(0.05, 1.0 - alpha_naive); }
};

// Build the panel the way a real analyst would: the firm at the sizes it has
// actually been. Then fit twice.
inline Cascade fit_cascade(int span, uint64_t seed, int lo = 430, int hi = 580, int step = 25) {
  Cascade c;
  std::vector<double> E, N;
  for (int n = lo; n <= hi; n += step) {
    Firm f = build_acme(n, span, seed);
    const double e = f.count_fn(FN_E) + f.count_fn(FN_WARRANT);
    E.push_back(e); N.push_back(f.size());
    c.alpha_true = f.alpha_true; c.F_true = f.F_true;
  }
  c.n_points = (int)E.size();
  // N = a*E + b, with a = 1/(1-alpha), b = F
  double sx = 0, sy = 0, sxx = 0, sxy = 0; const double m = (double)E.size();
  for (size_t i = 0; i < E.size(); ++i) { sx += E[i]; sy += N[i]; sxx += E[i] * E[i]; sxy += E[i] * N[i]; }
  const double den = m * sxx - sx * sx;
  const double a = (den != 0) ? (m * sxy - sx * sy) / den : 1.0;
  const double b = (m != 0) ? (sy - a * sx) / m : 0.0;
  c.alpha_hat = 1.0 - 1.0 / std::max(1e-6, a);
  c.F_hat = b;
  // the naive fit: through the origin. This is what a ratio analysis produces
  // and it is the one that promises the impossible company.
  double num = 0, dd = 0; for (size_t i = 0; i < E.size(); ++i) { num += E[i] * N[i]; dd += E[i] * E[i]; }
  const double a0 = dd != 0 ? num / dd : 1.0;
  c.alpha_naive = 1.0 - 1.0 / std::max(1e-6, a0);
  return c;
}

// ----------------------------------------------------------------------------
// §2 · THE RESIDUAL — four columns, and only two of them move
// ----------------------------------------------------------------------------
struct Residual {
  double warrant = 0;        // a signature, liability. A legal fact. Does not move.
  double counterparty = 0;   // the other side demands a person. Does not move.
  double frame = 0;          // what business are we in. Moves slowly, needs an arena.
  double thin_tape = 0;      // the world answers too slowly to license. Moves at the world's pace.
  double licensed = 0;       // decision mass under licence
  double total = 0;
};

inline Residual residual_of(const Ladder& lad, const World& w, const Compiled& C) {
  Residual r;
  for (int c = 0; c < lad.NC; ++c) {
    const ClassSpec& sp = cls_spec(c);
    const double mass = sp.arrival_per_day * w.demand_scale;
    r.total += mass;
    if (sp.warrant) { r.warrant += mass; continue; }
    // counterparty: high-value contested classes where the other side wants a person
    if (sp.value > 60000 && sp.decide_frac > 0.35f) { r.counterparty += mass; continue; }
    bool any = false, thin = true;
    for (int b = 0; b < NBAND; ++b) {
      const Lic& L = lad.at(c, b);
      if (L.rung >= 2) any = true;
      if (!L.unlicensable) thin = false;
    }
    const double cov = C.coverage.empty() ? 1.0 : C.coverage[c];
    if (any)       r.licensed += mass * std::min(1.0, cov);
    else if (thin) r.thin_tape += mass;
    else           r.frame += mass;
    if (any && cov < 1.0) r.frame += mass * (1.0 - cov);   // the uninstrumented tail of a licensed class
  }
  return r;
}

// ----------------------------------------------------------------------------
// §3 · THE PAIRED ARMS — the one thing a twin can do that a firm cannot
// ----------------------------------------------------------------------------
struct ArmResult {
  double writ_cost = 0;
  // THE BACKLOG IS ON THE BOOKS. An obligation nobody settled is not free: the
  // writ prices a stock (w_unplaced) and the solver charges it, so the arm
  // score charges it too. The first run scored settled obligations only, and
  // the incumbent's 124,000 unfinished ones cost it nothing.
  double backlog_cost = 0;
  long   open_n = 0;
  double total_cost() const { return writ_cost + backlog_cost; }
  long   settled = 0, good = 0, late = 0, bad = 0;
  long   breached = 0;
  double human_minutes = 0;
  double decide_minutes = 0;
  double cycle_days = 0;
  long   n_cycle = 0;
  double payroll = 0;
  int    headcount = 0;
  double kappa = 0;
  long   acted = 0, drafted = 0, frontier = 0, warrant = 0, held = 0;
  double accuracy() const { return settled ? (double)good / (double)settled : 0.0; }
  double mean_cycle() const { return n_cycle ? cycle_days / n_cycle : 0.0; }
};

inline ArmResult score_arm(const World& w, const Firm& f) {
  ArmResult r;
  for (const Obligation& o : w.ob) {
    if (o.state != OB_SETTLED) {
      ++r.open_n;
      r.backlog_cost += f.writ.w_unplaced * cls_spec(o.cls).value;
      continue;
    }
    ++r.settled;
    switch (o.outcome) { case OK_GOOD: ++r.good; break; case OK_LATE: ++r.late; break; default: ++r.bad; break; }
    if (o.day_decided > o.day_due) ++r.breached;
    r.writ_cost += outcome_cost(f.writ, cls_spec(o.cls), o.outcome);
    r.cycle_days += (double)(o.day_settled - o.day_open); ++r.n_cycle;
  }
  r.payroll = f.payroll(); r.headcount = f.size();
  return r;
}

// THE CANARY'S BIAS. A real deployment can never measure this: the canary and
// the control share one ledger, humans see the machine's decisions and best
// respond to them, and the contamination grows with the canary rate. A twin CAN
// measure it, because it can run the same seeded world twice with nothing
// shared. So the simulation's most useful output is not "the machine is better"
// — it is HOW WRONG THE CANARY ESTIMATE IS, which is the number the real
// programme has to budget for and cannot obtain.
struct CanaryBias {
  double true_effect = 0;      // paired arms, exact
  double canary_estimate = 0;  // what a within-firm canary would have reported
  double bias() const { return canary_estimate - true_effect; }
};

// ----------------------------------------------------------------------------
// §4 · THE COUNTERS
// ----------------------------------------------------------------------------
inline void print_time_ledger(const TimeLedger& L, const char* title) {
  const double t = std::max(1.0, L.total());
  std::printf("\n  %s — where %0.0f thousand human minutes went\n", title, t / 1000.0);
  struct Row { const char* n; double v; const char* what; };
  const Row rows[] = {
    { "glue",      L.glue.sum(),      "keeping up: reading, skimming, re-reading yesterday" },
    { "meeting",   L.meeting.sum(),   "context delivery between skulls" },
    { "fetch",     L.fetch.sum(),     "opening applications to find fields that were already stored" },
    { "frame",     L.frame.sum(),     "rendering state for another human's eyes" },
    { "decide",    L.decide.sum(),    "THE JOB" },
    { "commit",    L.commit.sum(),    "typing the answer into another application" },
    { "transport", L.transport.sum(), "handoffs and escalations" },
    { "rework",    L.rework.sum(),    "re-acquiring context a skull could not hold" },
  };
  for (const Row& r : rows) {
    const int bar = (int)(46.0 * r.v / t);
    std::printf("    %-10s %6.2f%%  ", r.n, 100.0 * r.v / t);
    for (int i = 0; i < 46; ++i) std::putchar(i < bar ? '#' : '.');
    std::printf("  %s\n", r.what);
  }
  std::printf("    %-10s %6.2f%%  <- the decide fraction. Everything else is the round trip.\n",
              "JUDGMENT", 100.0 * L.decide.sum() / t);
}

} // namespace acme
