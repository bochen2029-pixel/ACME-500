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
#include "ledger.h"
#include "machine.h"
// world.h is NOT included: the report reads the ledger, never the plant (O17)

namespace acme {

// ----------------------------------------------------------------------------
// §1 · THE α/E REGRESSION — an afternoon against payroll and the order book
// ----------------------------------------------------------------------------
struct Cascade {
  double alpha_hat = 0, F_hat = 0;        // fit WITH an intercept
  double alpha_naive = 0;                 // fit through the origin, the usual mistake
  int    n_points = 0;
  double predict(double E) const              { return E / std::max(0.05, 1.0 - alpha_hat) + F_hat; }
  double predict_naive(double E) const        { return E / std::max(0.05, 1.0 - alpha_naive); }
};

// The panel is what a real analyst has: the firm's payroll against its order
// book at the sizes it has actually been, as (E, N) points. C1: the report is
// handed the points and fits twice; it cannot generate a firm, because the
// generator is the plant's and the planted alpha and F are exactly what this
// regression must be caught missing.
struct CascadePoint { double E, N; };
inline Cascade fit_cascade(const std::vector<CascadePoint>& panel) {
  Cascade c;
  std::vector<double> E, N;
  for (const CascadePoint& p : panel) { E.push_back(p.E); N.push_back(p.N); }
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
  double frame_wait = 0;     //   of which: a licensable band still below rung 2. Moves with time.
  double frame_cov = 0;      //   of which: the uninstrumented tail of a licensed class. Moves with lane-buys.
  double thin_tape = 0;      // the world answers too slowly to license. Moves at the world's pace.
  double licensed = 0;       // decision mass under licence
  double total = 0;
};

// C1: decision mass is the MEASURED arrival rate (a fold of ARRIVE rows), and
// the counterparty column is the schema's authored flag, never a rule over a
// planted judgement intensity.
inline Residual residual_of(const Ladder& lad, const Compiled& C) {
  Residual r;
  for (int c = 0; c < lad.NC; ++c) {
    const ClassSpec& sp = cls_spec(c);
    const double mass = (c < (int)C.arrivals_per_day.size()) ? C.arrivals_per_day[c] : 0.f;
    r.total += mass;
    if (sp.warrant) { r.warrant += mass; continue; }
    if (sp.counterparty) { r.counterparty += mass; continue; }   // the other side demands a person
    bool any = false, thin = true;
    for (int b = 0; b < NBAND; ++b) {
      const Lic& L = lad.at(c, b);
      if (L.rung >= 2) any = true;
      if (!L.unlicensable) thin = false;
    }
    const double cov = C.coverage.empty() ? 1.0 : C.coverage[c];
    if (any)       r.licensed += mass * std::min(1.0, cov);
    else if (thin) r.thin_tape += mass;
    else         { r.frame += mass; r.frame_wait += mass; }                              // waits on the ladder
    if (any && cov < 1.0) { r.frame += mass * (1.0 - cov); r.frame_cov += mass * (1.0 - cov); }   // the uninstrumented tail of a licensed class
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

inline ArmResult score_arm(const Ledger& L, const Firm& f) {
  ArmResult r;
  for (const Obligation& o : L.ob) {
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

// ----------------------------------------------------------------------------
// §5 · THE PRINTS THAT ATTRIBUTE (ARCHITECTURE_v2 §9.3) — folds over the tape,
// the ledger and the ladder. Nothing here is an input to anything the machine
// computes; each one exists so that a number above it has a cause beside it.
// ----------------------------------------------------------------------------

// MINUTES BY VIA. Every human minute on the tape (ACT rows, the fetch minutes
// on CONTEXT rows, MEETING rows) attributed to how the cell it was spent on
// left the building. What bought the queue — the field deleting meeting-and-
// glue minutes, or drafts replacing decide-fetch-frame minutes — is not a
// reading until this prints. The full tensor minutes[seat][class][via] is the
// same fold with two more indices; this prints its via × kind marginal.
enum ViaRow { VR_PERSON = 0, VR_ACTED, VR_DRAFTED, VR_RENTED, VR_SIGNED, VR_UNDECIDED, VR_NOCELL, VR_N };
inline const char* via_row_name(int v) {
  static const char* n[] = { "a person decided (via 0)", "the resident acted (via 1)", "drafted, a person keyed (via 2)",
                             "a rented mind acted (via 3)", "a signer executed (via 4)", "never decided, open at the end",
                             "no cell: glue and meetings" };
  return n[v % VR_N];
}
struct MinutesByVia {
  double m[VR_N][ACT_N] = {};
  double row(int v) const { double s = 0; for (int k = 0; k < ACT_N; ++k) s += m[v][k]; return s; }
  double col(int k) const { double s = 0; for (int v = 0; v < VR_N; ++v) s += m[v][k]; return s; }
  double total() const { double s = 0; for (int v = 0; v < VR_N; ++v) s += row(v); return s; }
};
inline MinutesByVia minutes_by_via(const Tape& tape, const Ledger& L) {
  MinutesByVia M;
  auto bucket = [&](uint32_t oid) -> int {
    const Obligation* o = L.at_oid(oid);
    if (!o) return VR_NOCELL;
    if (o->via >= 1 && o->via <= 4) return (int)o->via;                       // VR_ACTED..VR_SIGNED line up with via
    return (o->state == OB_DECIDED || o->state == OB_SETTLED) ? VR_PERSON : VR_UNDECIDED;
  };
  tape.fold([&](const Rec& r) {
    if (r.arm != ARM_HUMAN) return;
    switch (r.type) {
      case R_ACT:     if (r.a >= 0 && r.a < ACT_N) M.m[bucket(r.oid)][r.a] += r.value; return;
      case R_CONTEXT: if (r.seat >= 0 && !(r.flags & RF_LOST)) M.m[bucket(r.oid)][ACT_FETCH] += r.value; return;
      case R_MEETING: M.m[VR_NOCELL][ACT_MEETING] += r.value; return;
      default: return;
    }
  });
  return M;
}
inline void print_minutes_by_via(const MinutesByVia& M, const char* title) {
  const double t = std::max(1.0, M.total());
  std::printf("\n  MINUTES BY VIA — %s: %0.0f thousand human minutes, by how the cell left the building\n", title, t / 1000.0);
  std::printf("  %-34s", "thousands of minutes");
  for (int k = 0; k < ACT_N; ++k) std::printf(" %8s", act_name(k));
  std::printf(" %9s %6s\n", "total", "share");
  for (int v = 0; v < VR_N; ++v) {
    if (M.row(v) <= 0.0) continue;
    std::printf("  %-34s", via_row_name(v));
    for (int k = 0; k < ACT_N; ++k) std::printf(" %8.1f", M.m[v][k] / 1000.0);
    std::printf(" %9.1f %5.1f%%\n", M.row(v) / 1000.0, 100.0 * M.row(v) / t);
  }
  std::printf("  %-34s", "all");
  for (int k = 0; k < ACT_N; ++k) std::printf(" %8.1f", M.col(k) / 1000.0);
  std::printf(" %9.1f %5.1f%%\n", M.total() / 1000.0, 100.0);
}

// THE BINDING REASON PER BAND. For every class-band, the first thing in the
// ladder's own order (license_from_history, then Ladder::step) that keeps the
// licence from being one rung wider. Where n_machine binds, volume is the
// lever; where no-history binds, the lever is the determinant the human read
// and the machine did not; where kappa binds, the licence costs more attention
// than it saves; where logE binds, the world has answered and the evidence has
// not crossed 1/alpha yet. Band 0 is the thin band: the gate drafts it or rents
// a mind for it at every rung, so no wager outcome can ever arrive there and
// nothing on the ladder applies. `unlicensable` (n0 unreachable inside a term
// at any tolerable canary rate) is a label the ladder never reads, so it is
// printed as a qualifier on the reason and never as the reason.
enum Binding { BR_TOP = 0, BR_THIN_BAND, BR_NO_HISTORY, BR_N_MACHINE, BR_LOGE, BR_KAPPA, BR_EXPIRY, BR_WIDENING, BR_N };
inline const char* binding_name(int r) {
  static const char* n[] = { "at-top", "thin-band", "no-history", "n_machine", "logE", "kappa", "expiry", "widening" };
  return n[r % BR_N];
}
inline const char* binding_lever(int r) {
  static const char* n[] = { "rung 5; nothing binds",
                             "the thin band: drafted or rented at every rung, never a wager; nothing on the ladder applies",
                             "never admitted: the determinant the human read and the machine did not",
                             "fewer wager outcomes than the floor: volume, the canary rate",
                             "outcomes past the floor, the e-process below 1/alpha: the world's answer",
                             "supervision created exceeds supervision removed: the licence costs more than it saves",
                             "past its expiry with no re-earned evidence: the next step narrows it",
                             "evidence past 1/alpha: widens at the next step" };
  return n[r % BR_N];
}
inline int binding_reason(const Lic& L, const Ladder& lad, const Writ& wr, uint32_t day, int band) {
  if (band == 0) return BR_THIN_BAND;
  if (L.rung >= 5) return BR_TOP;
  if (L.rung >= 1 && L.kappa() > wr.kappa_max && L.sup_removed > 60.0) return BR_KAPPA;
  if (L.rung > 1 && L.expiry_day > 0 && (int)day > L.expiry_day) return BR_EXPIRY;
  const bool enough = L.n_machine >= L.n0 || (L.history_licensed && band == 2 && L.n_machine >= std::max(20, L.n0 / 8));
  if (!enough) return (L.rung == 0 && !L.history_licensed) ? BR_NO_HISTORY : BR_N_MACHINE;
  if (L.logE <= std::log(1.0 / lad.alpha_promote)) return BR_LOGE;
  return BR_WIDENING;
}
inline void print_binding_reasons(const Ladder& lad, const Writ& wr, uint32_t day) {
  long n[BR_N][NBAND] = {}; long unl = 0, top_under_floor = 0;
  for (int c = 0; c < lad.NC; ++c) for (int b = 0; b < NBAND; ++b) {
    const Lic& L = lad.at(c, b);
    ++n[binding_reason(L, lad, wr, day, b)][b];
    if (b > 0 && L.unlicensable) ++unl;
    if (L.rung >= 5 && L.n_machine < L.n0) ++top_under_floor;
  }
  std::printf("\n  THE BINDING REASON PER BAND at day %u: the first thing in the ladder's own order that\n"
              "  keeps each class-band from one rung wider, and what moves it\n\n", day);
  std::printf("  %-12s", "reason");
  for (int b = 0; b < NBAND; ++b) std::printf("  band%d", b);
  std::printf("  %5s   %s\n", "all", "the lever");
  for (int r = 0; r < BR_N; ++r) {
    long tot = 0; for (int b = 0; b < NBAND; ++b) tot += n[r][b];
    if (!tot) continue;
    std::printf("  %-12s", binding_name(r));
    for (int b = 0; b < NBAND; ++b) std::printf("  %5ld", n[r][b]);
    std::printf("  %5ld   %s\n", tot, binding_lever(r));
  }
  std::printf("\n  %-22s %-34s %-11s %-18s %s\n", "class", "binding 0/1/2 (*: n0 unreachable)", "rung 0/1/2", "n_act 0/1/2", "n0");
  for (int c = 0; c < lad.NC; ++c) {
    char bind[80]; snprintf(bind, sizeof bind, "%s/%s%s/%s%s",
                            binding_name(binding_reason(lad.at(c, 0), lad, wr, day, 0)),
                            binding_name(binding_reason(lad.at(c, 1), lad, wr, day, 1)), lad.at(c, 1).unlicensable ? "*" : "",
                            binding_name(binding_reason(lad.at(c, 2), lad, wr, day, 2)), lad.at(c, 2).unlicensable ? "*" : "");
    char rung[32]; snprintf(rung, sizeof rung, "%d/%d/%d", lad.at(c, 0).rung, lad.at(c, 1).rung, lad.at(c, 2).rung);
    char nact[48]; snprintf(nact, sizeof nact, "%ld/%ld/%ld", lad.at(c, 0).n_machine, lad.at(c, 1).n_machine, lad.at(c, 2).n_machine);
    std::printf("  %-22s %-34s %-11s %-18s %d\n", cls_spec(c).name, bind, rung, nact, lad.at(c, 0).n0);
  }
  std::printf("\n  * %ld of %d bands past the thin band have n0 unreachable inside a term at any tolerable canary\n"
              "    rate (F14); the ladder never reads that label.  %ld bands sit at rung 5 with fewer wager\n"
              "    outcomes than n0: the floor is checked against cumulative n_machine, so it is paid once and\n"
              "    not per rung, and on a history-admitted band 2 it is n0/8 (F21, recorded and not fixed here).\n",
              unl, lad.NC * (NBAND - 1), top_under_floor);
}

// COGNITIVE PATH LENGTH (§9.3, D1). Per decided cell: the rows about it from
// its arrival to its terminal DECIDE or EFFECT (fetches, frames, handoffs,
// holds in force, proposals), the seats it passed through (hops, from the
// ledger) and the days from arrival to disposition; per class, for cells a
// person decided against cells the machine disposed of. It is the print that
// says whether the interior was deleted or moved (kill F-PATH): a licensed
// class disposed of in one read and one effect has path length two.
struct PathLen {
  int NC = 0;
  std::vector<double> rows_h, hops_h, days_h, rows_m, hops_m, days_m;
  std::vector<long>   n_h, n_m;
  void init(int nc) { NC = nc; rows_h.assign(nc, 0); hops_h.assign(nc, 0); days_h.assign(nc, 0); rows_m.assign(nc, 0); hops_m.assign(nc, 0); days_m.assign(nc, 0); n_h.assign(nc, 0); n_m.assign(nc, 0); }
  double mean(const std::vector<double>& v, const std::vector<long>& n, int c) const { return n[c] ? v[c] / n[c] : 0.0; }
};
inline PathLen path_length(const Tape& tape, const Ledger& L) {
  PathLen P; P.init(L.NC);
  std::vector<uint32_t> rows_to_term; std::vector<uint8_t> done;
  tape.fold([&](const Rec& r) {
    if (r.oid == 0) return;
    if (r.oid >= rows_to_term.size()) { rows_to_term.resize((size_t)r.oid + 1024, 0); done.resize((size_t)r.oid + 1024, 0); }
    if (done[r.oid]) return;
    ++rows_to_term[r.oid];
    if (r.type == R_DECIDE || r.type == R_EFFECT) done[r.oid] = 1;
  });
  for (const Obligation& o : L.ob) {
    if (o.state != OB_DECIDED && o.state != OB_SETTLED) continue;
    if (o.id >= done.size() || !done[o.id] || o.cls >= L.NC) continue;
    const double rows = rows_to_term[o.id], hops = o.hops, days = (double)(o.day_decided - o.day_open);
    if (o.by_machine) { P.rows_m[o.cls] += rows; P.hops_m[o.cls] += hops; P.days_m[o.cls] += days; ++P.n_m[o.cls]; }
    else              { P.rows_h[o.cls] += rows; P.hops_h[o.cls] += hops; P.days_h[o.cls] += days; ++P.n_h[o.cls]; }
  }
  return P;
}
inline void print_path_length(const PathLen& P, const char* title) {
  std::printf("\n  COGNITIVE PATH LENGTH — %s: rows from arrival to disposition, seats passed, days; per class,\n"
              "  cells a person decided against cells the machine disposed of (means)\n\n", title);
  std::printf("  %-22s %8s %6s %6s %6s   %8s %6s %6s %6s\n", "class", "person n", "rows", "seats", "days", "machine n", "rows", "seats", "days");
  double th_r = 0, th_h = 0, th_d = 0, tm_r = 0, tm_h = 0, tm_d = 0; long th_n = 0, tm_n = 0;
  for (int c = 0; c < P.NC; ++c) {
    th_r += P.rows_h[c]; th_h += P.hops_h[c]; th_d += P.days_h[c]; th_n += P.n_h[c];
    tm_r += P.rows_m[c]; tm_h += P.hops_m[c]; tm_d += P.days_m[c]; tm_n += P.n_m[c];
    std::printf("  %-22s %8ld %6.1f %6.2f %6.1f   %8ld %6.1f %6.2f %6.1f\n", cls_spec(c).name,
                P.n_h[c], P.mean(P.rows_h, P.n_h, c), P.mean(P.hops_h, P.n_h, c), P.mean(P.days_h, P.n_h, c),
                P.n_m[c], P.mean(P.rows_m, P.n_m, c), P.mean(P.hops_m, P.n_m, c), P.mean(P.days_m, P.n_m, c));
  }
  std::printf("  %-22s %8ld %6.1f %6.2f %6.1f   %8ld %6.1f %6.2f %6.1f\n", "all", th_n,
              th_n ? th_r / th_n : 0.0, th_n ? th_h / th_n : 0.0, th_n ? th_d / th_n : 0.0,
              tm_n, tm_n ? tm_r / tm_n : 0.0, tm_n ? tm_h / tm_n : 0.0, tm_n ? tm_d / tm_n : 0.0);
}

// THE FRONTIER BILL BY VALUE QUINTILE (§9.3, D1). Every rental of the rented
// mind is a row: an EFFECT via 3 or the resident's ESCALATE. Classes are sorted
// by the value at stake and cut into five groups; what the frontier is spent
// on, before step E ranks its queue by margin thinness x value.
inline void print_frontier_bill(const Tape& tape, int NC) {
  std::vector<long> rentals(NC, 0);
  tape.fold([&](const Rec& r) {
    if (r.cls >= NC) return;
    if ((r.type == R_EFFECT && r.via == 3) || (r.type == R_ESCALATE && r.arm == ARM_MACHINE)) ++rentals[r.cls];
  });
  std::vector<int> order(NC); for (int c = 0; c < NC; ++c) order[c] = c;
  std::sort(order.begin(), order.end(), [](int x, int y) { return cls_spec(x).value < cls_spec(y).value; });
  long total = 0; for (long k : rentals) total += k;
  std::printf("\n  THE FRONTIER BILL BY VALUE: %ld rentals, the classes in five groups from the least to the most at stake\n", total);
  std::printf("  %-8s %-24s %10s %7s   %s\n", "group", "value at stake", "rentals", "share", "classes");
  for (int g = 0; g < 5; ++g) {
    const int lo = g * NC / 5, hi = (g + 1) * NC / 5;
    long k = 0; std::string names; float vlo = 1e30f, vhi = 0.f;
    for (int i = lo; i < hi; ++i) { const int c = order[i]; k += rentals[c]; vlo = std::min(vlo, cls_spec(c).value); vhi = std::max(vhi, cls_spec(c).value);
      names += (names.empty() ? "" : " ") + std::string(cls_spec(c).name); }
    char range[40]; snprintf(range, sizeof range, "$%.0fk to $%.0fk", vlo / 1000.f, vhi / 1000.f);
    std::printf("  %-8d %-24s %10ld %6.1f%%   %s\n", g + 1, range, k, total ? 100.0 * k / total : 0.0, names.c_str());
  }
}

// THE ROW HISTOGRAM. Rows by type, from which the write share S of §1 becomes
// computable on writes rather than on minutes.
inline void print_row_histogram(const Tape& tape) {
  unsigned long long n[R_N] = {};
  tape.fold([&](const Rec& r) { if (r.type < R_N) ++n[r.type]; });
  std::printf("  rows by type:");
  int col = 15;
  for (int t = 0; t < R_N; ++t) {
    if (!n[t]) continue;
    char cell[48]; const int w = snprintf(cell, sizeof cell, " %s %llu", rec_type_name(t), n[t]);
    if (col + w > 100) { std::printf("\n               "); col = 15; }
    std::printf("%s", cell); col += w;
  }
  std::printf("\n");
}

} // namespace acme
