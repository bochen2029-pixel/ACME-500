// ============================================================================
//  acme/gate.h — THE GATE: deterministic, nothing learned, the only author of
//  actions; and the alphabet pin every row is written under              [D2]
//
//  Its own header, on purpose: the gate is a hashed library. tools/gate_hash.py
//  computes the BLAKE2b-256 of this file's text and pins it in gate_hash.inc;
//  the binary prints that pin in the automate's header and in tape.meta.json,
//  and the tool oracle O30a refuses a build whose gate.h no longer matches its
//  pin. What the gate establishes is that the specified checks passed on the
//  inputs it was given — never that the inputs were true or the checks
//  sufficient; O30 re-derives every EFFECT on the tape through this function
//  from rows alone.
//
//  It includes core.h and firm.h only: the writ's constants and the bands.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"

namespace acme {

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
  RS_NO_BUDGET, RS_BLOCKED, RS_AUDIT, RS_CANARY, RS_UNSURE,
  RS_UNREAD,         // C1: the read budget never reached this cell and it carries no proposal: a hold
  RS_SWITCH_OFF,     // D2: the switch is off; the machine decides nothing and says so
  RS_EXPOSURE,       // E0: the outstanding exposure would exceed the writ's cap; an act waits for a verdict to land
  RS_RETAINED,       // E3: the governor drew this cell into the retained stratum: a person decides it, at every rung (F14, O20)
  RS_N
};
// THE SWITCH. Read from a file the machine never writes (INTELLECT) or from the
// command line (the sim), carried on the HEADER row and on NOTE rows when it
// changes, folded into the ledger, and read by the gate as an input like any
// other. off: every cell holds with reason switch-off. shadow: the gate
// verdicts as at live, the hand writes the EFFECT row with RF_SHADOW and
// mutates nothing, the incumbent proceeds. live: the hand mutates. stop: the
// period returns before it reads.
enum Switch : int { SW_OFF = 0, SW_SHADOW = 1, SW_LIVE = 2, SW_STOP = 3 };
inline const char* switch_name(int sw) { static const char* n[] = {"off","shadow","live","stop"}; return (sw >= 0 && sw < 4) ? n[sw] : "?"; }
inline int switch_parse(const char* s) {
  if (!s) return SW_LIVE;
  if (!strcmp(s, "off")) return SW_OFF;
  if (!strcmp(s, "shadow")) return SW_SHADOW;
  if (!strcmp(s, "live")) return SW_LIVE;
  if (!strcmp(s, "stop")) return SW_STOP;
  return -1;
}
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
  static const char* n[] = {"ok","unlicensed","thin-margin","novel-case","irreversible","law","no-adjudication-budget","blocked-by-dep","audit-sample","canary","unsure-placement","unread","switch-off","exposure","retained"};
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
  bool  to_incumbent;    // E3: the retained stratum: a person decides this cell, at every rung; the machine holds it
  float budget_left;     // adjudication minutes remaining today
  int   sw;              // D2: the switch, folded from the tape; off holds everything
  float value;           // E0: the class value at stake, what an unattended act adds to the outstanding exposure
  float exposure_left;   // E0: the writ's cap less the outstanding exposure; 1e30 when uncapped
};
struct GateOut { uint8_t verdict; uint8_t reason; };

// THE ORDER OF REFUSAL IS PUBLISHED AND FIXED. It is part of the design, not an
// implementation detail, and the fact that budget is checked LAST is the safety
// property: running out of supervision produces a hold, never an act.
inline GateOut gate(const GateIn& g, const Writ& wr) {
  if (g.sw == SW_OFF)                         return { V_HOLD,     RS_SWITCH_OFF };   // D2: off is a hold, and the reason says so
  if (g.blocked)                              return { V_HOLD,     RS_BLOCKED };
  if (g.to_incumbent)                         return { V_HOLD,     RS_RETAINED };     // E3: the control arm, at every rung: a person decides it
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
  if (g.exposure_left < g.value)              return { V_HOLD,     RS_EXPOSURE };   // E0: risk in flight is capped; an act waits, never a person's decision
  return { V_ACT, g.in_canary ? RS_CANARY : RS_OK };
}


// D2: THE GATE'S INPUTS TRAVEL ON THE EFFECT ROW, so that every effect can be
// re-derived from rows alone (O30). The proposal carries the direction and the
// band, the STRATUM row the strata, the LICENSE rows the rung, the schema the
// class facts; what no row carried was the field's sharpness and the
// conformal novelty, so the EFFECT row's b packs them beside the previous
// state: prev_state in the low byte, novelty in 12 bits, sharpness in 12 bits.
inline int pack_gate_inputs(int prev_state, float novelty, float sharpness) {
  const int nv = (int)std::lround(std::min(1.f, std::max(0.f, novelty)) * 4095.f);
  const int sh = (int)std::lround(std::min(15.99f, std::max(0.f, sharpness)) * 255.f);   // 12 bits: 0..4095 over 0..16.06
  return (prev_state & 0xFF) | (nv << 8) | (sh << 20);
}
inline int   packed_prev_state(int b) { return b & 0xFF; }
inline float packed_novelty(int b)    { return (float)((b >> 8) & 0xFFF) / 4095.f; }
inline float packed_sharpness(int b)  { return (float)((b >> 20) & 0xFFF) / 255.f; }

} // namespace acme
