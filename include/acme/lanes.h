// ============================================================================
//  acme/lanes.h — THE TAPE OF TAPES: one roster, one merge order        [F0a]
//
//  ARCHITECTURE v4 §26. A tape with one chain has one writer, and every organ
//  v4 adds is a second writer: the licensor in a process the machine cannot
//  reach, a judge behind a port, a mouth on a language lane, an executor that
//  mints its own receipts, a counterparty's kernel, an instrument that plants
//  lies. v4 makes the tape a set of lanes and the fold a merge, once, so that
//  every later organ is a lane and nothing else.
//
//  WHAT A LANE IS. A lane is a row stream with its own BLAKE2b chain, written
//  by exactly one writer. The RECORD DOES NOT MOVE: `Rec` stays forty bytes and
//  the lane is not a field on it. A row's lane is a PURE FUNCTION of what the
//  row already carries (its type and its writer's seat), so a tape written as
//  one stream splits into lanes exactly, and lanes merge back into one fold.
//  That is what makes the two-process sim (§26.5, O58) a re-arrangement of this
//  run rather than a different program.
//
//  THE MERGE ORDER, AND THE CORRECTION THIS STEP MAKES TO §26.2. v4 §26.2 says
//  the fold ingests, at TICK t, "every lane l >= 1 in lane order, then lane 0's
//  records of period t". That order cannot hold, and the sim found it before a
//  line of the two-process build was written: THE WORLD PORT AND THE LICENSOR
//  EACH WRITE TWICE IN A PERIOD. The world port opens the period (TICK, ARRIVE)
//  and closes it (OUTCOME, SEAL); the licensor draws the strata before the
//  machine reads and moves the ladder after the outcomes land. Fold lane 0 last
//  and an ARRIVE row lands after the EFFECT on the cell it opened. So the merge
//  key is not (lane, sequence) but
//
//        (day, PHASE, lane, sequence within the lane)
//
//  where the phase is the published order of a period and is, like the lane, a
//  pure function of the row. Nine phases, named below. A writer may appear in
//  several phases; a phase may hold several lanes; within a phase the lanes
//  fold in lane order, and within a lane the rows fold in the order they were
//  written. O42 measures whether that merge reproduces the flat fold.
//
//  Kernel side: core.h only. No plant, no governor, no model.
// ============================================================================
#pragma once
#include "core.h"

namespace acme {

// ---- the roster: who writes, in v4 §26.4's order ---------------------------
enum Lane : int {
  LANE_WORLD = 0,     // the world port: the plant, or the CDC ingester. TICK, HEADER, ARRIVE, OUTCOME, SPAN, SEAL
  LANE_LICENSOR,      // acme-licensor (v4 §27): STRATUM, LICENSE, KAPPA, REGIME, STOP, STAKE, BREACH
  LANE_JUDGE,         // one per judge hash: PROPOSAL, SENSE, COUNSEL
  LANE_KERNEL,        // intellect / acme: EFFECT, HOLD, ESCALATE, CONTEXT, UNDO, CALIB, NOTE, PATCH (WANT and SPAN when E2 lands)
  LANE_HUMAN,         // the seats: DECIDE, ASSIGN, ACT, MEETING, and their holds and escalations
  LANE_EXEC,          // the adapter to the systems of record: RECEIPT, UNDO on refusal
  LANE_N
};
inline const char* lane_name(int l) {
  static const char* n[] = { "world", "licensor", "judge", "kernel", "human", "executor" };
  return (l >= 0 && l < LANE_N) ? n[l] : "?";
}

// ---- the phases: the published order of a period ---------------------------
// The correction of §26.2. A period runs these in order and nothing else; a row
// whose phase is later than a row it depends on is a defect the merge exposes.
enum Phase : int {
  PH_OPEN = 0,        // HEADER, TICK: the clock is a row and the period begins   (world)
  PH_ARRIVE,          // ARRIVE: intake lands                                     (world)
  PH_DRAW,            // STRATUM: the licensor draws before the machine reads     (licensor)
  PH_READ,            // PROPOSAL, SENSE, COUNSEL: the judge proposes             (judge)
  PH_GATE,            // EFFECT, HOLD, ESCALATE, CONTEXT, UNDO, RECEIPT           (kernel, executor)
  PH_HUMAN,           // DECIDE, ASSIGN, ACT, MEETING and the seats' holds        (human)
  PH_SETTLE,          // OUTCOME, SPAN, CORRECTION: the world answers             (world)
  PH_LADDER,          // LICENSE, KAPPA, REGIME: authority moves on the outcomes  (licensor)
  PH_CLOSE,           // CALIB, NOTE, PATCH, SEAL: the term's freezes and the seal (kernel, world)
  PH_N
};
inline const char* phase_name(int p) {
  static const char* n[] = { "open", "arrive", "draw", "read", "gate", "human", "settle", "ladder", "close" };
  return (p >= 0 && p < PH_N) ? n[p] : "?";
}

// ---- the two pure functions ------------------------------------------------
// A row's lane is its writer. Where one type has two writers (a hold is written
// by a seat and by the machine; a context is fetched by both), the seat says
// which: >= 0 a person, -1 the resident, -4 the executor, else the world port.
inline int lane_of(const Rec& r) {
  switch (r.type) {
    case R_HEADER: case R_TICK: case R_SEAL: case R_ARRIVE: case R_OUTCOME: return LANE_WORLD;
    case R_STRATUM: case R_LICENSE: case R_KAPPA: case R_REGIME:            return LANE_LICENSOR;
    case R_PROPOSAL: case R_SENSE: case R_COUNSEL:                          return LANE_JUDGE;
    case R_EFFECT: case R_CALIB: case R_NOTE: case R_PATCH:                 return LANE_KERNEL;
    case R_DECIDE: case R_ASSIGN: case R_ACT: case R_MEETING:               return LANE_HUMAN;
    case R_RECEIPT:                                                         return LANE_EXEC;
    case R_UNDO:          return r.arm == ARM_EXECUTOR ? LANE_EXEC : LANE_KERNEL;
    case R_HOLD: case R_ESCALATE: case R_CONTEXT: case R_CORRECTION:
      if (r.seat >= 0) return LANE_HUMAN;
      if (r.seat == -1) return LANE_KERNEL;
      if (r.seat == -4) return LANE_EXEC;
      return LANE_WORLD;
    default: return LANE_WORLD;
  }
}
// A row's phase is where in the period it was written. Same rule for the two
// types a writer produces in two phases: the seat distinguishes them.
inline int phase_of(const Rec& r) {
  switch (r.type) {
    case R_HEADER: case R_TICK:                        return PH_OPEN;
    case R_ARRIVE:                                     return PH_ARRIVE;
    case R_STRATUM:                                    return PH_DRAW;
    case R_PROPOSAL: case R_SENSE: case R_COUNSEL:     return PH_READ;
    case R_EFFECT: case R_UNDO: case R_RECEIPT:        return PH_GATE;
    case R_DECIDE: case R_ASSIGN: case R_ACT: case R_MEETING: return PH_HUMAN;
    case R_OUTCOME: case R_CORRECTION:                 return PH_SETTLE;
    case R_LICENSE: case R_KAPPA: case R_REGIME:       return PH_LADDER;
    case R_CALIB: case R_NOTE: case R_PATCH: case R_SEAL: return PH_CLOSE;
    case R_HOLD: case R_ESCALATE: case R_CONTEXT:      return r.seat >= 0 ? PH_HUMAN : PH_GATE;
    default: return PH_CLOSE;
  }
}

// ---- what a seal covers ----------------------------------------------------
// D1's law: A NOTE IS A ROW OF NO PERIOD. The TICK's row count excludes it, and
// that is what made a resumed tape equal an unkilled one. A seal is a period's
// attestation, so a row of no period is a row of no seal: a NOTE joins its
// lane's rows and never its lane's chain. Without this the restore marker moves
// every later merged head and O18's claim dies of its own bookkeeping.
inline bool seal_covers(const Rec& r) { return r.type != R_NOTE; }

// ---- the live lane set: per-lane chains, kept beside the one stream --------
// It is not a second sink: the tape has one sink (the durable file). The world
// port hands it the period's rows at the close and asks for the seal, so the
// lanes' heads are a fold of the same rows in the same order, and a cold split
// reproduces them exactly (O42a).
struct LaneSet {
  uint8_t  head[LANE_N][32] = {};
  uint64_t rows[LANE_N] = {};
  size_t   folded = 0;                  // rows of the flat tape already routed
  uint64_t sealed_rows = 0;             // rows sealed by the last SEAL

  void ingest(const Rec& r) {
    if (!seal_covers(r)) return;                 // a row of no period is a row of no seal
    const int l = lane_of(r);
    Blake2b b; b.update(head[l], 32); b.update(&r, sizeof(Rec)); b.final(head[l]);
    ++rows[l];
  }
  void ingest_from(const Tape& t, size_t from) { for (size_t i = from; i < t.rec.size(); ++i) ingest(t.rec[i]); folded = t.rec.size(); }
  // the firm's chain head for the period: BLAKE2b over every lane's head, in lane order
  void merged(uint8_t out[32]) const { Blake2b b; for (int l = 0; l < LANE_N; ++l) b.update(head[l], 32); b.final(out); }
  uint32_t merged_b() const {
    uint8_t h[32]; merged(h);
    return (uint32_t)h[0] | ((uint32_t)h[1] << 8) | ((uint32_t)h[2] << 16) | ((uint32_t)h[3] << 24);
  }
  uint64_t total() const { uint64_t n = 0; for (int l = 0; l < LANE_N; ++l) n += rows[l]; return n; }
};

// ---- the seal: the world port closes the period over every lane ------------
// Written last in the period, after the ladder has moved and the term's freezes
// are on the tape. `a` = the lanes sealed, `b` = the low 32 bits of the merged
// head at the seal, `value` = the rows sealed this period. The SEAL is itself a
// row of lane 0 and folds into that lane's chain after it is written, so the
// next period's merged head covers it.
inline void seal_period(Tape& tape, LaneSet& lanes, uint32_t day) {
  lanes.ingest_from(tape, lanes.folded);
  const uint64_t n = lanes.total() - lanes.sealed_rows;
  tape.put(R_SEAL, day, 0, 0, -3, ARM_GOVERNOR, LANE_N, (int)lanes.merged_b(), 0.f, (float)n);
  lanes.ingest(tape.rec.back());
  lanes.folded = tape.rec.size();
  lanes.sealed_rows = lanes.total();
}

// ---- the cold split: the same lanes, from the tape alone -------------------
struct LaneSplit {
  std::vector<Rec> lane[LANE_N];
  uint8_t head[LANE_N][32] = {};
  uint64_t rows[LANE_N] = {};
  void merged(uint8_t out[32]) const { Blake2b b; for (int l = 0; l < LANE_N; ++l) b.update(head[l], 32); b.final(out); }
};
// `upto` splits the first `upto` rows (SIZE_MAX for all), so a split at a SEAL's
// position reproduces the heads that SEAL carried.
inline LaneSplit split_lanes(const Tape& t, size_t upto = (size_t)-1) {
  LaneSplit s;
  const size_t n = std::min(upto, t.rec.size());
  for (size_t i = 0; i < n; ++i) {
    const Rec& r = t.rec[i]; const int l = lane_of(r);
    s.lane[l].push_back(r);                      // every row joins its lane, so the merge loses none
    if (!seal_covers(r)) continue;               // and a row of no period joins no chain
    Blake2b b; b.update(s.head[l], 32); b.update(&r, sizeof(Rec)); b.final(s.head[l]);
    ++s.rows[l];
  }
  return s;
}

// ---- the merge: (day, phase, lane, sequence) -------------------------------
// The lanes are merged back into one stream in the published order. A merge that
// reproduces the flat fold is what makes a lane a protocol rather than a copy.
// `lie_late` folds a sealed period's arrivals into the period after it: the late
// row §26.2 forbids, and O42's planted lie. It is a whole period's arrivals and
// not one row, because one row of a cell nothing else touched that period would
// reorder the fold without changing what the fold arrives at, and a lie that can
// pass is not a lie.
inline std::vector<Rec> merge_lanes(const LaneSplit& s, bool lie_late = false) {
  struct Key { uint32_t day; int phase, lane; size_t seq; };
  std::vector<std::pair<Key, const Rec*>> all;
  size_t total = 0; for (int l = 0; l < LANE_N; ++l) total += s.lane[l].size();
  all.reserve(total);
  uint32_t lie_day = 0xFFFFFFFFu;
  if (lie_late) for (const Rec& r : s.lane[LANE_WORLD]) if (r.type == R_ARRIVE && r.day > 0 && r.day < lie_day) lie_day = r.day;
  for (int l = 0; l < LANE_N; ++l)
    for (size_t i = 0; i < s.lane[l].size(); ++i) {
      const Rec& r = s.lane[l][i];
      uint32_t day = r.day;
      if (lie_late && r.type == R_ARRIVE && r.day == lie_day) day = r.day + 1;   // THE LIE (O42)
      all.push_back({ { day, phase_of(r), l, i }, &r });
    }
  std::stable_sort(all.begin(), all.end(), [](const std::pair<Key, const Rec*>& x, const std::pair<Key, const Rec*>& y) {
    if (x.first.day != y.first.day) return x.first.day < y.first.day;
    if (x.first.phase != y.first.phase) return x.first.phase < y.first.phase;
    if (x.first.lane != y.first.lane) return x.first.lane < y.first.lane;
    return x.first.seq < y.first.seq;
  });
  std::vector<Rec> out; out.reserve(all.size());
  for (const auto& kv : all) out.push_back(*kv.second);
  return out;
}

} // namespace acme
