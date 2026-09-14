// ============================================================================
//  acme/human.h — how ACME actually runs today: 500 skulls passing notes
//
//  THE FIVE ACTS. Every obligation a person handles decomposes into exactly
//  five, and only one of them is cognition:
//
//     FETCH      open N applications, read the fields. Minutes.
//     FRAME      render what you found into something another human can read —
//                a deck, a mail, a summary, a ticket. Minutes.
//     DECIDE     the judgement. This is the job.
//     COMMIT     write the answer back into a system. Minutes.
//     TRANSPORT  carry the state to whoever needs it next: the handoff, the
//                escalation, the status meeting. Minutes, times attendees.
//
//  The simulation meters all five separately, per class, so the DECIDE FRACTION
//  is measured rather than asserted. The planted truth in the schema is never
//  shown to the meter; O-DECIDE checks the meter recovers it.
//
//  THE COORDINATION LOAD IS DERIVED, NOT AUTHORED. Nobody types in "there are
//  120 hours of meetings a week." Meetings are generated from the shape:
//
//     a manager's weekly one-to-ones      = span × 30 min, both sides
//     a team status                       = (span+1) × 45 min, weekly
//     a rollup up the cone                = depth × prep + the meeting itself
//     a cross-wire coordination           = one per live dependency pair
//     an escalation                       = a full re-FETCH by the receiver
//
//  Change the span and the whole load moves. That is the point: the middle is a
//  coarsening ratio, and coordination is what a coarse grid costs when the
//  smoother is a person who cannot see the other cells.
//
//  AND THE HONEST PART. A meeting is not pure waste in this model. It is the
//  ONLY channel through which tacit determinants — the facts that live in no
//  system — reach the person deciding. So deleting meetings costs coverage, and
//  the simulation makes you pay for that rather than hand-waving it. What the
//  meeting cannot justify is the other 90% of its content, which is state that
//  was already in a database being read aloud to people who could have looked.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "world.h"

namespace acme {

// Minutes, metered by act. This is the instrument the whole thesis rests on.
struct TimeLedger {
  Acc fetch, frame, decide, commit, transport, meeting, rework, glue;
  double total() const { return fetch.sum() + frame.sum() + decide.sum() + commit.sum()
                              + transport.sum() + meeting.sum() + rework.sum() + glue.sum(); }
  double decide_fraction() const { const double t = total(); return t > 0 ? decide.sum() / t : 0.0; }
  void reset() { fetch.reset(); frame.reset(); decide.reset(); commit.reset(); transport.reset(); meeting.reset(); rework.reset(); glue.reset(); }
};

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

// Cost of one fetch: opening an application and finding the field. Includes the
// tax nobody counts — the app's own load time, the login, the search.
inline float fetch_minutes(uint64_t seed, uint32_t oid, int sysid) {
  return 5.5f + 7.0f * u01(seed, 3100 + sysid, oid);
}

// ----------------------------------------------------------------------------
// THE COORDINATION CALENDAR, generated from the cone
// ----------------------------------------------------------------------------
struct Meeting { int owner; std::vector<int> attend; float minutes; uint8_t kind; };
enum MtgKind : uint8_t { MG_ONE2ONE = 0, MG_STATUS, MG_ROLLUP, MG_CROSSWIRE, MG_N };

inline std::vector<std::vector<int>> reports_of(const Firm& f) {
  std::vector<std::vector<int>> r(f.size());
  for (const Seat& s : f.seat) if (s.boss >= 0) r[s.boss].push_back(s.id);
  return r;
}

// Build one day's meetings. Weekly cadences land on fixed weekdays so the load
// is lumpy the way a real week is, and so Monday actually costs something.
inline void build_meetings(const Firm& f, const std::vector<std::vector<int>>& rep,
                           uint32_t day, std::vector<Meeting>& out) {
  out.clear();
  const int dow = (int)(day % 7);
  if (dow >= 5) return;                                    // no weekend meetings
  for (const Seat& s : f.seat) {
    const auto& r = rep[s.id];
    if (r.empty()) continue;
    // Monday team status: everyone who reports, plus the manager
    if (dow == 0) {
      Meeting m; m.owner = s.id; m.attend = r; m.attend.push_back(s.id);
      m.minutes = 45.f + 6.f * (float)r.size();            // it runs long with more people
      m.kind = MG_STATUS; out.push_back(m);
    }
    // one-to-ones spread across the week
    if (s.kind >= SK_LEAD) {
      for (size_t i = 0; i < r.size(); ++i)
        if ((int)(i % 5) == dow) { Meeting m; m.owner = s.id; m.attend = { s.id, r[i] };
                                   m.minutes = 30.f; m.kind = MG_ONE2ONE; out.push_back(m); }
    }
    // Thursday rollup: managers and above prepare and present upward
    if (dow == 3 && s.kind >= SK_MANAGER) {
      Meeting m; m.owner = s.id; m.attend = { s.id }; if (s.boss >= 0) m.attend.push_back(s.boss);
      m.minutes = 25.f + 10.f * (float)s.depth;            // prep scales with how far it must travel
      m.kind = MG_ROLLUP; out.push_back(m);
    }
  }
}

// ----------------------------------------------------------------------------
// A human works one obligation. Returns minutes consumed; may decide, hold,
// escalate or hand off. This is the round trip, in full, and every branch of it
// writes a row.
// ----------------------------------------------------------------------------
struct WorkResult { float minutes = 0; bool progressed = false; bool terminal = false; };

inline WorkResult human_work(World& w, Firm& f, Tape& tape, HumanStats& st,
                             uint32_t idx, int seat_id, uint32_t day) {
  Obligation& o = w.ob[idx];
  Seat& s = f.seat[seat_id];
  const ClassSpec& sp = cls_spec(o.cls);
  const uint64_t seed = w.seed;
  WorkResult res;
  TimeLedger& TL = st.time; TimeLedger& CL = st.by_class[o.cls];
  // v2: every priced act is a row (R_ACT), so the minute meter is a fold of the
  // tape and the decide fraction is measured from rows a real firm could carry.
  auto act = [&](int kind, float m, float completeness = 0.f) {
    tape.put(R_ACT, day, o.id, o.cls, seat_id, ARM_HUMAN, kind, 0, completeness, m);
  };

  // --- REWORK. Coming back to a case you have not touched costs re-acquisition,
  // and this is the tax a resident does not pay because it never puts the case down.
  if (o.last_touch != day && (o.systems_opened || o.state == OB_INPROG)) {
    const float d = decay(day, o.last_touch);
    const float lost = (1.f - d);
    const float m = 8.0f * lost * (float)sp.n_systems * 0.35f;
    TL.rework.add(m); CL.rework.add(m); res.minutes += m; act(ACT_REWORK, m);
    // some of what was fetched has to be fetched again; the loss is a row too,
    // so a fold knows which systems are no longer in hand
    for (int k = 0; k < sp.n_systems; ++k)
      if (ucoin(seed, 3300 + k, o.id * 31 + day, lost * 0.6f) && (o.systems_opened & (1u << k))) {
        o.systems_opened &= ~(1u << k);
        tape.put(R_CONTEXT, day, o.id, o.cls, seat_id, ARM_HUMAN, k, 0, 0.f, 0.f, 0, RF_LOST);
      }
  }
  o.last_touch = day;

  // --- FETCH. Open the systems this class needs, one application at a time.
  // A person under time pressure opens fewer, which is exactly how completeness
  // is lost in a real firm: not by incompetence, by triage.
  const float pressure = std::min(1.f, (float)(day + 4 > o.day_due ? 1.4f : 0.6f));
  int want = sp.n_systems;
  if (s.attn_left < 60.f) want = std::max(1, want - 2);             // late in the day, cut corners
  if (pressure > 1.0f)    want = std::max(1, want - 1);
  for (int k = 0; k < want; ++k) {
    if (o.systems_opened & (1u << k)) continue;
    const float m = fetch_minutes(seed, o.id * 17 + k, k);
    if (s.attn_left - res.minutes < m) break;
    o.systems_opened |= (1u << k);
    TL.fetch.add(m); CL.fetch.add(m); res.minutes += m;
    tape.put(R_CONTEXT, day, o.id, o.cls, seat_id, ARM_HUMAN, k, 0, 0.f, m);   // fetch minutes ride on the CONTEXT row
  }

  // --- what is actually in hand, and therefore what this person will decide.
  // Tacit determinants reach a person only through conversation. A seat that
  // sat in this week's status with the people on this wire has some of them; a
  // seat that did not, has almost none. THIS IS THE ONE THING A MEETING DOES
  // THAT IS NOT A ROUND TRIP, and the simulation makes the machine pay for
  // losing it rather than pretending it was worthless.
  const float tacit_share = std::min(1.f, 0.10f + 0.16f * (float)s.meetings_today
                                          + (s.kind >= SK_MANAGER ? 0.18f : 0.f));
  const float comp0 = (o.cls < 32) ? s.skill[o.cls] : 0.5f;
  const Read rd = observe(w, o, o.systems_opened, true, tacit_share,
                          comp0 * (1.f - 0.30f * s.fatigue), 0x11ULL);
  o.completeness = rd.completeness;

  // --- FRAME. Everything a person renders for another person's eyes. The
  // fraction of work that exists purely because the next reader is human.
  // A day that ends mid-case is a row (HOLD, reason 3: in progress), so the fold
  // can carry the case's state and its last touch without a struct.
  // (value = the completeness in hand, so the fold carries what the reader held)
  auto in_progress = [&]() { o.state = OB_INPROG; res.progressed = true;
    tape.put(R_HOLD, day, o.id, o.cls, seat_id, ARM_HUMAN, 3, 0, o.margin, o.completeness, 0, 0, 0, PROV_H); return res; };
  const float frame_m = 6.0f + 26.0f * sp.decide_frac + 3.0f * (float)o.hops;
  if (s.attn_left - res.minutes < frame_m) return in_progress();
  TL.frame.add(frame_m); CL.frame.add(frame_m); res.minutes += frame_m; act(ACT_FRAME, frame_m, o.completeness);

  // --- DECIDE. The job. A contested claim is not a fifteen-minute affair, and
  // the fact that it spans days is exactly why context decays and why the
  // person re-reads their own notes tomorrow.
  const float dec_m = 12.0f + 105.0f * sp.decide_frac;
  if (s.attn_left - res.minutes < dec_m) return in_progress();
  TL.decide.add(dec_m); CL.decide.add(dec_m); res.minutes += dec_m; act(ACT_DECIDE, dec_m, o.completeness);
  s.fatigue = std::min(1.f, s.fatigue + 0.035f);

  // the decider's own confidence: high when the context is complete and the
  // class is routine; this is the margin that a gate would read
  // the decider's own confidence: how far from the fence their read landed,
  // scaled by how much of the case they hold. This is the margin a gate reads.
  o.margin = 4.0f * rd.signal * (0.35f + 0.65f * o.completeness) + 0.9f * (comp0 - 0.5f);
  // The human's decision is banded by its own margin, by the same rule the
  // machine's is, so the licence's incumbent arm is the firm's own rate in
  // that band and never a default.
  o.band = (uint8_t)band_of(o.margin, f.writ);

  // --- ESCALATE, or not. A person escalates when they are unsure or when the
  // money is above their authority. The receiving manager RE-FETCHES: they were
  // not there for the first fetch and cannot see the IC's screen. That double
  // fetch is the round trip in its purest form.
  const float authority = (s.kind == SK_IC) ? 25000.f : (s.kind == SK_LEAD) ? 60000.f
                        : (s.kind == SK_MANAGER) ? 150000.f : 1e9f;
  const bool over_authority = sp.value > authority;
  const bool unsure = std::fabs(o.margin) < 0.30f;   // UNSURE is small |margin|, not a negative one
  if ((over_authority || unsure) && s.boss >= 0 && o.escalations < 3) {
    const float tm = 6.0f + 4.0f * (float)sp.n_systems;    // writing it up for someone else
    TL.transport.add(tm); CL.transport.add(tm); res.minutes += tm; act(ACT_TRANSPORT, tm);
    o.state = OB_ESCALATED; o.seat = s.boss; ++o.escalations; ++o.hops;
    o.systems_opened = 0;                                   // THE ROUND TRIP: the boss starts cold
    ++st.n_escalated;
    tape.put(R_ESCALATE, day, o.id, o.cls, seat_id, ARM_HUMAN, s.boss, o.escalations, o.margin, sp.value, o.band, 0, 0, PROV_H);
    res.progressed = true;
    return res;
  }

  // --- warrant classes need a signature no matter how sure anyone is
  if (sp.warrant && s.kind < SK_VP && s.boss >= 0) {
    const float tm = 8.0f;
    TL.transport.add(tm); CL.transport.add(tm); res.minutes += tm; act(ACT_TRANSPORT, tm);
    o.state = OB_ESCALATED; o.seat = s.boss; ++o.escalations; ++o.hops; o.systems_opened = 0;
    ++st.n_escalated;
    tape.put(R_ESCALATE, day, o.id, o.cls, seat_id, ARM_HUMAN, s.boss, o.escalations, o.margin, sp.value, o.band, RF_WARRANT, 0, PROV_H);
    res.progressed = true;
    return res;
  }

  // --- COMMIT. Write the answer back. Into another application, of course.
  const float com_m = 6.0f + 4.0f * (float)sp.n_systems;
  if (s.attn_left - res.minutes < com_m) return in_progress();
  TL.commit.add(com_m); CL.commit.add(com_m); res.minutes += com_m; act(ACT_COMMIT, com_m);

  o.decision = rd.choice;
  o.state = OB_DECIDED;
  o.day_decided = day;
  o.by_machine = 0;
  o.seat = seat_id;
  ++st.n_decided; ++st.decided_by_class[o.cls];
  if (day > o.day_due) ++st.n_breached;
  st.completeness.add(o.completeness);
  st.hops.add(o.hops);
  st.cycle_days.add((double)(day - o.day_open));
  tape.put(R_DECIDE, day, o.id, o.cls, seat_id, ARM_HUMAN, o.decision, o.hops, o.margin, sp.value, o.band, 0, 0, PROV_H);
  res.terminal = true; res.progressed = true;
  return res;
}

// ----------------------------------------------------------------------------
// ONE DAY AT ACME
// ----------------------------------------------------------------------------
inline void human_day(World& w, Firm& f, Tape& tape, HumanStats& st,
                      const std::vector<std::vector<int>>& rep, uint32_t day) {
  // reset the day's attention
  for (Seat& s : f.seat) { s.attn_left = s.attention; s.meetings_today = 0; s.fatigue *= 0.55f; }

  // --- THE GLUE, charged first because it is charged first in life.
  // Reading the thread to stay current, skimming the channel, re-reading the
  // case you were on yesterday, answering the question someone asked because
  // they could not see your screen. It is a per-SEAT cost, not a per-item cost,
  // and it is most of the hours. A resident pays none of it: it never puts the
  // context down, so there is nothing to pick back up.
  if (day % 7 < 5) {
    float charged = 0.f;
    for (Seat& s : f.seat) {
      const float base = 0.26f + 0.10f * (float)s.kind / (float)SK_N;
      const float m = s.attention * base * (0.80f + 0.40f * u01(w.seed, 3900, s.id * 977 + day));
      const float take = std::min(s.attn_left, m);
      s.attn_left -= take; charged += take;
      tape.put(R_ACT, day, 0, 0, s.id, ARM_HUMAN, ACT_GLUE, 0, 0.f, take);   // glue is per seat, no class
    }
    st.time.glue.add(charged);
  }

  // --- the calendar runs first, because it does
  std::vector<Meeting> mtg; build_meetings(f, rep, day, mtg);
  for (const Meeting& m : mtg) {
    float charged = 0.f;
    for (int a : m.attend) {
      Seat& s = f.seat[a];
      const float take = std::min(s.attn_left, m.minutes);
      s.attn_left -= take; ++s.meetings_today; charged += take;
    }
    st.time.meeting.add(charged);
    ++st.n_meetings;
    tape.put(R_MEETING, day, 0, 0, m.owner, ARM_HUMAN, (int)m.attend.size(), m.kind, 0.f, charged);
  }

  // --- cross-wire coordination: one touch per live dependency, because two
  // people who cannot see the same state must talk to each other
  int deps_live = 0;
  for (uint32_t i : w.open_idx) if (w.ob[i].dep >= 0 && w.ob[w.ob[i].dep].state != OB_SETTLED) ++deps_live;
  if (deps_live > 0 && day % 7 < 5) {
    const float per = 12.f;
    float charged = 0.f;
    int touched = 0;
    for (Seat& s : f.seat) {
      if (s.kind < SK_LEAD || s.fn != FN_E) continue;
      const float take = std::min(s.attn_left, per * std::min(3.f, deps_live / (float)std::max(1, f.n_lead)));
      s.attn_left -= take; charged += take; ++touched;
    }
    st.time.meeting.add(charged);
    tape.put(R_MEETING, day, 0, 0, -2, ARM_HUMAN, touched, MG_CROSSWIRE, 0.f, charged);
  }

  // --- assignment. A lead pushes open work to whoever has room. Note what the
  // lead CANNOT do: see who is actually best at this class. That information
  // exists (skill[]) and no human in this org has it.
  // Projected load, so work spreads instead of piling on one desk. The lead's
  // information is deliberately poor: remaining attention and nothing else. The
  // fact that skill[] exists and no human in this org can see it is the point —
  // the field can, which is where the first honest gain comes from.
  std::vector<float> proj(f.size(), 0.f);
  // the pool is per CLASS now, because people specialise
  static thread_local std::vector<std::vector<int>> pool;
  pool.assign(32, {});
  for (const Seat& s : f.seat)
    if (s.fn == FN_E && s.kind == SK_IC)
      for (int c = 0; c < 32; ++c) if ((s.spec >> c) & 1u) pool[c].push_back(s.id);
  for (uint32_t i : w.open_idx) {
    const Obligation& q = w.ob[i];
    if (q.seat >= 0 && q.state != OB_SETTLED && q.state != OB_DECIDED)
      proj[q.seat] += 20.f + 9.0f * (float)cls_spec(q.cls).n_systems;
  }
  for (uint32_t i : w.open_idx) {
    Obligation& o = w.ob[i];
    if (o.state != OB_OPEN) continue;
    const ClassSpec& sp = cls_spec(o.cls);
    const auto& P = pool[o.cls < 32 ? o.cls : 0];
    if (P.empty()) continue;
    int best = -1; float best_room = -1e9f;
    for (int sid : P) {
      const float room = f.seat[sid].attn_left - proj[sid];
      if (room > best_room) { best_room = room; best = sid; }
    }
    if (best < 0 || best_room < 5.f) continue;             // nobody has room; it stays OPEN and ages
    proj[best] += 20.f + 9.0f * (float)sp.n_systems;
    o.seat = best; o.state = OB_QUEUED; ++o.hops;
    tape.put(R_ASSIGN, day, o.id, o.cls, best, ARM_HUMAN, 0, o.hops);
  }

  // --- the work. Each seat drains its queue in deadline order until attention
  // runs out. What is left is held — and a hold is a row, with its margin, so
  // that silence is auditable rather than merely absent.
  std::vector<std::vector<uint32_t>> queue(f.size());
  for (uint32_t i : w.open_idx) {
    const Obligation& o = w.ob[i];
    if ((o.state == OB_QUEUED || o.state == OB_INPROG || o.state == OB_ESCALATED) && o.seat >= 0)
      queue[o.seat].push_back(i);
  }
  for (int sid = 0; sid < f.size(); ++sid) {
    auto& q = queue[sid];
    if (q.empty()) continue;
    std::sort(q.begin(), q.end(), [&](uint32_t a, uint32_t b) {
      if (w.ob[a].day_due != w.ob[b].day_due) return w.ob[a].day_due < w.ob[b].day_due;
      return w.ob[a].id < w.ob[b].id; });
    Seat& s = f.seat[sid];
    for (uint32_t i : q) {
      Obligation& o = w.ob[i];
      if (o.dep >= 0 && w.ob[o.dep].state != OB_SETTLED && w.ob[o.dep].state != OB_DECIDED) {
        tape.put(R_HOLD, day, o.id, o.cls, sid, ARM_HUMAN, 1 /*blocked*/, 0, o.margin, 0.f, 0, 0, 0, PROV_H);
        ++st.n_held; continue;
      }
      if (s.attn_left < 12.f) {                              // out of day
        tape.put(R_HOLD, day, o.id, o.cls, sid, ARM_HUMAN, 2 /*no attention*/, 0, o.margin, 0.f, 0, 0, 0, PROV_H);
        ++st.n_held; continue;
      }
      const WorkResult r = human_work(w, f, tape, st, i, sid, day);
      s.attn_left -= r.minutes;
      st.attention_spent += r.minutes;
      if (s.attn_left < 0) s.attn_left = 0;
    }
  }
  for (const Seat& s : f.seat) st.attention_wasted += std::max(0.f, s.attn_left);
}

} // namespace acme
