// ============================================================================
//  acme/core.h — the substrate: determinism, the tape, and the schema
//
//  Three things live here and nothing else:
//
//   1. THE RNG IS STATELESS.  Counter-based. hash(seed, stream, index). No
//      generator object is ever carried, so two runs of the same world under
//      the same seed produce byte-identical tapes no matter what order the
//      firm's parts were visited in. This is the property that makes the
//      paired-arm comparison in §THE TWIN exact rather than approximate.
//
//   2. THE TAPE IS THE ONLY TRUTH.  Append-only, BLAKE2b-chained, verifiable.
//      Every table in ACME is a fold over it. If a fold disagrees with the
//      tape, the tape wins. Holds are rows: a decision NOT to act leaves a
//      record, because an organization fails by omission and an omission that
//      leaves no trace cannot be graded.
//
//   3. THE SCHEMA IS THE TOKENIZER.  The class table and the event alphabet
//      are the only hand-authored objects in this program besides the writ.
//      Everything downstream — the field, the invariants, the context specs,
//      the licence table — is learned or derived. Ten lines per wire is the
//      budget, and §WIRES keeps to it.
// ============================================================================
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

namespace acme {

// ----------------------------------------------------------------------------
// §1 · DETERMINISM
// ----------------------------------------------------------------------------
inline uint64_t mix64(uint64_t z) {
  z += 0x9E3779B97F4A7C15ULL;
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
  return z ^ (z >> 31);
}
inline uint64_t hash3(uint64_t seed, uint64_t stream, uint64_t index) {
  return mix64(seed ^ mix64(stream ^ mix64(index * 0xD1B54A32D192ED03ULL)));
}
// uniform on the OPEN interval (0,1) so logf() is always safe
inline float u01(uint64_t seed, uint64_t stream, uint64_t index) {
  return ((float)(hash3(seed, stream, index) >> 40) + 0.5f) * (1.0f / 16777216.0f);
}
inline float unrm(uint64_t seed, uint64_t stream, uint64_t index) {   // Box–Muller
  const float a = u01(seed, stream, index), b = u01(seed, stream ^ 0x5bd1e995ULL, index);
  return std::sqrt(-2.f * std::log(a)) * std::cos(6.2831853f * b);
}
inline int ubelow(uint64_t seed, uint64_t stream, uint64_t index, int n) {
  return n <= 1 ? 0 : (int)(hash3(seed, stream, index) % (uint64_t)n);
}
inline bool ucoin(uint64_t seed, uint64_t stream, uint64_t index, float p) {
  return u01(seed, stream, index) < p;
}

// Fixed-point accumulation. A float reduction is order-dependent; the firm is
// visited in a different order in each arm, so every accumulator that a number
// is later read off is integer. Bands come from the sum of squares.
struct Acc {
  uint64_t s = 0, s2 = 0; uint32_t n = 0;
  static constexpr double SCALE = 65536.0;
  void add(double v) { const double x = v < 0 ? 0 : v; s += (uint64_t)(x * SCALE + 0.5); s2 += (uint64_t)(x * x * SCALE + 0.5); ++n; }
  double sum()  const { return (double)s / SCALE; }
  double mean() const { return n ? sum() / n : 0.0; }
  double sd()   const { if (n < 2) return 0.0; const double m = mean(), v = (double)s2 / SCALE / n - m * m; return v > 0 ? std::sqrt(v) : 0.0; }
  double sem()  const { return n ? sd() / std::sqrt((double)n) : 0.0; }
  void reset() { s = s2 = 0; n = 0; }
};

// A signed counterpart, for quantities that legitimately go negative (deltas).
struct SAcc {
  int64_t s = 0; uint64_t s2 = 0; uint32_t n = 0;
  static constexpr double SCALE = 65536.0;
  void add(double v) { s += (int64_t)std::llround(v * SCALE); s2 += (uint64_t)(v * v * SCALE + 0.5); ++n; }
  double sum()  const { return (double)s / SCALE; }
  double mean() const { return n ? sum() / n : 0.0; }
  double sd()   const { if (n < 2) return 0.0; const double m = mean(), v = (double)s2 / SCALE / n - m * m; return v > 0 ? std::sqrt(v) : 0.0; }
  double sem()  const { return n ? sd() / std::sqrt((double)n) : 0.0; }
};

// ----------------------------------------------------------------------------
// §2 · BLAKE2b-256 (RFC 7693, unkeyed) — the estate's hash family, inline.
// ----------------------------------------------------------------------------
struct Blake2b {
  static constexpr size_t OUT = 32;
  uint64_t h[8], t[2]; uint8_t buf[128]; size_t buflen = 0;
  static uint64_t rotr(uint64_t x, int n) { return (x >> n) | (x << (64 - n)); }
  Blake2b() { reset(); }
  void reset() {
    static const uint64_t IV[8] = {
      0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
      0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL };
    for (int i = 0; i < 8; ++i) h[i] = IV[i];
    h[0] ^= 0x01010000ULL ^ (uint64_t)OUT;
    t[0] = t[1] = 0; buflen = 0;
  }
  void compress(const uint8_t* blk, bool last) {
    static const uint8_t S[12][16] = {
      {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3},
      {11,8,12,0,5,2,15,13,10,14,3,6,7,1,9,4},{7,9,3,1,13,12,11,14,2,6,5,10,4,0,15,8},
      {9,0,5,7,2,4,10,15,14,1,11,12,6,8,3,13},{2,12,6,10,0,11,8,3,4,13,7,5,15,14,1,9},
      {12,5,1,15,14,13,4,10,0,7,6,3,9,2,8,11},{13,11,7,14,12,1,3,9,5,0,15,4,8,6,2,10},
      {6,15,14,9,11,3,0,8,12,2,13,7,1,4,10,5},{10,2,8,4,7,6,1,5,15,11,9,14,3,12,13,0},
      {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3} };
    static const uint64_t IV[8] = {
      0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
      0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL };
    uint64_t m[16], v[16];
    for (int i = 0; i < 16; ++i) { uint64_t x = 0; for (int b = 7; b >= 0; --b) x = (x << 8) | blk[i * 8 + b]; m[i] = x; }
    for (int i = 0; i < 8; ++i) { v[i] = h[i]; v[i + 8] = IV[i]; }
    v[12] ^= t[0]; v[13] ^= t[1]; if (last) v[14] = ~v[14];
    auto G = [&](int a, int b, int c, int d, uint64_t x, uint64_t y) {
      v[a] = v[a] + v[b] + x; v[d] = rotr(v[d] ^ v[a], 32);
      v[c] = v[c] + v[d];     v[b] = rotr(v[b] ^ v[c], 24);
      v[a] = v[a] + v[b] + y; v[d] = rotr(v[d] ^ v[a], 16);
      v[c] = v[c] + v[d];     v[b] = rotr(v[b] ^ v[c], 63);
    };
    for (int r = 0; r < 12; ++r) {
      const uint8_t* s = S[r];
      G(0,4,8,12,m[s[0]],m[s[1]]);  G(1,5,9,13,m[s[2]],m[s[3]]);
      G(2,6,10,14,m[s[4]],m[s[5]]); G(3,7,11,15,m[s[6]],m[s[7]]);
      G(0,5,10,15,m[s[8]],m[s[9]]); G(1,6,11,12,m[s[10]],m[s[11]]);
      G(2,7,8,13,m[s[12]],m[s[13]]);G(3,4,9,14,m[s[14]],m[s[15]]);
    }
    for (int i = 0; i < 8; ++i) h[i] ^= v[i] ^ v[i + 8];
  }
  void update(const void* data, size_t len) {
    const uint8_t* p = (const uint8_t*)data;
    while (len) {
      if (buflen == 128) { t[0] += 128; if (t[0] < 128) ++t[1]; compress(buf, false); buflen = 0; }
      size_t take = 128 - buflen; if (take > len) take = len;
      memcpy(buf + buflen, p, take); buflen += take; p += take; len -= take;
    }
  }
  void final(uint8_t out[OUT]) {
    t[0] += buflen; if (t[0] < buflen) ++t[1];
    memset(buf + buflen, 0, 128 - buflen);
    compress(buf, true);
    for (int i = 0; i < 4; ++i) for (int b = 0; b < 8; ++b) out[i * 8 + b] = (uint8_t)(h[i] >> (8 * b));
  }
  static void digest(const void* d, size_t n, uint8_t out[OUT]) { Blake2b b; b.update(d, n); b.final(out); }
  static std::string hex(const uint8_t* d, size_t n) {
    static const char* H = "0123456789abcdef"; std::string s; s.reserve(n * 2);
    for (size_t i = 0; i < n; ++i) { s += H[d[i] >> 4]; s += H[d[i] & 15]; }
    return s;
  }
};

// ----------------------------------------------------------------------------
// §3 · THE TAPE
//
// One writer. Append-only. Every record carries the hash of the one before it,
// so a single flipped byte anywhere localises. Records are fixed-width structs
// rather than JSON because the whole point is that a fold over ten million of
// them is a scan, not a parse.
//
// THE RECORD TYPES ARE THE ORGANIZATION'S ENTIRE VOCABULARY. Note what is here
// and what is not: HOLD is a first-class record, so is ESCALATE, so is UNDO.
// There is no record type for a meeting, a status report or a rollup deck,
// because those are the interior and the interior is never instrumented.
// ----------------------------------------------------------------------------
enum RecType : uint16_t {
  R_NONE = 0,
  R_ARRIVE,      // an obligation opened at the boundary. a = day_due, b = the oid it waits on or -1
  R_ASSIGN,      // a seat took it (human or resident). b = hops
  R_HOLD,        // a decision NOT to act, with its margin. The majority of rows. a = reason
  R_DECIDE,      // a terminal decision was made by a person. a = decision, b = hops
  R_EFFECT,      // an effect left through the hand, with its inverse recorded first. a = decision, b = prev_state
  R_UNDO,        // the inverse was applied
  R_ESCALATE,    // handed up. a = the receiving seat (or the current seat when the resident escalates), b = escalations
  R_OUTCOME,     // the world graded it. THE ONLY THING THAT LICENSES ANYTHING. a = kind, b = via, seat = the decider
  R_CONTEXT,     // a fetch: which system (a), minutes (value); flags bit2 = the fetch was LOST to decay
  R_MEETING,     // interior coordination — recorded ONLY so it can be priced and deleted
  R_TICK,        // the clock moved; the world never dilates. a = period, b = rows in the previous period
  R_LICENSE,     // a class-band changed rung. a = rung, b = band
  R_KAPPA,       // supervision created / removed, per class-band, per term
  R_PATCH,       // a structural change with its inverse. margin = old value, value = new value
  R_NOTE,
  // ---- v2 (2026-09-14): the rows the fold was missing ----
  R_HEADER,      // first row of a tape: a = mode*10 + switch, b = schema hash, value = record version
  R_PROPOSAL,    // the judge's proposal on a cell, BEFORE the gate's verdict. a = choice, b = judge hash,
                 // margin = direction, value = completeness_hat. Recorded; feeds nothing.
  R_STRATUM,     // the governor drew this oid into a stratum this period. a = 1 canary / 2 audit / 3 retained
  R_COUNSEL,     // a rented mind's proposal, memoized on (oid, frame hash). a = judge hash, b = frame hash
  R_ACT,         // a human act, priced: a = kind (0 fetch 1 frame 2 decide 3 commit 4 transport 5 rework 6 glue 7 meeting),
                 // value = minutes, margin = completeness (on a decide act)
  R_RECEIPT,     // the executor confirmed (b = 0), refused (1) or failed (2) an effect. Never written by the machine.
  R_CORRECTION,  // a later reply corrected a reading: a = the corrected row's index, b = the reading judge's hash
  R_N
};
inline const char* rec_type_name(int t) {
  static const char* n[] = { "NONE","ARRIVE","ASSIGN","HOLD","DECIDE","EFFECT","UNDO","ESCALATE","OUTCOME","CONTEXT","MEETING",
                             "TICK","LICENSE","KAPPA","PATCH","NOTE","HEADER","PROPOSAL","STRATUM","COUNSEL","ACT","RECEIPT","CORRECTION" };
  return (t >= 0 && t < R_N) ? n[t] : "?";
}
enum ActKind : uint8_t { ACT_FETCH = 0, ACT_FRAME, ACT_DECIDE, ACT_COMMIT, ACT_TRANSPORT, ACT_REWORK, ACT_GLUE, ACT_MEETING, ACT_N };
inline const char* act_name(int k) { static const char* n[] = {"fetch","frame","decide","commit","transport","rework","glue","meeting"}; return (k >= 0 && k < ACT_N) ? n[k] : "?"; }
enum Prov : uint8_t { PROV_D = 0, PROV_H, PROV_M, PROV_R };   // deterministic · human-authored · model-authored · model-reduced
// Who wrote the row. Where a decision exists it is THE DECIDER; elsewhere it is
// the writer: 0 the human arm's code, 1 the resident, 2 the governor / the world
// port, 3 the executor. v1 stamped the RUN on every row (F18); a fold could not
// tell a human decision in the machine world from a machine one.
enum Arm : uint8_t { ARM_HUMAN = 0, ARM_MACHINE = 1, ARM_GOVERNOR = 2, ARM_EXECUTOR = 3 };
enum RecFlags : uint8_t { RF_REVERSIBLE = 1, RF_SHADOW = 2, RF_HISTORY = 4, RF_WARRANT = 8, RF_TORN_OR_SURVEY = 16, RF_LOST = 32 };

#pragma pack(push, 1)
struct Rec {                 // v2 · 40 bytes · a tape is a scan, not a parse
  uint16_t type;
  uint16_t cls;        // decision class
  uint32_t day;        // the TICK index. Never wall clock.
  uint32_t oid;        // obligation id; 0 for rows about no obligation
  int32_t  seat;       // >= 0 a seat · -1 the resident · -2 nobody · -3 the governor / world port · -4 the executor
  int32_t  a;          // type-specific
  int32_t  b;
  float    margin;     // the decision's own confidence, signed. Holds carry it too.
  float    value;      // money / minutes / a type-specific scalar
  uint8_t  arm;        // Arm: the decider where one exists, else the writer
  uint8_t  band;       // margin band, for the licence
  uint8_t  flags;      // RecFlags
  uint8_t  via;        // 0 person · 1 unattended act · 2 draft a person keyed · 3 rented mind acted · 4 signer executed
  uint16_t firm;       // tenant id; 0 in single-tenant (carried; tenancy deferred)
  uint8_t  prov;       // Prov
  uint8_t  ver;        // record version = 2
};
#pragma pack(pop)
static_assert(sizeof(Rec) == 40, "Rec v2 is 40 bytes; a tape is a scan, not a parse");
enum { REC_VER = 2 };

struct Tape {
  std::vector<Rec> rec;
  std::vector<uint8_t> chain;          // 32 bytes per record
  uint8_t head[32] = {0};
  bool chaining = true;                // off for the multiverse's throwaway forks

  void reset() { rec.clear(); chain.clear(); memset(head, 0, 32); last_tick_at = 0; }
  size_t size() const { return rec.size(); }

  void append(const Rec& r) {
    rec.push_back(r);
    if (!chaining) return;
    Blake2b b; b.update(head, 32); b.update(&r, sizeof(Rec)); b.final(head);
    const size_t off = chain.size(); chain.resize(off + 32); memcpy(chain.data() + off, head, 32);
  }
  void put(RecType t, uint32_t day, uint32_t oid, int cls, int seat, int arm,
           int a = 0, int b = 0, float margin = 0.f, float value = 0.f, int band = 0, int flags = 0,
           int via = 0, int prov = PROV_D) {
    Rec r{}; r.type = (uint16_t)t; r.cls = (uint16_t)cls; r.day = day; r.oid = oid; r.seat = seat;
    r.a = a; r.b = b; r.margin = margin; r.value = value; r.arm = (uint8_t)arm; r.band = (uint8_t)band; r.flags = (uint8_t)flags;
    r.via = (uint8_t)via; r.firm = 0; r.prov = (uint8_t)prov; r.ver = REC_VER;
    append(r);
  }
  // The first row of every tape: the mode, the switch and the schema pin. A fold
  // that must refuse a mismatched tape can do so before it opens a sidecar.
  void header(int mode, int sw, uint32_t schema_hash) {
    put(R_HEADER, 0, 0, 0, -3, ARM_GOVERNOR, mode * 10 + sw, (int)schema_hash, 0.f, (float)REC_VER);
  }
  // The clock is a row. b counts the rows of the period that just closed.
  void tick(uint32_t day) {
    const size_t since = rec.size() - last_tick_at;
    put(R_TICK, day, 0, 0, -3, ARM_GOVERNOR, (int)day, (int)since);
    last_tick_at = rec.size();
  }
  size_t last_tick_at = 0;
  // Walk the chain from zero. Returns the index of the first bad record, or -1.
  long verify() const {
    if (!chaining) return -1;
    uint8_t h[32] = {0};
    for (size_t i = 0; i < rec.size(); ++i) {
      Blake2b b; b.update(h, 32); b.update(&rec[i], sizeof(Rec)); b.final(h);
      if (memcmp(h, chain.data() + i * 32, 32) != 0) return (long)i;
    }
    return -1;
  }
  std::string head_hex() const { return Blake2b::hex(head, 32); }
  // A fold is any function of the tape. This is the primitive every meter uses.
  template <class F> void fold(F&& f) const { for (const Rec& r : rec) f(r); }
};

// ----------------------------------------------------------------------------
// §4 · THE SCHEMA — the one hand-written object besides the writ
//
// SIX WIRES, TWENTY-FOUR CLASSES. A wire is a core business process; a class is
// a kind of decision inside it. This is the whole authored surface: the rest of
// the program discovers classes by clustering the tape and must reproduce this
// table to pass oracle O-COMPILE.
//
// Each class carries only what a person could write in one line: which wire,
// how long the world takes to answer, its base rate of a bad outcome, its
// nominal arrival rate and value, and whether it is reversible, reserved to a
// signature, or contested by a counterparty who demands a person.
//
// C1: THE SCHEMA IS AUTHORED AND THE PLANTED IS NOT HERE. How many systems of
// record hold a class's determinants and what fraction of its round trip is
// real judgement are facts of the synthetic world, held by the plant
// (world.h, `planted(c)`) and never by cls_spec(). The compile step must
// recover both from the tape (O6, O13); the kernel's translation unit cannot
// name them (O17).
// ----------------------------------------------------------------------------
enum Wire { W_Q2C = 0, W_CLAIMS, W_PROCURE, W_HR, W_FIN, W_COMPLY, W_N };
inline const char* wire_name(int w) {
  static const char* n[] = { "quote-to-cash", "claims", "procurement", "people", "finance", "compliance" };
  return (w >= 0 && w < W_N) ? n[w] : "?";
}

struct ClassSpec {
  const char* name;
  uint8_t wire;
  uint8_t verdict_latency;// days until the world grades it
  float   base_rate;      // P(bad outcome) under competent handling — the prior for n0
  float   arrival_per_day;// nominal new obligations per day, firm-wide (the kernel measures the actual rate from ARRIVE rows)
  float   value;          // money at stake per instance
  uint8_t reversible;     // 1 = the effect carries a usable inverse
  uint8_t warrant;        // 1 = a signature is legally reserved. Never automatable, at any capability.
  uint8_t counterparty;   // 1 = the other side demands a person. A commercial fact, authored; does not move.
};

inline const ClassSpec* schema(int& n) {
  static const ClassSpec S[] = {
    // ---- quote-to-cash ----
    { "quote.price",           W_Q2C,     21, 0.06f, 34.0f,  18000, 1, 0, 0 },
    { "quote.approve.disc",    W_Q2C,     21, 0.05f, 12.0f,  26000, 1, 0, 0 },
    { "order.credit.check",    W_Q2C,     30, 0.04f, 28.0f,  15000, 1, 0, 0 },
    { "order.fulfil.sched",    W_Q2C,      7, 0.09f, 41.0f,   9000, 1, 0, 0 },
    { "invoice.issue",         W_Q2C,     45, 0.02f, 47.0f,  15000, 1, 0, 0 },
    { "collections.chase",     W_Q2C,     14, 0.11f, 22.0f,  12000, 1, 0, 0 },
    { "dispute.resolve",       W_Q2C,     30, 0.18f,  6.0f,  31000, 1, 0, 0 },
    // ---- claims ----
    { "claim.intake.triage",   W_CLAIMS,   5, 0.07f, 55.0f,   4000, 1, 0, 0 },
    { "claim.coverage.det",    W_CLAIMS,  20, 0.09f, 38.0f,  22000, 1, 0, 0 },
    { "claim.reserve.set",     W_CLAIMS,  60, 0.13f, 31.0f,  40000, 1, 0, 0 },
    { "claim.settle.auth",     W_CLAIMS,  30, 0.08f, 19.0f,  55000, 0, 1, 0 },
    { "claim.fraud.refer",     W_CLAIMS,  90, 0.21f,  4.0f,  70000, 1, 0, 1 },
    // ---- procurement ----
    { "po.match.3way",         W_PROCURE, 10, 0.03f, 63.0f,   8000, 1, 0, 0 },
    { "vendor.onboard",        W_PROCURE, 45, 0.10f,  3.0f,  12000, 1, 0, 0 },
    { "contract.renew",        W_PROCURE, 60, 0.12f,  5.0f,  90000, 0, 1, 0 },
    { "spend.approve",         W_PROCURE, 30, 0.05f, 26.0f,  20000, 1, 0, 0 },
    // ---- people ----
    { "req.approve",           W_HR,      60, 0.14f,  4.0f, 120000, 1, 0, 0 },
    { "candidate.screen",      W_HR,      30, 0.16f, 18.0f,   6000, 1, 0, 0 },
    { "leave.approve",         W_HR,       7, 0.02f, 15.0f,   1500, 1, 0, 0 },
    // ---- finance ----
    { "accrual.post",          W_FIN,     30, 0.04f, 24.0f,  17000, 1, 0, 0 },
    { "recon.break.clear",     W_FIN,     14, 0.08f, 33.0f,   9000, 1, 0, 0 },
    { "forecast.adjust",       W_FIN,     90, 0.22f,  7.0f,  60000, 1, 0, 0 },
    // ---- compliance ----
    { "kyc.review",            W_COMPLY,  30, 0.06f, 21.0f,  11000, 1, 0, 0 },
    { "reg.filing.prepare",    W_COMPLY,  90, 0.05f,  2.0f, 150000, 0, 1, 0 },
  };
  n = (int)(sizeof(S) / sizeof(S[0]));
  return S;
}
inline int n_classes() { int n; schema(n); return n; }
// The schema pin: BLAKE2b over the authored class table, low 32 bits. Train ==
// serve at the alphabet: the synthetic world and a real lane emit rows under the
// same pin, the HEADER row carries it, and a fold refuses a tape that differs.
inline uint32_t schema_hash() {
  int n; const ClassSpec* S = schema(n);
  Blake2b b;
  for (int c = 0; c < n; ++c) {
    b.update(S[c].name, strlen(S[c].name));
    b.update(&S[c].wire, 1); b.update(&S[c].verdict_latency, 1);
    b.update(&S[c].base_rate, 4); b.update(&S[c].arrival_per_day, 4); b.update(&S[c].value, 4);
    b.update(&S[c].reversible, 1); b.update(&S[c].warrant, 1); b.update(&S[c].counterparty, 1);
  }
  uint8_t h[32]; b.final(h);
  return (uint32_t)h[0] | ((uint32_t)h[1] << 8) | ((uint32_t)h[2] << 16) | ((uint32_t)h[3] << 24);
}
inline const ClassSpec& cls_spec(int c) { int n; const ClassSpec* s = schema(n); return s[c < 0 ? 0 : (c < n ? c : n - 1)]; }

// The identifiability floor, per class. n0 is NOT a constant: it is a function
// of the base rate and the margin you need to detect. This is the correction
// the three single-way documents all missed — the highest-stakes classes are the
// SLOWEST to license, because the thing you must detect is rare.
//   n0 ≈ (z_a + z_b)^2 * p(1-p) / delta^2 , one-sided non-inferiority
inline int n0_for(int c, float delta_rel = 0.30f) {
  const ClassSpec& s = cls_spec(c);
  const double p = std::max((double)s.base_rate, 0.005);
  const double delta = std::max(p * delta_rel, 0.005);
  const double z = 1.645 + 0.842;                       // alpha .05 one-sided, power .80
  const double n = z * z * p * (1.0 - p) / (delta * delta);
  return (int)std::ceil(n);
}

} // namespace acme
