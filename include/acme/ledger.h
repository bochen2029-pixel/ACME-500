// ============================================================================
//  acme/ledger.h — THE LEDGER: the cells, the open set, the minute meter,
//  and the fold that builds all of them from the tape       [v2, step B3 / C0]
//
//  core.h has said since the first line that every table is a fold over the
//  tape. The Ledger is that fold made concrete: every obligation, the open set,
//  the minute meter, rebuilt from rows alone (Ledger::fold). Since C0 it is
//  also the LIVE state: the plant's arm and the hand mutate it directly while
//  writing the rows that describe the mutation, and oracle O14 requires the
//  cold fold to equal the live ledger field by field at the end of a run.
//
//  This file includes nothing from the plant. The machine's translation unit
//  links it without world.h (oracle O17), which is the property that makes
//  the simulator and the product one program:
//
//     period(ledger = fold(tape <= h), store, clock) -> rows
//
//  The rules per row type are the table in ARCHITECTURE_v2 §3.2. A row type
//  whose effect is not listed here has no effect on the ledger by definition.
// ============================================================================
#pragma once
#include "core.h"

namespace acme {

// ----------------------------------------------------------------------------
// The cell
// ----------------------------------------------------------------------------
enum ObState : uint8_t {
  OB_OPEN = 0,      // arrived, nobody has it
  OB_QUEUED,        // assigned to a seat's queue
  OB_INPROG,        // being worked; partial context assembled
  OB_DECIDED,       // a terminal decision was made; effect out
  OB_ESCALATED,     // handed up; waiting on a warrant seat
  OB_SETTLED,       // the world graded it
  OB_N
};

struct Obligation {
  uint32_t id;
  uint16_t cls;
  uint32_t day_open;
  uint32_t day_due;
  uint32_t day_decided = 0;
  uint32_t day_settled = 0;
  int32_t  seat = -1;           // who holds it
  uint8_t  state = OB_OPEN;
  uint8_t  outcome = OK_NONE;
  uint8_t  hops = 0;            // how many seats it has passed through
  uint8_t  escalations = 0;
  float    completeness = 0.f;  // context in hand at decision time: what the reader held (a row carries it)
  float    work_done = 0.f;
  float    margin = 0.f;        // the decider's own confidence
  uint32_t systems_opened = 0;  // bitmask of systems fetched
  uint32_t last_touch = 0;      // for context decay
  int      decision = -1;       // the terminal choice, class-specific
  uint8_t  by_machine = 0;
  // HOW THE DECISION LEFT THE BUILDING. 0 a person decided; 1 the resident acted
  // unattended; 2 the resident drafted and a person keyed it; 3 a rented mind
  // acted; 4 a signer executed the resident's choice verbatim. Only 1 and 3 are
  // the wager.
  uint8_t  via = 0;
  uint8_t  band = 0;
  int      dep = -1;            // waits on another obligation (an index into the ledger)
};

// Memory decays. A case you last touched nine days ago costs you re-acquisition,
// and this is the reason a human's context window is the org chart's real
// constraint: skulls do not hold state between turns either.
inline float decay(uint32_t now, uint32_t last_touch, float half_life_days = 4.0f) {
  const float dt = (float)(now - last_touch);
  return std::pow(0.5f, dt / std::max(0.5f, half_life_days));
}

// ----------------------------------------------------------------------------
// The minute meter
// ----------------------------------------------------------------------------
struct TimeLedger {
  Acc fetch, frame, decide, commit, transport, meeting, rework, glue;
  double total() const { return fetch.sum() + frame.sum() + decide.sum() + commit.sum()
                              + transport.sum() + meeting.sum() + rework.sum() + glue.sum(); }
  double decide_fraction() const { const double t = total(); return t > 0 ? decide.sum() / t : 0.0; }
  void reset() { fetch.reset(); frame.reset(); decide.reset(); commit.reset(); transport.reset(); meeting.reset(); rework.reset(); glue.reset(); }
};

// The incumbent arm's own statistics: the plant's meter of its people. Printed
// by --sim; since the step-B remediation it is an input to nothing the machine
// computes (the compile step folds ACT rows instead).
struct HumanStats {
  TimeLedger time;
  std::vector<TimeLedger> by_class;
  Acc completeness, hops, cycle_days;
  uint64_t n_decided = 0, n_escalated = 0, n_held = 0, n_meetings = 0;
  uint64_t n_breached = 0;                 // decided after the deadline
  double   attention_wasted = 0;           // budget unspent because nobody could reach the work
  double   attention_spent = 0;
  std::vector<uint64_t> decided_by_class;
  void init(int NC) { by_class.assign(NC, TimeLedger{}); decided_by_class.assign(NC, 0); }
};

// ----------------------------------------------------------------------------
// The ledger
// ----------------------------------------------------------------------------
struct Ledger {
  int NC = 0;
  std::vector<Obligation> ob;                 // the cells, in arrival order
  std::vector<uint32_t>   idx_of_oid;         // oid -> index (+1; 0 = unknown)
  std::vector<uint32_t>   open_idx;           // not yet settled, in arrival order
  uint32_t next_id = 1;
  uint32_t day = 0;
  int      mode = -1, sw = -1; uint32_t schema = 0; int ver = 0;
  std::vector<TimeLedger> by_class;           // the minute meter per class (a fold of ACT rows)
  TimeLedger              total;
  // C1: THE MEMO. The last proposal per oid is a fold of PROPOSAL rows: the
  // kernel carries it forward while the cell's frame hash is unchanged and
  // reads the judge again only when it changed, or the floor sample says so.
  struct LastProp { int choice = 0; float direction = 0.f, completeness_hat = 0.f; uint32_t judge_hash = 0; uint32_t day = 0; uint8_t band = 0; bool valid = false; };
  std::vector<LastProp>   last_prop;          // [oid]
  // C1: THE STRATA. The governor draws canary / audit / retained per open oid
  // before the machine's period and writes STRATUM rows; the gate reads them
  // from here and never touches the salt. Valid for the period they were drawn in.
  std::vector<uint8_t>    stratum_kind;       // [oid] 0 none · 1 canary · 2 audit · 3 retained-control
  std::vector<uint32_t>   stratum_day;        // [oid] the period the draw belongs to (+1; 0 = never drawn)
  std::vector<float>      stratum_rate;       // [oid] the canary rate the last draw was made against

  void init(int nc) { NC = nc; by_class.assign(nc, TimeLedger{}); }
  const LastProp* memo(uint32_t oid) const { return (oid < last_prop.size() && last_prop[oid].valid) ? &last_prop[oid] : nullptr; }
  int stratum_of(uint32_t oid, uint32_t day_) const { return (oid < stratum_kind.size() && stratum_day[oid] == day_ + 1) ? (int)stratum_kind[oid] : 0; }
  float stratum_rate_of(uint32_t oid) const { return oid < stratum_rate.size() ? stratum_rate[oid] : 0.f; }

  Obligation* at_oid(uint32_t oid) {
    if (oid == 0 || oid >= idx_of_oid.size() || idx_of_oid[oid] == 0) return nullptr;
    return &ob[idx_of_oid[oid] - 1];
  }
  const Obligation* at_oid(uint32_t oid) const {
    if (oid == 0 || oid >= idx_of_oid.size() || idx_of_oid[oid] == 0) return nullptr;
    return &ob[idx_of_oid[oid] - 1];
  }
  // the live arm opens a cell directly and writes the row itself; the fold opens it from the row
  uint32_t open_cell(const Obligation& o) {
    if (o.id >= idx_of_oid.size()) idx_of_oid.resize((size_t)o.id + 1024, 0);
    idx_of_oid[o.id] = (uint32_t)ob.size() + 1;
    open_idx.push_back((uint32_t)ob.size());
    ob.push_back(o);
    if (o.id + 1 > next_id) next_id = o.id + 1;
    return (uint32_t)ob.size() - 1;
  }
  // the open set is derived: every unsettled cell, in arrival order
  void compact() {
    std::vector<uint32_t> keep; keep.reserve(open_idx.size());
    for (uint32_t i : open_idx) if (ob[i].state != OB_SETTLED) keep.push_back(i);
    open_idx.swap(keep);
  }

  // THE FOLD. One row in, the ledger moves exactly as the live world moved
  // when that row was written. Nothing else may move it.
  void apply(const Rec& r) {
    switch (r.type) {
      case R_HEADER: mode = r.a / 10; sw = r.a % 10; schema = (uint32_t)r.b; ver = (int)r.value; return;
      case R_TICK:   day = (uint32_t)r.a; return;
      case R_ARRIVE: {
        Obligation o{};
        o.id = r.oid; o.cls = r.cls; o.day_open = r.day; o.day_due = (uint32_t)r.a;
        o.state = OB_OPEN; o.last_touch = r.day;
        if (r.b >= 0) { Obligation* d = at_oid((uint32_t)r.b); o.dep = d ? (int)(idx_of_oid[(uint32_t)r.b] - 1) : -1; }
        open_cell(o);
        return;
      }
      case R_ASSIGN: { Obligation* o = at_oid(r.oid); if (!o) return;
        o->seat = r.seat; o->state = OB_QUEUED; o->hops = (uint8_t)r.b; return; }
      case R_CONTEXT: { Obligation* o = at_oid(r.oid); if (!o) return;
        if (r.flags & RF_LOST) o->systems_opened &= ~(1u << (r.a & 31));
        else { o->systems_opened |= (1u << (r.a & 31)); if (r.seat >= 0) { total.fetch.add(r.value); if (r.cls < NC) by_class[r.cls].fetch.add(r.value); } }
        if (r.seat >= 0) o->last_touch = r.day;
        return; }
      case R_ACT: {
        const bool cl = r.oid != 0 && r.cls < NC;
        switch (r.a) {
          case ACT_FETCH:     total.fetch.add(r.value);     if (cl) by_class[r.cls].fetch.add(r.value);     break;
          case ACT_FRAME:     total.frame.add(r.value);     if (cl) by_class[r.cls].frame.add(r.value);     break;
          case ACT_DECIDE:    total.decide.add(r.value);    if (cl) by_class[r.cls].decide.add(r.value);    break;
          case ACT_COMMIT:    total.commit.add(r.value);    if (cl) by_class[r.cls].commit.add(r.value);    break;
          case ACT_TRANSPORT: total.transport.add(r.value); if (cl) by_class[r.cls].transport.add(r.value); break;
          case ACT_REWORK:    total.rework.add(r.value);    if (cl) by_class[r.cls].rework.add(r.value);    break;
          case ACT_GLUE:      total.glue.add(r.value); break;
          case ACT_MEETING:   total.meeting.add(r.value); break;
        }
        if (Obligation* o = at_oid(r.oid)) {
          o->last_touch = r.day;
          if (r.a == ACT_FRAME || r.a == ACT_DECIDE) o->completeness = r.margin;   // what the reader held
        }
        return;
      }
      case R_MEETING: total.meeting.add(r.value); return;
      case R_HOLD: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        // reasons 1 (blocked) and 2 (no attention) are written without touching the
        // case; only reason 3, a day that ended mid-case, is a touch
        if (r.a == 3 && r.seat >= 0) { o->state = OB_INPROG; o->completeness = r.value; o->margin = r.margin; o->band = r.band; o->last_touch = r.day; }
        return;
      }
      case R_ESCALATE: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        o->state = OB_ESCALATED;
        if (r.seat >= 0) {            // a person handed it up: the receiver starts cold
          o->margin = r.margin; o->band = r.band;
          o->seat = r.a; o->escalations = (uint8_t)r.b; o->hops += 1; o->systems_opened = 0; o->last_touch = r.day;
        }                             // the resident's escalation moves nothing but the state
        return;
      }
      case R_DECIDE: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        o->decision = r.a; o->hops = (uint8_t)r.b; o->margin = r.margin; o->band = r.band;
        o->state = OB_DECIDED; o->day_decided = r.day; o->by_machine = 0; o->via = 0; o->seat = r.seat;
        o->last_touch = r.day;
        return;
      }
      case R_PROPOSAL: {
        if (r.oid >= last_prop.size()) last_prop.resize((size_t)r.oid + 1024, LastProp{});
        LastProp& p = last_prop[r.oid];
        p.choice = r.a; p.judge_hash = (uint32_t)r.b; p.direction = r.margin; p.completeness_hat = r.value;
        p.band = r.band; p.day = r.day; p.valid = true;
        return;
      }
      case R_STRATUM: {
        if (r.oid >= stratum_kind.size()) { stratum_kind.resize((size_t)r.oid + 1024, 0); stratum_day.resize((size_t)r.oid + 1024, 0); stratum_rate.resize((size_t)r.oid + 1024, 0.f); }
        stratum_kind[r.oid] = (uint8_t)r.a; stratum_day[r.oid] = r.day + 1; stratum_rate[r.oid] = (float)r.b * 1e-6f;
        return;
      }
      case R_EFFECT: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        o->decision = r.a; o->state = OB_DECIDED; o->day_decided = r.day; o->by_machine = 1;
        o->via = r.via; o->band = r.band; o->margin = r.margin;
        if (const LastProp* p = memo(r.oid)) o->completeness = p->completeness_hat;
        return;
      }
      case R_UNDO: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        o->state = (uint8_t)r.b; o->seat = (int32_t)r.margin; o->day_decided = (uint32_t)r.value;
        o->by_machine = 0; o->via = 0;
        return;
      }
      case R_OUTCOME: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        o->outcome = (uint8_t)r.a; o->state = OB_SETTLED; o->day_settled = r.day;
        return;
      }
      default: return;   // LICENSE, KAPPA, PATCH, COUNSEL, RECEIPT, CORRECTION, NOTE: no ledger effect here
    }
  }

  static Ledger fold(const Tape& tape, int NC) {
    Ledger L; L.init(NC);
    tape.fold([&](const Rec& r) { L.apply(r); });
    L.compact();
    return L;
  }
  // The minute meter alone, from ACT, CONTEXT and MEETING rows: what the compile
  // step reads for the decide fraction. A real firm can carry these rows (as
  // time-use survey rows tagged RF_TORN_OR_SURVEY); it cannot carry a meter.
  static std::vector<TimeLedger> fold_minutes(const Tape& tape, int NC) {
    Ledger L; L.init(NC);
    tape.fold([&](const Rec& r) {
      if (r.type == R_ACT || r.type == R_MEETING || (r.type == R_CONTEXT && !(r.flags & RF_LOST) && r.seat >= 0)) {
        if (r.type == R_CONTEXT) { L.total.fetch.add(r.value); if (r.cls < NC) L.by_class[r.cls].fetch.add(r.value); }
        else L.apply(r);
      }
    });
    return L.by_class;
  }
};

// O14 · FOLD-TO-IDENTITY. The cold fold against the live ledger, field by field.
// Returns the number of differing fields and names the first one.
struct FoldDiff { long fields = 0; char first[160] = {0}; };
inline FoldDiff ledger_diff(const Ledger& L, const Ledger& w) {
  FoldDiff d;
  auto miss = [&](const char* what, size_t i) { if (d.fields++ == 0) snprintf(d.first, sizeof d.first, "%s at index %zu", what, i); };
  if (L.ob.size() != w.ob.size()) { miss("ob.size", 0); return d; }
  for (size_t i = 0; i < w.ob.size(); ++i) {
    const Obligation& a = L.ob[i]; const Obligation& b = w.ob[i];
    if (a.id != b.id) miss("id", i);
    if (a.cls != b.cls) miss("cls", i);
    if (a.day_open != b.day_open) miss("day_open", i);
    if (a.day_due != b.day_due) miss("day_due", i);
    if (a.day_decided != b.day_decided) miss("day_decided", i);
    if (a.day_settled != b.day_settled) miss("day_settled", i);
    if (a.seat != b.seat) miss("seat", i);
    if (a.state != b.state) miss("state", i);
    if (a.outcome != b.outcome) miss("outcome", i);
    if (a.hops != b.hops) miss("hops", i);
    if (a.escalations != b.escalations) miss("escalations", i);
    if (a.completeness != b.completeness) miss("completeness", i);
    if (a.margin != b.margin) miss("margin", i);
    if (a.systems_opened != b.systems_opened) miss("systems_opened", i);
    if (a.last_touch != b.last_touch) miss("last_touch", i);
    if (a.decision != b.decision) miss("decision", i);
    if (a.by_machine != b.by_machine) miss("by_machine", i);
    if (a.via != b.via) miss("via", i);
    if (a.band != b.band) miss("band", i);
    if (a.dep != b.dep) miss("dep", i);
  }
  if (L.open_idx != w.open_idx) miss("open_idx", 0);
  if (L.next_id != w.next_id) miss("next_id", 0);
  // C1: the memo and the strata are folds too, and the live ledger keeps them
  // by applying the very row it wrote, so they must agree entry for entry.
  const size_t np = std::max(L.last_prop.size(), w.last_prop.size());
  for (size_t i = 0; i < np; ++i) {
    const Ledger::LastProp a = i < L.last_prop.size() ? L.last_prop[i] : Ledger::LastProp{};
    const Ledger::LastProp b = i < w.last_prop.size() ? w.last_prop[i] : Ledger::LastProp{};
    if (a.valid != b.valid || (a.valid && (a.choice != b.choice || a.direction != b.direction || a.completeness_hat != b.completeness_hat
                                          || a.judge_hash != b.judge_hash || a.day != b.day || a.band != b.band))) miss("last_prop", i);
  }
  const size_t ns = std::max(L.stratum_kind.size(), w.stratum_kind.size());
  for (size_t i = 0; i < ns; ++i) {
    const uint8_t ka = i < L.stratum_kind.size() ? L.stratum_kind[i] : 0, kb = i < w.stratum_kind.size() ? w.stratum_kind[i] : 0;
    const uint32_t da = i < L.stratum_day.size() ? L.stratum_day[i] : 0, db = i < w.stratum_day.size() ? w.stratum_day[i] : 0;
    const float ra = i < L.stratum_rate.size() ? L.stratum_rate[i] : 0.f, rb = i < w.stratum_rate.size() ? w.stratum_rate[i] : 0.f;
    if (ka != kb || da != db || ra != rb) miss("stratum", i);
  }
  return d;
}

// The live path writes a row and folds it into its own ledger in one motion, so
// the live state of the memo and the strata is the fold by construction.
inline void put_fold(Tape& tape, Ledger& L, RecType t, uint32_t day, uint32_t oid, int cls, int seat, int arm,
                     int a = 0, int b = 0, float margin = 0.f, float value = 0.f, int band = 0, int flags = 0,
                     int via = 0, int prov = PROV_D) {
  tape.put(t, day, oid, cls, seat, arm, a, b, margin, value, band, flags, via, prov);
  L.apply(tape.rec.back());
}

// The minute meter is a fold too: the folded ledger against the live HumanStats.
// Fixed-point accumulators make the per-class sums order-independent; the two
// per-seat kinds (glue, meeting) were metered as per-day float sums, so they
// are compared to a relative tolerance.
inline long minutes_diff(const Ledger& L, const HumanStats& hs, int NC, double rel = 1e-6) {
  long bad = 0;
  auto near = [&](double x, double y) { return std::fabs(x - y) <= rel * std::max(1.0, std::max(std::fabs(x), std::fabs(y))); };
  for (int c = 0; c < NC; ++c) {
    const TimeLedger& a = L.by_class[c]; const TimeLedger& b = hs.by_class[c];
    if (!near(a.fetch.sum(), b.fetch.sum()))         ++bad;
    if (!near(a.frame.sum(), b.frame.sum()))         ++bad;
    if (!near(a.decide.sum(), b.decide.sum()))       ++bad;
    if (!near(a.commit.sum(), b.commit.sum()))       ++bad;
    if (!near(a.transport.sum(), b.transport.sum())) ++bad;
    if (!near(a.rework.sum(), b.rework.sum()))       ++bad;
  }
  if (!near(L.total.glue.sum(), hs.time.glue.sum()))       ++bad;
  if (!near(L.total.meeting.sum(), hs.time.meeting.sum())) ++bad;
  if (!near(L.total.decide.sum(), hs.time.decide.sum()))   ++bad;
  return bad;
}

} // namespace acme
