// ============================================================================
//  acme/world.h — THE PLANT: the world outside the firm, and the boundary
//                 between them
//
//  An obligation is a thing that is owed, by someone, to someone, by a certain
//  time. It is the unit of work, and it is deliberately NOT a task: a task is a
//  description of how people did it, and encoding that sequence into software
//  preserves the fragmentation that made the sequence necessary.
//
//  THE DETERMINANTS ARE THE POINT OF THIS FILE.
//
//  Every obligation carries a vector of determinants — the facts that actually
//  decide it. Each determinant lives somewhere:
//
//     SYSTEM      in a database the firm already owns, behind an application UI
//     BOUNDARY    in the inbound document itself
//     TACIT       in a phone call, a hallway, a relationship — RECORDED NOWHERE
//
//  A human assembles context by opening applications, one fetch at a time, at a
//  cost in minutes, and by remembering — which decays. The resident assembles it
//  by query, at no cost, but ONLY for determinants that are instrumented.
//
//  So the ceiling on automation in this simulation is not capability. It is
//
//        coverage(class) = 1 − (tacit mass of that class)
//
//  and that is a property of instrumentation, which is buyable, rather than of
//  the model, which is not the bottleneck. The tacit fraction is planted per
//  class and the compile step must MEASURE it without being shown it.
//
//  C1: THIS FILE HOLDS EVERYTHING PLANTED. The per-class round-trip facts (how
//  many systems hold a class's determinants, how much of it is judgement), the
//  generator of the firm, the determinants, the read physics and the grader.
//  The kernel's translation unit cannot include it (O17); what the kernel knows
//  of any of this, it recovered from rows.
// ============================================================================
#pragma once
#ifdef ACME_NO_PLANT
#error "world.h is the PLANT: planted truth and the generators. The machine's translation unit must not include it (oracle O17)."
#endif
#include "core.h"
#include "firm.h"
#include "ledger.h"
#include "port.h"
#include "license.h"   // E3: the boundary judge reads the governor's frozen bar (the instrument's side, never the kernel's)

namespace acme {

// E3: the governor's ladder, exposed to the boundary judge for the run's
// duration so that O31's judge can sit on each TERM's frozen boundary rather
// than on its own daily estimate of it (the bar moves between terms; a term's
// process is a null instance only against that term's bar). Set by the world
// port around a run; read by BoundaryJudge and by nothing else.
inline const Ladder* g_boundary_ladder = nullptr;

// ----------------------------------------------------------------------------
// §0 · THE PLANTED HALF OF THE CLASS TABLE
//
// n_systems is the round-trip cost driver: every system is a fetch a human must
// do by hand through a UI, and the fetches are most of the day. decide_frac is
// the fraction of the round trip that is real judgement. The compile step must
// measure both from the tape (O6 recovers the join graph exactly; O13 recovers
// the ranking of judgement intensity, never its level) and is never shown them.
// ----------------------------------------------------------------------------
struct PlantedSpec {
  const char* name;       // must match the schema row of the same index; make_world checks
  uint8_t n_systems;      // how many systems of record hold this class's determinants
  float   decide_frac;    // PLANTED TRUTH: fraction of the round trip that is real judgment
};
inline const PlantedSpec* planted_table(int& n) {
  static const PlantedSpec P[] = {
    { "quote.price",          4, 0.22f },
    { "quote.approve.disc",   3, 0.30f },
    { "order.credit.check",   3, 0.14f },
    { "order.fulfil.sched",   5, 0.11f },
    { "invoice.issue",        2, 0.04f },
    { "collections.chase",    3, 0.09f },
    { "dispute.resolve",      5, 0.41f },
    { "claim.intake.triage",  3, 0.12f },
    { "claim.coverage.det",   5, 0.35f },
    { "claim.reserve.set",    4, 0.28f },
    { "claim.settle.auth",    4, 0.24f },
    { "claim.fraud.refer",    6, 0.52f },
    { "po.match.3way",        3, 0.05f },
    { "vendor.onboard",       5, 0.33f },
    { "contract.renew",       4, 0.38f },
    { "spend.approve",        2, 0.16f },
    { "req.approve",          3, 0.31f },
    { "candidate.screen",     3, 0.27f },
    { "leave.approve",        2, 0.03f },
    { "accrual.post",         3, 0.08f },
    { "recon.break.clear",    4, 0.19f },
    { "forecast.adjust",      5, 0.45f },
    { "kyc.review",           4, 0.20f },
    { "reg.filing.prepare",   6, 0.29f },
  };
  n = (int)(sizeof(P) / sizeof(P[0]));
  return P;
}
inline const PlantedSpec& planted(int c) { int n; const PlantedSpec* p = planted_table(n); return p[c < 0 ? 0 : (c < n ? c : n - 1)]; }
// The two tables are one table split in two; this is the seam check.
inline void assert_planted_aligned() {
  int ns, np; const ClassSpec* S = schema(ns); const PlantedSpec* P = planted_table(np);
  if (ns != np) { std::fprintf(stderr, "schema/planted tables differ in length (%d vs %d)\n", ns, np); std::abort(); }
  for (int c = 0; c < ns; ++c)
    if (strcmp(S[c].name, P[c].name) != 0) { std::fprintf(stderr, "schema/planted row %d: %s vs %s\n", c, S[c].name, P[c].name); std::abort(); }
}

enum DetWhere : uint8_t { DW_SYSTEM = 0, DW_BOUNDARY, DW_TACIT, DW_N };

struct Determinant {
  uint8_t where;
  uint8_t system;      // which system of record, if DW_SYSTEM
  float   weight;      // how much of the decision this fact carries; sums to 1 per class
};

// A class's context spec as the WORLD defines it (ground truth). The compile
// step produces its own estimate of this and is graded against it by O-COVERAGE.
struct TrueSpec {
  std::vector<Determinant> det;
  float tacit_mass = 0.f;      // sum of weights of DW_TACIT determinants
  float system_mass = 0.f;
};

// THE PLANT. Since C0 this holds only the planted truth and the generators;
// the cells, the open set and the clock live in the Ledger (ledger.h), which
// the plant's arm and the hand mutate while writing the rows that describe it.
struct World {
  uint64_t seed;
  std::vector<TrueSpec>   spec;      // per class, ground truth
  // planted per-class truth the compile step must recover
  std::vector<float> tacit;          // coverage ceiling = 1 - tacit
  std::vector<int>   n_det;

  int NC = 0;
  float demand_scale = 1.0f;   // tunes the firm's utilisation; 1.0 is the schema's nominal rate
  // E0: THE SHARED FACT. One determinant per class, in one of its systems,
  // whose value is shared by every cell of the class and moves at a planted
  // rate (a supplier's date, a rate table, a policy line): one change moves
  // every open frame of the class at once. Off by default; every reading is
  // identical to D3 until it is on.
  bool  shared = false;
  float flip_rate = 0.f;                   // per class per day
  std::vector<int> shared_det;             // [class] the index of the shared determinant, or -1
  std::vector<std::vector<uint32_t>> flips;// [class] the days the fact changed, ascending
  // E0: THE WORLD THAT DOES NOT ANSWER. A decided cell is never graded with
  // this probability; the writ's write-off rule closes it as UNRESOLVED.
  float unresolved_rate = 0.f;
  // E3: A PLANTED SHIFT of the arrival rate from a day (O59's world): the
  // regime detector must hear it, and must hear nothing in a world without one.
  uint32_t shift_day = 0;
  float    shift_mult = 1.f;
};
// the epoch of a class's shared fact on a day: how many changes are at or before it
inline uint32_t shared_epoch(const World& w, int c, uint32_t day) {
  if (!w.shared) return 0;
  const std::vector<uint32_t>& f = w.flips[c];
  uint32_t e = 0; for (uint32_t d : f) { if (d <= day) ++e; else break; }
  return e;
}
inline float shared_x(const World& w, int c, uint32_t epoch) { return w.shared ? unrm(w.seed, 5100 + c, epoch) : 0.f; }

// ----------------------------------------------------------------------------
// §1 · Build ACME: a 500-seat knowledge-work headquarters.   [moved here in C1]
//
// The shape is derived from the span, not typed in. E-seats are allotted to the
// six wires in proportion to arrival rate × handling cost, and then the support
// pyramid is grown on top of the resulting headcount — which is the whole point:
// the α-functions are sized from N, so when N falls they fall with it, whether
// or not anyone automates what they do. The support coefficient and the fixed
// floor it plants come back in `PlantedFirm`, beside the firm and never on it:
// the cascade's regression must be caught missing F, so the kernel cannot hold F.
// ----------------------------------------------------------------------------
struct PlantedFirm { float alpha_true = 0.f; float F_true = 0.f; };

inline Firm build_acme(int target_n = 500, int span = 7, uint64_t seed = 20260913, PlantedFirm* pf = nullptr) {
  Firm f; f.span = span;
  int NC; const ClassSpec* S = schema(NC);
  assert_planted_aligned();

  // --- how much E-work exists, per wire, in units of IC-days per day
  double wire_load[W_N] = {0};
  for (int c = 0; c < NC; ++c) {
    // a class's daily human cost is arrivals × (fetch cost + decide cost)
    const double fetch = 0.12 * planted(c).n_systems;          // hours per instance, per system opened
    const double decide = 0.35 * planted(c).decide_frac + 0.05;
    wire_load[S[c].wire] += S[c].arrival_per_day * (fetch + decide);
  }
  double total_load = 0; for (int w = 0; w < W_N; ++w) total_load += wire_load[w];

  // --- E-seats: ~62% of the firm is front-line E-work at a 500-seat HQ
  const int n_e_ic = (int)std::round(target_n * 0.46);
  int assigned = 0;
  std::vector<int> wire_ic(W_N, 0);
  for (int w = 0; w < W_N; ++w) { wire_ic[w] = (int)std::round(n_e_ic * (wire_load[w] / total_load)); assigned += wire_ic[w]; }
  wire_ic[0] += n_e_ic - assigned;

  auto add = [&](uint8_t kind, uint8_t fn, uint8_t wire, int boss, int depth, float salary, float attn) -> int {
    Seat s{}; s.id = (int)f.seat.size(); s.kind = kind; s.fn = fn; s.wire = wire; s.boss = boss;
    s.depth = depth; s.salary = salary; s.attention = attn; s.attn_left = attn;
    for (int c = 0; c < 32; ++c) s.skill[c] = 0.f;
    f.seat.push_back(s); return s.id;
  };

  // --- the top
  const int ceo = add(SK_CSUITE, FN_WARRANT, W_N, -1, 0, 780000.f, 240.f);
  std::vector<int> vps;
  for (int w = 0; w < W_N; ++w)
    vps.push_back(add(SK_VP, FN_WARRANT, (uint8_t)w, ceo, 1, 340000.f, 300.f));
  // two α-VPs: the ones that exist because the company has people, not customers
  const int vp_ops = add(SK_VP, FN_ALPHA, W_N, ceo, 1, 300000.f, 300.f);   // IT, facilities, PMO
  const int vp_ppl = add(SK_VP, FN_ALPHA, W_N, ceo, 1, 295000.f, 300.f);   // HR, training, comms

  // --- E-line: directors → managers → leads → ICs, span-driven
  for (int w = 0; w < W_N; ++w) {
    const int ics = wire_ic[w];
    if (ics <= 0) continue;
    const int leads = std::max(1, (int)std::ceil(ics / (double)span));
    const int mgrs  = std::max(1, (int)std::ceil(leads / (double)span));
    const int dirs  = std::max(1, (int)std::ceil(mgrs / (double)span));
    std::vector<int> dir_ids, mgr_ids, lead_ids;
    for (int d = 0; d < dirs; ++d)
      dir_ids.push_back(add(SK_DIRECTOR, FN_E, (uint8_t)w, vps[w], 2, 215000.f, 240.f));
    for (int m = 0; m < mgrs; ++m)
      mgr_ids.push_back(add(SK_MANAGER, FN_E, (uint8_t)w, dir_ids[m % dirs], 3, 165000.f, 260.f));
    for (int l = 0; l < leads; ++l)
      lead_ids.push_back(add(SK_LEAD, FN_E, (uint8_t)w, mgr_ids[l % mgrs], 4, 132000.f, 300.f));
    for (int i = 0; i < ics; ++i)
      add(SK_IC, FN_E, (uint8_t)w, lead_ids[i % leads], 5, 98000.f, 330.f);
  }

  // --- the support pyramid: sized from N, which is the whole cascade mechanic.
  // Ratios are the α-coefficients; they are what the regression must recover.
  const int n_so_far = f.size();
  struct AlphaFn { const char* name; double per_head; float salary; int boss; };
  const AlphaFn A[] = {
    { "IT / endpoint / network", 1.0 / 32.0, 118000.f, vp_ops },
    { "internal helpdesk",       1.0 / 55.0,  86000.f, vp_ops },
    { "facilities",              1.0 / 60.0,  74000.f, vp_ops },
    { "PMO / programme",         1.0 / 45.0, 142000.f, vp_ops },
    { "procurement of software", 1.0 / 90.0, 118000.f, vp_ops },
    { "HR generalist",           1.0 / 48.0, 108000.f, vp_ppl },
    { "L&D / training",          1.0 / 85.0,  99000.f, vp_ppl },
    { "internal comms",          1.0 /110.0, 104000.f, vp_ppl },
    { "recruiting",              1.0 / 70.0, 102000.f, vp_ppl },
    { "QA of human error",       1.0 / 38.0, 112000.f, vp_ops },
    { "reporting / BI",          1.0 / 65.0, 128000.f, vp_ops },
  };
  // Solve N = n_E + F + alpha*N for the α-headcount, then instantiate.
  double alpha_sum = 0; for (const AlphaFn& a : A) alpha_sum += a.per_head;
  const int F_floor = 9;                                     // the term a within-firm fit cannot see
  const int N_star  = (int)std::round((n_so_far + F_floor) / (1.0 - alpha_sum));
  if (pf) { pf->alpha_true = (float)alpha_sum; pf->F_true = (float)F_floor; }

  for (const AlphaFn& a : A) {
    int k = std::max(1, (int)std::round(a.per_head * N_star));
    // each α-function above ~span gets its own manager, which is more α
    if (k > span) { const int m = add(SK_MANAGER, FN_ALPHA, W_N, a.boss, 2, 158000.f, 260.f);
                    for (int i = 0; i < k; ++i) add(SK_IC, FN_ALPHA, W_N, m, 3, a.salary, 330.f); }
    else          { for (int i = 0; i < k; ++i) add(SK_IC, FN_ALPHA, W_N, a.boss, 2, a.salary, 330.f); }
  }
  for (int i = 0; i < F_floor; ++i) add(SK_IC, FN_ALPHA, W_N, vp_ops, 2, 96000.f, 330.f);

  // --- trim or pad to the target headcount, from the E-line's ICs
  while (f.size() > target_n) {
    int victim = -1;
    for (int i = f.size() - 1; i >= 0; --i) if (f.seat[i].kind == SK_IC && f.seat[i].fn == FN_E) { victim = i; break; }
    if (victim < 0) break;
    f.seat.erase(f.seat.begin() + victim);
    for (size_t i = 0; i < f.seat.size(); ++i) { f.seat[i].id = (int)i; if (f.seat[i].boss > victim) --f.seat[i].boss; }
  }
  while (f.size() < target_n) {
    int host = -1; for (const Seat& s : f.seat) if (s.kind == SK_LEAD) { host = s.id; break; }
    add(SK_IC, FN_E, f.seat[host].wire, host, 5, 98000.f, 330.f);
  }

  // --- specialisation and skill. Each seat handles a few classes inside its
  // wire, not all of them, and a handful of people are genuinely the expert at
  // one of them. THE ORG DOES NOT KNOW WHO. Nothing in the assignment path
  // reads skill[]; the lead sees remaining attention and a specialty tag, which
  // is exactly the information a real lead has.
  for (Seat& s : f.seat) {
    s.spec = 0u;
    std::vector<int> own;
    for (int c = 0; c < NC && c < 32; ++c) if (s.wire == S[c].wire) own.push_back(c);
    if (s.fn != FN_ALPHA && !own.empty()) {
      const int want = std::min<int>((int)own.size(), (s.kind == SK_IC) ? 2 + ubelow(seed, 640, s.id, 2)
                                                                       : (int)own.size());
      for (int k = 0; k < want; ++k) s.spec |= (1u << own[(ubelow(seed, 650 + k, s.id, (int)own.size()))]);
      if (!s.spec) s.spec |= (1u << own[0]);
    }
    for (int c = 0; c < NC && c < 32; ++c) {
      if (s.fn == FN_ALPHA) { s.skill[c] = 0.f; continue; }
      const bool mine = (s.spec >> c) & 1u;
      const bool in_wire = (s.wire == S[c].wire || s.wire == W_N);
      float base = mine ? 0.55f : (in_wire ? 0.30f : 0.12f);
      base += 0.05f * std::min<int>(s.kind, SK_DIRECTOR);
      float sk = base + 0.17f * unrm(seed, 700 + c, s.id);
      if (mine && ucoin(seed, 800 + c, s.id, 0.14f)) sk += 0.32f;   // the planted expert
      s.skill[c] = std::min(0.99f, std::max(0.02f, sk));
    }
  }
  // counts
  for (const Seat& s : f.seat) switch (s.kind) {
    case SK_IC: ++f.n_ic; break; case SK_LEAD: ++f.n_lead; break; case SK_MANAGER: ++f.n_mgr; break;
    case SK_DIRECTOR: ++f.n_dir; break; case SK_VP: ++f.n_vp; break; default: ++f.n_c; break; }
  return f;
}

// ----------------------------------------------------------------------------
// §2 · Build the world's ground truth: which facts decide what, and where they live.
// ----------------------------------------------------------------------------
inline World make_world(uint64_t seed, bool shared = false, float flip_rate = 0.025f, float unresolved_rate = 0.f, int horizon_days = 4000) {
  World w; w.seed = seed; w.shared = shared; w.flip_rate = shared ? flip_rate : 0.f; w.unresolved_rate = unresolved_rate;
  schema(w.NC);
  assert_planted_aligned();
  w.spec.resize(w.NC); w.tacit.assign(w.NC, 0.f); w.n_det.assign(w.NC, 0);
  w.shared_det.assign(w.NC, -1); w.flips.assign(w.NC, {});

  for (int c = 0; c < w.NC; ++c) {
    const PlantedSpec& s = planted(c);
    // more systems means more determinants, and a judgement-heavy class has a
    // longer tail of them
    const int nd = 3 + s.n_systems + (int)(6.0f * s.decide_frac);
    TrueSpec& t = w.spec[c]; t.det.resize(nd);
    float tot = 0.f;
    for (int d = 0; d < nd; ++d) {
      Determinant& x = t.det[d];
      const float r = u01(seed, 1300 + c, d);
      // the tacit share rises with how much of the class is real judgement:
      // routine work is written down, contested work lives in a phone call
      const float p_tacit = 0.02f + 0.38f * s.decide_frac;
      if (d == 0)                  x.where = DW_BOUNDARY;
      else if (r < p_tacit)        x.where = DW_TACIT;
      else                         x.where = DW_SYSTEM;
      x.system = (uint8_t)ubelow(seed, 1400 + c, d, std::max<int>(1, s.n_systems));
      x.weight = 0.f;
    }
    // Zipf weights over a PERMUTATION of the determinants: a couple of facts
    // carry most of the decision, which is why ablation can find a minimal
    // sufficient set at all — but which facts those are is not correlated with
    // where they happen to live. Sometimes the heaviest determinant is the one
    // nobody wrote down, and that class is the hard one.
    {
      std::vector<int> perm(nd); for (int d = 0; d < nd; ++d) perm[d] = d;
      for (int d = nd - 1; d > 0; --d) std::swap(perm[d], perm[ubelow(seed, 1500 + c, d, d + 1)]);
      for (int k = 0; k < nd; ++k) {
        const float wgt = 1.0f / std::pow(1.0f + (float)k, 0.85f);
        t.det[perm[k]].weight = wgt; tot += wgt;
      }
    }
    for (Determinant& x : t.det) x.weight /= tot;
    // E0: the shared determinant, appended, in one of the class's systems, at a
    // fifth of the class's mass; its value is shared_x(c, epoch), never per cell
    if (shared) {
      Determinant x; x.where = DW_SYSTEM; x.system = (uint8_t)ubelow(seed, 1450 + c, 0, std::max<int>(1, s.n_systems)); x.weight = 0.25f;
      for (Determinant& y : t.det) y.weight /= 1.25f;
      x.weight /= 1.25f;
      t.det.push_back(x); w.shared_det[c] = (int)t.det.size() - 1;
      for (int d = 0; d < horizon_days; ++d) if (u01(seed, 5150 + c, (uint32_t)d) < w.flip_rate) w.flips[c].push_back((uint32_t)d);
    }
    for (const Determinant& x : t.det) {
      if (x.where == DW_TACIT) t.tacit_mass += x.weight;
      if (x.where == DW_SYSTEM) t.system_mass += x.weight;
    }
    w.tacit[c] = t.tacit_mass; w.n_det[c] = (int)t.det.size();
  }
  return w;
}

// ----------------------------------------------------------------------------
// Arrivals. Poisson-ish, with a weekly seasonality and a slow regime change so
// the drift detector has something to find.
// ----------------------------------------------------------------------------
inline int arrivals_today(const World& w, int c, uint32_t day) {
  const ClassSpec& s = cls_spec(c);
  const float dow = (day % 7 >= 5) ? 0.18f : 1.18f;                       // weekends
  const float season = 1.0f + 0.22f * std::sin(day * 0.0172f);
  const float regime = (day > 260) ? 1.25f : 1.0f;                        // a planted regime change
  const float shift = (w.shift_day > 0 && day >= w.shift_day) ? w.shift_mult : 1.0f;   // E3: the planted shift, O59's
  const double lam = s.arrival_per_day * w.demand_scale * dow * season * regime * shift;
  // inverse-transform Poisson, deterministic on (day, class)
  double L = std::exp(-lam), p = 1.0; int k = 0;
  do { ++k; p *= u01(w.seed, 2000 + c, day * 131 + k); } while (p > L && k < 400);
  return k - 1;
}

inline void world_arrive(World& w, Ledger& L, Tape& tape, uint32_t day) {
  for (int c = 0; c < w.NC; ++c) {
    const ClassSpec& s = cls_spec(c);
    const int k = arrivals_today(w, c, day);
    for (int i = 0; i < k; ++i) {
      Obligation o{};
      o.id = L.next_id;
      o.cls = (uint16_t)c;
      o.day_open = day;
      const int slack = 3 + (int)(u01(w.seed, 2100 + c, o.id) * 12);
      o.day_due = day + slack;
      o.state = OB_OPEN;
      o.last_touch = day;
      // ~9% of obligations wait on another one — the dependency that makes the
      // period a joint problem rather than N independent ones
      if (!L.ob.empty() && ucoin(w.seed, 2200, o.id, 0.09f)) {
        const int j = ubelow(w.seed, 2201, o.id, (int)L.ob.size());
        if (L.ob[j].state != OB_SETTLED) o.dep = (int)j;
      }
      L.open_cell(o);
      // v2: the due day and the dependency are ON THE ROW (F19). A cold fold of
      // the tape can now rebuild the cell; v1 wrote a = b = 0 here.
      tape.put(R_ARRIVE, day, o.id, c, -2, ARM_GOVERNOR, (int)o.day_due,
               o.dep >= 0 ? (int)L.ob[(size_t)o.dep].id : -1, 0.f, s.value);
    }
  }
}

// ----------------------------------------------------------------------------
// CONTEXT ASSEMBLY — the mechanic the whole simulation turns on.
//
// gather(): what fraction of a class's determinant mass a decider actually
// holds, given which systems they opened, whether they read the inbound
// document, and whether the tacit facts reached them, and the signal that
// mass carries. (C0's completeness_from, the same sum without the signal, had
// no caller once the judge read through gather() and was deleted in C1.)
//
// A human gets tacit facts only by TALKING TO SOMEONE — which is what a meeting
// or a hallway conversation actually is, and it is the one thing a meeting does
// that is not pure round trip. The simulation gives meetings exactly that value
// and no more, which is why deleting them is not free and is still worth it.
// ----------------------------------------------------------------------------
// THE FACT ITSELF. Each determinant of each obligation carries a value; the
// weighted sum of all of them is the signal, and the sign of the signal is what
// the decision SHOULD be. Nobody in the firm can see the whole sum. That is the
// entire epistemic situation of an organisation, in one function.
inline float det_value(const World& w, uint32_t oid, int d) {
  return unrm(w.seed, 5000 + d, oid);
}
// E0: the value of determinant d of a cell at a shared epoch: the shared
// determinant reads the class's shared value, every other one its own
inline float det_value_at(const World& w, int cls, uint32_t oid, int d, uint32_t epoch) {
  if (w.shared && d == w.shared_det[cls]) return shared_x(w, cls, epoch);
  return det_value(w, oid, d);
}
inline float full_signal(const World& w, const Obligation& o, uint32_t epoch) {
  const TrueSpec& t = w.spec[o.cls];
  float s = 0.f;
  for (size_t d = 0; d < t.det.size(); ++d) s += t.det[d].weight * det_value_at(w, o.cls, o.id, (int)d, epoch);
  return s;
}
// the truth of a cell is the truth in force when it was decided: the shared
// fact's epoch on the day of the decision
inline int truth_decision(const World& w, const Obligation& o) { return full_signal(w, o, shared_epoch(w, o.cls, o.day_decided)) > 0.f ? 1 : 0; }

// What a decider gathers, before any judgement: the determinants in hand,
// summed with their weights, and the completeness that sum represents.
// Determinants they did not gather contribute nothing — they do not get a wrong
// value, they get NO value, which is exactly how a missing fact biases a real
// decision toward the prior. C1 splits this out of observe() so that a judge
// behind the port can add its OWN noise from its OWN key (F16).
struct Gathered { float signal; float completeness; };
inline Gathered gather(const World& w, uint32_t oid, int cls, uint32_t systems_mask,
                       bool read_boundary, float tacit_share, uint32_t epoch) {
  const TrueSpec& t = w.spec[cls];
  Gathered g{}; g.signal = 0.f; g.completeness = 0.f;
  for (size_t d = 0; d < t.det.size(); ++d) {
    const Determinant& x = t.det[d];
    float share = 0.f;
    switch (x.where) {
      case DW_BOUNDARY: share = read_boundary ? 1.f : 0.f; break;
      case DW_SYSTEM:   share = (systems_mask & (1u << x.system)) ? 1.f : 0.f; break;
      case DW_TACIT:    share = tacit_share; break;
    }
    g.signal += share * x.weight * det_value_at(w, cls, oid, (int)d, epoch);
    g.completeness += share * x.weight;
  }
  g.completeness = std::min(1.f, g.completeness);
  return g;
}
// The judgement noise: it scales with how much of the class is judgement and
// how little competence the decider has. A routine class is robust to a
// mediocre decider, a contested one is not.
inline float judgement_noise_sd(int cls, float competence) {
  return 0.08f + 0.90f * planted(cls).decide_frac * (1.f - competence);
}

// What a HUMAN decider actually reads: the gathered sum plus noise drawn from
// the world's seed (people are part of the plant). The machine's read is the
// judge's, behind the port, on its own key.
struct Read { float signal; float completeness; int choice; };
inline Read observe(const World& w, const Obligation& o, uint32_t systems_mask,
                    bool read_boundary, float tacit_share, float competence,
                    uint64_t noise_key, uint32_t day) {
  const Gathered g = gather(w, o.id, o.cls, systems_mask, read_boundary, tacit_share, shared_epoch(w, o.cls, day));
  Read r{}; r.signal = g.signal; r.completeness = g.completeness;
  r.signal += judgement_noise_sd(o.cls, competence) * unrm(w.seed ^ noise_key, 5200, o.id);
  r.choice = (r.signal > 0.f) ? 1 : 0;
  return r;
}

// The world settles what has been decided, after each class's verdict latency.
// This is the ONLY function that writes an outcome, and it never sees who
// decided — the grader is exogenous by construction.
inline void world_settle(World& w, Ledger& L, const Firm& f, Tape& tape, uint32_t day) {
  for (uint32_t idx : L.open_idx) {
    Obligation& o = L.ob[idx];
    if (o.state != OB_DECIDED) continue;
    const ClassSpec& s = cls_spec(o.cls);
    if (day < o.day_decided + s.verdict_latency) continue;
    // E0: THE WORLD THAT DOES NOT ANSWER. With the planted rate the verdict never
    // comes; the writ's write-off rule closes the cell as UNRESOLVED after
    // write_off_terms latencies, by an explicit row that is evidence for nothing
    // and releases the exposure it held. A timeout never becomes a success.
    const bool answers = (w.unresolved_rate <= 0.f) || (u01(w.seed, 5300 + o.cls, o.id) >= w.unresolved_rate);
    if (!answers) {
      if (day < o.day_decided + (uint32_t)f.writ.write_off_terms * s.verdict_latency) continue;
      o.outcome = OK_UNRESOLVED; o.state = OB_SETTLED; o.day_settled = day;
      L.release_exposure(o);
      tape.put(R_OUTCOME, day, o.id, o.cls, o.by_machine ? -1 : o.seat, o.by_machine ? ARM_MACHINE : ARM_HUMAN,
               o.outcome, (int)o.via, o.margin, (float)outcome_cost(f.writ, s, o.outcome), o.band, 0, o.via, PROV_D);
      continue;
    }
    // THE EXOGENOUS GRADER. Was the choice the right one, and was it in time?
    // Nothing inside the firm writes this, and it does not know or care who
    // decided — the physics has no branch on human versus machine.
    const bool right = (o.decision == truth_decision(w, o));
    const bool on_time = (o.day_decided <= o.day_due);
    o.outcome = (uint8_t)(right ? (on_time ? OK_GOOD : OK_LATE) : OK_BAD);
    o.state = OB_SETTLED;
    o.day_settled = day;
    L.release_exposure(o);
    // v2: the row names THE DECIDER (seat, arm) and carries the via, so the
    // ladder is a fold of OUTCOME rows joined to nothing else (F18, F19).
    tape.put(R_OUTCOME, day, o.id, o.cls, o.by_machine ? -1 : o.seat, o.by_machine ? ARM_MACHINE : ARM_HUMAN,
             o.outcome, (int)o.via, o.margin, (float)outcome_cost(f.writ, s, o.outcome), o.band, 0, o.via, PROV_D);
  }
  L.compact();
}

// ----------------------------------------------------------------------------
// §3 · THE PLANT BEHIND THE PORT.
//
// The STORE answers what a cell's record holds: in the synthetic world the
// determinant values behind the instrumented systems and the boundary, which
// are a pure function of (oid, class, mask), so the frame hash is a hash of
// exactly that. It knows nothing of coverage: the kernel stamps its own
// estimate on the frame before the judge sees it.
//
// The JUDGE is the plant's read arithmetic over what the store gathered, with
// a competence of its own and noise drawn from ITS OWN seed (F16: v1 drew the
// machine's noise and the rented mind's coin from the world's seed). Two
// instances stand behind the port in ACME: the resident judge, which starts
// clumsy and learns its competence from the outcomes the kernel tells it about
// [P], and the frontier judge at a fixed high competence [P]. Both are stand-ins
// for a model; the gym sweeps their competence so the kernel is trained robust
// to how good the real judge turns out to be.
// ----------------------------------------------------------------------------
struct PlantStore : Store {
  const World* w;
  explicit PlantStore(const World* world) : w(world) {}
  Frame frame(uint32_t oid, int cls, uint32_t systems_mask, uint32_t day) const override {
    Frame f; f.cls = cls; f.oid = oid; f.systems_mask = systems_mask; f.boundary = true;
    f.coverage_hat = 0.f;                                                  // the kernel's to stamp
    // the record's content hash: in the synthetic world the record is a pure
    // function of (oid, cls, mask) and, E0, the shared fact's epoch; a real
    // store hashes the cell's rows and the spans. base_hash holds the shared
    // fact out, so the kernel can tell the two kinds of change apart.
    const uint32_t epoch = shared_epoch(*w, cls, day);
    const bool in_frame = w->shared && w->shared_det[cls] >= 0 && (systems_mask & (1u << w->spec[cls].det[w->shared_det[cls]].system));
    Blake2b b; b.update(&oid, 4); b.update(&cls, 4); b.update(&systems_mask, 4);
    uint8_t h[32]; b.final(h);
    f.base_hash = (uint32_t)h[0] | ((uint32_t)h[1] << 8) | ((uint32_t)h[2] << 16) | ((uint32_t)h[3] << 24);
    if (in_frame) { Blake2b b2; b2.update(&f.base_hash, 4); b2.update(&epoch, 4); b2.final(h);
      f.frame_hash = (uint32_t)h[0] | ((uint32_t)h[1] << 8) | ((uint32_t)h[2] << 16) | ((uint32_t)h[3] << 24);
      f.shared_epoch = epoch; f.shared_value = shared_x(*w, cls, epoch);
    } else f.frame_hash = f.base_hash;
    return f;
  }
};

struct PlantJudge : Judge {
  const World* w;
  uint64_t judge_seed;                 // the judge's own key: never the world's seed
  uint32_t id_hash;
  bool     learns;
  int      invert_class = -1;          // O47's lie: on this class the judge flips its choice where it is most confident (|signal| > 1)
  std::vector<float> comp, comp_n;     // competence per class, and the evidence behind it
  PlantJudge(const World* world, uint64_t seed, uint32_t hash_, float comp0, bool learns_)
    : w(world), judge_seed(seed), id_hash(hash_), learns(learns_), comp(world->NC, comp0), comp_n(world->NC, 0.f) {}
  Proposal read(const Frame& f) override {
    const Gathered g = gather(*w, f.oid, f.cls, f.systems_mask, f.boundary, 0.f, f.shared_epoch);
    Proposal p;
    p.signal = g.signal + judgement_noise_sd(f.cls, competence(f.cls)) * unrm(judge_seed, 5200, f.oid);
    if ((int)f.cls == invert_class && std::fabs(p.signal) > 1.0f) p.signal = -p.signal;   // THE LIE (O47): confidently wrong
    p.choice = (p.signal > 0.f) ? 1 : 0;
    p.completeness_hat = f.coverage_hat;                                   // it reports what it was told it holds
    p.judge_hash = id_hash;
    // E0: an arithmetic judge can say what the shared fact weighs in ITS read:
    // its own certificate, through the port. Its noise is per cell and does not
    // move with the fact, so the certificate is exact for this judge.
    if (w->shared && w->shared_det[f.cls] >= 0) {
      const Determinant& x = w->spec[f.cls].det[w->shared_det[f.cls]];
      p.has_sens = true;
      p.sensitivity = (f.systems_mask & (1u << x.system)) ? x.weight : 0.f;
    }
    return p;
  }
  // The rented mind's coin, drawn by the judge on its own key (F16).
  bool act_coin(int cls, uint32_t oid, float p) override { return u01(judge_seed, 7200 + cls, oid) < p; }
  // The kernel tells the judge what the world said about a cell it decided.
  // A stand-in model gets better with feedback [P]; a real model may ignore it.
  // This is never evidence for a licence: the ladder consumes OUTCOME rows.
  void observe(int cls, bool right) override {
    if (!learns || cls < 0 || cls >= (int)comp.size()) return;
    const float k = 1.f / (comp_n[cls] + 8.f);
    comp[cls] += k * ((right ? 1.f : 0.f) - comp[cls]);
    comp_n[cls] += 1.f;
  }
  float competence(int cls) const {
    if (cls < 0 || cls >= (int)comp.size()) return 0.6f;
    return std::min(0.97f, std::max(0.40f, comp[cls]));
  }
  uint32_t hash() const override { return id_hash; }
};

// The two judges ACME runs with, and the keys that name them.
enum : uint32_t { JUDGE_RESIDENT_HASH = 0x504C4A32u,    // 'PLJ2': the plant's arithmetic, own key, learning competence
                  JUDGE_FRONTIER_HASH = 0x504C4631u };  // 'PLF1': the plant's arithmetic at a fixed high competence
inline PlantJudge make_resident_judge(const World* w) { return PlantJudge(w, 0x4A55444745ULL /*'JUDGE'*/, JUDGE_RESIDENT_HASH, 0.55f, true); }
inline PlantJudge make_frontier_judge(const World* w) { return PlantJudge(w, 0x46524F4E54ULL /*'FRONT'*/, JUDGE_FRONTIER_HASH, 0.96f, false); }

// ----------------------------------------------------------------------------
// E3: THE BOUNDARY JUDGE — O31's instrument.
//
// The ladder's false-promotion rate is the probability that a band whose judge
// is NO BETTER than the null promotes anyway. Measuring it needs a judge that
// sits exactly on the null's boundary, and only the plant can build one,
// because only the plant knows the truth. This judge's DIRECTION is the plant's
// arithmetic at a fixed competence, so its bands are populated like a real
// judge's; its CHOICE is the truth with a probability chosen so that its good
// rate lands on a target per class-band: the incumbent's own rate in the band
// as the ladder's plug-in reads it (`at bar`, the null of the rule as built),
// or the rate a delta worse on the failure side, g0 = 1 - (1 - gbar)(1 + delta)
// (`at g0`, the null the ladder v2 tests). A cell already past its due day
// cannot be good whatever is chosen, so the coin is spent on on-time reads only,
// scaled by the running late share of the band, and the achieved rate is
// printed beside the target so the distance is a reading.
//
// It reads the plant's ledger for the incumbent's rates and the due day: it is
// the plant's judge, never the kernel's. `shift` lifts the target: the lie a
// judge above the boundary tells under the boundary judge's name (O31's lie).
// ----------------------------------------------------------------------------
enum BoundaryAt { BJ_AT_BAR = 0, BJ_AT_G0 = 1, BJ_AT_G0U = 2 };   // g0 from the plug-in bar (the rule as built); g0 from the bar at its upper bound (the ladder v2)
enum : uint32_t { JUDGE_BOUNDARY_HASH = 0x504C4231u };   // 'PLB1'
struct BoundaryJudge : Judge {
  const World* w; const Ledger* L; const Writ* wr;
  int   at;
  float delta, comp, shift;
  uint64_t key = 0x424F554E44ULL;                      // 'BOUND': its own key, never the world's
  std::vector<long>   reads, late_reads;               // [cls*NBAND+b], reads and reads already past due (a reading, not an input)
  std::vector<double> target, gbar, late_act;          // [cls*NBAND+b], refreshed once a day from the ledger
  std::vector<double> used_sum; std::vector<long> used_n;   // [cls*NBAND+b] the target as used when a coin was spent, so the reading is fair
  std::vector<float>  pc_used;                         // [oid] the probability of a correct choice the coin was spent at (1 on a late read; -1 never read)
  uint32_t target_day = 0xFFFFFFFFu;
  BoundaryJudge(const World* world, const Ledger* ledger, const Writ* writ, int at_, float delta_, float comp_, float shift_)
    : w(world), L(ledger), wr(writ), at(at_), delta(delta_), comp(comp_), shift(shift_),
      reads((size_t)world->NC * NBAND, 0), late_reads((size_t)world->NC * NBAND, 0),
      target((size_t)world->NC * NBAND, 0.72), gbar((size_t)world->NC * NBAND, 0.72), late_act((size_t)world->NC * NBAND, 0.0),
      used_sum((size_t)world->NC * NBAND, 0.0), used_n((size_t)world->NC * NBAND, 0) {}
  double target_used(size_t k) const { return used_n[k] ? used_sum[k] / (double)used_n[k] : target[k]; }
  // Once a day, from the ledger: the incumbent's good rate per class-band,
  // folded from the settled human cells exactly as the ladder's plug-in reads
  // it (license.h: the rate past twelve outcomes, 0.72 before); and the late
  // share of the WAGER's own acts per band (via 1 and 3, decided past their due
  // day), which is the share of the judge's acted cells that cannot be good
  // whatever it chose, so that the coin on the on-time reads lands the band's
  // good rate on the target. The first estimate of that share, over first
  // reads, was dominated by the warm backlog and clipped the coin to certainty
  // for weeks while the fresh arrivals were on time: nine points above target.
  void refresh() {
    if (target_day == L->day) return;
    target_day = L->day;
    std::vector<long> n((size_t)w->NC * NBAND, 0), g((size_t)w->NC * NBAND, 0);
    std::vector<long> an((size_t)w->NC * NBAND, 0), al((size_t)w->NC * NBAND, 0);
    for (const Obligation& o : L->ob) {
      if (o.cls >= w->NC) continue;
      const size_t k = (size_t)o.cls * NBAND + (o.band < NBAND ? o.band : NBAND - 1);
      // over the last thirty days only: the cumulative share lagged the backlog's
      // clearing and ran the coin hot once the fresh arrivals dominated (+3.7 points)
      if (o.by_machine && (o.via == 1 || o.via == 3) && (o.state == OB_DECIDED || o.state == OB_SETTLED) && o.day_decided + 30 > L->day) {
        ++an[k]; if (o.day_decided > o.day_due) ++al[k];
      }
      if (o.state != OB_SETTLED || o.by_machine || o.outcome == OK_UNRESOLVED) continue;
      ++n[k]; if (o.outcome == OK_GOOD) ++g[k];
    }
    for (size_t k = 0; k < n.size(); ++k) {
      double p = (n[k] >= 12) ? (double)g[k] / (double)n[k] : 0.72;
      if (at == BJ_AT_G0U && n[k] >= 12) {                        // the ladder v2's bar: the rate at its upper one-sided bound
        const double se = std::sqrt(std::max(0.0, p * (1.0 - p)) / (double)n[k]);
        p = std::min(0.98, std::max(0.02, p + (double)wr->bar_z * se));
      }
      gbar[k] = p;
      double t = (at == BJ_AT_BAR) ? p : 1.0 - (1.0 - p) * (1.0 + (double)delta);
      // the ladder v2's null: the TERM's frozen boundary itself, where the governor has one
      if (at == BJ_AT_G0U && g_boundary_ladder && (int)(k / NBAND) < g_boundary_ladder->NC) {
        const Lic& L = g_boundary_ladder->at((int)(k / NBAND), (int)(k % NBAND));
        if (L.bar_g0 > 0.0) t = L.bar_g0;
      }
      target[k] = std::min(0.98, std::max(0.02, t + (double)shift));
      late_act[k] = an[k] ? (double)al[k] / (double)an[k] : 0.0;
    }
  }
  Proposal read(const Frame& f) override {
    refresh();
    const Gathered gth = gather(*w, f.oid, f.cls, f.systems_mask, f.boundary, 0.f, f.shared_epoch);
    const float signal = gth.signal + judgement_noise_sd(f.cls, comp) * unrm(key, 5200, f.oid);
    const float direction = 4.0f * signal * (0.35f + 0.65f * f.coverage_hat);
    const int b = band_of(direction, *wr);
    const size_t k = (size_t)f.cls * NBAND + b;
    Obligation o{}; o.id = f.oid; o.cls = (uint16_t)f.cls;
    const int truth = full_signal(*w, o, f.shared_epoch) > 0.f ? 1 : 0;
    const Obligation* cell = L->at_oid(f.oid);
    const bool late = cell && L->day > cell->day_due;
    ++reads[k]; if (late) ++late_reads[k];
    if (f.oid >= pc_used.size()) pc_used.resize((size_t)f.oid + 1024, -1.f);
    // The coin is the target itself on an on-time read, and nothing corrects it
    // for the machine's own late share: every estimate of that share tried (the
    // cumulative, a trailing window, the wager's acted cells) ran the coin to
    // certainty during the backlog era and placed the judge above its target.
    // So the judge's good rate is the target times the share of its acts that
    // were on time, which is INSIDE the null by construction, and its true
    // position is read off the coins it spent (pc_used) and printed. Where a
    // judge at the boundary itself is wanted, --shift lifts the target.
    bool correct = true;                                              // a late read cannot be good; the coin is not spent on it
    if (!late) {
      const double pc = std::min(1.0, target[k]);
      correct = u01(key, 7300 + f.cls, f.oid) < (float)pc;
      used_sum[k] += target[k]; used_n[k] += 1;                      // the target as it stood when the coin was spent
      pc_used[f.oid] = (float)pc;                                     // the coin itself, per cell, so the judge's true position is a reading
    } else pc_used[f.oid] = 1.f;
    Proposal p; p.choice = correct ? truth : 1 - truth;
    p.signal = std::fabs(signal) * (p.choice ? 1.f : -1.f);   // the magnitude is the read's; the sign is the choice's
    p.completeness_hat = f.coverage_hat; p.judge_hash = JUDGE_BOUNDARY_HASH;
    return p;
  }
  bool act_coin(int cls, uint32_t oid, float p) override { return u01(key, 7200 + cls, oid) < p; }
  uint32_t hash() const override { return JUDGE_BOUNDARY_HASH; }
};

} // namespace acme
