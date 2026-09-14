// ============================================================================
//  ACME-500 — a virtual enterprise, and its automation, in one binary
//
//  Five hundred white-collar seats in a cone with a span of seven. Six core
//  wires, twenty-four decision classes, a world that arrives every day and
//  grades what the firm did at its own pace. It is a digital twin because the
//  physics is one function that both arms call, and it is a demonstrator
//  because the automation is not a script — it is a resident, a field, a gate
//  and a ladder, and every number it prints is a reading rather than a target.
//
//  MODES
//    acme --sim        [--days 260]              run ACME as it is. Print where the minutes go.
//    acme --twin       [--days 260]              BOTH arms on one seeded world. The exact counterfactual.
//    acme --automate   [--warm 180 --days 400]   the whole programme: compile, replay,
//                                                license the agreement bands, canary everything
//                                                at the floor, climb on arrivals, cascade.
//    acme --multiverse [--futures 64]            policy search: many ACMEs at once, writ-reweighted.
//    acme --selftest   [--lie N]                 the oracles. Each carries a lie.
//
//  WHAT IS NOT CLAIMED. This world is synthetic with planted truth. It tests the
//  instrument, never a business. Every constant in the schema is a guess made in
//  the open so that it can be argued with, and the first real receipt is still
//  the one the crystallisation named: replay a historical quarter of one core
//  wire and compare against what the organisation actually did.
// ============================================================================
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/world.h"
#include "acme/human.h"
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/ledger.h"
#include "acme/report.h"
#include <cstdio>
#include <cstring>
#include <chrono>

using namespace acme;

struct Args {
  const char* mode = "--selftest";
  int days = 260, warm = 180, futures = 64, span = 7, n = 500, lie = -1;
  float demand = 1.5f;
  uint64_t seed = 20260913;
  bool quiet = false;
};
static Args parse(int argc, char** argv) {
  Args a;
  for (int i = 1; i < argc; ++i) {
    const char* s = argv[i];
    auto next_i = [&](int d) { return (i + 1 < argc) ? atoi(argv[++i]) : d; };
    auto next_f = [&](float d) { return (i + 1 < argc) ? (float)atof(argv[++i]) : d; };
    if (!strncmp(s, "--sim", 5) || !strncmp(s, "--twin", 6) || !strncmp(s, "--automate", 10)
     || !strncmp(s, "--multiverse", 12) || !strncmp(s, "--selftest", 10)) a.mode = s;
    else if (!strcmp(s, "--days")) a.days = next_i(260);
    else if (!strcmp(s, "--warm")) a.warm = next_i(180);
    else if (!strcmp(s, "--futures")) a.futures = next_i(64);
    else if (!strcmp(s, "--span")) a.span = next_i(7);
    else if (!strcmp(s, "--n")) a.n = next_i(500);
    else if (!strcmp(s, "--lie")) a.lie = next_i(-1);
    else if (!strcmp(s, "--demand")) a.demand = next_f(1.5f);
    else if (!strcmp(s, "--seed")) a.seed = (uint64_t)next_i(20260913);
    else if (!strcmp(s, "--quiet")) a.quiet = true;
    else { std::fprintf(stderr, "unknown flag %s\n", s); exit(2); }
  }
  return a;
}
static double now_s() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
static void rule(const char* t) { std::printf("\n=== %s ", t); for (int i = (int)strlen(t); i < 66; ++i) std::putchar('='); std::putchar('\n'); }

// ----------------------------------------------------------------------------
// The incumbent arm: ACME as it is
// ----------------------------------------------------------------------------
struct Run { World w; Ledger L; Firm f; Tape tape; HumanStats hs;
             void init_ledger() { L.init(w.NC); } };   // C0: the cells live in the ledger, the plant keeps the truth

// mode * 10 + switch, as the HEADER row carries it: world 0 synthetic · 1 fitted · 2 replay · 3 real;
// switch 0 off · 1 shadow · 2 live. ACME is (synthetic, live) by construction.
enum { MODE_SYNTHETIC = 0, SW_OFF = 0, SW_SHADOW = 1, SW_LIVE = 2 };

static void run_human(Run& R, int days, bool chain = true) {
  R.tape.chaining = chain;
  R.hs.init(R.w.NC); if (R.L.NC == 0) R.init_ledger();
  auto rep = reports_of(R.f);
  if (R.tape.size() == 0) R.tape.header(MODE_SYNTHETIC, SW_LIVE, alphabet_hash());
  for (int d = 0; d < days; ++d) {
    R.tape.tick((uint32_t)d);                       // v2: the clock is a row
    world_arrive(R.w, R.L, R.tape, (uint32_t)d);
    human_day(R.w, R.L, R.f, R.tape, R.hs, rep, (uint32_t)d);
    world_settle(R.w, R.L, R.f, R.tape, (uint32_t)d);
  }
}

static int cmd_sim(const Args& a) {
  Run R; R.f = build_acme(a.n, a.span, a.seed); R.w = make_world(a.seed); R.w.demand_scale = a.demand;
  rule("ACME CORP, AS IT IS");
  std::printf("  %d seats, span %d, %d layers.  %d IC / %d lead / %d manager / %d director / %d VP / %d C\n",
              R.f.size(), R.f.span, 6, R.f.n_ic, R.f.n_lead, R.f.n_mgr, R.f.n_dir, R.f.n_vp, R.f.n_c);
  std::printf("  payroll $%.1fM.  %d seats do external work, %d exist because the firm has people, %d hold a warrant.\n",
              R.f.payroll() / 1e6, R.f.count_fn(FN_E), R.f.count_fn(FN_ALPHA), R.f.count_fn(FN_WARRANT));
  const double t0 = now_s();
  run_human(R, a.days);
  const double dt = now_s() - t0;
  const ArmResult ar = score_arm(R.L, R.f);

  print_time_ledger(R.hs.time, "THE YEAR");
  rule("WHAT THE FIRM PRODUCED");
  std::printf("  settled %ld obligations in %d days: %.1f%% good, %.1f%% late, %.1f%% wrong\n",
              ar.settled, a.days, 100.0 * ar.good / std::max(1L, ar.settled),
              100.0 * ar.late / std::max(1L, ar.settled), 100.0 * ar.bad / std::max(1L, ar.settled));
  std::printf("  mean cycle %.1f days   deadline breaches %ld (%.1f%%)   still open %zu\n",
              ar.mean_cycle(), ar.breached, 100.0 * ar.breached / std::max(1L, ar.settled), R.L.open_idx.size());
  std::printf("  escalations %llu (%.2f per decision)   meetings held %llu   holds recorded %llu\n",
              (unsigned long long)R.hs.n_escalated, (double)R.hs.n_escalated / std::max<uint64_t>(1, R.hs.n_decided),
              (unsigned long long)R.hs.n_meetings, (unsigned long long)R.hs.n_held);
  std::printf("  writ cost of the outcomes: $%.1fM   backlog at w_unplaced: $%.1fM (%ld open)   total $%.1fM\n",
              ar.writ_cost / 1e6, ar.backlog_cost / 1e6, ar.open_n, ar.total_cost() / 1e6);
  std::printf("\n  tape: %zu rows, chain head %s..., verify %s   (%.1f s, %.0f rows/s)\n",
              R.tape.size(), R.tape.head_hex().substr(0, 16).c_str(),
              R.tape.verify() < 0 ? "OK" : "BROKEN", dt, R.tape.size() / std::max(1e-9, dt));
  print_row_histogram(R.tape);
  std::printf("\n  READ THE LEDGER AGAIN. The escalation line is the round trip in its purest form:\n"
              "  every one of those %llu escalations made a manager re-open systems an IC had\n"
              "  already opened, because the manager could not see the IC's screen.\n",
              (unsigned long long)R.hs.n_escalated);
  return 0;
}

// ----------------------------------------------------------------------------
// The full programme
// ----------------------------------------------------------------------------
struct AutoOut {
  ArmResult arm;
  Compiled  C;
  ReplayOut RP;
  Ladder    lad;
  Residual  res;
  MachineStats ms;
  double    machine_days = 0;
  long      hist_licensed = 0;
  double    replay_ms = 0, period_ms = 0;
  int       periods = 0;
};

static AutoOut run_machine(const Args& a, Run& R, int warm_days, int total_days, bool verbose) {
  AutoOut out;
  // ---- PHASE 0/1: the boundary log and the warm history. Nobody uses anything.
  run_human(R, warm_days);
  out.C = compile_from_tape(R.tape, R.w.NC, a.seed, true);

  // ---- PHASE 2: the resident, compiled, not yet acting
  Resident res;
  res.init(R.w.NC, R.f.size(), R.w.demand_scale, out.C, R.f.writ, a.seed);
  // C0: the machine touches the world through the port only
  PlantStore store(&R.w); PlantJudge judge(&R.w);

  // ---- PHASE 3: REPLAY. The fast grader, on its own support.
  double t0 = now_s();
  out.RP = replay(R.L, R.f, res.fd, out.C, res.lad, R.f.writ, store, judge);
  out.replay_ms = (now_s() - t0) * 1000.0;
  for (int c = 0; c < R.w.NC; ++c) out.C.coverage[c] = out.RP.coverage[c];
  out.hist_licensed = license_from_history(res.lad, out.RP, (uint32_t)warm_days, R.tape);

  // ---- PHASE 4+: live. Both the remaining humans and the resident, on one world.
  auto rep = reports_of(R.f);
  t0 = now_s();
  for (int d = warm_days; d < total_days; ++d) {
    R.tape.tick((uint32_t)d);
    world_arrive(R.w, R.L, R.tape, (uint32_t)d);
    res.period(R.L, R.f, R.tape, (uint32_t)d, store, judge);   // the resident goes first: it never sleeps
    human_day(R.w, R.L, R.f, R.tape, R.hs, rep, (uint32_t)d);
    world_settle(R.w, R.L, R.f, R.tape, (uint32_t)d);
    res.grade(R.L, R.f, R.tape, (uint32_t)d);
    ++out.periods;
  }
  out.period_ms = (now_s() - t0) * 1000.0 / std::max(1, out.periods);
  out.lad = res.lad; out.ms = res.st;
  out.res = residual_of(res.lad, R.w.demand_scale, out.C);
  out.arm = score_arm(R.L, R.f);
  (void)verbose;
  return out;
}

static int cmd_automate(const Args& a) {
  Run R; R.f = build_acme(a.n, a.span, a.seed); R.w = make_world(a.seed); R.w.demand_scale = a.demand;
  rule("PHASE 0-1 · INSTRUMENT THE BOUNDARY, THEN WATCH");
  std::printf("  Change data capture off the systems of record into one hash-chained tape:\n"
              "  events in, effects out, outcomes when they land. NEVER the interior. The\n"
              "  meeting, the deck and the ticket are not instrumented, because the label\n"
              "  was always the arrival and never what a person selected.\n");
  const AutoOut A = run_machine(a, R, a.warm, a.days, true);

  rule("PHASE 2 · COMPILE, FROM THE APPLICATION");
  std::printf("  %d classes seen. The join graph is recovered from the query log — which\n"
              "  systems each screen actually opened — not from asking anybody.\n\n", A.C.classes_seen);
  std::printf("  %-22s %5s %5s  %8s %8s   %s\n", "class", "sys^", "sys", "cover^", "cover", "decide-fraction (measured)");
  int exact = 0;
  for (int c = 0; c < R.w.NC; ++c) {
    if (A.C.n_systems[c] == cls_spec(c).n_systems) ++exact;
    if (c % 4) continue;
    std::printf("  %-22s %5d %5d  %8.3f %8.3f   %.3f\n", cls_spec(c).name,
                A.C.n_systems[c], cls_spec(c).n_systems, A.C.coverage[c], 1.f - R.w.tacit[c], A.C.decide_frac[c]);
  }
  std::printf("  join graph recovered exactly on %d of %d classes.\n", exact, R.w.NC);
  auto inv = mine_invariants(R.tape, R.L, R.w.NC);
  int hard = 0; for (const auto& i : inv) if (i.hard) ++hard;
  std::printf("  invariants: %zu predicates proposed, %d held with ZERO violations over the\n"
              "  history and become -inf masks before normalisation. The rest are soft terms.\n", inv.size(), hard);

  rule("PHASE 3 · REPLAY — THE FAST GRADER, ON ITS OWN SUPPORT");
  long n = 0, ag = 0, agg = 0, dis = 0, dfg = 0, dfb = 0;
  for (int c = 0; c < R.w.NC; ++c) { n += A.RP.n[c]; ag += A.RP.agree[c]; agg += A.RP.agree_good[c];
                                     dis += A.RP.disagree[c]; dfg += A.RP.disagree_firm_good[c]; dfb += A.RP.disagree_firm_bad[c]; }
  std::printf("  re-decided %ld settled obligations in %.0f ms. The four-cell table:\n\n", n, A.replay_ms);
  std::printf("      %-34s %10s %10s\n", "", "firm good", "firm bad");
  std::printf("      %-34s %10ld %10ld   <- GRADED. identical action, the world's own verdict\n", "resident agreed", agg, ag - agg);
  std::printf("      %-34s %10ld %10ld   <- NOT GRADED, either cell.\n", "resident disagreed", dfg, dfb);
  std::printf("\n  The fourth cell is the tempting one and this program refuses it: the firm's\n"
              "  choice failed, but the resident's alternative was never executed, so it says\n"
              "  nothing about whether the resident would have done better. It is a reason to\n"
              "  instrument, not a reason to license.\n");
  long agw = 0; for (int c = 0; c < R.w.NC; ++c) agw += A.RP.agree_wrong[c];
  std::printf("\n  agreement-band mass %.1f%%.  Coverage is NOT read off agreement: two readers with\n"
              "  the same blind spot agree across it. It is read off the world: where both chose the\n"
              "  same action and the world said WRONG (%ld of %ld agreed), both missed the determinant\n"
              "  that decided the case, and only an unrecorded one can be missed by both.\n",
              100.0 * ag / std::max(1L, n), agw, ag);
  double ce = 0, ct = 0; for (int c = 0; c < R.w.NC; ++c) { ce += A.C.coverage[c]; ct += 1.f - R.w.tacit[c]; }
  std::printf("  estimated mean coverage %.3f against a planted truth of %.3f (error %+.3f).\n",
              ce / R.w.NC, ct / R.w.NC, (ce - ct) / R.w.NC);
  std::printf("  %ld class-bands licensed to rung 1 by history alone, in the time that replay took.\n", A.hist_licensed);

  rule("PHASE 4 · THE LADDER — CLIMBING ON ARRIVALS ONLY");
  std::printf("  n_act is the WAGER (unattended acts, the only outcomes that widen a licence);\n"
              "  n_asst is drafts a person keyed and warrants a signer executed, graded but never fed.\n"
              "  good_h is the firm's own rate in the SAME band, which the e-process is paired against.\n\n");
  std::printf("  %-22s %4s  %5s %6s %6s %5s   %7s %7s %7s %6s  %s\n",
              "class", "band", "rung", "n_act", "n_asst", "n0", "good_a", "good_s", "good_h", "kappa", "state");
  int licensed_bands = 0, unlicensable = 0;
  for (int c = 0; c < R.w.NC; ++c) for (int b = NBAND - 1; b >= 1; --b) {
    const Lic& L = A.lad.at(c, b);
    if (L.rung > 0) ++licensed_bands;
    if (L.unlicensable) ++unlicensable;
    if (L.n_machine + L.n_assisted < 30 && L.rung == 0) continue;
    if ((c % 5) || b != 2) continue;
    std::printf("  %-22s %4d  %5d %6ld %6ld %5d   %7.3f %7.3f %7.3f %6.2f  %s\n", cls_spec(c).name, b, L.rung,
                L.n_machine, L.n_assisted, L.n0,
                L.n_machine ? (double)L.good_machine / L.n_machine : 0.0,
                L.n_assisted ? (double)L.good_assisted / L.n_assisted : 0.0,
                L.n_incumbent ? (double)L.good_incumbent / L.n_incumbent : 0.0,
                L.kappa(), L.history_licensed ? "history-admitted" : "wager only");
  }
  std::printf("\n  %d class-bands past rung 0.  %d bands are UNLICENSABLE at any tolerable canary\n"
              "  rate: n0 is not a constant, it is n0(p, delta), so the rarest-failure classes\n"
              "  are the SLOWEST to license. The honest account prints them as human rather\n"
              "  than holding them in shadow forever.\n", licensed_bands, unlicensable);
  print_binding_reasons(A.lad, R.f.writ, (uint32_t)(a.days - 1));

  rule("PHASE 5 · WHAT THE RESIDENT DID");
  std::printf("  acted unattended %llu   drafted for a person %llu   rented a frontier mind %llu\n"
              "  sent to a warrant seat %llu   held (a row, with its margin) %llu\n",
              (unsigned long long)A.ms.acted, (unsigned long long)A.ms.drafted,
              (unsigned long long)A.ms.frontier, (unsigned long long)A.ms.warrant, (unsigned long long)A.ms.held);
  std::printf("  canary instances %llu   audit sample %llu   effects undone %llu\n",
              (unsigned long long)A.ms.canary, (unsigned long long)A.ms.audit, (unsigned long long)A.ms.undone);
  std::printf("\n  the gate's refusal reasons, which are the only vocabulary it has:\n");
  for (int r = 0; r < RS_N; ++r) if (A.ms.reason_count[r])
    std::printf("    %-24s %10llu\n", reason_name(r), (unsigned long long)A.ms.reason_count[r]);
  std::printf("\n  KAPPA — supervision created over supervision removed: %.3f\n", A.ms.kappa());
  std::printf("  (%.0fk minutes of human review caused, %.0fk minutes of human work removed)\n",
              A.ms.sup_created_min / 1000.0, A.ms.sup_removed_min / 1000.0);
  std::printf("  A class whose licence costs more attention than it saves is demoted the same\n"
              "  day, regardless of how good the model looks. That is the only meter in this\n"
              "  program that can fail while every other number improves.\n");
  print_minutes_by_via(minutes_by_via(R.tape, R.L), "the resident arm, warm period included");

  rule("PHASE 6 · THE CASCADE — THE MIDDLE LEAVES BY ARITHMETIC");
  const Cascade cs = fit_cascade(a.span, a.seed);
  std::printf("  fitted from %d points of the firm's own payroll against its order book:\n", cs.n_points);
  std::printf("    with an intercept:   alpha = %.3f, F = %.1f      (planted: alpha = %.3f, F = %.1f)\n",
              cs.alpha_hat, cs.F_hat, cs.alpha_true, cs.F_true);
  std::printf("    through the origin:  alpha = %.3f, F = 0        <- the usual ratio analysis\n", cs.alpha_naive);
  const double E_now = R.f.count_fn(FN_E) + R.f.count_fn(FN_WARRANT);
  const double licensed_frac = A.res.total > 0 ? A.res.licensed / A.res.total : 0.0;
  const double E_new = E_now * (1.0 - licensed_frac);
  std::printf("\n  licensed decision mass %.1f%%  ->  E falls from %.0f to %.0f\n", 100.0 * licensed_frac, E_now, E_new);
  std::printf("    N with the honest fit  = %.0f\n", cs.predict(E_new));
  std::printf("    N with the naive fit   = %.0f   <- promises a company physics does not allow\n", cs.predict_naive(E_new));
  std::printf("    the gap, %.0f seats, is F: the floor a within-firm regression cannot see,\n"
              "    because the firm has never been small. You get F from cross-firm data at the\n"
              "    target size, never from the payroll you have.\n",
              cs.predict(E_new) - cs.predict_naive(E_new));
  std::printf("\n  and the order matters: the alpha-functions leave AFTER the E-work is licensed,\n"
              "  not before, because deleting them first breaks the people still doing the E-work.\n");

  rule("PHASE 7 · THE RESIDUAL — AND ONLY TWO COLUMNS MOVE");
  const double T = std::max(1e-9, A.res.total);
  std::printf("  licensed        %6.1f%%   under a rung, with an expiry, re-earning its place\n", 100 * A.res.licensed / T);
  std::printf("  warrant         %6.1f%%   a signature, liability, someone who can be sued. Does not move.\n", 100 * A.res.warrant / T);
  std::printf("  counterparty    %6.1f%%   the other side demands a person. Does not move.\n", 100 * A.res.counterparty / T);
  std::printf("  frame           %6.1f%%   uninstrumented determinants and classes that are two classes.\n", 100 * A.res.frame / T);
  std::printf("     of which     %6.1f%%   wait: a licensable band still below rung 2. Moves with time.\n", 100 * A.res.frame_wait / T);
  std::printf("                  %6.1f%%   coverage: the uninstrumented tail of licensed classes. Moves with lane-buys.\n", 100 * A.res.frame_cov / T);
  std::printf("  thin tape       %6.1f%%   the world answers too slowly to ever license. Moves at the\n"
              "                          world's pace, not the model's.\n", 100 * A.res.thin_tape / T);
  std::printf("\n  Two of the four do not move when models improve. That is the honest answer to\n"
              "  \"why not a hundred percent\", and it is a legal and instrumentation fact rather\n"
              "  than a capability one.\n");

  rule("THE COUNTER ON THE WALL");
  long mach_out = 0, asst_out = 0, need = 0;
  for (int c = 0; c < R.w.NC; ++c) for (int b = 0; b < NBAND; ++b) { mach_out += A.lad.at(c,b).n_machine; asst_out += A.lad.at(c,b).n_assisted; need += A.lad.at(c,b).n0; }
  std::printf("  outcomes of unattended acts received, against n0:  %ld / %ld   (the wager)\n", mach_out, need);
  std::printf("  outcomes of drafts and warrants a person executed:  %ld   (assisted; graded, never fed)\n", asst_out);
  std::printf("  Everything else in this program is upstream of that number or downstream of it.\n");
  std::printf("\n  tape %zu rows, chain %s.  resident period %.2f ms over %d periods.\n",
              R.tape.size(), R.tape.verify() < 0 ? "VERIFIED" : "BROKEN", A.period_ms, A.periods);
  print_row_histogram(R.tape);
  return 0;
}

// ----------------------------------------------------------------------------
// THE TWIN — the exact counterfactual, which no real deployment can obtain
// ----------------------------------------------------------------------------
static int cmd_twin(const Args& a) {
  rule("THE TWIN · TWO ACMES, ONE SEED, ONE PHYSICS");
  std::printf("  Same arrivals, same determinants, same luck on every obligation id. The only\n"
              "  difference is who decides. A real firm cannot run this comparison — its canary\n"
              "  and its control share one ledger, and the humans see what the machine did and\n"
              "  best-respond to it. A twin can, and the most useful thing it prints is not\n"
              "  which arm won. It is HOW WRONG THE CANARY ESTIMATE WOULD HAVE BEEN.\n");

  Run A; A.f = build_acme(a.n, a.span, a.seed); A.w = make_world(a.seed); A.w.demand_scale = a.demand;
  Run B; B.f = build_acme(a.n, a.span, a.seed); B.w = make_world(a.seed); B.w.demand_scale = a.demand;

  const double t0 = now_s();
  run_human(A, a.days, false);
  const AutoOut mb = run_machine(a, B, a.warm, a.days, false);
  const double dt = now_s() - t0;

  const ArmResult ra = score_arm(A.L, A.f);
  const ArmResult rb = mb.arm;

  std::printf("\n  %-30s %14s %14s %12s\n", "", "incumbent", "resident", "delta");
  auto row = [&](const char* n, double x, double y, const char* fmt) {
    char bx[64], by[64], bd[64];
    snprintf(bx, 64, fmt, x); snprintf(by, 64, fmt, y); snprintf(bd, 64, fmt, y - x);
    std::printf("  %-30s %14s %14s %12s\n", n, bx, by, bd);
  };
  row("obligations settled",     (double)ra.settled, (double)rb.settled, "%.0f");
  row("good outcomes  %",        100.0 * ra.good / std::max(1L, ra.settled), 100.0 * rb.good / std::max(1L, rb.settled), "%.2f");
  row("wrong outcomes %",        100.0 * ra.bad / std::max(1L, ra.settled),  100.0 * rb.bad / std::max(1L, rb.settled),  "%.2f");
  row("late outcomes  %",        100.0 * ra.late / std::max(1L, ra.settled), 100.0 * rb.late / std::max(1L, rb.settled), "%.2f");
  row("deadline breaches %",     100.0 * ra.breached / std::max(1L, ra.settled), 100.0 * rb.breached / std::max(1L, rb.settled), "%.2f");
  row("mean cycle, days",        ra.mean_cycle(), rb.mean_cycle(), "%.2f");
  row("writ cost of outcomes, $M", ra.writ_cost / 1e6, rb.writ_cost / 1e6, "%.2f");
  row("backlog at w_unplaced, $M", ra.backlog_cost / 1e6, rb.backlog_cost / 1e6, "%.2f");
  row("TOTAL cost, $M",          ra.total_cost() / 1e6, rb.total_cost() / 1e6, "%.2f");
  row("human minutes, thousands", A.hs.time.total() / 1000.0, B.hs.time.total() / 1000.0, "%.0f");
  row("still open at the end",   (double)A.L.open_idx.size(), (double)B.L.open_idx.size(), "%.0f");
  // §9.3: what bought the queue. The same fold on both arms' tapes.
  print_minutes_by_via(minutes_by_via(A.tape, A.L), "the incumbent arm");
  print_minutes_by_via(minutes_by_via(B.tape, B.L), "the resident arm");

  // THE BIAS OF THE CANARY ESTIMATOR
  CanaryBias cb;
  cb.true_effect = (100.0 * rb.good / std::max(1L, rb.settled)) - (100.0 * ra.good / std::max(1L, ra.settled));
  long mg = 0, mn = 0, hg = 0, hn = 0, ag = 0, an = 0;
  for (const Obligation& o : B.L.ob) {
    if (o.state != OB_SETTLED) continue;
    if (o.via == 1 || o.via == 3)      { ++mn; if (o.outcome == OK_GOOD) ++mg; }   // the wager: unattended acts
    else if (o.via == 2 || o.via == 4) { ++an; if (o.outcome == OK_GOOD) ++ag; }   // assisted: a person executed it
    else                               { ++hn; if (o.outcome == OK_GOOD) ++hg; }
  }
  cb.canary_estimate = (mn && hn) ? 100.0 * ((double)mg / mn - (double)hg / hn) : 0.0;
  rule("THE NUMBER A REAL PROGRAMME CANNOT GET");
  std::printf("  true effect, paired arms          %+7.2f points of good-outcome rate (total cost delta $%+.1fM)\n",
              cb.true_effect, (rb.total_cost() - ra.total_cost()) / 1e6);
  std::printf("  within-firm canary estimate       %+7.2f  (unattended acts %ld vs human-decided %ld in the same firm)\n", cb.canary_estimate, mn, hn);
  std::printf("  assisted, not in the estimate     %ld drafts and warrants a person executed, %.1f%% good\n", an, an ? 100.0 * ag / an : 0.0);
  std::printf("  BIAS OF THE ESTIMATOR             %+7.2f\n", cb.bias());
  std::printf("\n  The canary is not measuring the same thing as the twin, and the gap is not\n"
              "  noise. Two mechanisms drive it and both grow with the canary rate: the machine\n"
              "  is routed the cases it was licensed for, which is selection; and the humans in\n"
              "  the same firm are working a queue the machine already drained, which is\n"
              "  interference. So r has an interior optimum, not just the floor n0/V that the\n"
              "  identifiability arithmetic gives. Raise r to clear n0 faster and you contaminate\n"
              "  the comparison; lower it to protect the comparison and you never clear n0.\n");
  std::printf("\n  Budget for a bias of about this size in any real programme. It is the term the\n"
              "  three single-way documents all left out, and a simulation is the only place it\n"
              "  can be measured at all.\n");
  std::printf("\n  both arms in %.1f s.\n", dt);
  return 0;
}

// ----------------------------------------------------------------------------
// THE MULTIVERSE — many ACMEs, writ-reweighted. The card's real job.
// ----------------------------------------------------------------------------
struct Patch { const char* name; int span; float thin; float eps; float kappa_max; int instrument_cls; };

static double eval_patch(const Args& a, const Patch& p, uint64_t seed, int days) {
  Run R; R.f = build_acme(a.n, p.span, seed); R.w = make_world(seed); R.w.demand_scale = a.demand;
  // v2: every patch lands as a PATCH row carrying its inverse (margin = old, value = new)
  // before the run, so a fold of the fork's tape knows which universe it is.
  R.tape.header(MODE_SYNTHETIC, SW_LIVE, alphabet_hash());
  const Writ base;
  auto patch_row = [&](int kind, int param, float oldv, float newv) {
    if (oldv != newv) R.tape.put(R_PATCH, 0, 0, param < 0 ? 0 : param, -3, ARM_GOVERNOR, kind, param, oldv, newv);
  };
  patch_row(0, 0, (float)a.span, (float)p.span);
  patch_row(2, 0, base.thin_margin, p.thin);
  patch_row(2, 1, base.eps_floor, p.eps);
  patch_row(2, 2, base.kappa_max, p.kappa_max);
  R.f.writ.thin_margin = p.thin; R.f.writ.eps_floor = p.eps; R.f.writ.kappa_max = p.kappa_max;
  if (p.instrument_cls >= 0 && p.instrument_cls < R.w.NC) {
    // BUY THE LANE: move this class's tacit determinants into a system. This is
    // the only lever that raises coverage, and it costs money rather than model
    // quality — which is the whole point of measuring coverage separately.
    TrueSpec& t = R.w.spec[p.instrument_cls];
    patch_row(1, p.instrument_cls, t.tacit_mass, 0.f);
    for (Determinant& d : t.det) if (d.where == DW_TACIT) { d.where = DW_SYSTEM; d.system = 0; }
    t.tacit_mass = 0.f; R.w.tacit[p.instrument_cls] = 0.f;
  }
  Args aa = a; aa.days = days; aa.warm = days / 3;
  const AutoOut o = run_machine(aa, R, aa.warm, days, false);
  const double salary = R.f.payroll() * (days / 365.0);
  return o.arm.total_cost() + salary + 60.0 * o.ms.frontier_calls;   // the writ's own cost function, backlog included
}

static int cmd_multiverse(const Args& a) {
  rule("THE MULTIVERSE · MANY ACMES AT ONCE");
  std::printf("  Best-of-N continuations under EDITED CONSTRAINTS, sharing one prefix and one\n"
              "  physics, reweighted by the writ. This is the search over STRUCTURE, and it is\n"
              "  derivative-free on purpose: an org chart has no gradient. On a card each\n"
              "  universe is one block; here they are serial and it still fits in a coffee break.\n\n");
  std::vector<Patch> P = {
    { "BASELINE",                       a.span, 0.55f, 0.03f, 1.00f, -1 },
    { "span 5 (deeper cone)",                5, 0.55f, 0.03f, 1.00f, -1 },
    { "span 10 (flatter cone)",             10, 0.55f, 0.03f, 1.00f, -1 },
    { "thin margin 0.30 (act sooner)",  a.span, 0.30f, 0.03f, 1.00f, -1 },
    { "thin margin 0.90 (act later)",   a.span, 0.90f, 0.03f, 1.00f, -1 },
    { "eps floor 0.10 (explore more)",  a.span, 0.55f, 0.10f, 1.00f, -1 },
    { "kappa cap 0.60 (stricter brake)",a.span, 0.55f, 0.03f, 0.60f, -1 },
    { "BUY THE LANE on claim.coverage", a.span, 0.55f, 0.03f, 1.00f,  8 },
    { "BUY THE LANE on dispute.resolve",a.span, 0.55f, 0.03f, 1.00f,  6 },
    { "BUY THE LANE on fraud.refer",    a.span, 0.55f, 0.03f, 1.00f, 11 },
  };
  const int R = std::max(2, a.futures / (int)P.size());
  const int days = 200;
  std::vector<double> mean(P.size(), 0), sd(P.size(), 0);
  const double t0 = now_s();
  for (size_t i = 0; i < P.size(); ++i) {
    Acc acc;
    for (int r = 0; r < R; ++r) acc.add(eval_patch(a, P[i], a.seed ^ (0x9E3779B9ULL * (r + 1)), days) / 1e6);
    mean[i] = acc.mean(); sd[i] = acc.sd();
  }
  const double dt = now_s() - t0;
  // MPPI weights with ensemble pessimism: exp(-(J + beta*sigma - min)/lambda)
  const double beta = 0.5, lambda = 2.0;
  double mn = 1e30; for (size_t i = 0; i < P.size(); ++i) mn = std::min(mn, mean[i] + beta * sd[i]);
  double z = 0; std::vector<double> wt(P.size());
  for (size_t i = 0; i < P.size(); ++i) { wt[i] = std::exp(-((mean[i] + beta * sd[i]) - mn) / lambda); z += wt[i]; }
  for (double& x : wt) x /= z;
  std::vector<int> ord(P.size()); for (size_t i = 0; i < P.size(); ++i) ord[i] = (int)i;
  std::sort(ord.begin(), ord.end(), [&](int x, int y) { return mean[x] + beta * sd[x] < mean[y] + beta * sd[y]; });

  std::printf("  %4s %10s %8s %10s %8s  %s\n", "rank", "J mean $M", "sd", "J pess", "weight", "patch");
  for (size_t r = 0; r < ord.size(); ++r) {
    const int i = ord[r];
    std::printf("  %4zu %10.2f %8.2f %10.2f %8.4f  %s%s\n", r, mean[i], sd[i], mean[i] + beta * sd[i], wt[i],
                P[i].name, strcmp(P[i].name, "BASELINE") ? "" : "   <- do nothing");
  }
  std::printf("\n  %zu universes x %d rollouts x %d days in %.1f s.\n", P.size(), R, days, dt);
  std::printf("  Every one of these is a structural patch with an inverse. A patch is DEPLOYED\n"
              "  with its forecast attached, and if the arrived quarter misses the forecast it\n"
              "  unwinds. A search result is not a licence; it is a proposal with a receipt.\n");
  return 0;
}

// ----------------------------------------------------------------------------
// THE ORACLES — each carries a lie. An oracle that passes its own lie is broken.
// ----------------------------------------------------------------------------
static int g_pass = 0, g_fail = 0;
static void ck(bool ok, const char* name, const char* detail = "") {
  std::printf("  [%s] %-52s %s\n", ok ? "PASS" : "FAIL", name, detail); (ok ? g_pass : g_fail)++;
}

static int cmd_selftest(const Args& a) {
  rule("THE ORACLE BATTERY");
  char buf[300];
  const int LIE = a.lie;
  if (LIE >= 0) std::printf("  --lie %d : oracle %d is running against a deliberately corrupted mechanism.\n"
                            "            Its line prints PASS only if it CAUGHT the corruption; a FAIL there is a\n"
                            "            broken oracle. A healthy lie run therefore exits 0, like the plain battery.\n\n", LIE, LIE);

  // --- O0: BLAKE2b against the published vector, then the tape chain
  {
    uint8_t d[32]; Blake2b::digest("abc", 3, d);
    ck(Blake2b::hex(d, 32) == "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319",
       "O0a  BLAKE2b-256(\"abc\") matches RFC 7693");
    Tape t;
    for (int i = 0; i < 500; ++i) t.put(R_DECIDE, i, i, i % 24, i % 50, 0, 1, 0, 0.5f, 100.f);
    ck(t.verify() < 0, "O0b  tape chain verifies over 500 rows");
    if (LIE == 0) t.rec[271].margin = 0.51f;                     // THE LIE
    const long bad = t.verify();
    snprintf(buf, sizeof buf, "(localised at row %ld)", bad);
    ck(LIE == 0 ? (bad == 271) : (bad < 0), "O0c  a single flipped field localises to its row", buf);
  }

  // --- O1: determinism. Same seed, byte-identical tape.
  {
    Run A; A.f = build_acme(200, 7, 7); A.w = make_world(7); A.w.demand_scale = 1.5f;
    Run B; B.f = build_acme(200, 7, 7); B.w = make_world(7); B.w.demand_scale = 1.5f;
    run_human(A, 40); run_human(B, 40);
    if (LIE == 1) B.tape.rec[10].day ^= 1u;                      // THE LIE
    const bool same = A.tape.size() == B.tape.size()
                   && memcmp(A.tape.rec.data(), B.tape.rec.data(), A.tape.size() * sizeof(Rec)) == 0;
    snprintf(buf, sizeof buf, "(%zu rows, memcmp)", A.tape.size());
    ck(LIE == 1 ? !same : same, "O1   same seed produces a byte-identical tape", buf);
  }

  // --- O1b: determinism over the MACHINE arm. O1 ran the human arm only and
  // compared rec without chain (F20, the REV 2 spec S0): the resident had never
  // been under a determinism oracle, and every fold-to-identity oracle that
  // follows needs this baseline. Two runs, one seed: the tape, the chain and
  // the licence table must all be byte-identical.
  {
    Args aa = a; aa.n = 200; aa.span = 7; aa.seed = 7; aa.warm = 40; aa.days = 70; aa.demand = 1.5f; aa.lie = -1;
    Run A; A.f = build_acme(200, 7, 7); A.w = make_world(7); A.w.demand_scale = 1.5f;
    Run B; B.f = build_acme(200, 7, 7); B.w = make_world(7); B.w.demand_scale = 1.5f;
    const AutoOut oa = run_machine(aa, A, aa.warm, aa.days, false);
    const AutoOut ob = run_machine(aa, B, aa.warm, aa.days, false);
    if (LIE == 14) {                                               // THE LIE: a resident that read the clock
      const long long t = (long long)std::chrono::steady_clock::now().time_since_epoch().count();
      for (Rec& r : B.tape.rec) if (r.type == R_EFFECT) { r.margin += 1e-6f * (float)(1 + (t % 7)); break; }
    }
    const bool same_rec   = A.tape.size() == B.tape.size()
                         && memcmp(A.tape.rec.data(), B.tape.rec.data(), A.tape.size() * sizeof(Rec)) == 0;
    const bool same_chain = A.tape.chain.size() == B.tape.chain.size()
                         && memcmp(A.tape.chain.data(), B.tape.chain.data(), A.tape.chain.size()) == 0;
    bool same_lad = oa.lad.lic.size() == ob.lad.lic.size();
    for (size_t i = 0; same_lad && i < oa.lad.lic.size(); ++i) {
      const Lic& x = oa.lad.lic[i]; const Lic& y = ob.lad.lic[i];
      same_lad = x.rung == y.rung && x.expiry_day == y.expiry_day && x.logE == y.logE && x.logE_demote == y.logE_demote
              && x.n_machine == y.n_machine && x.n_incumbent == y.n_incumbent && x.n_assisted == y.n_assisted
              && x.good_machine == y.good_machine && x.good_incumbent == y.good_incumbent && x.good_assisted == y.good_assisted
              && x.history_licensed == y.history_licensed && x.unlicensable == y.unlicensable && x.n0 == y.n0;
    }
    const bool same = same_rec && same_chain && same_lad;
    snprintf(buf, sizeof buf, "(%zu rows; rec %s, chain %s, licence table %s)", A.tape.size(),
             same_rec ? "same" : "DIFF", same_chain ? "same" : "DIFF", same_lad ? "same" : "DIFF");
    ck(LIE == 14 ? !same : same, "O1b  same seed produces a byte-identical MACHINE arm: tape, chain, licence table", buf);

    // --- O14: FOLD-TO-IDENTITY. The cold fold of A's tape must equal A's live
    //          world field by field, the open set included, and the folded minute
    //          meter must equal the live one. The lie is a struct field set with
    //          no row: exactly the defect the v1 program had everywhere.
    {
      if (LIE == 15) A.L.ob[A.L.ob.size() / 2].hops += 1;                  // THE LIE
      const Ledger L = Ledger::fold(A.tape, A.w.NC);
      const FoldDiff fd_ = ledger_diff(L, A.L);
      const long mins = minutes_diff(L, A.hs, A.w.NC);
      const bool ok = fd_.fields == 0 && mins == 0 && L.schema == alphabet_hash() && L.ver == REC_VER;
      snprintf(buf, sizeof buf, "(%zu cells folded from %zu rows; %ld field diffs%s%s; %ld minute diffs; alphabet pin %s)",
               L.ob.size(), A.tape.size(), fd_.fields, fd_.fields ? ", first: " : "", fd_.fields ? fd_.first : "",
               mins, L.schema == alphabet_hash() ? "matches" : "DIFFERS");
      ck(LIE == 15 ? !ok : ok, "O14  the ledger folds from the tape: cold fold == live world (a verified shadow; no consumer reads it yet)", buf);
    }
    // --- O29: ROW-SHAPE CONFORMANCE. One scan of the tape; per type, the fields
    //          the v2 table says are populated must be populated. This is the
    //          oracle that catches table drift for the rest of the programme.
    //          The lie is an ARRIVE written with no due day.
    {
      long bad = 0, n = 0; char first[120] = {0}; long headers = 0;
      auto shape_fail = [&](const Rec& r, const char* why) { if (bad++ == 0) snprintf(first, sizeof first, "%s on %s at row %ld", why, rec_type_name(r.type), n); };
      for (const Rec& r0 : A.tape.rec) {
        Rec r = r0;
        if (LIE == 17 && r.type == R_ARRIVE && n == 5) r.a = 0;                  // THE LIE
        if (r.ver != REC_VER) shape_fail(r, "ver");
        switch (r.type) {
          case R_HEADER:   ++headers; if (n != 0) shape_fail(r, "HEADER not first"); if ((uint32_t)r.b != alphabet_hash()) shape_fail(r, "pin"); break;
          case R_TICK:     if (r.seat != -3 || r.arm != ARM_GOVERNOR) shape_fail(r, "writer"); break;
          case R_ARRIVE:   if (r.a <= 0 || (uint32_t)r.a < r.day) shape_fail(r, "day_due"); if (r.seat != -2) shape_fail(r, "seat"); break;
          case R_ASSIGN:   if (r.seat < 0 || r.b <= 0) shape_fail(r, "seat/hops"); break;
          case R_CONTEXT:  if (r.a < 0 || r.a > 31) shape_fail(r, "system"); break;
          case R_ACT:      if (r.a < 0 || r.a >= ACT_N || r.value < 0.f) shape_fail(r, "kind/minutes"); break;
          case R_DECIDE:   if (r.seat < 0 || r.arm != ARM_HUMAN || r.prov != PROV_H || r.via != 0) shape_fail(r, "decider"); break;
          case R_PROPOSAL: if (r.seat != -1 || r.arm != ARM_MACHINE || r.prov != PROV_M || r.b == 0) shape_fail(r, "judge hash"); break;
          case R_HOLD:     if ((r.seat >= 0 && r.prov != PROV_H) || (r.seat == -1 && r.prov != PROV_M)) shape_fail(r, "prov"); break;
          case R_ESCALATE: if ((r.seat >= 0 && r.prov != PROV_H) || (r.seat == -1 && r.prov != PROV_M)) shape_fail(r, "prov"); break;
          case R_EFFECT:   if (r.seat != -1 || r.arm != ARM_MACHINE || r.prov != PROV_M || r.via < 1 || r.via > 4) shape_fail(r, "via"); break;
          case R_OUTCOME:  if (r.seat == -2 || r.a < OK_GOOD || r.a > OK_BAD || r.b != r.via || (r.arm == ARM_MACHINE) != (r.seat == -1)) shape_fail(r, "decider/kind/via"); break;
          case R_LICENSE: case R_KAPPA: if (r.seat != -3 || r.arm != ARM_GOVERNOR) shape_fail(r, "governor"); break;
          default: break;
        }
        ++n;
      }
      if (headers != 1) shape_fail(A.tape.rec[0], "one HEADER");
      const bool ok = (bad == 0);
      snprintf(buf, sizeof buf, "(%ld rows scanned, %ld shape faults%s%s)", n, bad, bad ? ", first: " : "", bad ? first : "");
      ck(LIE == 17 ? !ok : ok, "O29  every row carries what the v2 table says it carries", buf);
    }
    // --- O15: THE LADDER IS A FOLD. Every licence's counts rebuilt from OUTCOME
    //          rows alone, from the resident's first period, must equal the live
    //          ladder's. The lie is an outcome counted twice.
    {
      Ladder LF = fold_ladder(A.tape, A.w.NC, (uint32_t)aa.warm);
      if (LIE == 16) LF.at(0, 2).n_machine += 1;                            // THE LIE
      const long bad = ladder_diff(LF, oa.lad);
      snprintf(buf, sizeof buf, "(%zu class-bands; %ld count diffs against the live ladder)", LF.lic.size(), bad);
      ck(LIE == 16 ? !(bad == 0) : (bad == 0), "O15  the ladder is a fold of OUTCOME rows: counts rebuilt == live", buf);
    }
  }

  // --- O2: the gate can never widen. Exhaustive over the input lattice.
  {
    Writ wr; bool ok = true; long acts = 0, cases = 0;
    for (int rung = 0; rung <= 5; ++rung)
     for (int rev = 0; rev < 2; ++rev)
      for (int war = 0; war < 2; ++war)
       for (int blk = 0; blk < 2; ++blk)
        for (int can = 0; can < 2; ++can)
         for (int aud = 0; aud < 2; ++aud)
          for (int di = 0; di < 9; ++di)
           for (int nv = 0; nv < 5; ++nv) {
             GateIn g{}; g.rung = rung; g.reversible = rev; g.warrant_reserved = war; g.blocked = blk;
             g.in_canary = can; g.in_audit = aud; g.direction = -2.f + 0.5f * di; g.novelty = 0.2f * nv;
             g.sharpness = 0.f; g.budget_left = 1000.f;
             const GateOut v = gate(g, wr); ++cases;
             if (v.verdict == V_ACT) { ++acts;
               if (rung <= 0 || war || blk) ok = false;           // an unlicensed act is a widening
               if (!rev && rung < 4) ok = false;
               if (std::fabs(g.direction) < wr.thin_margin) ok = false;
             }
           }
    if (LIE == 2) ok = !ok;                                       // THE LIE
    snprintf(buf, sizeof buf, "(%ld lattice points, %ld reached ACT)", cases, acts);
    ck(LIE == 2 ? !ok : ok, "O2   the gate never authorises outside its licence", buf);
  }

  // --- O3: running out of adjudication budget produces a HOLD, never an ACT
  {
    Writ wr; bool ok = true;
    for (int rung = 1; rung <= 2; ++rung) {
      GateIn g{}; g.rung = rung; g.reversible = true; g.direction = 3.f; g.novelty = 0.1f;
      g.in_canary = true; g.budget_left = 0.f;
      const GateOut v = gate(g, wr);
      if (v.verdict == V_ACT) ok = false;
      if (v.verdict != V_HOLD || v.reason != RS_NO_BUDGET) ok = false;
    }
    if (LIE == 3) { GateIn g{}; g.rung = 9; g.reversible = true; g.direction = 3.f; g.in_canary = true;
                    g.budget_left = 0.f; ok = (gate(g, wr).verdict == V_HOLD); }
    ck(LIE == 3 ? !ok : ok, "O3   out of supervision degrades to HOLD, never to acting");
  }

  // --- O4: Sinkhorn conservation. Rows ship their supply, columns never exceed
  //         capacity, and the unplaced stock absorbs exactly the remainder.
  {
    Transport tr; const int N = 220, S = 14;
    tr.size_to(N, S, 400, 400);
    for (int j = 0; j < S; ++j) tr.cap[j] = 3.f;
    auto cost = [&](int i, int j) -> float {
      if (j >= S) return (j - S == STOCK_UNPLACED) ? 1.2f : 1.0f;
      return 0.2f + 0.9f * u01(99, 400 + j, i);
    };
    tr.run(cost, 60);
    double over = 0, placed = 0;
    for (int j = 0; j < S; ++j) { over = std::max(over, (double)((tr.colsum[j] - tr.cap[j]) / tr.cap[j])); placed += tr.colsum[j]; }
    const double stock = tr.colsum[S + STOCK_UNPLACED] + tr.colsum[S + STOCK_UNADJ];
    const double err = std::fabs(placed + stock - N) / N;
    bool ok = (over < 0.02) && (err < 0.02);
    if (LIE == 4) { tr.cap[0] = 0.001f; ok = (tr.colsum[0] <= tr.cap[0] * 1.02f); }   // THE LIE: stale colsum
    snprintf(buf, sizeof buf, "(max column overshoot %.2e, placed+stock err %.2e)", over, err);
    ck(LIE == 4 ? !ok : ok, "O4   transport conserves: rows ship, columns never exceed", buf);
  }

  // --- O5: the stock columns carry a FINITE price. The T17 fix: with an
  //         infinite cap the dual is always exactly zero, a theorem not a bug.
  {
    Transport tr; const int N = 200, S = 8;
    tr.size_to(N, S, 30, 30);
    for (int j = 0; j < S; ++j) tr.cap[j] = 1.5f;                 // deliberately starved
    auto cost = [&](int i, int j) -> float { return j >= S ? 1.1f : 0.3f + 0.5f * u01(5, 500 + j, i); };
    tr.run(cost, 80);
    float maxp = 0; for (int j = 0; j < S; ++j) maxp = std::max(maxp, (float)tr.price(j));
    const float stockp = tr.price(S + STOCK_UNPLACED);
    bool ok = (maxp > 0.01f) && (stockp > 0.001f);
    if (LIE == 5) { tr.cap[S + STOCK_UNPLACED] = 1e30f; tr.v[S + STOCK_UNPLACED] = 0.f;
                    ok = (tr.price(S + STOCK_UNPLACED) > 0.001f); }   // THE LIE
    snprintf(buf, sizeof buf, "(binding seat price %.4f, unplaced-stock price %.4f)", maxp, stockp);
    ck(LIE == 5 ? !ok : ok, "O5   a binding capacity has a nonzero price, stocks included", buf);
  }

  // --- O6: the compile step recovers the join graph from the query log alone
  {
    Run R; R.f = build_acme(300, 7, 11); R.w = make_world(11); R.w.demand_scale = 1.5f;
    run_human(R, 80);
    Compiled C = compile_from_tape(R.tape, R.w.NC, 11, true);
    if (LIE == 6) C.join_graph[3] = 1u;                            // THE LIE
    // The oracle reads the join graph itself, not the count cached beside it:
    // the first run planted the lie in join_graph while the check read
    // n_systems, which compile_from_tape had filled before the lie landed, so
    // the lie was invisible and O6 passed its own corruption.
    int exact = 0, seen = 0;
    for (int c = 0; c < R.w.NC; ++c) { if (!C.join_graph[c]) continue; ++seen;
      if (__builtin_popcount(C.join_graph[c]) == cls_spec(c).n_systems) ++exact; }
    const bool ok = (seen > 20) && (exact == seen);
    snprintf(buf, sizeof buf, "(%d of %d classes, exactly)", exact, seen);
    ck(LIE == 6 ? !ok : ok, "O6   the join graph is read off the application, not asked for", buf);
  }

  // --- O7: coverage. The agreement-band estimate must track the planted tacit
  //         mass, because that is the only ceiling on automation that exists.
  {
    Run R; R.f = build_acme(300, 7, 13); R.w = make_world(13); R.w.demand_scale = 1.5f;
    run_human(R, 150);
    Compiled C = compile_from_tape(R.tape, R.w.NC, 13, true);
    Field fd; fd.init(R.w.NC, R.f.size(), 13); Ladder lad; lad.init(R.w.NC);
    // THE LIE: a world that never says wrong. The estimator reads coverage off
    // the world's verdicts on the agreement diagonal, so a grader that always
    // says GOOD must leave it blind, and the oracle must see that it is blind.
    // (The first lie moved one determinant to tacit after the outcomes had
    // already settled; it did not reach the mechanism and O7 passed it.)
    if (LIE == 7) for (Obligation& o : R.L.ob) if (o.state == OB_SETTLED) o.outcome = OK_GOOD;
    PlantStore store(&R.w); PlantJudge judge(&R.w);
    ReplayOut RP = replay(R.L, R.f, fd, C, lad, R.f.writ, store, judge);
    double num = 0, den = 0, sx = 0, sy = 0, sxy = 0, sxx = 0, syy = 0; int k = 0;
    for (int c = 0; c < R.w.NC; ++c) {
      if (RP.n[c] < 60) continue;
      const double x = 1.0 - R.w.tacit[c], y = RP.coverage[c];
      num += std::fabs(x - y); den += 1; sx += x; sy += y; sxy += x * y; sxx += x * x; syy += y * y; ++k;
    }
    const double mae = den ? num / den : 1.0;
    const double r = (k * sxy - sx * sy) / std::max(1e-9, std::sqrt((k * sxx - sx * sx) * (k * syy - sy * sy)));
    const bool ok = (mae < 0.12) && (r > 0.45);
    snprintf(buf, sizeof buf, "(mean |err| %.3f, corr %.3f over %d classes)", mae, r, k);
    ck(LIE == 7 ? !ok : ok, "O7   coverage read off the world's verdicts tracks the tacit mass", buf);
  }

  // --- O8: THE FOURTH CELL IS NEVER CREDITED. The disagree/firm-failed cell
  //         must contribute nothing to any licence, however tempting it is.
  {
    Run R; R.f = build_acme(250, 7, 17); R.w = make_world(17); R.w.demand_scale = 1.5f;
    run_human(R, 120);
    Compiled C = compile_from_tape(R.tape, R.w.NC, 17, true);
    Field fd; fd.init(R.w.NC, R.f.size(), 17); Ladder lad; lad.init(R.w.NC);
    PlantStore store(&R.w); PlantJudge judge(&R.w);
    ReplayOut RP = replay(R.L, R.f, fd, C, lad, R.f.writ, store, judge);
    long dfb = 0; for (int c = 0; c < R.w.NC; ++c) dfb += RP.disagree_firm_bad[c];
    long credited = 0;
    for (int c = 0; c < R.w.NC; ++c) for (int b = 0; b < NBAND; ++b) {
      const Lic& L = lad.at(c, b); credited += L.agree_total; }
    long agree_total = 0; for (int c = 0; c < R.w.NC; ++c) agree_total += RP.agree[c];
    bool ok = (dfb > 50) && (credited == agree_total);
    if (LIE == 8) ok = (credited == agree_total + dfb);            // THE LIE
    snprintf(buf, sizeof buf, "(%ld cells available, %ld credited, %ld agreements)", dfb, credited, agree_total);
    ck(LIE == 8 ? !ok : ok, "O8   the disagree/firm-failed cell licenses nothing", buf);
  }

  // --- O9: the field recovers the planted experts from outcomes alone. Nobody
  //         in the firm can do this; the information exists and the org chart
  //         has no channel that carries it.
  {
    Run R; R.f = build_acme(260, 7, 19); R.w = make_world(19); R.w.demand_scale = 1.7f;
    run_human(R, 320);
    Compiled C = compile_from_tape(R.tape, R.w.NC, 19, true);
    Resident res; res.init(R.w.NC, R.f.size(), R.w.demand_scale, C, R.f.writ, 19);
    // seed the estimator the way the resident does: train the head on arrivals
    // first, then read the seat as what the head could not explain
    float fq[FT_N];
    for (int pass = 0; pass < 2; ++pass)
      for (const Obligation& o : R.L.ob) {
        if (o.state != OB_SETTLED || o.seat < 0 || o.cls >= 32) continue;
        const ClassSpec& sq = cls_spec(o.cls);
        Field::feats(sq, o.completeness, 0.5f, (float)((int)o.day_decided - (int)o.day_due) / 7.f,
                     0.f, (float)o.hops, fq);
        const int g = (o.outcome != OK_BAD) ? 1 : 0;   // CORRECTNESS, not timeliness
        if (pass == 0) res.fd.learn(o.cls, fq, g, 0.03f);
        else           res.fd.observe_skill(o.seat, o.cls, g, Field::sigm(res.fd.logit(o.cls, fq)));
      }
    if (LIE == 9) for (auto& x : res.fd.skill_hat) x = 0.5f;       // THE LIE
    // WITHIN-CLASS correlation. Across classes the estimate is dominated by how
    // hard the class is, which is a property of the class and not of the seat;
    // the question is whether the field can rank the PEOPLE inside a class, and
    // that is the ranking the org chart has no channel for.
    double rsum = 0; int nclass = 0; int pairs = 0;
    for (int c = 0; c < R.w.NC && c < 32; ++c) {
      // THE ORACLE STATES ITS OWN CONDITIONS. A class with a handful of
      // specialists gives a correlation over six points, which is noise dressed
      // as a finding; those classes are excluded and the exclusion is printed.
      // No class is excluded for being inconvenient — only for being thin.
      double sx = 0, sy = 0, sxy = 0, sxx = 0, syy = 0; int k = 0;
      for (int s2 = 0; s2 < R.f.size(); ++s2) {
        const size_t i = (size_t)s2 * R.w.NC + c;
        if (res.fd.skill_n[i] < 25) continue;
        const double x = R.f.seat[s2].skill[c], y = res.fd.skill_hat[i];
        sx += x; sy += y; sxy += x * y; sxx += x * x; syy += y * y; ++k;
      }
      if (k < 14) continue;   // below this the within-class correlation is noise
      const double den = std::sqrt(std::max(1e-12, (k * sxx - sx * sx) * (k * syy - sy * sy)));
      rsum += (k * sxy - sx * sy) / den; ++nclass; pairs += k;
    }
    const double r = nclass ? rsum / nclass : 0.0;
    const bool ok = (nclass >= 6) && (r > 0.12);
    snprintf(buf, sizeof buf, "(mean within-class corr %.3f over %d classes, %d seat-class pairs)", r, nclass, pairs);
    ck(LIE == 9 ? !ok : ok, "O9   the field recovers competence no human in ACME can see", buf);
  }

  // --- O10: kappa can fail while everything else improves. A class that saves
  //          work but generates more review is demoted the same day.
  {
    Ladder lad; lad.init(4); Writ wr; wr.kappa_max = 1.0f; Tape t;
    Lic& L = lad.at(1, 2); L.rung = 3; L.sup_created = 900; L.sup_removed = 400;
    lad.step(10, wr, t);
    bool ok = (lad.at(1, 2).rung == 1);
    if (LIE == 10) { Lic& M = lad.at(2, 2); M.rung = 3; M.sup_created = 1; M.sup_removed = 1000;
                     lad.step(11, wr, t); ok = (lad.at(2, 2).rung == 1); }   // THE LIE
    snprintf(buf, sizeof buf, "(kappa %.2f -> rung %d)", L.kappa(), lad.at(1, 2).rung);
    ck(LIE == 10 ? !ok : ok, "O10  kappa demotes a class that costs more than it saves", buf);
  }

  // --- O11: the integrator. Every committed effect carries a usable inverse,
  //          and the window closes the moment the world reacts.
  {
    Run R; R.f = build_acme(120, 7, 23); R.w = make_world(23); R.w.demand_scale = 1.0f;
    R.init_ledger(); Tape t; world_arrive(R.w, R.L, t, 0);
    Integrator ig;
    std::vector<uint8_t> before(R.L.ob.size());
    for (size_t i = 0; i < R.L.ob.size(); ++i) before[i] = R.L.ob[i].state;
    int n = 0;
    for (uint32_t i = 0; i < (uint32_t)R.L.ob.size() && n < 60; ++i)
      if (cls_spec(R.L.ob[i].cls).reversible) { ig.commit(R.L, t, i, 1, 0, 1.5f, 2); ++n; }
    for (size_t k = ig.ledger.size(); k-- > 0; ) ig.unwind(R.L, t, k, 1);
    bool restored = true;
    for (size_t i = 0; i < R.L.ob.size(); ++i) if (R.L.ob[i].state != before[i]) restored = false;
    // and after the world reacts, the inverse must be refused
    ig.commit(R.L, t, 0, 1, 2, 1.f, 2);
    R.L.ob[0].state = OB_SETTLED;
    const bool refused = !ig.unwind(R.L, t, ig.ledger.size() - 1, 3);
    bool ok = restored && refused && (n > 20);
    if (LIE == 11) ok = ig.unwind(R.L, t, ig.ledger.size() - 1, 3);       // THE LIE
    snprintf(buf, sizeof buf, "(%d effects committed and unwound; post-settlement reversal refused)", n);
    ck(LIE == 11 ? !ok : ok, "O11  effects carry inverses; the window closes at settlement", buf);
  }

  // --- O12: the cascade's naive fit must understate the floor. This oracle
  //          exists to keep the program honest about its own best argument.
  {
    Cascade cs = fit_cascade(7, 29);
    if (LIE == 12) cs.F_hat = 0;                                    // THE LIE
    const double E = 200;
    const bool ok = (cs.predict(E) > cs.predict_naive(E) + 3.0) && (std::fabs(cs.alpha_hat - cs.alpha_true) < 0.10);
    snprintf(buf, sizeof buf, "(alpha %.3f vs %.3f planted; F %.1f vs %.1f; gap at E=200 is %.0f seats)",
             cs.alpha_hat, cs.alpha_true, cs.F_hat, cs.F_true, cs.predict(E) - cs.predict_naive(E));
    ck(LIE == 12 ? !ok : ok, "O12  the origin-fit cascade promises an impossible company", buf);
  }

  // --- O13: the decide fraction is MEASURED and must rank the planted
  //          judgement intensity. It is never told the number.
  {
    Run R; R.f = build_acme(300, 7, 31); R.w = make_world(31); R.w.demand_scale = 1.5f;
    run_human(R, 100);
    Compiled C = compile_from_tape(R.tape, R.w.NC, 31, true);
    if (LIE == 13) for (auto& x : C.decide_frac) x = 0.25f;         // THE LIE
    // SPEARMAN, because the claim is about order. The measured level is biased
    // upward — fetch and commit scale with the number of systems, which has
    // nothing to do with judgement intensity — and only the ranking survives.
    // control for the number of systems: fetch and commit scale with it and it
    // has nothing to do with judgement. The analyst KNOWS n_systems — the join
    // graph recovered it in O6 — so partialling it out is legitimate rather
    // than a thumb on the scale.
    std::vector<std::pair<double,double>> pr;
    {
      double bx = 0, by = 0, mx = 0, my = 0, mn = 0, vn = 0; int m = 0;
      for (int c = 0; c < R.w.NC; ++c) if (C.decide_frac[c] > 0) {
        mx += cls_spec(c).decide_frac; my += C.decide_frac[c]; mn += C.n_systems[c]; ++m; }
      mx /= m; my /= m; mn /= m;
      for (int c = 0; c < R.w.NC; ++c) if (C.decide_frac[c] > 0) {
        const double dn = C.n_systems[c] - mn;
        bx += dn * (cls_spec(c).decide_frac - mx); by += dn * (C.decide_frac[c] - my); vn += dn * dn; }
      bx /= std::max(1e-9, vn); by /= std::max(1e-9, vn);
      for (int c = 0; c < R.w.NC; ++c) if (C.decide_frac[c] > 0) {
        const double dn = C.n_systems[c] - mn;
        pr.push_back({ cls_spec(c).decide_frac - bx * dn, C.decide_frac[c] - by * dn }); }
    }
    const int k = (int)pr.size();
    std::vector<int> ix(k), iy(k); for (int i = 0; i < k; ++i) ix[i] = iy[i] = i;
    std::sort(ix.begin(), ix.end(), [&](int p1, int p2){ return pr[p1].first  < pr[p2].first;  });
    std::sort(iy.begin(), iy.end(), [&](int p1, int p2){ return pr[p1].second < pr[p2].second; });
    std::vector<double> rx(k), ry(k);
    for (int i = 0; i < k; ++i) { rx[ix[i]] = i; ry[iy[i]] = i; }
    double d2 = 0; for (int i = 0; i < k; ++i) d2 += (rx[i] - ry[i]) * (rx[i] - ry[i]);
    const double r = k > 2 ? 1.0 - 6.0 * d2 / ((double)k * (k * k - 1)) : 0.0;
    const bool ok = (k > 15) && (r > 0.80);
    snprintf(buf, sizeof buf, "(partial Spearman %.3f over %d classes, n_systems controlled; the LEVEL is never recovered)", r, k);
    ck(LIE == 13 ? !ok : ok, "O13  the measured decide fraction ranks the planted intensity", buf);
  }

  std::printf("\n  %d passed, %d failed", g_pass, g_fail);
  if (LIE >= 0) std::printf("   (with oracle %d lied to: PASS on its line means the lie was caught; FAIL there means it was not)", LIE);
  std::printf("\n");
  return g_fail == 0 ? 0 : 1;
}

int main(int argc, char** argv) {
  const Args a = parse(argc, argv);
  if (!strcmp(a.mode, "--sim"))        return cmd_sim(a);
  if (!strcmp(a.mode, "--twin"))       return cmd_twin(a);
  if (!strcmp(a.mode, "--automate"))   return cmd_automate(a);
  if (!strcmp(a.mode, "--multiverse")) return cmd_multiverse(a);
  return cmd_selftest(a);
}
