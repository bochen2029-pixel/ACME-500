// ============================================================================
//  acme/world.h — the world outside the firm, and the boundary between them
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
// ============================================================================
#pragma once
#ifdef ACME_NO_PLANT
#error "world.h is the PLANT: planted truth and the generators. The machine's translation unit must not include it (oracle O17)."
#endif
#include "core.h"
#include "firm.h"
#include "ledger.h"
#include "port.h"

namespace acme {

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
};

// ----------------------------------------------------------------------------
// Build the world's ground truth: which facts decide what, and where they live.
// ----------------------------------------------------------------------------
inline World make_world(uint64_t seed) {
  World w; w.seed = seed;
  const ClassSpec* S = schema(w.NC);
  w.spec.resize(w.NC); w.tacit.assign(w.NC, 0.f); w.n_det.assign(w.NC, 0);

  for (int c = 0; c < w.NC; ++c) {
    const ClassSpec& s = S[c];
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
    for (const Determinant& x : t.det) {
      if (x.where == DW_TACIT) t.tacit_mass += x.weight;
      if (x.where == DW_SYSTEM) t.system_mass += x.weight;
    }
    w.tacit[c] = t.tacit_mass; w.n_det[c] = nd;
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
  const double lam = s.arrival_per_day * w.demand_scale * dow * season * regime;
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
// completeness_from_fetch: what fraction of a class's determinant mass a decider
// actually holds, given which systems they opened, whether they read the inbound
// document, and whether the tacit facts reached them.
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
inline float full_signal(const World& w, const Obligation& o) {
  const TrueSpec& t = w.spec[o.cls];
  float s = 0.f;
  for (size_t d = 0; d < t.det.size(); ++d) s += t.det[d].weight * det_value(w, o.id, (int)d);
  return s;
}
inline int truth_decision(const World& w, const Obligation& o) { return full_signal(w, o) > 0.f ? 1 : 0; }

// What a decider actually reads. Determinants they did not gather contribute
// nothing — they do not get a wrong value, they get NO value, which is exactly
// how a missing fact biases a real decision toward the prior. Judgement noise
// scales with how much of the class is judgement and how little competence the
// decider has: a routine class is robust to a mediocre decider, a contested one
// is not.
struct Read { float signal; float completeness; int choice; };
inline Read observe(const World& w, const Obligation& o, uint32_t systems_mask,
                    bool read_boundary, float tacit_share, float competence,
                    uint64_t noise_key) {
  const TrueSpec& t = w.spec[o.cls];
  const ClassSpec& sp = cls_spec(o.cls);
  Read r{}; r.signal = 0.f; r.completeness = 0.f;
  for (size_t d = 0; d < t.det.size(); ++d) {
    const Determinant& x = t.det[d];
    float share = 0.f;
    switch (x.where) {
      case DW_BOUNDARY: share = read_boundary ? 1.f : 0.f; break;
      case DW_SYSTEM:   share = (systems_mask & (1u << x.system)) ? 1.f : 0.f; break;
      case DW_TACIT:    share = tacit_share; break;
    }
    r.signal += share * x.weight * det_value(w, o.id, (int)d);
    r.completeness += share * x.weight;
  }
  const float noise_sd = 0.08f + 0.90f * sp.decide_frac * (1.f - competence);
  r.signal += noise_sd * unrm(w.seed ^ noise_key, 5200, o.id);
  r.choice = (r.signal > 0.f) ? 1 : 0;
  r.completeness = std::min(1.f, r.completeness);
  return r;
}

inline float completeness_from(const World& w, int c, uint32_t systems_mask,
                               bool read_boundary, float tacit_share) {
  const TrueSpec& t = w.spec[c];
  float got = 0.f;
  for (const Determinant& d : t.det) {
    switch (d.where) {
      case DW_BOUNDARY: if (read_boundary) got += d.weight; break;
      case DW_SYSTEM:   if (systems_mask & (1u << d.system)) got += d.weight; break;
      case DW_TACIT:    got += d.weight * tacit_share; break;
    }
  }
  return std::min(1.f, got);
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
    // THE EXOGENOUS GRADER. Was the choice the right one, and was it in time?
    // Nothing inside the firm writes this, and it does not know or care who
    // decided — the physics has no branch on human versus machine.
    (void)f;
    const bool right = (o.decision == truth_decision(w, o));
    const bool on_time = (o.day_decided <= o.day_due);
    o.outcome = (uint8_t)(right ? (on_time ? OK_GOOD : OK_LATE) : OK_BAD);
    o.state = OB_SETTLED;
    o.day_settled = day;
    // v2: the row names THE DECIDER (seat, arm) and carries the via, so the
    // ladder is a fold of OUTCOME rows joined to nothing else (F18, F19).
    tape.put(R_OUTCOME, day, o.id, o.cls, o.by_machine ? -1 : o.seat, o.by_machine ? ARM_MACHINE : ARM_HUMAN,
             o.outcome, (int)o.via, o.margin, (float)outcome_cost(f.writ, s, o.outcome), o.band, 0, o.via, PROV_D);
  }
  L.compact();
}

// ----------------------------------------------------------------------------
// THE PLANT BEHIND THE PORT. C0: the machine reads cells through Store::frame
// and judges them through Judge::read, and what stands behind both is the
// plant's own arithmetic, bit for bit what v1 computed inline. C1 replaces
// the coverage with the compile step's estimate and the noise key and the act
// coin with the judge's own; every number that moves then is named.
// ----------------------------------------------------------------------------
struct PlantStore : Store {
  const World* w;
  explicit PlantStore(const World* world) : w(world) {}
  Frame frame(uint32_t oid, int cls, uint32_t systems_mask) const override {
    Frame f; f.cls = cls; f.oid = oid; f.systems_mask = systems_mask; f.boundary = true;
    f.coverage_hat = completeness_from(*w, cls, systems_mask, true, 0.f);   // C0: the plant's completeness for the mask
    f.frame_hash = 0;                                                      // C1: f(rows, template pin, span contents)
    return f;
  }
};

struct PlantJudge : Judge {
  const World* w;
  explicit PlantJudge(const World* world) : w(world) {}
  Proposal read(const Frame& f, float competence, uint64_t noise_key) override {
    Obligation o{}; o.id = f.oid; o.cls = (uint16_t)f.cls;                  // observe() reads only id and cls
    const Read r = observe(*w, o, f.systems_mask, f.boundary, 0.f, competence, noise_key);
    Proposal p; p.choice = r.choice; p.signal = r.signal; p.completeness_hat = r.completeness; p.judge_hash = hash();
    return p;
  }
  // F16 preserved in C0 so the numbers do not move: the coin is still drawn from
  // the world's seed. C1 keys it on the judge.
  bool act_coin(int cls, uint32_t oid, float p) override { return u01(w->seed, 7200 + cls, oid) < p; }
  uint32_t hash() const override { return 0x504C4A31u; }                  // 'PLJ1': the plant's read arithmetic, v1
};

} // namespace acme
