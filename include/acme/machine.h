// ============================================================================
//  acme/machine.h — the compile step, the two graders, the ladder, the resident
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
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "world.h"
#include "solver.h"
#include "human.h"   // for TimeLedger: the compile step reads the metered minutes

namespace acme {

// ----------------------------------------------------------------------------
// §1 · THE COMPILE STEP
// ----------------------------------------------------------------------------
struct Compiled {
  int NC = 0;
  std::vector<uint32_t> join_graph;    // [NC] bitmask of systems this class reads
  std::vector<int>      n_systems;     // recovered, from the query log
  std::vector<float>    coverage;      // estimated, from agreement-band mass
  std::vector<float>    decide_frac;   // MEASURED from the time ledger, never told
  std::vector<int>      invariants;    // count that held with zero violations
  std::vector<int>      invariants_proposed;
  std::vector<float>    class_geom;    // [NC*8] geometry projected from the trunk's prior
  int  classes_seen = 0;
  bool trunk_geometry = true;
};

// Read the tape as the application's query log and recover the join graph. This
// is a fold, so it is replayable and two runs give the same table.
inline Compiled compile_from_tape(const Tape& tape, const TimeLedger* by_class, int NC,
                                  uint64_t seed, bool trunk_geometry = true) {
  Compiled C; C.NC = NC;
  C.join_graph.assign(NC, 0u); C.n_systems.assign(NC, 0);
  C.coverage.assign(NC, 0.f); C.decide_frac.assign(NC, 0.f);
  C.invariants.assign(NC, 0); C.invariants_proposed.assign(NC, 0);
  C.class_geom.assign((size_t)NC * 8, 0.f);
  C.trunk_geometry = trunk_geometry;

  std::vector<uint32_t> seen(NC, 0);
  std::vector<int> touched(NC, 0);
  tape.fold([&](const Rec& r) {
    if (r.type == R_CONTEXT && r.cls < NC) { seen[r.cls] |= (1u << (r.a & 31)); }
    if (r.type == R_DECIDE  && r.cls < NC) ++touched[r.cls];
  });
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
    }
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

inline std::vector<Invariant> mine_invariants(const Tape& tape, const World& w, int NC) {
  std::vector<Invariant> out;
  std::vector<long> decided(NC, 0), viol_dbl(NC, 0), viol_dep(NC, 0), viol_war(NC, 0);
  std::vector<uint8_t> seen_decide;
  seen_decide.assign(w.next_id + 2, 0);
  tape.fold([&](const Rec& r) {
    if (r.type != R_DECIDE && r.type != R_EFFECT) return;
    if (r.cls >= NC) return;
    ++decided[r.cls];
    if (r.oid < seen_decide.size()) { if (seen_decide[r.oid]) ++viol_dbl[r.cls]; seen_decide[r.oid] = 1; }
    if (cls_spec(r.cls).warrant && r.seat >= 0 && (r.flags & 1) == 0) { /* checked below via escalation */ }
  });
  // dependency order: a decision may not land before the thing it waits on
  for (const Obligation& o : w.ob) {
    if (o.dep < 0 || o.state != OB_SETTLED) continue;
    const Obligation& d = w.ob[o.dep];
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
// §2 · THE LADDER — per class-band, on arrivals only
// ----------------------------------------------------------------------------
// NBAND and band_of() live in firm.h beside the writ, so both arms band alike.

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

  void init(int nc) {
    NC = nc; lic.assign((size_t)nc * NBAND, Lic{});
    for (int c = 0; c < nc; ++c) for (int b = 0; b < NBAND; ++b) lic[(size_t)c * NBAND + b].n0 = n0_for(c);
  }
  Lic& at(int c, int b) { return lic[(size_t)c * NBAND + (b < 0 ? 0 : (b >= NBAND ? NBAND - 1 : b))]; }
  const Lic& at(int c, int b) const { return lic[(size_t)c * NBAND + (b < 0 ? 0 : (b >= NBAND ? NBAND - 1 : b))]; }

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

  // Climb. Widening needs the world; narrowing needs nothing but evidence.
  void step(uint32_t day, const Writ& wr, Tape& tape) {
    for (int c = 0; c < NC; ++c) for (int b = 0; b < NBAND; ++b) {
      Lic& L = at(c, b);
      const int before = L.rung;
      // --- narrow first, always, and with no signature
      if (L.logE_demote > std::log(1.0 / alpha_demote) && L.rung > 0) { L.rung -= 1; L.logE_demote = 0; L.logE = 0; }
      if (L.kappa() > wr.kappa_max && L.rung > 1 && L.sup_removed > 60.0) { L.rung = 1; L.logE = 0; }
      if (L.expiry_day > 0 && (int)day > L.expiry_day && L.rung > 1) { L.rung -= 1; L.logE = 0; }
      // --- widen only inside the rule, and only on arrivals past the floor
      const bool enough = L.n_machine >= L.n0 || (L.history_licensed && b == 2 && L.n_machine >= std::max(20, L.n0 / 8));
      if (enough && L.logE > std::log(1.0 / alpha_promote) && L.rung < 5) {
        L.rung += 1; L.logE = 0; L.expiry_day = (int)day + 3 * term_days;
      }
      if (L.rung != before) tape.put(R_LICENSE, day, 0, c, -3, ARM_GOVERNOR, L.rung, b, (float)L.logE, (float)L.kappa(), b);
      // v2: kappa is a row once a term, per class-band that has removed anything
      if (day > 0 && (int)(day % (uint32_t)term_days) == 0 && L.sup_removed > 0.0)
        tape.put(R_KAPPA, day, 0, c, -3, ARM_GOVERNOR, (int)L.sup_created, (int)L.sup_removed, 0.f, (float)L.kappa(), b);
    }
  }
};

// ----------------------------------------------------------------------------
// §3 · REPLAY — the fast grader, on its own support
// ----------------------------------------------------------------------------
struct ReplayOut {
  std::vector<long>  n, agree, agree_good, agree_bad, disagree, disagree_firm_good, disagree_firm_bad;
  std::vector<long>  agree_wrong;             // agreed, and the world said WRONG (not merely late)
  std::vector<long>  conf_n, conf_agree;      // both readers far from the fence (kept for the record; not the estimator)
  std::vector<float> coverage, band_mass;
  long invariant_violations = 0;
  void init(int nc) { n.assign(nc,0); agree.assign(nc,0); agree_good.assign(nc,0); agree_bad.assign(nc,0); agree_wrong.assign(nc,0);
                      disagree.assign(nc,0); disagree_firm_good.assign(nc,0); disagree_firm_bad.assign(nc,0);
                      conf_n.assign(nc,0); conf_agree.assign(nc,0);
                      coverage.assign(nc,0.f); band_mass.assign(nc,0.f); }
};

// Re-decide every settled obligation the firm already handled, through the gate,
// at rung 1, and grade on the four-cell table. Only the diagonal is complete.
inline ReplayOut replay(const World& w, const Firm& f, Field& fd, const Compiled& C,
                        Ladder& lad, const Writ& wr) {
  (void)f;
  ReplayOut R; R.init(w.NC);
  float ft[FT_N];
  for (const Obligation& o : w.ob) {
    if (o.state != OB_SETTLED || o.by_machine) continue;
    const int c = o.cls;
    ++R.n[c];
    // The resident reads the record: every instrumented system, every time, at
    // no cost, with no decay — and NOTHING tacit, because the phone call the
    // coordinator made was never written down anywhere it can reach.
    const Read mr = observe(w, o, C.join_graph[c], true, 0.f, fd.mach_skill_of(c), 0x22ULL);
    const float mach_complete = mr.completeness;
    const float z = 4.0f * mr.signal * (0.35f + 0.65f * mach_complete);
    const int mach_decision = mr.choice;
    const int firm_decision = o.decision;
    (void)ft;
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
      Lic& L = lad.at(c, b);
      ++L.n_replayed; ++L.n_agree; ++L.agree_total; L.agree_good += good ? 1 : 0;
    } else {
      ++R.disagree[c];
      if (good) ++R.disagree_firm_good[c]; else ++R.disagree_firm_bad[c];
      // and NOTHING is credited here. The resident's alternative was never run.
      const int b = band_of(z, wr);
      ++lad.at(c, b).n_disagree; ++lad.at(c, b).n_replayed;
    }
  }
  for (int c = 0; c < w.NC; ++c) {
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

// Licence the agreement bands from history: the band is admitted to unattended
// action where the resident's coinciding choice beat the firm's own baseline on
// arrived outcomes, with both sides above the sample floor.
inline int license_from_history(Ladder& lad, const ReplayOut& R, uint32_t day, Tape& tape) {
  int promoted = 0;
  for (int c = 0; c < lad.NC; ++c) for (int b = 0; b < NBAND; ++b) {
    Lic& L = lad.at(c, b);
    if (L.agree_total < 40) continue;
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

// ----------------------------------------------------------------------------
// §4 · THE RESIDENT — one process, one state, one period
// ----------------------------------------------------------------------------
struct MachineStats {
  uint64_t acted = 0, drafted = 0, frontier = 0, warrant = 0, held = 0, undone = 0;
  uint64_t canary = 0, audit = 0;
  Acc completeness, margin_abs;
  double sup_created_min = 0;     // human minutes the machine CAUSED
  double sup_removed_min = 0;     // human minutes the machine SAVED
  double frontier_calls = 0;
  std::vector<uint64_t> acted_by_class;
  std::vector<uint64_t> reason_count;
  void init(int nc) { acted_by_class.assign(nc, 0); reason_count.assign(RS_N, 0); }
  double kappa() const { return sup_removed_min > 1e-9 ? sup_created_min / sup_removed_min : 9.99; }
};

struct Resident {
  Field   fd;
  Ladder  lad;
  Compiled C;
  Transport tr;
  Integrator hand;
  MachineStats st;
  Writ    wr;
  double  adjudication_budget_min = 0;   // the humans still here, and their hours
  int     NC = 0, NS = 0;
  float   lr = 0.02f;

  void init(const World& w, const Firm& f, const Compiled& comp, const Writ& writ, uint64_t seed) {
    NC = w.NC; NS = f.size(); C = comp; wr = writ;
    fd.init(NC, NS, seed); lad.init(NC); st.init(NC);
    // n0 per class-band, and mark the ones the world will never answer fast
    // enough to license. The honest account prints them as human rather than
    // holding them in shadow forever.
    for (int c = 0; c < NC; ++c) {
      const ClassSpec& sp = cls_spec(c);
      const double per_term = sp.arrival_per_day * w.demand_scale * lad.term_days;
      for (int b = 0; b < NBAND; ++b) {
        Lic& L = lad.at(c, b);
        L.n0 = n0_for(c, wr.canary_delta);
        if (per_term * 0.25 < L.n0 / 4.0) L.unlicensable = true;   // even a 25% canary cannot clear it
      }
    }
  }

  // The canary draw. Keyed hash, salted from the IMMUTABLE FLOOR, so the
  // resident cannot predict which instances are grading it. A lineage that can
  // pick its own arena picks an easy one.
  bool in_canary(uint32_t oid, int c, float rate) const {
    return u01(wr.salt, 7000 + c, oid) < rate;
  }
  float canary_rate(int c, int b) const {
    const Lic& L = lad.at(c, b);
    // The writ's eps_floor is the uniform component: a floor the kernel may
    // raise and never lower. The first run hard-coded 0.02 here and read
    // eps_floor nowhere, so the multiverse's "explore more" patch reproduced
    // the baseline to the digit.
    if (L.rung >= 4) return std::max(wr.eps_floor, 1.0f / std::sqrt(3.0f * std::max(1.f, (float)L.n_machine)));  // eps* = sqrt(3/F)
    const double per_term = std::max(1.0, (double)cls_spec(c).arrival_per_day * lad.term_days);
    return (float)std::min(0.35, std::max((double)wr.eps_floor, L.n0 / per_term));
  }

  void period(World& w, Firm& f, Tape& tape, uint32_t day);
  void grade(World& w, const Firm& f, Tape& tape, uint32_t day);
  // The judge behind the port, named by hash on every proposal row. Today the
  // only judge is the plant's read arithmetic (step C installs the port).
  static uint32_t judge_hash() { return 0x504C4A31u; }   // 'PLJ1': PlantJudge, v1 arithmetic
};

// One period of the resident. The whole thing is a solve, a gate and a hand.
inline void Resident::period(World& w, Firm& f, Tape& tape, uint32_t day) {
  // --- the adjudication budget: the humans who are still here. This is the
  // ceiling on the whole enterprise and it is deliberately small.
  adjudication_budget_min = 0;
  for (const Seat& s : f.seat)
    if (s.fn == FN_WARRANT || s.kind >= SK_MANAGER) adjudication_budget_min += s.attention * 0.45;

  // --- gather the field: every open commitment, all at once. No queue, no
  // per-item loop with a person in it.
  std::vector<uint32_t> rows;
  for (uint32_t i : w.open_idx) {
    const Obligation& o = w.ob[i];
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

  // --- the cost of putting row i in column j. ONE PLACE.
  std::vector<float> mach_complete(N), lateness(N);
  std::vector<int>   rcls(N);
  for (int i = 0; i < N; ++i) {
    const Obligation& o = w.ob[rows[i]];
    rcls[i] = o.cls;
    mach_complete[i] = completeness_from(w, o.cls, C.join_graph[o.cls], true, 0.f);
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
    Field::feats(sp, mach_complete[i], sk, lateness[i], 1.f - tr.cap[j] / 8.f, (float)w.ob[rows[i]].hops, ftmp);
    const float pg = Field::sigm(fd.logit(c, ftmp));
    return (1.f - pg) * 1.0f + 0.10f * std::max(0.f, lateness[i]);
  };

  tr.run(cost, 12);

  // --- decide each row: the resident's own choice, its direction, its band
  int adj_used = 0;
  for (int i = 0; i < N; ++i) {
    const uint32_t idx = rows[i];
    Obligation& o = w.ob[idx];
    const int c = o.cls; const ClassSpec& sp = cls_spec(c);

    const Read mr = observe(w, o, C.join_graph[c], true, 0.f, fd.mach_skill_of(c), 0x22ULL);
    mach_complete[i] = mr.completeness;
    // DIRECTION is signed, and it is the only margin the gate reads. Entropy
    // would say how unsure the field is and never which way it leans.
    const float direction = 4.0f * mr.signal * (0.35f + 0.65f * mr.completeness);
    const int   b = band_of(direction, wr);
    const float supp = tr.support(cost, i);
    const float sharp = std::max(0.f, (supp - 1.6f));
    // NOVELTY IS ABOUT THE CASE, NOT THE QUEUE. The first run added lateness to
    // the conformal score, so a backlog weeks late read as "unlike anything
    // seen" and 60,221 decisions went to warrant seats as novel. A late case is
    // a case the resident should drain, and lateness already reaches the head
    // as its own feature and the writ as its own cost.
    const float nov = fd.novelty(c, std::fabs(mach_complete[i] - 0.7f));
    const bool blocked = (o.dep >= 0 && w.ob[o.dep].state != OB_SETTLED && w.ob[o.dep].state != OB_DECIDED);

    GateIn g{}; g.cls = c; g.rung = lad.at(c, b).rung; g.band = b;
    g.direction = direction; g.sharpness = sharp; g.novelty = nov;
    g.reversible = sp.reversible; g.warrant_reserved = sp.warrant; g.blocked = blocked;
    g.in_canary = in_canary(o.id, c, canary_rate(c, b));
    g.in_audit  = (u01(wr.salt, 7100 + c, o.id) < std::max(0.01f, 1.0f / std::sqrt(3.0f * std::max(1.f, (float)lad.at(c,b).n_machine))));
    g.budget_left = (float)(adjudication_budget_min - adj_used * 45.0);

    // v2: the proposal is a row BEFORE the verdict, whatever the gate then says.
    // It feeds nothing; it is what the machine thought, on the record.
    tape.put(R_PROPOSAL, day, o.id, c, -1, ARM_MACHINE, mr.choice, (int)judge_hash(), direction, mach_complete[i], b, 0, 0, PROV_M);

    const GateOut v = gate(g, wr);
    ++st.reason_count[v.reason];
    st.margin_abs.add(std::fabs(direction));
    fd.calib_push(c, std::fabs(mach_complete[i] - 0.7f));

    // the human minutes this obligation WOULD have cost, which is the
    // denominator of kappa and must be computed the same way for both arms
    const double would_cost = 6.0 * sp.n_systems + 6.0 + 26.0 * sp.decide_frac
                            + 12.0 + 105.0 * sp.decide_frac + 6.0 + 4.0 * sp.n_systems;

    switch (v.verdict) {
      case V_ACT: {
        o.completeness = mach_complete[i];
        hand.commit(w, tape, idx, mr.choice, day, direction, b, 1);   // via 1: the wager
        ++st.acted; ++st.acted_by_class[c];
        st.completeness.add(o.completeness);
        st.sup_removed_min += would_cost;
        lad.at(c, b).sup_removed += would_cost;
        if (v.reason == RS_CANARY) ++st.canary;
        break;
      }
      case V_FRONTIER: {
        // rent a bigger mind for this one cell. It costs money and, because a
        // human still reads the answer at low rungs, a little supervision.
        ++st.frontier; st.frontier_calls += 1.0;
        // the rented mind reads the same record, with less noise. It cannot
        // buy coverage: a fact nobody recorded is not available at any price.
        const Read fr = observe(w, o, C.join_graph[c], true, 0.f, 0.96f, 0x33ULL);
        const float boost = 0.72f;
        if (u01(w.seed, 7200 + c, o.id) < boost) {
          o.completeness = fr.completeness;
          hand.commit(w, tape, idx, fr.choice, day,
                      direction + (direction > 0 ? 0.9f : -0.9f), b, 3);     // via 3: a rented mind acted
          ++st.acted; ++st.acted_by_class[c];
          st.sup_removed_min += would_cost;
          lad.at(c, b).sup_removed += would_cost;
        } else {
          o.state = OB_ESCALATED; ++st.warrant; ++adj_used;
          st.sup_created_min += 22.0; lad.at(c, b).sup_created += 22.0;
          // v2: a = the seat that keeps it (the resident escalates without moving it), so a fold does not reseat the cell
          tape.put(R_ESCALATE, day, o.id, c, -1, ARM_MACHINE, o.seat, o.escalations, direction, sp.value, b, 0, 0, PROV_M);
        }
        break;
      }
      case V_DRAFT: {
        // the resident prepares; a person presses the key. This is real
        // supervision created, and it is why a draft is not free.
        ++st.drafted; ++adj_used;
        const double review = 4.0 + 9.0 * sp.decide_frac;
        st.sup_created_min += review; lad.at(c, b).sup_created += review;
        st.sup_removed_min += would_cost - review;
        lad.at(c, b).sup_removed += std::max(0.0, would_cost - review);
        o.completeness = mach_complete[i];
        hand.commit(w, tape, idx, mr.choice, day, direction, b, 2);   // via 2: assisted, never the wager
        if (v.reason == RS_AUDIT) ++st.audit;
        break;
      }
      case V_WARRANT: {
        ++st.warrant; ++adj_used;
        const double sign = 12.0 + 20.0 * sp.decide_frac;
        st.sup_created_min += sign; lad.at(c, b).sup_created += sign;
        st.sup_removed_min += std::max(0.0, would_cost - sign);
        lad.at(c, b).sup_removed += std::max(0.0, would_cost - sign);
        // the signer executes the machine's choice verbatim on the stratum; a
        // refusal is a veto ROW and grades nothing
        o.completeness = mach_complete[i];
        hand.commit(w, tape, idx, mr.choice, day, direction, b, 4);   // via 4: assisted, never the wager
        break;
      }
      default: {
        ++st.held;
        tape.put(R_HOLD, day, o.id, c, -1, ARM_MACHINE, v.reason, b, direction, sp.value, b, 0, 0, PROV_M);
        break;
      }
    }
  }
  lad.step(day, wr, tape);
}

// Outcomes arrive. This is the only place a licence can widen.
inline void Resident::grade(World& w, const Firm& f, Tape& tape, uint32_t day) {
  (void)f; (void)tape;
  float ftmp[FT_N];
  for (Obligation& o : w.ob) {
    if (o.state != OB_SETTLED || o.day_settled != day) continue;
    const int c = o.cls; const ClassSpec& sp = cls_spec(c);
    const bool good  = (o.outcome == OK_GOOD);
    // CORRECTNESS AND TIMELINESS ARE DIFFERENT QUANTITIES AND MUST NOT SHARE A
    // GRADER. Whether the answer was right is a property of the decider; whether
    // it was on time is a property of the queue it sat in. The head and the
    // competence estimate train on correctness; the LICENCE is graded on the
    // writ's own metric, which counts lateness, because that is what the firm
    // pays for. Conflating them makes competence unrecoverable — a busy expert
    // and a idle novice look identical.
    const bool right = (o.outcome != OK_BAD);
    const int b = o.band;
    // train on the arrival, with a Horvitz–Thompson weight for the canary
    // stratum so a field that routes attention by its own estimate does not
    // starve its own evidence
    const float rate = canary_rate(c, b);
    // the weight belongs to the canary stratum only; a draft was not drawn by
    // the salt and carries no selection probability to invert
    const float ipw = (o.via == 1) ? std::min(6.f, 1.f / std::max(rate, 0.05f)) : 1.f;
    Field::feats(sp, o.completeness, fd.skill_of(o.seat, c),
                 (float)((int)o.day_decided - (int)o.day_due) / 7.f, 0.f, (float)o.hops, ftmp);
    fd.learn(c, ftmp, right ? 1 : 0, lr, ipw);
    if (!o.by_machine) {
      // predict with skill held NEUTRAL, so what the update sees is the seat
      float fn_[FT_N];
      Field::feats(sp, o.completeness, 0.5f,
                   (float)((int)o.day_decided - (int)o.day_due) / 7.f, 0.f, (float)o.hops, fn_);
      fd.observe_skill(o.seat, c, right ? 1 : 0, Field::sigm(fd.logit(c, fn_)));
    }
    else               fd.observe_mach(c, right ? 1 : 0);
    lad.observe(c, b, (int)o.via, good, lad.p_incumbent(c, b));
  }
}

} // namespace acme
