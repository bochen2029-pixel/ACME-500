// ============================================================================
//  acme/solver.h — the field, the gate, the hand
//
//  THE FIELD is every open commitment in ACME, relaxed jointly, once per period.
//  Rows are obligations. Columns are (seat × slot) plus two STOCK columns:
//
//      UNPLACED        nobody has capacity — this is backlog, and backlog is a
//                      STOCK, not a flow. Mass here is the densify signal.
//      UNADJUDICATED   the humans who must review have run out of hours. Mass
//                      here is the ceiling on the whole enterprise, and it is
//                      the quantity kappa is watching.
//
//  Both stock columns carry FINITE capacity and take the same dual update as
//  any other column. This is the T17 fix carried over from ledger_lattice.cu:
//  with logcap = +inf the stock price is always exactly zero — a theorem, not a
//  bug, and the reason an 88%-loaded seat once priced at 0.000.
//
//  THE PLAN IS NEVER MATERIALISED. An N×M transport is stored as u[N] + v[M]
//  and a row is recomputed when needed. O(N+M) resident against O(N·M). The
//  second copy is not optimised, it is deleted.
//
//  WHAT THE SOLVER LEARNS, AND WHAT IT IS NEVER TOLD.
//  It learns, from arrived outcomes only, a per-(seat,class) competence estimate
//  and a per-class bias. It is never shown Firm::skill[]. Recovering the planted
//  experts from outcomes is oracle O-EXPERT, and it is the first thing the field
//  can do that no human in this organisation can: the information exists, and
//  the org chart has no channel that carries it.
//
//  THE GATE IS THE ONLY AUTHOR OF ACTIONS. It contains nothing learned. It reads
//  the proposal, the licence row, the reversibility class, the writ's
//  prohibitions and the budget, and emits exactly one verdict with a reason
//  drawn from a closed vocabulary. There is no "allow": the gate can only ever
//  narrow what the licence already permits. And there is no silent path — a hold
//  is a verdict and it is a row.
//
//  THREE NUMBERS, NOT ENTROPY. The prior design read the entropy of the
//  distribution as its margin. Entropy is unsigned, so it can say how unsure the
//  field is and never which way it leans, and it conflates a genuinely variable
//  case with a case unlike anything seen. Replaced by:
//      DIRECTION   signed log-odds between act-now and do-not-act mass
//      SHARPNESS   excess over this class's own typical spread
//      NOVELTY     a conformal score against a held-out calibration sample,
//                  computed WITHOUT asking the model how confident it feels
//  A licence is a statement about a population; it does not transfer to a case
//  outside it, however confident anything is.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "ledger.h"   // the hand mutates the LEDGER; solver.h never sees the plant (O17)

namespace acme {

// ----------------------------------------------------------------------------
// §1 · THE COST MODEL — the one dynamics source of the machine arm
// ----------------------------------------------------------------------------
// Features of placing obligation i with decider d at lateness L. Deliberately
// small and inspectable: this is the "explicit, inspectable probe" the driving
// model attaches to a shared trunk, not the trunk itself.
enum { FT_BIAS = 0, FT_COMPLETE, FT_SKILL, FT_LATE, FT_LOAD, FT_VALUE, FT_HOPS, FT_DFRAC, FT_N };

struct Field {
  int NC = 0, NS = 0;
  // learned parameters
  std::vector<float> w;            // [NC * FT_N]   per-class linear head
  std::vector<float> skill_hat;    // [NS * NC]     recovered competence
  std::vector<float> skill_n;      // [NS * NC]     evidence count behind it
  std::vector<float> cls_mu, cls_sd;   // per-class margin calibration (for SHARPNESS)
  std::vector<float> calib;        // conformal calibration scores, per class, ring buffer
  std::vector<int>   calib_n;
  static constexpr int CALIB = 64;

  // Adam state
  std::vector<float> m_w, v_w; int t_adam = 0;

  void init(int nc, int ns, uint64_t seed) {
    NC = nc; NS = ns;
    w.assign((size_t)nc * FT_N, 0.f);
    for (int c = 0; c < nc; ++c) {
      w[(size_t)c * FT_N + FT_BIAS]     =  0.10f;
      w[(size_t)c * FT_N + FT_COMPLETE] =  1.20f;   // a sane prior: context helps
      w[(size_t)c * FT_N + FT_SKILL]    =  0.70f;
      w[(size_t)c * FT_N + FT_LATE]     = -0.90f;
      for (int k = 0; k < FT_N; ++k) w[(size_t)c * FT_N + k] += 0.05f * unrm(seed, 5100 + c, k);
    }
    skill_hat.assign((size_t)ns * nc, 0.5f);
    skill_n.assign((size_t)ns * nc, 0.f);
    cls_mu.assign(nc, 0.f); cls_sd.assign(nc, 1.f);
    calib.assign((size_t)nc * CALIB, 0.f); calib_n.assign(nc, 0);
    init_mach(nc);
    m_w.assign(w.size(), 0.f); v_w.assign(w.size(), 0.f); t_adam = 0;
  }

  static void feats(const ClassSpec& sp, float completeness, float skill, float late,
                    float load, float hops, float out[FT_N]) {
    out[FT_BIAS]     = 1.f;
    out[FT_COMPLETE] = completeness - 0.6f;
    out[FT_SKILL]    = skill - 0.5f;
    out[FT_LATE]     = late;
    out[FT_LOAD]     = load;
    out[FT_VALUE]    = std::log10(std::max(1.f, sp.value)) - 4.f;
    out[FT_HOPS]     = hops * 0.25f;
    out[FT_DFRAC]    = sp.decide_frac - 0.2f;
  }
  // logit of P(good). One place. Training, decoding and forecasting all call it.
  float logit(int c, const float f[FT_N]) const {
    const float* p = &w[(size_t)c * FT_N]; float z = 0.f;
    for (int k = 0; k < FT_N; ++k) z += p[k] * f[k];
    return z;
  }
  static float sigm(float z) { return 1.f / (1.f + std::exp(-z)); }

  // one SGD/Adam step on binary cross-entropy against an ARRIVED outcome.
  // Nothing else may train this: a forecast the model made about itself is not
  // a training signal, and a model trained on its own preferred futures becomes
  // confidently and increasingly wrong.
  void learn(int c, const float f[FT_N], int good, float lr, float ipw = 1.f) {
    const float z = logit(c, f), p = sigm(z), g = (p - (float)good) * ipw;
    ++t_adam;
    const float b1 = 0.9f, b2 = 0.999f, eps = 1e-8f;
    const float c1 = 1.f - std::pow(b1, (float)t_adam), c2 = 1.f - std::pow(b2, (float)t_adam);
    for (int k = 0; k < FT_N; ++k) {
      const size_t i = (size_t)c * FT_N + k; const float gk = g * f[k];
      m_w[i] = b1 * m_w[i] + (1 - b1) * gk;
      v_w[i] = b2 * v_w[i] + (1 - b2) * gk * gk;
      w[i] -= lr * (m_w[i] / c1) / (std::sqrt(v_w[i] / c2) + eps);
    }
  }
  // THE RESIDENT'S OWN READING SKILL, per class, learned from arrivals. It is
  // not given: a fresh field reads the record clumsily and gets better as the
  // world grades it. This is the only sense in which the machine "improves",
  // and it is bounded above by what is instrumented, never by model quality.
  std::vector<float> mach_skill, mach_n;
  void init_mach(int nc) { mach_skill.assign(nc, 0.55f); mach_n.assign(nc, 0.f); }
  void observe_mach(int c, int good) {
    if (c < 0 || c >= (int)mach_skill.size()) return;
    const float k = 1.f / (mach_n[c] + 8.f);
    mach_skill[c] += k * ((float)good - mach_skill[c]);
    mach_n[c] += 1.f;
  }
  float mach_skill_of(int c) const {
    if (c < 0 || c >= (int)mach_skill.size()) return 0.6f;
    return std::min(0.97f, std::max(0.40f, mach_skill[c]));
  }

  // COMPETENCE IS A RESIDUAL, not a hit rate. A seat's raw good-rate is
  // dominated by how complete its context happened to be, which is a property
  // of its calendar and not of the person. So the update is against what the
  // field already PREDICTED from completeness and lateness: what is left over
  // is the seat. Without this the estimator recovers the org's workload
  // distribution and calls it talent.
  void observe_skill(int seat, int c, int good, float predicted) {
    if (seat < 0 || seat >= NS || c < 0 || c >= NC) return;
    const size_t i = (size_t)seat * NC + c;
    const float k = 1.f / (skill_n[i] + 6.f);
    skill_hat[i] += k * (((float)good - predicted) - (skill_hat[i] - 0.5f));
    skill_n[i] += 1.f;
  }
  float skill_of(int seat, int c) const {
    if (seat < 0 || seat >= NS) return 0.5f;
    return std::min(0.98f, std::max(0.02f, 0.5f + 1.8f * (skill_hat[(size_t)seat * NC + c] - 0.5f)));
  }
  // NOVELTY, conformal: where does this case's feature norm fall in the
  // calibration sample? Computed without consulting the model's confidence,
  // because a model is most confidently wrong exactly where the input is strange.
  void calib_push(int c, float score) {
    const int n = calib_n[c] % CALIB; calib[(size_t)c * CALIB + n] = score; ++calib_n[c];
  }
  float novelty(int c, float score) const {
    const int n = std::min(calib_n[c], CALIB);
    if (n < 8) return 0.5f;
    int above = 0; for (int i = 0; i < n; ++i) if (calib[(size_t)c * CALIB + i] >= score) ++above;
    return 1.f - (float)above / (float)n;                 // 1.0 = more extreme than anything seen
  }
};

// ----------------------------------------------------------------------------
// §2 · THE TRANSPORT — Sinkhorn over the lattice, duals only, stocks finite
// ----------------------------------------------------------------------------
enum { STOCK_UNPLACED = 0, STOCK_UNADJ = 1, N_STOCK = 2 };

struct Transport {
  int N = 0, M = 0, NSEAT = 0;
  std::vector<float> u, v;         // duals: THE PRICES. The plan is never built.
  std::vector<float> cap;          // [M]
  std::vector<float> supply;       // [N]
  std::vector<float> colsum;
  float T = 0.35f;

  void size_to(int n, int nseat, int nstock_cap_unplaced, int nstock_cap_unadj) {
    N = n; NSEAT = nseat; M = nseat + N_STOCK;
    u.assign(N, 0.f); v.assign(M, 0.f); colsum.assign(M, 0.f); cap.assign(M, 1.f); supply.assign(N, 1.f);
    cap[NSEAT + STOCK_UNPLACED] = (float)nstock_cap_unplaced;
    cap[NSEAT + STOCK_UNADJ]    = (float)nstock_cap_unadj;
  }
  // cost of row i in column j. The ONE place a placement is priced.
  // Passed in as a callback so the solver and the report cannot disagree.
  // Log-domain Sinkhorn. K_ij = exp((-C_ij + u_i + v_j)/T). Rows are equalities
  // (each commitment ships itself somewhere); columns are INEQUALITIES (a seat
  // takes at most its capacity), so a column dual moves only when it binds.
  template <class CostF>
  void run(CostF&& cost, int iters) {
    for (int it = 0; it < iters; ++it) {
      for (int i = 0; i < N; ++i) {
        float mx = -1e30f;
        for (int j = 0; j < M; ++j) { const float c = cost(i, j); if (c > 1e29f) continue;
                                      const float z = (-c + v[j]) / T; if (z > mx) mx = z; }
        if (mx < -1e29f) { u[i] = 0.f; continue; }
        float s = 0.f;
        for (int j = 0; j < M; ++j) { const float c = cost(i, j); if (c > 1e29f) continue;
                                      s += std::exp((-c + v[j]) / T - mx); }
        const float lse = mx + std::log(std::max(s, 1e-30f));
        u[i] = T * (std::log(std::max(supply[i], 1e-9f)) - lse);
      }
      for (int j = 0; j < M; ++j) {
        float mx = -1e30f;
        for (int i = 0; i < N; ++i) { const float c = cost(i, j); if (c > 1e29f) continue;
                                      const float z = (-c + u[i]) / T; if (z > mx) mx = z; }
        if (mx < -1e29f) { colsum[j] = 0.f; continue; }
        float s = 0.f;
        for (int i = 0; i < N; ++i) { const float c = cost(i, j); if (c > 1e29f) continue;
                                      s += std::exp((-c + u[i]) / T - mx); }
        const float log_mass = mx + std::log(std::max(s, 1e-30f)) + v[j] / T;
        colsum[j] = std::exp(std::min(30.f, log_mass));
        const float over = log_mass - std::log(std::max(cap[j], 1e-9f));
        if (over > 0.f) v[j] -= T * over;      // THE PRICE MOVES ONLY WHEN IT BINDS
      }
    }
  }

  // The support spectrum: exp(H) of a row's mass. Low means the placement is
  // forced and the class is automation-ready; high means the field is genuinely
  // unsure where this belongs, and that is the frontier bill.
  template <class CostF>
  float support(CostF&& cost, int i) const {
    float mx = -1e30f;
    for (int j = 0; j < M; ++j) { const float c = cost(i, j); if (c > 1e29f) continue;
                                  const float z = (-c + v[j]) / T; if (z > mx) mx = z; }
    if (mx < -1e29f) return 1.f;
    float s = 0.f;
    for (int j = 0; j < M; ++j) { const float c = cost(i, j); if (c > 1e29f) continue;
                                  s += std::exp((-c + v[j]) / T - mx); }
    float H = 0.f;
    for (int j = 0; j < M; ++j) { const float c = cost(i, j); if (c > 1e29f) continue;
      const float p = std::exp((-c + v[j]) / T - mx) / s; if (p > 1e-9f) H -= p * std::log(p); }
    return std::exp(H);
  }
  float price(int j) const { return -v[j]; }
};

// ----------------------------------------------------------------------------
// §3 · THE GATE — deterministic, nothing learned, the only author of actions
// ----------------------------------------------------------------------------
enum Verdict : uint8_t {
  V_HOLD = 0,        // recorded. The majority of decisions in any period.
  V_ACT,             // the resident acts, unattended
  V_DRAFT,           // the resident prepares; a person presses the key
  V_FRONTIER,        // thin margin: rent a bigger mind, then re-gate
  V_WARRANT,         // a signature the law reserves, or nothing legal is left
  V_N
};
enum Reason : uint8_t {
  RS_OK = 0, RS_UNLICENSED, RS_THIN, RS_NOVEL, RS_IRREVERSIBLE, RS_LAW,
  RS_NO_BUDGET, RS_BLOCKED, RS_AUDIT, RS_CANARY, RS_UNSURE, RS_N
};
inline const char* verdict_name(int v) { static const char* n[] = {"HOLD","ACT","DRAFT","FRONTIER","WARRANT"}; return n[v % V_N]; }
inline const char* reason_name(int r);
// THE ALPHABET PIN. Train == serve at the tokenizer: the synthetic world and a
// real lane emit rows under one pin, the HEADER row carries it, a fold refuses
// a tape whose pin differs. It covers everything a row can say: the record
// version, every record type by name, every verb, every refusal reason, every
// act kind, every provenance, the band count, the outcome kinds, and the
// authored class table (schema_hash). Adding a reason or a row type changes it,
// which is the widening the pin exists to make loud.
inline uint32_t alphabet_hash() {
  Blake2b b;
  const uint32_t ver = REC_VER; b.update(&ver, 4);
  const uint32_t nt = R_N;  b.update(&nt, 4);
  for (int t = 0; t < R_N; ++t)  { const char* s = rec_type_name(t); b.update(s, strlen(s)); }
  const uint32_t nv = V_N;  b.update(&nv, 4);
  for (int v = 0; v < V_N; ++v)  { const char* s = verdict_name(v);  b.update(s, strlen(s)); }
  const uint32_t nr = RS_N; b.update(&nr, 4);
  for (int r = 0; r < RS_N; ++r) { const char* s = reason_name(r);   b.update(s, strlen(s)); }
  const uint32_t na = ACT_N; b.update(&na, 4);
  for (int k = 0; k < ACT_N; ++k) { const char* s = act_name(k);     b.update(s, strlen(s)); }
  const uint32_t nb = NBAND, no = OK_N, np = 4; b.update(&nb, 4); b.update(&no, 4); b.update(&np, 4);
  const uint32_t sh = schema_hash(); b.update(&sh, 4);
  uint8_t h[32]; b.final(h);
  return (uint32_t)h[0] | ((uint32_t)h[1] << 8) | ((uint32_t)h[2] << 16) | ((uint32_t)h[3] << 24);
}
inline const char* reason_name(int r) {
  static const char* n[] = {"ok","unlicensed","thin-margin","novel-case","irreversible","law","no-adjudication-budget","blocked-by-dep","audit-sample","canary","unsure-placement"};
  return n[r % RS_N];
}

struct GateIn {
  int   cls;
  int   rung;            // the licence rung of THIS class-band, not of the system
  int   band;
  float direction;       // signed
  float sharpness;       // excess spread over the class's own baseline
  float novelty;         // conformal, [0,1]
  bool  reversible;
  bool  warrant_reserved;
  bool  blocked;
  bool  in_canary;       // drawn by keyed hash the resident cannot predict
  bool  in_audit;        // the sampled review fraction that never reaches zero
  float budget_left;     // adjudication minutes remaining today
};
struct GateOut { uint8_t verdict; uint8_t reason; };

// THE ORDER OF REFUSAL IS PUBLISHED AND FIXED. It is part of the design, not an
// implementation detail, and the fact that budget is checked LAST is the safety
// property: running out of supervision produces a hold, never an act.
inline GateOut gate(const GateIn& g, const Writ& wr) {
  if (g.blocked)                              return { V_HOLD,     RS_BLOCKED };
  if (g.warrant_reserved)                     return { V_WARRANT,  RS_IRREVERSIBLE };
  if (g.novelty > 0.97f)                      return { V_WARRANT,  RS_NOVEL };  // outside the population the licence was earned on
  if (g.rung <= 0)                            return { V_DRAFT,    RS_UNLICENSED };
  if (g.in_audit)                             return { V_DRAFT,    RS_AUDIT };
  if (std::fabs(g.direction) < wr.thin_margin) {
    if (g.rung >= 2)                          return { V_FRONTIER, RS_THIN };
    return { V_DRAFT, RS_THIN };
  }
  // A wide support spectrum means the field cannot say where this belongs; that
  // is uncertainty about PLACEMENT, not a case outside the licensed population,
  // and the first run printed both under one word.
  if (g.sharpness > 2.5f)                     return { V_FRONTIER, RS_UNSURE };
  if (!g.reversible && g.rung < 4)            return { V_WARRANT,  RS_IRREVERSIBLE };
  if (g.rung == 1 && !g.in_canary)            return { V_DRAFT,    RS_UNLICENSED };
  if (g.budget_left <= 0.f && g.rung < 3)     return { V_HOLD,     RS_NO_BUDGET };
  return { V_ACT, g.in_canary ? RS_CANARY : RS_OK };
}

// ----------------------------------------------------------------------------
// §4 · THE HAND — the single writer. Every effect carries its inverse.
// ----------------------------------------------------------------------------
struct Effect {
  uint32_t oid; uint32_t day; int cls; int decision;
  uint8_t prev_state; int prev_seat; uint32_t prev_decided;
  uint8_t reversible;
};

struct Integrator {
  std::vector<Effect> ledger;
  uint64_t n_undone = 0;

  void commit(Ledger& L, Tape& tape, uint32_t idx, int decision, uint32_t day, float margin, int band, int via = 1) {
    Obligation& o = L.ob[idx];
    Effect e{}; e.oid = o.id; e.day = day; e.cls = o.cls; e.decision = decision;
    e.prev_state = o.state; e.prev_seat = o.seat; e.prev_decided = o.day_decided;
    e.reversible = cls_spec(o.cls).reversible;
    // THE INVERSE IS RECORDED BEFORE THE EFFECT LEAVES. v2: the row carries the
    // via, so a fold knows whether this was the wager or a draft a person keyed.
    tape.put(R_EFFECT, day, o.id, o.cls, -1, ARM_MACHINE, decision, (int)e.prev_state, margin,
             cls_spec(o.cls).value, band, e.reversible ? RF_REVERSIBLE : 0, via, PROV_M);
    ledger.push_back(e);
    o.decision = decision; o.state = OB_DECIDED; o.day_decided = day; o.by_machine = 1; o.band = (uint8_t)band; o.via = (uint8_t)via;
    o.margin = margin;
  }
  // An effect is reversible only until the world reacts to it, and the world's
  // reaction is the outcome being graded. So the window closes at settlement,
  // and a reversal after that is reclassified irreversible the same day.
  bool unwind(Ledger& L, Tape& tape, size_t k, uint32_t day) {
    if (k >= ledger.size()) return false;
    const Effect& e = ledger[k];
    if (!e.reversible) return false;
    for (Obligation& o : L.ob) {
      if (o.id != e.oid) continue;
      if (o.state == OB_SETTLED) return false;            // the world already reacted
      o.state = e.prev_state; o.seat = e.prev_seat; o.day_decided = e.prev_decided; o.by_machine = 0; o.via = 0;
      // the row carries everything the inverse restores: prev_state (b), prev_seat (margin), prev_decided (value)
      tape.put(R_UNDO, day, o.id, o.cls, -1, ARM_MACHINE, e.decision, (int)e.prev_state,
               (float)e.prev_seat, (float)e.prev_decided, 0, 0, 0, PROV_M);
      ++n_undone; return true;
    }
    return false;
  }
};

} // namespace acme
