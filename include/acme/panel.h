// ============================================================================
//  acme/panel.h — THE ACCOUNT: the instrument panel, folds over rows     [E4]
//
//  What a chip architect has had for fifty years and the firm never had for
//  its thinking, computed from the tape and the ledger and nothing else:
//
//    the five metrics    cycle time per promise; promises settled per period;
//                        decisions per context moved; cost per committed
//                        effect; the hierarchy's hit rates (memo, certificate,
//                        counsel)
//    CPI for the firm    minutes per CORRECTLY discharged obligation, split by
//                        where the minutes went (processing, transfer, recovery,
//                        coherence, static) and beside it the two waits in days;
//                        the handled-count CPI printed next to it so the
//                        flattering numerator is visible, never used
//    the roofline        per class: fresh judgments a day, systems per read,
//                        verdicts a day, verdicts per wager act, and the roof
//                        that binds licensing (grading, admission, baseline,
//                        kappa, none), read off the ladder's own binding reason
//    the fusion ledger   the incumbent's minutes by reader class: kept (the
//                        read and the decision), transfer (the handoff),
//                        recovery (rework), coherence (meetings), static (glue);
//                        the legal dividend is what fuses away
//    the jury meter      a juror's disagreement with the judge on the settled
//                        history, and whether its disagreements are enriched
//                        for the judge's errors: a juror that never disagrees is
//                        decoration; one that disagrees at random is noise
//    the validity record per class, the pins the licence depends on
//
//  Every number here is a reading, not a mechanism; nothing in this file feeds
//  the gate, the ladder or the judge. O48 asserts the panel folds from the tape.
//  Kernel side: core, firm, ledger, license, machine, port, gate.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "ledger.h"
#include "license.h"
#include "machine.h"
#include "port.h"
#include "gate.h"
#include "report.h"   // the ladder's binding reason, read as the roofline's binding roof

namespace acme {

// ---- minutes by reader class, a cold fold of ACT, MEETING and CONTEXT rows --
struct PanelMinutes {
  double processing = 0, transfer = 0, recovery = 0, coherence = 0, stat = 0;   // fetch+frame+decide+commit · transport · rework · meeting · glue
  double total() const { return processing + transfer + recovery + coherence + stat; }
  std::vector<double> by_class_processing, by_class_transfer, by_class_recovery;   // [NC]
};
inline PanelMinutes panel_minutes(const Tape& tape, int NC, bool lie_drop_meetings = false) {
  PanelMinutes P; P.by_class_processing.assign(NC, 0); P.by_class_transfer.assign(NC, 0); P.by_class_recovery.assign(NC, 0);
  tape.fold([&](const Rec& r) {
    if (r.type == R_MEETING) { if (!lie_drop_meetings) P.coherence += r.value; return; }   // THE LIE (O48): a fold that drops the meetings
    if (r.type == R_CONTEXT) { if (!(r.flags & RF_LOST) && r.seat >= 0) { P.processing += r.value; if (r.cls < (uint32_t)NC) P.by_class_processing[r.cls] += r.value; } return; }
    if (r.type != R_ACT) return;
    const double m = r.value;
    switch (r.a) {
      case ACT_FETCH: case ACT_FRAME: case ACT_DECIDE: case ACT_COMMIT: P.processing += m; if (r.cls < (uint32_t)NC) P.by_class_processing[r.cls] += m; break;
      case ACT_TRANSPORT: P.transfer += m; if (r.cls < (uint32_t)NC) P.by_class_transfer[r.cls] += m; break;
      case ACT_REWORK:    P.recovery += m; if (r.cls < (uint32_t)NC) P.by_class_recovery[r.cls] += m; break;
      case ACT_MEETING:   P.coherence += m; break;
      case ACT_GLUE:      P.stat += m; break;
      default: break;
    }
  });
  return P;
}

// ---- the cells' side: counts and the two waits, per arm ---------------------
struct PanelCells {
  long decided = 0, settled = 0, good = 0, wager_acts = 0, wager_settled = 0;
  double wait_to_decide_days = 0, wait_to_settle_days = 0;     // means
  long n_dec = 0, n_set = 0;
};
// arm: 0 the incumbent's cells (by_machine 0) · 1 the machine's · 2 every cell
inline PanelCells panel_cells(const Ledger& L, int arm) {
  PanelCells C; double sd = 0, ss = 0;
  for (const Obligation& o : L.ob) {
    if (arm == 0 && o.by_machine) continue;
    if (arm == 1 && !o.by_machine) continue;
    if (o.state == OB_DECIDED || o.state == OB_SETTLED) { ++C.decided; sd += (double)o.day_decided - (double)o.day_open; ++C.n_dec; }
    if (o.state == OB_SETTLED) { ++C.settled; ss += (double)o.day_settled - (double)o.day_decided; ++C.n_set; if (o.outcome == OK_GOOD) ++C.good; }
    if (o.by_machine && (o.via == 1 || o.via == 3) && (o.state == OB_DECIDED || o.state == OB_SETTLED)) { ++C.wager_acts; if (o.state == OB_SETTLED && o.outcome != OK_UNRESOLVED) ++C.wager_settled; }
  }
  C.wait_to_decide_days = C.n_dec ? sd / C.n_dec : 0.0;
  C.wait_to_settle_days = C.n_set ? ss / C.n_set : 0.0;
  return C;
}

// ---- CPI FOR THE FIRM ------------------------------------------------------
inline void print_cpi(const Tape& tape, const Ledger& L, int NC, const char* title) {
  const PanelMinutes M = panel_minutes(tape, NC);
  const PanelCells   C = panel_cells(L, 2);
  const double tot = M.total();
  std::printf("\n  CPI FOR THE FIRM (%s) — minutes per CORRECTLY discharged obligation, the way a core reports cycles per\n"
              "  instruction: split by where the minutes went. The handled-count CPI is printed beside it because it flatters,\n"
              "  and it is used by nothing.\n", title);
  std::printf("  %-34s %12s %8s\n", "bucket", "k-minutes", "share");
  std::printf("  %-34s %12.1f %7.1f%%   the read and the decision on the cell (fetch, frame, decide, commit)\n", "processing (kept)", M.processing / 1000, tot > 0 ? 100 * M.processing / tot : 0);
  std::printf("  %-34s %12.1f %7.1f%%   the handoff between seats (transport): fuses away\n", "transfer", M.transfer / 1000, tot > 0 ? 100 * M.transfer / tot : 0);
  std::printf("  %-34s %12.1f %7.1f%%   rework: served by the tape's inverse where the design holds\n", "recovery", M.recovery / 1000, tot > 0 ? 100 * M.recovery / tot : 0);
  std::printf("  %-34s %12.1f %7.1f%%   meetings: coherence at skull bandwidth; collapses to hash cost\n", "coherence", M.coherence / 1000, tot > 0 ? 100 * M.coherence / tot : 0);
  std::printf("  %-34s %12.1f %7.1f%%   glue with no cell: no effect, no evidence, no control\n", "static (leakage)", M.stat / 1000, tot > 0 ? 100 * M.stat / tot : 0);
  std::printf("  %-34s %12.1f\n", "all", tot / 1000);
  std::printf("  correctly discharged (OUTCOME good) %ld · handled (decided) %ld · settled %ld\n", C.good, C.decided, C.settled);
  std::printf("  CPI %.1f minutes per correctly discharged obligation   (handled-count CPI %.1f: the flattering number)\n",
              C.good ? tot / C.good : 0.0, C.decided ? tot / C.decided : 0.0);
  std::printf("  the two waits, in days: to a decision %.1f · from the decision to the world's answer %.1f\n", C.wait_to_decide_days, C.wait_to_settle_days);
}

// ---- THE FIVE METRICS -----------------------------------------------------
struct CounselCount { long rentals = 0, distinct = 0; };
inline CounselCount counsel_count(const Tape& tape, uint32_t warm) {
  CounselCount c; std::vector<uint8_t> seen;
  tape.fold([&](const Rec& r) {
    if (r.type != R_COUNSEL || (r.flags & RF_SHADOW) || r.day < warm) return;
    ++c.rentals;
    if (r.oid >= seen.size()) seen.resize((size_t)r.oid + 1024, 0);
    if (!seen[r.oid]) { seen[r.oid] = 1; ++c.distinct; }
  });
  return c;
}
inline void print_five_metrics(const Tape& tape, const Ledger& L, const MachineStats& st, int NC, uint32_t warm, uint32_t days) {
  (void)NC;
  const PanelCells H = panel_cells(L, 0), Mc = panel_cells(L, 1);
  const double live_days = days > warm ? (double)(days - warm) : 1.0;
  const CounselCount cc = counsel_count(tape, warm);
  const double reads = (double)st.reads, carries = (double)st.memo_hits;
  std::printf("\n  THE FIVE METRICS — what architects have used for fifty years, on the firm's thinking (the live period, %u days)\n", days - warm);
  std::printf("  %-44s %14s %14s\n", "", "a person", "the machine");
  std::printf("  %-44s %14.1f %14.1f   ARRIVE to DECIDE, days\n", "cycle time per promise, to a decision", H.wait_to_decide_days, Mc.wait_to_decide_days);
  std::printf("  %-44s %14.1f %14.1f   DECIDE to OUTCOME, days: the world's clock\n", "cycle time per promise, to the verdict", H.wait_to_settle_days, Mc.wait_to_settle_days);
  std::printf("  %-44s %14.1f %14.1f   settled cells a day over the live period\n", "promises settled per period", H.settled / live_days, Mc.settled / live_days);
  std::printf("  %-44s %14s %14.2f   fresh reads per settled machine cell; %.1f%% of a period's judgments carried by the memo\n", "decisions per context moved", "",
              Mc.settled ? reads / Mc.settled : 0.0, (reads + carries) > 0 ? 100 * carries / (reads + carries) : 0.0);
  std::printf("  %-44s %14s %14.1f   supervision minutes caused per unattended act; %.2f rentals per act\n", "cost per committed effect", "",
              st.acted ? st.sup_created_min / (double)st.acted : 0.0, st.acted ? st.frontier_calls / (double)st.acted : 0.0);
  std::printf("  %-44s %14s %14s   memo %.1f%% · certificate %s · counsel 0.0%% (%ld rentals on %ld distinct cells: the L2 that E1' builds buys %ld a year)\n",
              "hit rates", "", "", (reads + carries) > 0 ? 100 * carries / (reads + carries) : 0.0,
              st.shared_affected ? (std::string(std::to_string((int)(1000.0 * st.incr_updates / st.shared_affected) / 10.0)) + "%").c_str() : "n/a (no shared fact)",
              cc.rentals, cc.distinct, cc.rentals - cc.distinct);
}

// ---- THE ROOFLINE ---------------------------------------------------------
inline const char* roof_name(int br) {
  switch (br) {
    case BR_TOP: return "none";
    case BR_THIN_BAND: return "thin";
    case BR_NO_HISTORY: return "admission";
    case BR_BAR: return "baseline";
    case BR_LOGE: case BR_TERM: return "grading";
    case BR_KAPPA: return "kappa";
    case BR_WIDENING: return "widening";
    default: return "?";
  }
}
inline void print_roofline(const Tape& tape, const Ledger& L, const Ladder& lad, const Writ& wr, const Compiled& C, uint32_t judge_hash,
                           int NC, uint32_t warm, uint32_t days) {
  std::vector<long> fresh(NC, 0), verdicts(NC, 0), acts(NC, 0), wsettled(NC, 0);
  tape.fold([&](const Rec& r) {
    if (r.day < warm || r.cls >= (uint32_t)NC) return;
    if (r.type == R_PROPOSAL && (uint32_t)r.b == judge_hash && r.via != 1) ++fresh[r.cls];
    if (r.type == R_OUTCOME && (r.via == 1 || r.via == 3) && r.a != OK_UNRESOLVED) ++verdicts[r.cls];
    if (r.type == R_EFFECT && !(r.flags & RF_SHADOW) && (r.via == 1 || r.via == 3)) ++acts[r.cls];
  });
  for (const Obligation& o : L.ob) if (o.by_machine && (o.via == 1 || o.via == 3) && o.state == OB_SETTLED && o.outcome != OK_UNRESOLVED && o.cls < NC) ++wsettled[o.cls];
  const double live_days = days > warm ? (double)(days - warm) : 1.0;
  std::printf("\n  THE ROOFLINE — per class, which roof binds licensing: the compute roof (judgments a day at the read budget), the\n"
              "  bandwidth roof (systems read per judgment), or the grading roof (verdicts a day), which is the world's and which no\n"
              "  engineering of the interior raises. The roof is read off the ladder's own binding reason for bands 1 and 2.\n");
  std::printf("  %-22s %9s %8s %10s %9s   %-10s %-10s\n", "class", "judg/day", "sys/read", "verd/day", "verd/act", "roof b1", "roof b2");
  int grading = 0, admission = 0, baseline = 0, none = 0;
  for (int c = 0; c < NC; ++c) {
    const int b1 = binding_reason(lad.at(c, 1), lad, wr, days - 1, 1), b2 = binding_reason(lad.at(c, 2), lad, wr, days - 1, 2);
    for (int b : {b1, b2}) { if (b == BR_LOGE || b == BR_TERM) ++grading; else if (b == BR_NO_HISTORY) ++admission; else if (b == BR_BAR) ++baseline; else if (b == BR_TOP) ++none; }
    std::printf("  %-22s %9.1f %8d %10.2f %9.2f   %-10s %-10s\n", cls_spec(c).name, fresh[c] / live_days, __builtin_popcount(C.join_graph[c]),
                verdicts[c] / live_days, acts[c] ? (double)wsettled[c] / (double)acts[c] : 0.0, roof_name(b1), roof_name(b2));
  }
  std::printf("  bands bound by the grading roof %d · by admission %d · by the baseline %d · at the top %d (of %d wager bands)\n",
              grading, admission, baseline, none, 2 * NC);
}

// ---- THE FUSION LEDGER ----------------------------------------------------
inline void print_fusion_ledger(const Tape& tape, int NC) {
  const PanelMinutes M = panel_minutes(tape, NC);
  const double tot = M.total(), legal = M.transfer + M.coherence + M.stat;
  std::printf("\n  THE FUSION LEDGER — the firm's minutes by who reads the intermediate: kept (the read and the decision),\n"
              "  transfer (the handoff, fuses away), recovery (rework, served by the tape), coherence (meetings, hash cost on the\n"
              "  plane), static (glue). The legal dividend is transfer plus coherence plus static, by minutes, per class where the\n"
              "  rows carry a class.\n");
  std::printf("  %-22s %10s %10s %10s %8s\n", "class", "kept k-min", "transfer", "recovery", "fusible");
  for (int c = 0; c < NC; ++c) {
    const double k = M.by_class_processing[c], t = M.by_class_transfer[c], r = M.by_class_recovery[c], s = k + t + r;
    std::printf("  %-22s %10.1f %10.1f %10.1f %7.1f%%\n", cls_spec(c).name, k / 1000, t / 1000, r / 1000, s > 0 ? 100 * t / s : 0.0);
  }
  std::printf("  the whole firm: kept %.1fk · transfer %.1fk · recovery %.1fk · coherence %.1fk · static %.1fk; the legal dividend %.1f%% of all minutes\n",
              M.processing / 1000, M.transfer / 1000, M.recovery / 1000, M.coherence / 1000, M.stat / 1000, tot > 0 ? 100 * legal / tot : 0.0);
}

// ---- THE JURY METER -------------------------------------------------------
// Over the settled history the firm decided: the judge's choice, the juror's, the
// firm's, and the world's verdict. Where the judge agreed with the firm its
// correctness is graded by the arrival. Enrichment is the judge's wrong-rate
// where the juror disagreed over its wrong-rate where the juror agreed: a juror
// worth its price disagrees where the errors are.
struct JuryRow { long n = 0, disagree = 0, graded_dis = 0, wrong_dis = 0, graded_agr = 0, wrong_agr = 0; };
inline void print_jury_meter(const Ledger& L, const Compiled& C, const Store& store, Judge& judge, Judge& juror, int NC, const char* juror_name) {
  std::vector<JuryRow> J(NC); JuryRow all;
  for (const Obligation& o : L.ob) {
    if (o.state != OB_SETTLED || o.by_machine || o.cls >= NC || o.outcome == OK_UNRESOLVED) continue;
    Frame fr = store.frame(o.id, o.cls, C.join_graph[o.cls], o.day_decided); fr.coverage_hat = C.coverage_hat(o.cls);
    const int p = judge.read(fr).choice, j = juror.read(fr).choice, f = o.decision;
    const bool wrong = (o.outcome == OK_BAD), dis = (p != j), graded = (p == f);
    JuryRow& r = J[o.cls]; ++r.n; ++all.n;
    if (dis) { ++r.disagree; ++all.disagree; }
    if (graded) {
      if (dis) { ++r.graded_dis; ++all.graded_dis; if (wrong) { ++r.wrong_dis; ++all.wrong_dis; } }
      else     { ++r.graded_agr; ++all.graded_agr; if (wrong) { ++r.wrong_agr; ++all.wrong_agr; } }
    }
  }
  auto enrich = [](const JuryRow& r, double& wd, double& wa) {
    wd = r.graded_dis ? (double)r.wrong_dis / r.graded_dis : 0.0; wa = r.graded_agr ? (double)r.wrong_agr / r.graded_agr : 0.0;
    return (wa > 0 && r.graded_dis >= 30 && r.graded_agr >= 30) ? wd / wa : 0.0; };
  std::printf("\n  THE JURY METER — juror %s against the judge on the settled history: a juror that never disagrees is decoration; one\n"
              "  whose disagreements are not enriched for the judge's errors is noise. (Classes with at least 30 graded cells on each side.)\n", juror_name);
  std::printf("  %-22s %7s %9s %11s %11s %10s\n", "class", "cells", "disagree", "wrong|dis", "wrong|agr", "enrich");
  for (int c = 0; c < NC; ++c) {
    double wd, wa; const double e = enrich(J[c], wd, wa);
    if (J[c].graded_dis < 30 || J[c].graded_agr < 30) continue;
    std::printf("  %-22s %7ld %8.1f%% %10.1f%% %10.1f%% %9.2fx\n", cls_spec(c).name, J[c].n, J[c].n ? 100.0 * J[c].disagree / J[c].n : 0.0, 100 * wd, 100 * wa, e);
  }
  double wd, wa; const double e = enrich(all, wd, wa);
  std::printf("  %-22s %7ld %8.1f%% %10.1f%% %10.1f%% %9.2fx   %s\n", "all", all.n, all.n ? 100.0 * all.disagree / all.n : 0.0, 100 * wd, 100 * wa, e,
              all.disagree == 0 ? "DECORATION: never disagrees" : (e < 1.2 ? "NOISE: disagreements not enriched for the judge's errors" : "a juror: disagrees where the errors are"));
}

// ---- THE VALIDITY RECORD --------------------------------------------------
inline uint32_t calib_pin_of(const Ledger& L, int c) {
  Blake2b h;
  for (int b = 0; b < CALIB_NBIN; ++b) { const float f = L.calib_fit_of(c, b); const long n = L.calib_n_of(c, b); h.update(&f, sizeof f); h.update(&n, sizeof n); }
  const uint8_t m = L.calib_measured_of(c) ? 1 : 0, o = L.calib_monotone_of(c) ? 1 : 0; h.update(&m, 1); h.update(&o, 1);
  uint8_t d[32]; h.final(d);
  return (uint32_t)d[0] | ((uint32_t)d[1] << 8) | ((uint32_t)d[2] << 16) | ((uint32_t)d[3] << 24);
}
inline void print_validity(const Ledger& L, const Ladder& lad, const Compiled& C, uint32_t judge_hash, int NC, uint32_t day) {
  std::printf("\n  THE VALIDITY RECORD — per class, the pins the licence depends on; a change in any reads rung 0 until re-earned\n"
              "  (a guard set, blocking in one tick; drift no pin names is the regime detector's, at term latency)\n");
  std::printf("  %-22s %10s %10s %10s %10s %6s %6s\n", "class", "schema", "template", "judge", "calib", "term", "policy");
  const uint32_t schema = alphabet_hash(); const int term = lad.term_index_of(day);
  for (int c = 0; c < NC; ++c)
    std::printf("  %-22s 0x%08x 0x%08x 0x%08x 0x%08x %6d %6d\n", cls_spec(c).name, schema, C.template_hash[c], judge_hash, calib_pin_of(L, c), term, 0);
}

} // namespace acme
