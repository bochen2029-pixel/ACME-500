// ============================================================================
//  acme/firm.h — ACME Corp: 500 seats, and the one function that decides
//                whether a decision was any good
//
//  THE CONE.  Span of control ~7, four layers plus a C-suite. That number is
//  not a style choice: it is the coarsening ratio of human attention, and it is
//  why the org chart has the depth it has. A GPU's coarsening ratio is N, which
//  is the entire reason the middle stops being necessary rather than becoming
//  automated. The cone here is generated from the span, so changing the span
//  changes the whole shape and the whole cascade downstream.
//
//  THE α/E SPLIT.  Every seat is tagged E (its headcount tracks external work)
//  or α (its headcount tracks headcount). IT for people, training for people,
//  the PMO, internal comms, most of HR, the QA that exists because humans err:
//  none of these scale with orders, all of them scale with staff.
//        N = E / (1 − α) + F
//  F is the fixed floor — one IT person exists at five people — and it is the
//  term a within-firm regression cannot see, because the firm has never been
//  small. It is planted here so the estimator can be caught missing it.
//
//  THE ONE DYNAMICS SOURCE.  decision_quality() is the only place in this
//  program where the goodness of a decision is defined. The human arm calls it.
//  The machine arm calls it. The twin calls it. If they called different
//  functions the comparison would be a cheat with extra steps, which is the
//  booster-lander prime directive carried across.
//
//  And the load-bearing variable inside it is COMPLETENESS, not intelligence.
//  A frontier model with the right context beats the expert; that premise is
//  taken as settled and is not what this simulation tests. What it tests is who
//  can assemble the context, at what cost, at what rate. So a human's
//  completeness falls out of how many systems they had time to open and how
//  stale their memory of the case is; a machine's falls out of instrumentation
//  coverage. Same curve, different inputs.
// ============================================================================
#pragma once
#include "core.h"

namespace acme {

enum SeatKind : uint8_t {
  SK_IC = 0,        // front line: does the E-work
  SK_LEAD,          // team lead: does E-work AND coordination
  SK_MANAGER,       // middle: mostly coordination, some adjudication
  SK_DIRECTOR,      // middle: rollups, cross-wire arbitration
  SK_VP,            // warrant-adjacent
  SK_CSUITE,        // the writ, and the signatures the law reserves
  SK_N
};
inline const char* seat_kind_name(int k) {
  static const char* n[] = { "IC", "lead", "manager", "director", "VP", "C-suite" };
  return (k >= 0 && k < SK_N) ? n[k] : "?";
}

// Why a seat exists. This is the tag the α/E regression must recover from
// payroll and the order book WITHOUT being told.
enum Funct : uint8_t {
  FN_E = 0,         // scales with external volume
  FN_ALPHA,         // scales with headcount
  FN_WARRANT,       // exists because someone must be liable
  FN_N
};

struct Seat {
  int      id;
  uint8_t  kind;
  uint8_t  fn;
  uint8_t  wire;            // primary wire, or W_N for cross-cutting
  int      boss;            // -1 for the top
  int      depth;
  float    salary;          // fully loaded, annual
  float    attention;       // productive minutes per day. THE SCARCE RESOURCE.
  float    skill[32];       // per-class competence in [0,1]
  uint32_t spec;            // the classes this seat actually handles. People specialise, and
                            // specialisation is why a (seat,class) pair accumulates enough
                            // evidence for anyone — the field included — to rank it.
  int      n_reports;
  // live state, reset by the day loop
  float    attn_left;
  float    fatigue;
  int      meetings_today;
};

struct Writ {
  // The only authored objective. Never fitted, never learned.
  float w_bad        = 1.0f;    // cost weight on a bad outcome
  float w_late       = 0.25f;
  float w_salary     = 1.0f;
  float w_unplaced   = 0.6f;    // backlog is a stock, and a stock has a price
  float w_frontier   = 1.0f;    // the rented-mind bill
  float kappa_max    = 1.0f;    // a class whose supervision cost exceeds its saving is demoted
  float thin_margin  = 0.55f;   // below this, the on-card surrogate asks the frontier
  float eps_floor    = 0.03f;   // the uniform draw. A floor the kernel may raise, never lower.
  float canary_delta = 0.30f;   // the non-inferiority margin that sets n0
  int   audit_min    = 3;       // a sampled review fraction never reaches zero
  uint64_t salt      = 0xAC3E5A17ULL;  // THE ARENA'S SALT LIVES IN THE FLOOR.
                                       // A lineage that picks its own canary picks an easy one.
};

// The margin bands the licence is kept in. |direction|: 0 = thin, 1 = mid,
// 2 = wide. It lives beside the writ because BOTH arms must be banded by the
// same rule: the first run banded only the machine's decisions, so every human
// outcome fell into band 0 and the wager was graded against a constant.
enum { NBAND = 3 };
inline int band_of(float direction, const Writ& wr) {
  const float a = std::fabs(direction);
  if (a < wr.thin_margin) return 0;
  if (a < wr.thin_margin * 2.2f) return 1;
  return 2;
}

struct Firm {
  std::vector<Seat> seat;
  int n_ic = 0, n_lead = 0, n_mgr = 0, n_dir = 0, n_vp = 0, n_c = 0;
  int span = 7;
  float alpha_true = 0.f;       // planted: the true support coefficient
  float F_true = 0.f;           // planted: the fixed floor a within-firm fit cannot see
  Writ writ;

  int size() const { return (int)seat.size(); }
  double payroll() const { double s = 0; for (const Seat& x : seat) s += x.salary; return s; }
  int count_fn(int f) const { int k = 0; for (const Seat& x : seat) if (x.fn == f) ++k; return k; }
  int count_kind(int k) const { int n = 0; for (const Seat& x : seat) if (x.kind == k) ++n; return n; }
};

// ----------------------------------------------------------------------------
// Build ACME: a 500-seat knowledge-work headquarters.
//
// The shape is derived from the span, not typed in. E-seats are allotted to the
// six wires in proportion to arrival rate × handling cost, and then the support
// pyramid is grown on top of the resulting headcount — which is the whole point:
// the α-functions are sized from N, so when N falls they fall with it, whether
// or not anyone automates what they do.
// ----------------------------------------------------------------------------
inline Firm build_acme(int target_n = 500, int span = 7, uint64_t seed = 20260913) {
  Firm f; f.span = span;
  int NC; const ClassSpec* S = schema(NC);

  // --- how much E-work exists, per wire, in units of IC-days per day
  double wire_load[W_N] = {0};
  for (int c = 0; c < NC; ++c) {
    // a class's daily human cost is arrivals × (fetch cost + decide cost)
    const double fetch = 0.12 * S[c].n_systems;          // hours per instance, per system opened
    const double decide = 0.35 * S[c].decide_frac + 0.05;
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
  f.alpha_true = (float)alpha_sum; f.F_true = (float)F_floor;

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
// THE ONE DYNAMICS SOURCE
//
// Everything that decides how good a decision was goes through here. Note what
// it does NOT take: it does not take "is this a human or a machine". A decider
// is a competence and a completeness, and that is all the physics knows.
// ----------------------------------------------------------------------------
struct Decider {
  float competence;     // skill at this class, [0,1]
  float completeness;   // fraction of the class's determinants actually in hand
  float fatigue;        // [0,1]; humans only, but the physics does not care why
};

// P(good | class, decider). Completeness is the load-bearing term by design:
// at completeness 1.0 even a mediocre decider is close to ceiling, and at
// completeness 0.4 a brilliant one is not. That is the whole thesis, expressed
// as a curve rather than an assertion, and it is what the sim lets you falsify.
inline float p_good(const ClassSpec& s, const Decider& d) {
  const float ceiling = 1.0f - s.base_rate;
  // the judgement term only bites in proportion to how much of the class is judgement
  const float judged  = 0.35f + 0.65f * d.competence;
  const float ctx     = std::pow(std::max(0.02f, d.completeness), 0.70f + 1.30f * s.decide_frac);
  const float tired   = 1.0f - 0.35f * d.fatigue;
  return std::min(0.995f, std::max(0.01f, ceiling * ctx * (1.0f - s.decide_frac + s.decide_frac * judged) * tired));
}

enum OutcomeKind : uint8_t { OK_NONE = 0, OK_GOOD, OK_LATE, OK_BAD, OK_N };
inline const char* outcome_name(int o) { static const char* n[] = {"none","good","late","bad"}; return n[o & 3]; }

// The world's verdict. Exogenous: nothing inside the firm writes this, and the
// draw is keyed on the obligation id alone, so the same obligation decided the
// same way by either arm gets the same luck. That is what makes the paired
// comparison in §THE TWIN a clean counterfactual instead of two noisy samples.
inline int realize_outcome(const ClassSpec& s, const Decider& d, bool on_time,
                           uint64_t seed, uint64_t oid) {
  const float pg = p_good(s, d);
  const float u  = u01(seed, 0x0BADCAFEULL, oid);
  if (!on_time) return (u < 0.72f) ? OK_LATE : OK_BAD;
  if (u < pg) return OK_GOOD;
  // the remainder splits between merely late and actually wrong, and the split
  // is class-specific: a mispriced quote is bad, a slow chaser is late
  const float split = 0.35f + 0.45f * s.decide_frac;
  return (u01(seed, 0x0BADD00DULL, oid) < split) ? OK_BAD : OK_LATE;
}

// The writ's cost of a landed outcome. One cost source; both arms are scored
// with it and nothing else.
inline double outcome_cost(const Writ& w, const ClassSpec& s, int outcome) {
  switch (outcome) {
    case OK_BAD:  return w.w_bad  * s.value;
    case OK_LATE: return w.w_late * s.value;
    default:      return 0.0;
  }
}

} // namespace acme
