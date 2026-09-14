// ============================================================================
//  acme/ledger.h — THE LEDGER IS A FOLD OF THE TAPE            [v2, step B3]
//
//  core.h has said since the first line that every table is a fold over the
//  tape. Until step B3 no C++ in this program was: every consumer read
//  World::ob, a struct the tape only described. This file is the fold. Given
//  the rows in order it rebuilds every obligation, the open set, the minute
//  meter and the licence counts, and oracle O14 requires the cold fold to be
//  byte-equal to the live world at the end of a run. The invariant that
//  makes the simulator and the product one program is exactly this one:
//
//     period(ledger = fold(tape <= h), store, clock) -> rows
//
//  The rules per row type are the table in ARCHITECTURE_v2 §3.2. A row type
//  whose effect is not listed here has no effect on the ledger by definition.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "world.h"
#include "human.h"

namespace acme {

struct Ledger {
  std::vector<Obligation> ob;                 // the cells, in arrival order
  std::vector<uint32_t>   idx_of_oid;         // oid -> index (+1; 0 = unknown)
  std::vector<uint32_t>   open_idx;           // not yet settled, in arrival order
  uint32_t next_id = 1;
  uint32_t day = 0;
  int      mode = -1, sw = -1; uint32_t schema = 0; int ver = 0;
  // the minute meter, per class and act, plus the two per-seat kinds
  std::vector<TimeLedger> by_class;           // [NC]
  TimeLedger              total;
  // (the licence counts are folded by fold_ladder() in machine.h, which owns the Ladder type)
  // last proposal per oid (completeness_hat), so an EFFECT can carry it
  std::vector<float>      last_prop_complete;

  void init(int NC) { by_class.assign(NC, TimeLedger{}); }

  Obligation* at_oid(uint32_t oid) {
    if (oid == 0 || oid >= idx_of_oid.size() || idx_of_oid[oid] == 0) return nullptr;
    return &ob[idx_of_oid[oid] - 1];
  }
  void note_touch(const Rec& r, Obligation& o) { (void)r; o.last_touch = r.day; }

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
        if (r.oid >= idx_of_oid.size()) idx_of_oid.resize((size_t)r.oid + 1024, 0);
        idx_of_oid[r.oid] = (uint32_t)ob.size() + 1;
        open_idx.push_back((uint32_t)ob.size());
        ob.push_back(o);
        if (r.oid + 1 > next_id) next_id = r.oid + 1;
        return;
      }
      case R_ASSIGN: { Obligation* o = at_oid(r.oid); if (!o) return;
        o->seat = r.seat; o->state = OB_QUEUED; o->hops = (uint8_t)r.b; return; }
      case R_CONTEXT: { Obligation* o = at_oid(r.oid); if (!o) return;
        if (r.flags & RF_LOST) o->systems_opened &= ~(1u << (r.a & 31));
        else { o->systems_opened |= (1u << (r.a & 31)); if (r.seat >= 0) { total.fetch.add(r.value); by_class[r.cls].fetch.add(r.value); } }
        if (r.seat >= 0) o->last_touch = r.day;
        return; }
      case R_ACT: {
        switch (r.a) {
          case ACT_FETCH:     total.fetch.add(r.value);     if (r.oid) by_class[r.cls].fetch.add(r.value);     break;
          case ACT_FRAME:     total.frame.add(r.value);     if (r.oid) by_class[r.cls].frame.add(r.value);     break;
          case ACT_DECIDE:    total.decide.add(r.value);    if (r.oid) by_class[r.cls].decide.add(r.value);    break;
          case ACT_COMMIT:    total.commit.add(r.value);    if (r.oid) by_class[r.cls].commit.add(r.value);    break;
          case ACT_TRANSPORT: total.transport.add(r.value); if (r.oid) by_class[r.cls].transport.add(r.value); break;
          case ACT_REWORK:    total.rework.add(r.value);    if (r.oid) by_class[r.cls].rework.add(r.value);    break;
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
        if (r.a == 3 && r.seat >= 0) { o->state = OB_INPROG; o->completeness = r.value; o->margin = r.margin; o->last_touch = r.day; }
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
        if (r.oid >= last_prop_complete.size()) last_prop_complete.resize((size_t)r.oid + 1024, 0.f);
        last_prop_complete[r.oid] = r.value;
        return;
      }
      case R_EFFECT: {
        Obligation* o = at_oid(r.oid); if (!o) return;
        o->decision = r.a; o->state = OB_DECIDED; o->day_decided = r.day; o->by_machine = 1;
        o->via = r.via; o->band = r.band; o->margin = r.margin;
        if (r.oid < last_prop_complete.size()) o->completeness = last_prop_complete[r.oid];
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
      default: return;   // LICENSE, KAPPA, PATCH, STRATUM, COUNSEL, RECEIPT, CORRECTION, NOTE: no ledger effect here
    }
  }

  // the open set is derived: every unsettled cell, in arrival order
  void compact() {
    open_idx.clear();
    for (uint32_t i = 0; i < ob.size(); ++i) if (ob[i].state != OB_SETTLED) open_idx.push_back(i);
  }

  static Ledger fold(const Tape& tape, int NC) {
    Ledger L; L.init(NC);
    tape.fold([&](const Rec& r) { L.apply(r); });
    L.compact();
    return L;
  }
};

// O14 · FOLD-TO-IDENTITY. The cold fold against the live world, field by field.
// Returns the number of differing fields and names the first one.
struct FoldDiff { long fields = 0; char first[160] = {0}; };
inline FoldDiff ledger_diff(const Ledger& L, const World& w) {
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
  return d;
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
