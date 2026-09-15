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
  float canary_cap   = 0.35f;   // the most of a class the wager may take in a term
  int   read_budget  = 50000;   // C1: Judge::read calls per period, a dial [BUDGET]: one resident
                                // card at ~0.6 reads a second over a day-long period. Behind the port a
                                // read is a forward pass; the field spends them ripest-first on cells
                                // with no valid proposal, then the floor sample; a cell it never
                                // reached holds as UNREAD. The run prints how often it bound. At
                                // 4,000 it binds on the warm backlog for 22 periods and costs the
                                // twin seven points of good rate (the C1 receipt); --budget sweeps it.
  // C1: the arena's salt is NOT here. It lives in the governor (governor.h), which
  // draws the strata and writes them as rows before the machine's period; the
  // kernel reads the rows and never the salt. v1 kept it in the writ the
  // resident held, which is the lineage picking its own arena.
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
  Writ writ;

  int size() const { return (int)seat.size(); }
  double payroll() const { double s = 0; for (const Seat& x : seat) s += x.salary; return s; }
  int count_fn(int f) const { int k = 0; for (const Seat& x : seat) if (x.fn == f) ++k; return k; }
  int count_kind(int k) const { int n = 0; for (const Seat& x : seat) if (x.kind == k) ++n; return n; }
};

// ----------------------------------------------------------------------------
// C1: build_acme, the generator of the 500-seat headquarters, lives in world.h.
// It sizes the wires from the planted round-trip costs (how many systems a
// class opens, how much of it is judgement), which the kernel must not be able
// to name. What stays here is the Firm as data: seats, the writ, the bands and
// the price of an outcome. The support coefficient and the fixed floor the
// generator plants are returned beside the firm, never on it.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// WHERE THE PHYSICS LIVES (and what used to be here)
//
// Until 2026-09-14 this section carried `Decider`, `p_good` and
// `realize_outcome`: a stochastic quality curve described as "the one dynamics
// source both arms call". No code called it. The 09-13 refactor that gave a
// decision real content replaced it and left it standing, and the documents
// kept describing the curve. It is deleted (finding F15, the REV 2 spec §5
// S10b) rather than resurrected, because the physics the outcomes actually
// come from is better and is in world.h:
//
//   full_signal / truth_decision  the correct decision is the sign of the full
//                                 weighted sum over the class's determinants;
//   observe                       a decider sums only the determinants it
//                                 gathered, plus noise scaled by the class's
//                                 judgement intensity and its own competence,
//                                 and BOTH arms read through this one function
//                                 with different masks;
//   world_settle                  the exogenous grader: right = the decision
//                                 equals the truth, on time = decided by the
//                                 due day. It does not know who decided.
//
// Completeness is still the load-bearing variable, through WHICH determinants
// get summed, not through a curve. What stays here is the outcome vocabulary
// and the writ's price of an outcome.
// ----------------------------------------------------------------------------
enum OutcomeKind : uint8_t { OK_NONE = 0, OK_GOOD, OK_LATE, OK_BAD, OK_N };

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
