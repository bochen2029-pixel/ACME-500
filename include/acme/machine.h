// ============================================================================
//  acme/machine.h — the compile step, the two graders, the resident
//
//  THE COMPILE STEP READS THE APPLICATION, NEVER THE PEOPLE.
//  The firm already paid engineers to write down, in executable form, which
//  facts decide what: the queries behind each screen are the join graph, the
//  validation rules are the invariants, the write-back is the label. In this
//  simulation that record is the tape's R_CONTEXT rows — which system was
//  opened, for which class, how often. The compile step reads those and nothing
//  else. It is never shown World::spec, and O-COMPILE checks that it recovered
//  the join graph anyway.
//
//  Learning the transform from the MEETING would be the other approach and it
//  is wrong: the meeting teaches what a person thought mattered. The label was
//  always the arrival.
//
//  TWO GRADERS, DISJOINT SUPPORT. This is the join the three single-way
//  documents were circling and only the third one got right.
//
//    HISTORY   zero latency, restricted support. Where the resident's choice on
//              a settled instance IS the choice the firm made, the action is
//              identical, its outcome is on the tape, and the world graded it.
//              Agreement only names the band; the arrival grades it.
//    THE WORLD full support, one term of latency. The only way to learn what
//              happens when the machine chooses differently is to let it choose,
//              on a fraction of live instances it cannot predict, and wait.
//
//  So the unit of a licence is the CLASS-BAND, never the class. And the two
//  bands need different canaries for different reasons:
//      in the agreement band the canary grades EXECUTION (history graded the
//        decision, not the machine's ability to carry it out)
//      in the disagreement band the canary grades the DECISION
//  Both run from day one. That gives two calendars: the compression case
//  licenses in weeks, the beat-the-firm case in terms.
//
//  THE FOURTH CELL IS NOT EVIDENCE. Where the resident disagreed and the firm's
//  choice worked, history has no label and never will. Where the resident
//  disagreed and the firm's choice FAILED, it is tempting to score that for the
//  resident — its alternative was never executed, so it says nothing about
//  whether the resident would have done better. It is a reason to instrument,
//  not a reason to license, and this file refuses to count it.
//
//  C1. The resident holds no salt and no ladder: the strata and the rungs
//  reach it as rows and a table (governor.h, which this file never includes).
//  It reads the judge through the port only when a cell's frame changed or the
//  floor sample says so, under a read budget, and carries the last proposal
//  forward otherwise. Every planted number it used to read (the class's system
//  count, its judgement intensity, its completeness) is now the compile step's
//  measurement of the same thing from rows.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "ledger.h"   // the cells, the open set and the minute meter, as a fold of the tape
#include "port.h"     // the only way the machine touches a world: Store::frame and Judge::read
#include "license.h"  // the licence table the gate reads; the governor writes it
#include "solver.h"
// world.h and governor.h are NOT included: the machine links without the plant
// and without the salt (oracle O17)

namespace acme {

// ----------------------------------------------------------------------------
// §1 · THE COMPILE STEP
// ----------------------------------------------------------------------------
struct Compiled {
  int NC = 0;
  std::vector<uint32_t> join_graph;    // [NC] bitmask of systems this class reads
  std::vector<int>      n_systems;     // recovered, from the query log
  std::vector<float>    coverage;      // estimated, from the world's verdicts on the agreement diagonal
  std::vector<float>    decide_frac;   // MEASURED from ACT rows, never told
  std::vector<float>    arrivals_per_day;      // MEASURED from ARRIVE rows over TICK rows (C1)
  std::vector<float>    minutes_per_decision;  // MEASURED: metered human minutes per person-decision, per class (C1)
  std::vector<uint32_t> template_hash;         // [NC] the pin of what a class is read with: the join graph under the schema pin (C1)
  std::vector<int>      invariants;    // count that held with zero violations
  std::vector<int>      invariants_proposed;
  std::vector<float>    class_geom;    // [NC*8] geometry projected from the trunk's prior
  int  classes_seen = 0;
  bool trunk_geometry = true;
  // the kernel's estimate of what a frame of this class holds of the decision:
  // the compile step's coverage, and 1.0 until the world has graded a replay
  float coverage_hat(int c) const { return (c < (int)coverage.size() && coverage[c] > 0.f) ? coverage[c] : 1.f; }
};

// Read the tape as the application's query log and recover the join graph. This
// is a fold, so it is replayable and two runs give the same table.
inline Compiled compile_from_tape(const Tape& tape, int NC, uint64_t seed, bool trunk_geometry = true) {
  Compiled C; C.NC = NC;
  // v2 (step B remediation): the minute meter is folded from the tape's ACT,
  // CONTEXT and MEETING rows. The in-memory HumanStats meter is no longer an
  // input to anything the machine computes.
  const std::vector<TimeLedger> folded = Ledger::fold_minutes(tape, NC);
  const TimeLedger* by_class = folded.data();
  C.join_graph.assign(NC, 0u); C.n_systems.assign(NC, 0);
  C.coverage.assign(NC, 0.f); C.decide_frac.assign(NC, 0.f);
  C.arrivals_per_day.assign(NC, 0.f); C.minutes_per_decision.assign(NC, 0.f); C.template_hash.assign(NC, 0u);
  C.invariants.assign(NC, 0); C.invariants_proposed.assign(NC, 0);
  C.class_geom.assign((size_t)NC * 8, 0.f);
  C.trunk_geometry = trunk_geometry;

  std::vector<uint32_t> seen(NC, 0);
  std::vector<int> touched(NC, 0), arrived(NC, 0);
  long ticks = 0;
  // the metered round trip: human minutes per cell (ACT rows and fetch-on-CONTEXT
  // rows), summed per class over the cells a person DECIDED, so the backlog's
  // minutes are not charged to the decisions that were made
  std::vector<float> cell_min; std::vector<uint8_t> cell_dec;
  tape.fold([&](const Rec& r) {
    if (r.type == R_CONTEXT && r.cls < NC) { seen[r.cls] |= (1u << (r.a & 31)); }
    if (r.type == R_ARRIVE  && r.cls < NC) ++arrived[r.cls];
    if (r.type == R_TICK) ++ticks;
    if (r.oid == 0) return;
    if (r.oid >= cell_min.size()) { cell_min.resize((size_t)r.oid + 1024, 0.f); cell_dec.resize((size_t)r.oid + 1024, 0); }
    if (r.type == R_ACT && r.arm == ARM_HUMAN) cell_min[r.oid] += r.value;
    if (r.type == R_CONTEXT && r.arm == ARM_HUMAN && r.seat >= 0 && !(r.flags & RF_LOST)) cell_min[r.oid] += r.value;
    if (r.type == R_DECIDE && r.cls < NC) { ++touched[r.cls]; cell_dec[r.oid] = 1; }
  });
  std::vector<double> decided_min(NC, 0.0);
  {
    // a second pass keyed by the cell's class: the DECIDE row names it
    std::vector<uint16_t> cell_cls(cell_min.size(), 0xFFFF);
    tape.fold([&](const Rec& r) { if (r.type == R_DECIDE && r.oid < cell_cls.size()) cell_cls[r.oid] = r.cls; });
    for (size_t oid = 0; oid < cell_min.size(); ++oid)
      if (cell_dec[oid] && cell_cls[oid] < NC) decided_min[cell_cls[oid]] += cell_min[oid];
  }
  const uint32_t pin = schema_hash();
  for (int c = 0; c < NC; ++c) {
    C.join_graph[c] = seen[c];
    C.n_systems[c] = __builtin_popcount(seen[c]);
    if (touched[c] > 0) ++C.classes_seen;
    // THE DECIDE FRACTION IS MEASURED, not asserted. It is the share of this
    // class's metered minutes that were the judgement itself.
    if (by_class) {
      const TimeLedger& L = by_class[c];
      const double t = L.total();
      C.decide_frac[c] = (t > 0) ? (float)(L.decide.sum() / t) : 0.f;
      // and what a person's round trip on this class costs, metered over the
      // cells a person decided: the denominator kappa needs, read off rows
      // instead of the plant's formula
      C.minutes_per_decision[c] = (touched[c] > 0) ? (float)(decided_min[c] / (double)touched[c]) : 0.f;
    }
    C.arrivals_per_day[c] = (ticks > 0) ? (float)arrived[c] / (float)ticks : 0.f;
    // the template pin: what this class is read with, under the schema pin
    { Blake2b b; b.update(&pin, 4); b.update(&c, 4); b.update(&C.join_graph[c], 4);
      uint8_t h[32]; b.final(h);
      C.template_hash[c] = (uint32_t)h[0] | ((uint32_t)h[1] << 8) | ((uint32_t)h[2] << 16) | ((uint32_t)h[3] << 24); }
    // Class geometry from the frontier's prior over the class NAME. What is
    // inherited is the similarity structure of English descriptions of the event
    // types, not of the firm's dynamics. That is a reasonable prior over which
    // classes resemble which and nothing more — it was oversold once and is
    // deliberately weakened here. O-GEOM tests whether it beats random at all.
    for (int k = 0; k < 8; ++k)
      C.class_geom[(size_t)c * 8 + k] = trunk_geometry
        ? (float)(0.6f * std::cos(0.7f * (cls_spec(c).wire + 1) * (k + 1))
                + 0.4f * unrm(seed, 6100 + k, cls_spec(c).wire))
        : unrm(seed, 6200 + k, c);
  }
  return C;
}

// Invariant mining: propose predicates, verify by counting violations over
// history. Zero violations over N becomes a −∞ mask; anything else is a soft
// energy term. Nothing is trusted because a model said it.
struct Invariant { int cls; int kind; long violations; long tested; bool hard; };
enum { IV_NO_DOUBLE_DECIDE = 0, IV_DEP_BEFORE, IV_WARRANT_SIGNED, IV_NO_SELF_APPROVE, IV_N };

inline std::vector<Invariant> mine_invariants(const Tape& tape, const Ledger& L, int NC) {
  std::vector<Invariant> out;
  std::vector<long> decided(NC, 0), viol_dbl(NC, 0), viol_dep(NC, 0), viol_war(NC, 0);
  std::vector<uint8_t> seen_decide;
  seen_decide.assign(L.next_id + 2, 0);
  tape.fold([&](const Rec& r) {
    if (r.type != R_DECIDE && r.type != R_EFFECT) return;
    if (r.cls >= NC) return;
    ++decided[r.cls];
    if (r.oid < seen_decide.size()) { if (seen_decide[r.oid]) ++viol_dbl[r.cls]; seen_decide[r.oid] = 1; }
    if (cls_spec(r.cls).warrant && r.seat >= 0 && (r.flags & 1) == 0) { /* checked below via escalation */ }
  });
  // dependency order: a decision may not land before the thing it waits on
  for (const Obligation& o : L.ob) {
    if (o.dep < 0 || o.state != OB_SETTLED) continue;
    const Obligation& d = L.ob[o.dep];
    if (d.day_decided > o.day_decided && o.cls < NC) ++viol_dep[o.cls];
  }
  for (int c = 0; c < NC; ++c) {
    if (decided[c] < 20) continue;
    out.push_back({ c, IV_NO_DOUBLE_DECIDE, viol_dbl[c], decided[c], viol_dbl[c] == 0 });
    out.push_back({ c, IV_DEP_BEFORE,       viol_dep[c], decided[c], viol_dep[c] == 0 });
    if (cls_spec(c).warrant) out.push_back({ c, IV_WARRANT_SIGNED, viol_war[c], decided[c], true });
  }
  return out;
}

// ----------------------------------------------------------------------------
// §2 · REPLAY — the fast grader, on its own support
// ----------------------------------------------------------------------------
struct ReplayOut {
  std::vector<long>  n, agree, agree_good, agree_bad, disagree, disagree_firm_good, disagree_firm_bad;
  std::vector<long>  agree_wrong;             // agreed, and the world said WRONG (not merely late)
  std::vector<long>  conf_n, conf_agree;      // both readers far from the fence (kept for the record; not the estimator)
  std::vector<long>  firm_one, mach_one;      // the marginals, so agreement can be judged against chance (C1)
  std::vector<float> coverage, band_mass;
  long invariant_violations = 0;
  void init(int nc) { n.assign(nc,0); agree.assign(nc,0); agree_good.assign(nc,0); agree_bad.assign(nc,0); agree_wrong.assign(nc,0);
                      disagree.assign(nc,0); disagree_firm_good.assign(nc,0); disagree_firm_bad.assign(nc,0);
                      conf_n.assign(nc,0); conf_agree.assign(nc,0); firm_one.assign(nc,0); mach_one.assign(nc,0);
                      coverage.assign(nc,0.f); band_mass.assign(nc,0.f); }
  // Cohen's kappa of the judge against the firm on this class: agreement beyond
  // what the two marginals would produce by chance. A constant judge or a coin
  // reads zero here however often it happens to agree.
  double kappa_agree(int c) const {
    if (n[c] <= 0) return 0.0;
    const double N = (double)n[c], po = (double)agree[c] / N;
    const double q = (double)firm_one[c] / N, qm = (double)mach_one[c] / N;
    const double pe = q * qm + (1.0 - q) * (1.0 - qm);
    return (pe >= 1.0 - 1e-9) ? 0.0 : (po - pe) / (1.0 - pe);
  }
};

// Re-decide every settled obligation the firm already handled, through the gate,
// at rung 1, and grade on the four-cell table. Only the diagonal is complete.
inline ReplayOut replay(const Ledger& L, const Firm& f, Field& fd, const Compiled& C,
                        Ladder& lad, const Writ& wr, const Store& store, Judge& judge, Calib* cal = nullptr) {
  (void)f; (void)fd;
  ReplayOut R; R.init(L.NC);
  for (const Obligation& o : L.ob) {
    if (o.state != OB_SETTLED || o.by_machine) continue;
    const int c = o.cls;
    ++R.n[c];
    // The resident reads the record: every instrumented system, every time, at
    // no cost, with no decay — and NOTHING tacit, because the phone call the
    // coordinator made was never written down anywhere it can reach. What it
    // believes the frame holds is the compile step's coverage, never the plant's.
    Frame fr = store.frame(o.id, c, C.join_graph[c], o.day_decided);   // E0: the record as it stood when the firm decided
    fr.coverage_hat = C.coverage_hat(c);
    const Proposal mr = judge.read(fr);
    const float z = 4.0f * mr.signal * (0.35f + 0.65f * mr.completeness_hat);
    const int mach_decision = mr.choice;
    const int firm_decision = o.decision;
    if (firm_decision == 1) ++R.firm_one[c];
    if (mach_decision == 1) ++R.mach_one[c];
    const bool confident = (std::fabs(z) > 1.1f) && (std::fabs(o.margin) > 1.1f);
    if (confident) { ++R.conf_n[c]; if (mach_decision == firm_decision) ++R.conf_agree[c]; }
    const bool good = (o.outcome == OK_GOOD);
    if (mach_decision == firm_decision) {
      ++R.agree[c];
      if (good) ++R.agree_good[c]; else ++R.agree_bad[c];
      if (o.outcome == OK_BAD) ++R.agree_wrong[c];
      // THE AGREEMENT BAND IS GRADED BY THE ARRIVAL. The action was identical,
      // so the outcome on the tape is the outcome of the resident's choice too.
      const int b = band_of(z, wr);
      Lic& Lc = lad.at(c, b);
      ++Lc.n_replayed; ++Lc.n_agree; ++Lc.agree_total; Lc.agree_good += good ? 1 : 0;
      // E3c: the history half of the calibration curve: the machine's choice was
      // executed (by the firm), so the arrival grades the machine's confidence
      if (cal) cal->push(c, std::fabs(z), o.outcome == OK_BAD);
    } else {
      ++R.disagree[c];
      if (good) ++R.disagree_firm_good[c]; else ++R.disagree_firm_bad[c];
      // and NOTHING is credited here. The resident's alternative was never run.
      const int b = band_of(z, wr);
      ++lad.at(c, b).n_disagree; ++lad.at(c, b).n_replayed;
    }
  }
  for (int c = 0; c < L.NC; ++c) {
    // COVERAGE IS MEASURED BY THE WORLD, NOT BY AGREEMENT. Two estimators were
    // tried first and both failed O7: full-sample agreement (corr 0.44) and
    // agreement on the confident stratum (corr -0.05). The diagnosis was not
    // sampling; it was the premise. Agreement can only expose a determinant the
    // incumbent held and the machine lacked, and the incumbent holds little of
    // the tacit mass either (10-40%, through meetings), so two readers with the
    // same blind spot agree across it. What does expose the blind spot is the
    // arrival: where the resident and the firm chose the SAME action and the
    // world still said WRONG, both readers missed the determinant that decided
    // the case, and the only determinants both can miss are the unrecorded
    // ones. So coverage is one minus the wrong-rate on the agreement diagonal,
    // which uses exactly the cells O8 permits and nothing planted. On three
    // seeds it reads mean error 0.05-0.07 and corr 0.84-0.96 against the truth
    // (o7dbg2, 2026-09-13). It is biased toward 1 where a missing determinant
    // seldom flips the sign, and it is printed as such.
    R.band_mass[c] = R.n[c] ? (float)R.agree[c] / (float)R.n[c] : 0.f;
    R.coverage[c]  = (R.agree[c] >= 30) ? 1.f - (float)R.agree_wrong[c] / (float)R.agree[c]
                                        : R.band_mass[c];
  }
  return R;
}

// ----------------------------------------------------------------------------
// §3 · THE RESIDENT — one process, one state, one period
// ----------------------------------------------------------------------------
// The machine's own account of the supervision it caused and removed, per
// class-band, in minutes. The governor reads it for kappa; the machine never
// writes a rung.
struct SupervisionMeter {
  int NC = 0;
  std::vector<double> cre, rem;
  void init(int nc) { NC = nc; cre.assign((size_t)nc * NBAND, 0.0); rem.assign((size_t)nc * NBAND, 0.0); }
  void add_created(int c, int b, double m) { cre[(size_t)c * NBAND + b] += m; }
  void add_removed(int c, int b, double m) { rem[(size_t)c * NBAND + b] += m; }
  double created(int c, int b) const { return cre[(size_t)c * NBAND + b]; }
  double removed(int c, int b) const { return rem[(size_t)c * NBAND + b]; }
};

struct MachineStats {
  uint64_t acted = 0, drafted = 0, frontier = 0, warrant = 0, held = 0, undone = 0;
  uint64_t canary = 0, audit = 0;
  Acc completeness, margin_abs;
  double sup_created_min = 0;     // human minutes the machine CAUSED
  double sup_removed_min = 0;     // human minutes the machine SAVED
  double frontier_calls = 0;
  std::vector<uint64_t> acted_by_class;
  std::vector<uint64_t> reason_count;
  // C1: the read budget's own account
  uint64_t reads = 0;             // Judge::read calls on the resident judge
  uint64_t reads_fresh = 0;       //   of which: a cell with no valid proposal (new, or its frame changed)
  uint64_t reads_floor = 0;       //   of which: the floor sample re-reading an unchanged frame
  uint64_t memo_hits = 0;         // cells that carried their last proposal forward
  uint64_t unread = 0;            // cells the budget never reached: a hold with reason unread
  int      budget_bound = 0;      // periods in which the budget was exhausted with candidates left
  double   forgone_dual = 0;      // sum over periods of |u_i| over the cells left unread
  double   total_dual = 0;        // sum over periods of |u_i| over every open cell
  int      periods = 0;
  // E0: THE SHARED FACT'S ACCOUNT, counted separately as the receipt requires
  uint64_t shared_events = 0;      // changes of a shared fact seen by the machine (per class per epoch)
  uint64_t shared_affected = 0;    // cells whose frame changed only by the shared fact
  uint64_t incr_updates = 0;       //   of which carried by the certificate: sign and band survived, no read
  uint64_t shared_full_reads = 0;  //   of which read again: the sign or the band would cross, or no certificate
  uint64_t shared_flip_choice = 0; //     of those reads: the choice changed
  uint64_t shared_flip_band = 0;   //     of those reads: the band changed
  uint64_t incr_checked = 0;       // incremental updates re-read against the judge (--check-shared)
  uint64_t incr_discrepancies = 0; //   of which the judge's sign or band disagreed with the certificate
  uint64_t exposure_holds = 0;     // acts held because the outstanding exposure would exceed the cap
  double   out_max = 0.0;          // the largest outstanding exposure seen at a period's close
  void init(int nc) { acted_by_class.assign(nc, 0); reason_count.assign(RS_N, 0); }
  double kappa() const { return sup_removed_min > 1e-9 ? sup_created_min / sup_removed_min : 9.99; }
};

struct Resident {
  Field   fd;
  Compiled C;
  Transport tr;
  Integrator hand;
  MachineStats st;
  SupervisionMeter sup;
  Writ    wr;
  double  adjudication_budget_min = 0;   // the humans still here, and their hours
  int     NC = 0, NS = 0;
  float   lr = 0.02f;
  std::vector<uint32_t> memo_hash;       // [oid] the frame hash the cell's last proposal was read under
  std::vector<uint32_t> memo_base;       // [oid] E0: the record's hash with the shared fact held out, at that read
  std::vector<uint32_t> epoch_seen;      // [cls] E0: the last shared epoch the machine saw for the class (+1; 0 none)
  bool    check_shared = false;          // E0: re-read every incremental update against the judge and count disagreements (a measurement mode)
  bool    lie_stale_certificate = false; // O37's lie: carry the old direction across a change of the shared fact. Never set outside the battery.
  bool    lie_ignore_cap = false;        // O38's lie: one cell in a hundred acts past the exposure cap. Never set outside the battery.
  bool    lie_reads_clock = false;       // O25's lie: a resident that reads the TICK row's wall value. Never set outside the battery.
  bool    lie_effect_under_off = false;  // O19's lie: a resident that lets one cell through with the switch off. Never set outside the battery.
  bool    lie_silent_proposal = false;   // O30's lie: a proposal folded into the memo without its row. Never set outside the battery.
  bool    lie_stratum_off = false;       // O20's lie: a resident that acts on retained cells of the wide band. Never set outside the battery.
  bool    lie_calib_blind = false;       // O47's lie: a resident that admits a class whose curve is not monotone. Never set outside the battery.
  // E3c: THE CALIBRATION. The counts the curve is measured from (the replay's
  // agreement cases, then every live outcome on a cell whose executed choice
  // was the last proposal's), frozen per term into CALIB rows; the gate reads
  // the frozen curve back from the ledger's fold of those rows, never from here.
  Calib   calib;
  std::vector<float>   cal_dir;          // [oid] |direction| of the cell's last proposal (the live half of the curve)
  std::vector<int8_t>  cal_choice;       // [oid] the last proposal's choice, -1 none
  std::vector<int8_t>  cal_human;        // [oid] the person's decision on the cell, -1 none

  // C1: the resident is told the class count and the seat count, holds the
  // compile step's output and the writ (which no longer carries the salt), and
  // never a World, a ladder or a salt.
  void init(int nc, int ns, const Compiled& comp, const Writ& writ, uint64_t seed) {
    NC = nc; NS = ns; C = comp; wr = writ;
    fd.init(NC, NS, seed); st.init(NC); sup.init(NC); calib.init(NC);
  }

  // D0: the machine reads its day from the ledger, where the TICK row put it.
  void period(Ledger& L, Firm& f, Tape& tape, const Store& store, Judge& judge, Judge& frontier, const Ladder& lic);
  void grade(Ledger& L, Judge& judge, Judge& frontier);
  // E3c: the live half of the curve, folded from the day's rows (PROPOSAL, DECIDE, OUTCOME)
  void calib_ingest(const Tape& tape, size_t from_row);
  // E3c: the term opens: freeze the curve and write it as CALIB rows, one per
  // class per bin, folded into the ledger as they are written so the gate reads
  // the same curve a cold fold reads (O14): the fit on margin, the raw rate on
  // value, the count on b, measured on via, monotone on band
  void calib_open_term(Ledger& L, Tape& tape, uint32_t day) {
    calib.freeze(wr.m_calib);
    for (int c = 0; c < NC; ++c) for (int b = 0; b < CALIB_NBIN; ++b) {
      const size_t k = (size_t)c * CALIB_NBIN + b;
      put_fold(tape, L, R_CALIB, day, 0, c, -1, ARM_MACHINE, b, (int)std::min<long>(calib.n[k], 0x7fffffff),
               calib.fit[k], calib.raw[k], (uint8_t)calib.monotone[c], 0, calib.measured[c] ? 1 : 0, PROV_M);
    }
  }
};

// One period of the resident. The whole thing is a solve, a read budget, a gate
// and a hand. It runs when the world port has folded a TICK: the day is the
// ledger's and nothing else's.
inline void Resident::period(Ledger& L, Firm& f, Tape& tape, const Store& store,
                             Judge& judge, Judge& frontier, const Ladder& lic) {
  const uint32_t day = L.day;
  if (L.sw == SW_STOP) return;                              // D2: stop: the period returns before it reads
  const bool shadow = (L.sw == SW_SHADOW);                  // D2: shadow: verdicts as at live, effects flagged, nothing moves
  ++st.periods;
  // THE LIE (O25): a resident that reads the clock. The TICK row's wall value
  // is on the tape for the operator; the machine reads the day and nothing else.
  const int budget_now = wr.read_budget;
  const bool clock_high = lie_reads_clock && tape.last_tick_at > 0 && tape.rec[tape.last_tick_at - 1].value > 50.f;
  // --- the adjudication budget: the humans who are still here. This is the
  // ceiling on the whole enterprise and it is deliberately small.
  adjudication_budget_min = 0;
  for (const Seat& s : f.seat)
    if (s.fn == FN_WARRANT || s.kind >= SK_MANAGER) adjudication_budget_min += s.attention * 0.45;

  // --- gather the field: every open commitment, all at once. No queue, no
  // per-item loop with a person in it.
  std::vector<uint32_t> rows;
  for (uint32_t i : L.open_idx) {
    const Obligation& o = L.ob[i];
    if (o.state == OB_OPEN || o.state == OB_QUEUED || o.state == OB_INPROG || o.state == OB_ESCALATED)
      rows.push_back(i);
  }
  if (rows.empty()) return;

  const int N = (int)rows.size();
  // seats that can still take work (the humans who remain), plus two stocks
  std::vector<int> seats;
  for (const Seat& s : f.seat) if (s.fn == FN_E && s.kind <= SK_MANAGER) seats.push_back(s.id);
  const int NSEAT = (int)seats.size();
  tr.size_to(N, NSEAT, (int)(0.35 * N + 8), (int)(adjudication_budget_min / 45.0));
  for (int j = 0; j < NSEAT; ++j) tr.cap[j] = std::max(0.5f, f.seat[seats[j]].attn_left / 90.f);
  for (int i = 0; i < N; ++i) tr.supply[i] = 1.f;

  // --- the cost of putting row i in column j. ONE PLACE. What the machine
  // believes a frame of class c holds is the compile step's coverage (C1),
  // never the plant's completeness; the field prices every cell on it before
  // any judge is read, so the transport needs no read.
  std::vector<float> mach_complete(N), lateness(N);
  std::vector<int>   rcls(N);
  for (int i = 0; i < N; ++i) {
    const Obligation& o = L.ob[rows[i]];
    rcls[i] = o.cls;
    mach_complete[i] = C.coverage_hat(o.cls);
    lateness[i] = (float)((int)day - (int)o.day_due) / 7.f;
  }
  float ftmp[FT_N];
  auto cost = [&](int i, int j) -> float {
    const int c = rcls[i]; const ClassSpec& sp = cls_spec(c);
    if (j >= NSEAT) {
      const int stk = j - NSEAT;
      if (stk == STOCK_UNPLACED) return wr.w_unplaced * (1.f + std::max(0.f, lateness[i]));
      return 0.85f;                                   // unadjudicated: cheaper than a bad call, dearer than a good one
    }
    const int sid = seats[j];
    if (c < 32 && !((f.seat[sid].spec >> c) & 1u)) return 1e30f;   // law: -inf BEFORE normalisation
    const float sk = fd.skill_of(sid, c);
    Field::feats(sp, C.decide_frac[c], mach_complete[i], sk, lateness[i], 1.f - tr.cap[j] / 8.f, (float)L.ob[rows[i]].hops, ftmp);
    const float pg = Field::sigm(fd.logit(c, ftmp));
    return (1.f - pg) * 1.0f + 0.10f * std::max(0.f, lateness[i]);
  };

  tr.run(cost, 12);

  // --- THE READ BUDGET (C1). Behind the port a read is a forward pass. A cell
  // whose frame hash is unchanged since its last proposal carries that proposal
  // forward from the ledger's memo (a fold of PROPOSAL rows); a cell with no
  // valid proposal, or one drawn by the floor sample, is a candidate for a read.
  // Candidates are read ripest first up to the writ's budget; a cell the budget
  // never reached holds with reason UNREAD. The floor sample is the kernel's
  // own hygiene (it checks the judge's consistency, not the arena) and is drawn
  // on the kernel's key, never the governor's salt.
  std::vector<Frame> frames(N);
  std::vector<uint8_t> has_prop(N, 0), needs_read(N, 0), shared_read(N, 0);
  std::vector<int> cand;
  if ((int)epoch_seen.size() < NC) epoch_seen.assign(NC, 0u);
  for (int i = 0; i < N; ++i) {
    const Obligation& o = L.ob[rows[i]]; const int c = o.cls;
    Frame fr = store.frame(o.id, c, C.join_graph[c], day);
    fr.frame_hash ^= C.template_hash[c];                       // the memo key: the record and the template, never the clock
    fr.base_hash  ^= C.template_hash[c];
    fr.coverage_hat = mach_complete[i];
    frames[i] = fr;
    if (o.id >= memo_hash.size()) { memo_hash.resize((size_t)o.id + 1024, 0u); memo_base.resize((size_t)o.id + 1024, 0u); }
    if (fr.shared_epoch + 1 > epoch_seen[c]) { if (epoch_seen[c] != 0) ++st.shared_events; epoch_seen[c] = fr.shared_epoch + 1; }
    const Ledger::LastProp* m = L.memo(o.id);
    const bool same_judge = m && m->judge_hash == judge.hash();
    bool valid = same_judge && memo_hash[o.id] == fr.frame_hash;
    // E0: ONLY THE SHARED FACT MOVED. The record's own hash still matches and
    // the fact's epoch does not: the proposal is carried across the change by
    // the judge's certificate when the sign and the band both survive, as a
    // PROPOSAL row with via 1 and its SENSE row, so O30 re-derives the effect
    // and a fold re-derives the carry; otherwise the cell is read again.
    if (!valid && same_judge && memo_base[o.id] == fr.base_hash && m->shared_epoch != fr.shared_epoch) {
      ++st.shared_affected;
      bool carried = false;
      if (m->has_sens && !lie_stale_certificate) {
        const float dir_new = m->direction + 4.0f * m->sens * (0.35f + 0.65f * m->completeness_hat) * (fr.shared_value - m->shared_x);
        // the certificate carries only with a margin of safety at the sign and at
        // both band edges: a direction within CERT_TOL of an edge is read again,
        // so float rounding between the carry and a fresh read can never differ
        const float tol = 1e-3f; const int old_b = band_of(m->direction, wr);
        if (std::fabs(dir_new) > tol && (dir_new > 0.f) == (m->direction > 0.f)
            && band_of(dir_new - tol, wr) == old_b && band_of(dir_new + tol, wr) == old_b) {
          const int b = band_of(dir_new, wr);
          put_fold(tape, L, R_PROPOSAL, day, o.id, c, -1, ARM_MACHINE, m->choice, (int)m->judge_hash, dir_new, m->completeness_hat, b, 0, 1, PROV_M);
          put_fold(tape, L, R_SENSE, day, o.id, c, -1, ARM_MACHINE, (int)fr.shared_epoch, 1, m->sens, fr.shared_value, b, 0, 0, PROV_M);
          memo_hash[o.id] = fr.frame_hash; memo_base[o.id] = fr.base_hash;
          carried = true; ++st.incr_updates;
          if (check_shared) {                                   // the measurement mode: the judge's own read against the certificate
            const Proposal chk = judge.read(fr);
            const float dir_chk = 4.0f * chk.signal * (0.35f + 0.65f * chk.completeness_hat);
            ++st.incr_checked;
            if ((dir_chk > 0.f) != (dir_new > 0.f) || band_of(dir_chk, wr) != b) ++st.incr_discrepancies;
          }
        }
      } else if (lie_stale_certificate) {                       // THE LIE (O37): the old proposal carried as if nothing moved
        memo_hash[o.id] = fr.frame_hash; memo_base[o.id] = fr.base_hash; carried = true; ++st.incr_updates;
      }
      if (carried) valid = true; else shared_read[i] = 1;
    }
    const bool floor = valid && !clock_high && (u01(0x524541444BULL /*'READK'*/, 7400 + c, o.id * 131u + day) < wr.eps_floor);   // clock_high is O25's lie
    if (valid) has_prop[i] = 1;
    if (!valid || floor) { needs_read[i] = 1; cand.push_back(i); }
  }
  std::sort(cand.begin(), cand.end(), [&](int x, int y) {
    if (lateness[x] != lateness[y]) return lateness[x] > lateness[y];          // ripest first
    return L.ob[rows[x]].id < L.ob[rows[y]].id; });
  int reads_now = 0;
  for (int i : cand) {
    if (reads_now >= budget_now) break;
    const Obligation& o = L.ob[rows[i]]; const int c = o.cls;
    const Proposal mr = judge.read(frames[i]);
    const float direction = 4.0f * mr.signal * (0.35f + 0.65f * mr.completeness_hat);
    const int   b = band_of(direction, wr);
    if (shared_read[i]) {                                        // E0: a read the shared fact forced: did anything change?
      ++st.shared_full_reads;
      const Ledger::LastProp* old = L.memo(o.id);
      if (old) { if (old->choice != mr.choice) ++st.shared_flip_choice; if (band_of(old->direction, wr) != b) ++st.shared_flip_band; }
    }
    // v2: the proposal is a row BEFORE the verdict, whatever the gate then says.
    // It feeds nothing; it is what the machine thought, on the record. C1: it is
    // written on a read only, and folded into the ledger's memo as it is written.
    // a floor re-read is a read of a cell that already carried a valid proposal
    if (has_prop[i]) ++st.reads_floor; else ++st.reads_fresh;
    if (lie_silent_proposal && (o.id % 50u) == 3u)             // THE LIE (O30): a proposal the memo knows and the tape never saw
      L.apply(Tape::make(R_PROPOSAL, day, o.id, c, -1, ARM_MACHINE, mr.choice, (int)mr.judge_hash, direction, mr.completeness_hat, b, 0, 0, PROV_M));
    else
      put_fold(tape, L, R_PROPOSAL, day, o.id, c, -1, ARM_MACHINE, mr.choice, (int)mr.judge_hash, direction, mr.completeness_hat, b, 0, 0, PROV_M);
    if (mr.has_sens)                                             // E0: the certificate beside the proposal, on the record
      put_fold(tape, L, R_SENSE, day, o.id, c, -1, ARM_MACHINE, (int)frames[i].shared_epoch, 1, mr.sensitivity, frames[i].shared_value, b, 0, 0, PROV_M);
    memo_hash[o.id] = frames[i].frame_hash; memo_base[o.id] = frames[i].base_hash;
    has_prop[i] = 1; ++reads_now; ++st.reads;
  }
  for (int i = 0; i < N; ++i) if (has_prop[i] && !needs_read[i]) ++st.memo_hits;
  if (reads_now >= budget_now && (int)cand.size() > reads_now) ++st.budget_bound;
  // the forgone dual: what the budget left unread, priced by the field's own duals
  for (int i = 0; i < N; ++i) { st.total_dual += std::fabs(tr.u[i]); if (!has_prop[i]) st.forgone_dual += std::fabs(tr.u[i]); }

  // --- decide each row: the resident's own choice, its direction, its band
  int adj_used = 0;
  for (int i = 0; i < N; ++i) {
    const uint32_t idx = rows[i];
    Obligation& o = L.ob[idx];
    const int c = o.cls; const ClassSpec& sp = cls_spec(c);

    // D0: the machine's hold is written when it changes (reason or band) and
    // stands until the cell is touched by an effect or the next hold differs
    auto mhold = [&](int reason, int band, float direction_) {
      ++st.held; ++st.reason_count[reason];
      if (o.mhold_reason == (uint8_t)(reason + 1) && o.mhold_band == (uint8_t)band) return;
      o.mhold_reason = (uint8_t)(reason + 1); o.mhold_band = (uint8_t)band;
      tape.put(R_HOLD, day, o.id, c, -1, ARM_MACHINE, reason, band, direction_, sp.value, band, 0, 0, PROV_M);
    };
    if (!has_prop[i]) {                                          // the budget never reached it
      ++st.unread; mhold(RS_UNREAD, 0, 0.f);
      continue;
    }
    const Ledger::LastProp& m = *L.memo(o.id);
    // DIRECTION is signed, and it is the only margin the gate reads. Entropy
    // would say how unsure the field is and never which way it leans.
    const float direction = m.direction;
    const int   b = band_of(direction, wr);
    mach_complete[i] = m.completeness_hat;
    const float supp = tr.support(cost, i);
    const float sharp = std::max(0.f, (supp - 1.6f));
    // NOVELTY IS ABOUT THE CASE, NOT THE QUEUE. The first run added lateness to
    // the conformal score, so a backlog weeks late read as "unlike anything
    // seen" and 60,221 decisions went to warrant seats as novel. A late case is
    // a case the resident should drain, and lateness already reaches the head
    // as its own feature and the writ as its own cost.
    const float nov = fd.novelty(c, std::fabs(mach_complete[i] - 0.7f));
    const bool blocked = (o.dep >= 0 && L.ob[o.dep].state != OB_SETTLED && L.ob[o.dep].state != OB_DECIDED);

    // the strata reach the gate as rows the governor wrote before this period;
    // the rung reaches it as the table the governor keeps, keyed to this judge
    const int stratum = L.stratum_of(o.id);
    GateIn g{}; g.cls = c; g.rung = lic.rung_for(c, b, judge.hash()); g.band = b;
    g.direction = direction; g.sharpness = sharp; g.novelty = nov;
    g.reversible = sp.reversible; g.warrant_reserved = sp.warrant; g.blocked = blocked;
    g.in_canary = (stratum == 1);
    g.in_audit  = (stratum == 2);
    g.to_incumbent = (stratum == 3);                              // E3: the retained stratum holds for a person, at every rung
    if (lie_stratum_off && b == 2) g.to_incumbent = false;        // THE LIE (O20): the wide band's control arm switched off
    g.budget_left = (float)(adjudication_budget_min - adj_used * 45.0);
    g.sw = L.sw;
    g.value = sp.value;                                           // E0: what an unattended act would put in flight
    g.exposure_left = (wr.exposure_cap > 0.f) ? (float)((double)wr.exposure_cap - L.outstanding_total) : 1e30f;
    // E3c: the calibrated wrong-rate and the key's admissibility, from the ledger's fold of this term's CALIB rows
    g.calib_measured = L.calib_measured_of(c) && L.calib_monotone_of(c);
    g.calib_wrong = L.calib_wrong_of(c, std::fabs(direction));
    if (lie_calib_blind && L.calib_measured_of(c)) g.calib_measured = true;   // THE LIE (O47): a non-monotone curve admitted
    if (lie_ignore_cap && (o.id % 100u) == 7u) g.exposure_left = 1e30f;   // THE LIE (O38)
    if (lie_effect_under_off && L.sw == SW_OFF && (i % 500) == 7) g.sw = SW_LIVE;   // THE LIE (O19)

    const GateOut v = gate(g, wr);
    if (v.verdict != V_HOLD) ++st.reason_count[v.reason];       // a hold counts its reason where it is written
    st.margin_abs.add(std::fabs(direction));
    fd.calib_push(c, std::fabs(mach_complete[i] - 0.7f));

    // the human minutes this obligation WOULD have cost, which is the
    // denominator of kappa: metered from the firm's own rows for this class
    // (C1), never a formula over planted constants
    const double would_cost = C.minutes_per_decision[c];

    switch (v.verdict) {
      case V_ACT: {
        if (!shadow) o.completeness = mach_complete[i];
        if (!hand.commit(L, tape, idx, m.choice, day, direction, b, 1, nov, sharp, shadow)) break;   // via 1: the wager (a shadow row only on change)
        ++st.acted; ++st.acted_by_class[c];
        st.completeness.add(o.completeness);
        st.sup_removed_min += would_cost; sup.add_removed(c, b, would_cost);
        if (v.reason == RS_CANARY) ++st.canary;
        break;
      }
      case V_FRONTIER: {
        // E0: the rented mind's act is the wager too; it consumes exposure like
        // an unattended act, so the cap holds it before the rental is paid
        if (g.exposure_left < g.value) { ++st.exposure_holds; mhold(RS_EXPOSURE, b, direction); break; }
        // rent a bigger mind for this one cell. It costs money and, because a
        // human still reads the answer at low rungs, a little supervision.
        ++st.frontier; st.frontier_calls += 1.0;
        // the rented mind reads the same frame, with less noise. It cannot
        // buy coverage: a fact nobody recorded is not available at any price.
        const Proposal fr = frontier.read(frames[i]);
        // E4: THE RENTAL IS A ROW. A rented mind's proposal, keyed on the frame hash it
        // was read under (a = its judge hash, b = the frame hash), so the counsel memo of
        // E1' has its fold and the account can count rentals against distinct cells.
        tape.put(R_COUNSEL, day, o.id, c, -1, ARM_MACHINE, (int)fr.judge_hash, (int)memo_hash[o.id],
                 4.0f * fr.signal * (0.35f + 0.65f * fr.completeness_hat), fr.completeness_hat, b, shadow ? RF_SHADOW : 0, 3, PROV_M);
        const float boost = 0.72f;
        if (frontier.act_coin(c, o.id, boost)) {               // F16: the coin is the judge's, on its own key
          if (!shadow) o.completeness = fr.completeness_hat;
          if (!hand.commit(L, tape, idx, fr.choice, day,
                           direction + (direction > 0 ? 0.9f : -0.9f), b, 3, nov, sharp, shadow)) break;   // via 3: a rented mind acted
          ++st.acted; ++st.acted_by_class[c];
          st.sup_removed_min += would_cost; sup.add_removed(c, b, would_cost);
        } else {
          if (!shadow) o.state = OB_ESCALATED;
          ++st.warrant; ++adj_used;
          st.sup_created_min += 22.0; sup.add_created(c, b, 22.0);
          // v2: a = the seat that keeps it (the resident escalates without moving it), so a fold does not reseat the cell
          tape.put(R_ESCALATE, day, o.id, c, -1, ARM_MACHINE, o.seat, o.escalations, direction, sp.value, b, shadow ? RF_SHADOW : 0, 0, PROV_M);
        }
        break;
      }
      case V_DRAFT: {
        // the resident prepares; a person presses the key. This is real
        // supervision created, and it is why a draft is not free.
        if (!shadow) o.completeness = mach_complete[i];
        if (!hand.commit(L, tape, idx, m.choice, day, direction, b, 2, nov, sharp, shadow)) break;   // via 2: assisted, never the wager
        ++st.drafted; ++adj_used;
        const double review = 4.0 + 9.0 * C.decide_frac[c];
        st.sup_created_min += review; sup.add_created(c, b, review);
        st.sup_removed_min += would_cost - review;
        sup.add_removed(c, b, std::max(0.0, would_cost - review));
        if (v.reason == RS_AUDIT) ++st.audit;
        break;
      }
      case V_WARRANT: {
        // the signer executes the machine's choice verbatim on the stratum; a
        // refusal is a veto ROW and grades nothing
        if (!shadow) o.completeness = mach_complete[i];
        if (!hand.commit(L, tape, idx, m.choice, day, direction, b, 4, nov, sharp, shadow)) break;   // via 4: assisted, never the wager
        ++st.warrant; ++adj_used;
        const double sign = 12.0 + 20.0 * C.decide_frac[c];
        st.sup_created_min += sign; sup.add_created(c, b, sign);
        st.sup_removed_min += std::max(0.0, would_cost - sign);
        sup.add_removed(c, b, std::max(0.0, would_cost - sign));
        break;
      }
      default: {
        if (v.reason == RS_EXPOSURE) ++st.exposure_holds;
        mhold(v.reason, b, direction);
        break;
      }
    }
  }
  st.out_max = std::max(st.out_max, L.outstanding_total);      // E0: the exposure ledger at the period's close
}

// Outcomes arrive. The field learns from them; the judge is told about the cells
// it decided. Nothing here widens a licence: that is the governor's fold of the
// same OUTCOME rows.
// E3c: THE LIVE HALF OF THE CALIBRATION CURVE, from the day's rows. A PROPOSAL
// row names the last proposal's choice and direction; a DECIDE row names the
// person's decision; an OUTCOME row on a cell whose executed choice was the
// last proposal's grades the machine's confidence: via 1 (the wager), 2 (a
// person keyed the draft), 4 (a signer executed it), or via 0 where the person
// chose what the proposal said. The frontier's own choice (via 3) grades the
// frontier, not this judge, and is left out.
inline void Resident::calib_ingest(const Tape& tape, size_t from_row) {
  for (size_t i = from_row; i < tape.rec.size(); ++i) {
    const Rec& r = tape.rec[i];
    if (r.oid == 0) continue;
    if (r.oid >= cal_dir.size()) { const size_t n = (size_t)r.oid + 1024; cal_dir.resize(n, 0.f); cal_choice.resize(n, -1); cal_human.resize(n, -1); }
    if (r.type == R_PROPOSAL && r.prov == PROV_M) { cal_dir[r.oid] = std::fabs(r.margin); cal_choice[r.oid] = (int8_t)r.a; }
    else if (r.type == R_DECIDE) { cal_human[r.oid] = (int8_t)r.a; }
    else if (r.type == R_OUTCOME && r.a != OK_UNRESOLVED && r.cls < NC && cal_choice[r.oid] >= 0) {
      const bool executed_machine_choice = (r.via == 1 || r.via == 2 || r.via == 4) || (r.via == 0 && cal_human[r.oid] == cal_choice[r.oid]);
      if (executed_machine_choice) calib.push(r.cls, cal_dir[r.oid], r.a == OK_BAD);
    }
  }
}

inline void Resident::grade(Ledger& L, Judge& judge, Judge& frontier) {
  const uint32_t day = L.day;
  float ftmp[FT_N];
  for (Obligation& o : L.ob) {
    if (o.state != OB_SETTLED || o.day_settled != day) continue;
    if (o.outcome == OK_UNRESOLVED) continue;                    // E0: a write-off is no evidence, for the head or the judge
    const int c = o.cls; const ClassSpec& sp = cls_spec(c);
    // CORRECTNESS AND TIMELINESS ARE DIFFERENT QUANTITIES AND MUST NOT SHARE A
    // GRADER. Whether the answer was right is a property of the decider; whether
    // it was on time is a property of the queue it sat in. The head and the
    // competence estimate train on correctness; the LICENCE is graded on the
    // writ's own metric, which counts lateness, because that is what the firm
    // pays for. Conflating them makes competence unrecoverable — a busy expert
    // and a idle novice look identical.
    const bool right = (o.outcome != OK_BAD);
    // train on the arrival, with a Horvitz–Thompson weight for the canary
    // stratum so a field that routes attention by its own estimate does not
    // starve its own evidence. The weight belongs to the canary stratum only; a
    // draft was not drawn by the salt and carries no selection probability to
    // invert. The rate is the one the governor drew this cell against, read
    // off the stratum row it wrote (C1): the kernel never computes a rate.
    const float ipw = (o.via == 1) ? std::min(6.f, 1.f / std::max(L.stratum_rate_of(o.id), 0.05f)) : 1.f;
    Field::feats(sp, C.decide_frac[c], o.completeness, fd.skill_of(o.seat, c),
                 (float)((int)o.day_decided - (int)o.day_due) / 7.f, 0.f, (float)o.hops, ftmp);
    fd.learn(c, ftmp, right ? 1 : 0, lr, ipw);
    if (!o.by_machine) {
      // predict with skill held NEUTRAL, so what the update sees is the seat
      float fn_[FT_N];
      Field::feats(sp, C.decide_frac[c], o.completeness, 0.5f,
                   (float)((int)o.day_decided - (int)o.day_due) / 7.f, 0.f, (float)o.hops, fn_);
      fd.observe_skill(o.seat, c, right ? 1 : 0, Field::sigm(fd.logit(c, fn_)));
    } else if (o.via == 3) frontier.observe(c, right);
    else                   judge.observe(c, right);
  }
}

} // namespace acme
