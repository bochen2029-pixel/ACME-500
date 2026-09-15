// ============================================================================
//  acme/checkpoint.h — THE CHECKPOINT: the run's state at a period boundary,
//  written beside the durable tape so a restore continues the run bit for bit
//                                                                          [D1]
//
//  A checkpoint is the ledger (cells, index, open set, memo, strata, meter),
//  the incumbent's meter, the seats' live state, the writ, the resident (field,
//  hand, stats, supervision, memo hashes, the compiled table), the governor
//  (salt, ladder, rates), the judges' learned competence, the replay's counts,
//  and the tape's cursor and chain head at that row. It is written as
//  ckpt/NNNNNN.state and then ckpt/NNNNNN.meta, the meta LAST by atomic
//  rename: a checkpoint without its meta was in flight when the process died
//  and is ignored by a restore.
//
//  THE CURSOR IS THE COMMITTED PREFIX. A restore truncates the tape to the
//  newest valid checkpoint's cursor, drops the rows of the period that was in
//  flight (in a real lane they are re-ingested from the source's own cursor;
//  in the sim the plant regenerates them), puts a NOTE row on the tape saying
//  what was dropped, and re-runs from there. O18 requires the continued run to
//  equal an unkilled run row for row, NOTE rows aside.
//
//  This file is wired by main and includes the governor and the plant's judge;
//  the kernel never includes it.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"
#include "ledger.h"
#include "license.h"
#include "machine.h"
#include "governor.h"
#include "world.h"
#include "tapefile.h"
#include "gate.h"
#include <cstdio>
#include <ctime>
#include <string>

namespace acme {

// ---- a byte writer and reader over a FILE, for POD and POD vectors -------------
struct Ser {
  FILE* f; bool ok = true; Blake2b h;
  explicit Ser(FILE* file) : f(file) {}
  void w(const void* p, size_t n) { if (std::fwrite(p, 1, n, f) != n) ok = false; h.update(p, n); }
  template <class T> void pod(const T& x) { w(&x, sizeof x); }
  template <class T> void vec(const std::vector<T>& v) { const uint64_t n = v.size(); pod(n); if (n) w(v.data(), n * sizeof(T)); }
};
struct Des {
  FILE* f; bool ok = true;
  explicit Des(FILE* file) : f(file) {}
  void r(void* p, size_t n) { if (std::fread(p, 1, n, f) != n) ok = false; }
  template <class T> void pod(T& x) { r(&x, sizeof x); }
  template <class T> void vec(std::vector<T>& v) { uint64_t n = 0; pod(n); if (!ok || n > (1ull << 31)) { ok = false; return; } v.resize((size_t)n); if (n) r(v.data(), n * sizeof(T)); }
};

// ---- per type: write and read mirror each other line for line ----------------
inline void ser(Ser& s, const Ledger& L) {
  s.pod(L.NC); s.vec(L.ob); s.vec(L.idx_of_oid); s.vec(L.open_idx); s.pod(L.next_id); s.pod(L.day);
  s.pod(L.mode); s.pod(L.sw); s.pod(L.schema); s.pod(L.ver); s.vec(L.by_class); s.pod(L.total);
  s.vec(L.last_prop); s.vec(L.stratum_kind); s.vec(L.stratum_day); s.vec(L.stratum_rate); s.vec(L.stratum_audit);
  s.vec(L.outstanding); s.pod(L.outstanding_total);
}
inline void des(Des& d, Ledger& L) {
  d.pod(L.NC); d.vec(L.ob); d.vec(L.idx_of_oid); d.vec(L.open_idx); d.pod(L.next_id); d.pod(L.day);
  d.pod(L.mode); d.pod(L.sw); d.pod(L.schema); d.pod(L.ver); d.vec(L.by_class); d.pod(L.total);
  d.vec(L.last_prop); d.vec(L.stratum_kind); d.vec(L.stratum_day); d.vec(L.stratum_rate); d.vec(L.stratum_audit);
  d.vec(L.outstanding); d.pod(L.outstanding_total);
}
inline void ser(Ser& s, const HumanStats& h) {
  s.pod(h.time); s.vec(h.by_class); s.pod(h.completeness); s.pod(h.hops); s.pod(h.cycle_days);
  s.pod(h.n_decided); s.pod(h.n_escalated); s.pod(h.n_held); s.pod(h.n_meetings); s.pod(h.n_breached);
  s.pod(h.attention_wasted); s.pod(h.attention_spent); s.vec(h.decided_by_class);
}
inline void des(Des& d, HumanStats& h) {
  d.pod(h.time); d.vec(h.by_class); d.pod(h.completeness); d.pod(h.hops); d.pod(h.cycle_days);
  d.pod(h.n_decided); d.pod(h.n_escalated); d.pod(h.n_held); d.pod(h.n_meetings); d.pod(h.n_breached);
  d.pod(h.attention_wasted); d.pod(h.attention_spent); d.vec(h.decided_by_class);
}
// the seats' live state; the cone itself is regenerated from the args
inline void ser(Ser& s, const Firm& f) {
  s.pod(f.writ); const uint64_t n = f.seat.size(); s.pod(n);
  for (const Seat& x : f.seat) { s.pod(x.attn_left); s.pod(x.fatigue); s.pod(x.meetings_today); }
}
inline void des(Des& d, Firm& f) {
  d.pod(f.writ); uint64_t n = 0; d.pod(n); if (n != f.seat.size()) { d.ok = false; return; }
  for (Seat& x : f.seat) { d.pod(x.attn_left); d.pod(x.fatigue); d.pod(x.meetings_today); }
}
inline void ser(Ser& s, const Field& F) {
  s.pod(F.NC); s.pod(F.NS); s.vec(F.w); s.vec(F.skill_hat); s.vec(F.skill_n); s.vec(F.cls_mu); s.vec(F.cls_sd);
  s.vec(F.calib); s.vec(F.calib_n); s.vec(F.m_w); s.vec(F.v_w); s.pod(F.t_adam);
}
inline void des(Des& d, Field& F) {
  d.pod(F.NC); d.pod(F.NS); d.vec(F.w); d.vec(F.skill_hat); d.vec(F.skill_n); d.vec(F.cls_mu); d.vec(F.cls_sd);
  d.vec(F.calib); d.vec(F.calib_n); d.vec(F.m_w); d.vec(F.v_w); d.pod(F.t_adam);
}
inline void ser(Ser& s, const Compiled& C) {
  s.pod(C.NC); s.vec(C.join_graph); s.vec(C.n_systems); s.vec(C.coverage); s.vec(C.decide_frac); s.vec(C.arrivals_per_day);
  s.vec(C.minutes_per_decision); s.vec(C.template_hash); s.vec(C.invariants); s.vec(C.invariants_proposed); s.vec(C.class_geom);
  s.pod(C.classes_seen); s.pod(C.trunk_geometry);
}
inline void des(Des& d, Compiled& C) {
  d.pod(C.NC); d.vec(C.join_graph); d.vec(C.n_systems); d.vec(C.coverage); d.vec(C.decide_frac); d.vec(C.arrivals_per_day);
  d.vec(C.minutes_per_decision); d.vec(C.template_hash); d.vec(C.invariants); d.vec(C.invariants_proposed); d.vec(C.class_geom);
  d.pod(C.classes_seen); d.pod(C.trunk_geometry);
}
inline void ser(Ser& s, const MachineStats& m) {
  s.pod(m.acted); s.pod(m.drafted); s.pod(m.frontier); s.pod(m.warrant); s.pod(m.held); s.pod(m.undone); s.pod(m.canary); s.pod(m.audit);
  s.pod(m.completeness); s.pod(m.margin_abs); s.pod(m.sup_created_min); s.pod(m.sup_removed_min); s.pod(m.frontier_calls);
  s.vec(m.acted_by_class); s.vec(m.reason_count); s.pod(m.reads); s.pod(m.reads_fresh); s.pod(m.reads_floor); s.pod(m.memo_hits);
  s.pod(m.unread); s.pod(m.budget_bound); s.pod(m.forgone_dual); s.pod(m.total_dual); s.pod(m.periods);
}
inline void des(Des& d, MachineStats& m) {
  d.pod(m.acted); d.pod(m.drafted); d.pod(m.frontier); d.pod(m.warrant); d.pod(m.held); d.pod(m.undone); d.pod(m.canary); d.pod(m.audit);
  d.pod(m.completeness); d.pod(m.margin_abs); d.pod(m.sup_created_min); d.pod(m.sup_removed_min); d.pod(m.frontier_calls);
  d.vec(m.acted_by_class); d.vec(m.reason_count); d.pod(m.reads); d.pod(m.reads_fresh); d.pod(m.reads_floor); d.pod(m.memo_hits);
  d.pod(m.unread); d.pod(m.budget_bound); d.pod(m.forgone_dual); d.pod(m.total_dual); d.pod(m.periods);
}
inline void ser(Ser& s, const Resident& r) {
  ser(s, r.fd); ser(s, r.C); s.vec(r.hand.ledger); s.pod(r.hand.n_undone); ser(s, r.st);
  s.pod(r.sup.NC); s.vec(r.sup.cre); s.vec(r.sup.rem); s.pod(r.wr); s.pod(r.adjudication_budget_min);
  s.pod(r.NC); s.pod(r.NS); s.pod(r.lr); s.vec(r.memo_hash); s.vec(r.memo_base); s.vec(r.epoch_seen);
}
inline void des(Des& d, Resident& r) {
  des(d, r.fd); des(d, r.C); d.vec(r.hand.ledger); d.pod(r.hand.n_undone); des(d, r.st);
  d.pod(r.sup.NC); d.vec(r.sup.cre); d.vec(r.sup.rem); d.pod(r.wr); d.pod(r.adjudication_budget_min);
  d.pod(r.NC); d.pod(r.NS); d.pod(r.lr); d.vec(r.memo_hash); d.vec(r.memo_base); d.vec(r.epoch_seen);
}
inline void ser(Ser& s, const Ladder& L) {
  s.pod(L.NC); s.vec(L.lic); s.pod(L.alpha_promote); s.pod(L.alpha_demote); s.pod(L.term_days);
  s.pod(L.schema_pin); s.pod(L.judge_hash); s.vec(L.template_hash);
}
inline void des(Des& d, Ladder& L) {
  d.pod(L.NC); d.vec(L.lic); d.pod(L.alpha_promote); d.pod(L.alpha_demote); d.pod(L.term_days);
  d.pod(L.schema_pin); d.pod(L.judge_hash); d.vec(L.template_hash);
}
inline void ser(Ser& s, const Governor& g) { s.pod(g.salt); ser(s, g.lad); s.pod(g.wr); s.pod(g.NC); s.vec(g.per_term); s.pod(g.lie_band_dependent); }
inline void des(Des& d, Governor& g)       { d.pod(g.salt); des(d, g.lad); d.pod(g.wr); d.pod(g.NC); d.vec(g.per_term); d.pod(g.lie_band_dependent); }
inline void ser(Ser& s, const PlantJudge& j) { s.pod(j.judge_seed); s.pod(j.id_hash); s.pod(j.learns); s.vec(j.comp); s.vec(j.comp_n); }
inline void des(Des& d, PlantJudge& j)       { d.pod(j.judge_seed); d.pod(j.id_hash); d.pod(j.learns); d.vec(j.comp); d.vec(j.comp_n); }
inline void ser(Ser& s, const ReplayOut& R) {
  s.vec(R.n); s.vec(R.agree); s.vec(R.agree_good); s.vec(R.agree_bad); s.vec(R.disagree); s.vec(R.disagree_firm_good); s.vec(R.disagree_firm_bad);
  s.vec(R.agree_wrong); s.vec(R.conf_n); s.vec(R.conf_agree); s.vec(R.firm_one); s.vec(R.mach_one); s.vec(R.coverage); s.vec(R.band_mass); s.pod(R.invariant_violations);
}
inline void des(Des& d, ReplayOut& R) {
  d.vec(R.n); d.vec(R.agree); d.vec(R.agree_good); d.vec(R.agree_bad); d.vec(R.disagree); d.vec(R.disagree_firm_good); d.vec(R.disagree_firm_bad);
  d.vec(R.agree_wrong); d.vec(R.conf_n); d.vec(R.conf_agree); d.vec(R.firm_one); d.vec(R.mach_one); d.vec(R.coverage); d.vec(R.band_mass); d.pod(R.invariant_violations);
}

// ---- the checkpoint files -----------------------------------------------------
struct CkptHeader {                       // the first bytes of NNNNNN.state
  char     magic[8];                      // "ACMECKPT"
  uint32_t ver;                           // 1
  uint32_t pin;                           // alphabet_hash()
  uint64_t cursor;                        // rows on the tape this state is valid after
  uint32_t day;                           // the last period run
  uint32_t reserved;
  uint8_t  head[32];                      // the chain head at the cursor
};
struct CkptInfo { uint64_t cursor = 0; uint32_t day = 0; uint8_t head[32] = {0}; std::string state_path, meta_path; bool valid = false; };

inline std::string ckpt_base(const std::string& dir, uint32_t day) { char b[32]; snprintf(b, sizeof b, "/ckpt/%06u", day); return dir + b; }

// Writes NNNNNN.state, then NNNNNN.meta by atomic rename, carrying the cursor,
// the day, the chain head at the cursor and the BLAKE2b of the state file.
template <class WriteBody>
inline bool ckpt_write(const std::string& dir, uint32_t day, uint64_t cursor, const uint8_t head[32], WriteBody&& body) {
  make_dir(dir + "/ckpt");
  const std::string base = ckpt_base(dir, day);
  FILE* f = std::fopen((base + ".state").c_str(), "wb"); if (!f) return false;
  Ser s(f);
  CkptHeader h{}; memcpy(h.magic, "ACMECKPT", 8); h.ver = 1; h.pin = alphabet_hash(); h.cursor = cursor; h.day = day; memcpy(h.head, head, 32);
  s.pod(h);
  body(s);
  std::fflush(f); std::fclose(f);
  if (!s.ok) return false;
  uint8_t dg[32]; s.h.final(dg);
  FILE* m = std::fopen((base + ".meta.tmp").c_str(), "wb"); if (!m) return false;
  std::fprintf(m, "{\"day\":%u,\"cursor\":%llu,\"chain_head\":\"%s\",\"state_blake2b\":\"%s\",\"pin\":%u}\n",
               day, (unsigned long long)cursor, Blake2b::hex(head, 32).c_str(), Blake2b::hex(dg, 32).c_str(), alphabet_hash());
  std::fclose(m);
  return atomic_rename(base + ".meta.tmp", base + ".meta");
}

// The newest checkpoint whose meta exists, whose state digests to what the
// meta says, and whose cursor is not beyond the tape. Scans days downward from
// `max_day`. A state without a meta is skipped: it was in flight.
template <class ReadBody>
inline CkptInfo ckpt_find_and_read(const std::string& dir, uint32_t max_day, size_t tape_rows, ReadBody&& body) {
  CkptInfo info;
  for (int day = (int)max_day; day >= 0; --day) {
    const std::string base = ckpt_base(dir, (uint32_t)day);
    uint64_t sz = 0;
    if (!file_size_of(base + ".meta", sz)) continue;
    FILE* m = std::fopen((base + ".meta").c_str(), "rb"); if (!m) continue;
    char buf[512] = {0}; const size_t got = std::fread(buf, 1, sizeof buf - 1, m); std::fclose(m); (void)got;
    unsigned long long cursor = 0; unsigned mday = 0; char head_hex[80] = {0}, dg_hex[80] = {0};
    if (std::sscanf(buf, "{\"day\":%u,\"cursor\":%llu,\"chain_head\":\"%64[0-9a-f]\",\"state_blake2b\":\"%64[0-9a-f]\"", &mday, &cursor, head_hex, dg_hex) != 4) continue;
    if (cursor > tape_rows) continue;
    // digest the state file and compare
    FILE* f = std::fopen((base + ".state").c_str(), "rb"); if (!f) continue;
    { Blake2b hb; uint8_t chunk[65536]; size_t n; while ((n = std::fread(chunk, 1, sizeof chunk, f)) > 0) hb.update(chunk, n);
      uint8_t dg[32]; hb.final(dg); if (Blake2b::hex(dg, 32) != dg_hex) { std::fclose(f); continue; } }
    std::rewind(f);
    Des d(f); CkptHeader h{}; d.pod(h);
    if (!d.ok || memcmp(h.magic, "ACMECKPT", 8) != 0 || h.cursor != cursor || h.day != mday) { std::fclose(f); continue; }
    body(d);
    std::fclose(f);
    if (!d.ok) continue;
    info.cursor = cursor; info.day = mday; memcpy(info.head, h.head, 32);
    info.state_path = base + ".state"; info.meta_path = base + ".meta"; info.valid = true;
    return info;
  }
  return info;
}

// The tape's sidecar: pins, hashes, rows, head, cursor. Rewritten by rename.
inline bool tape_meta_write(const std::string& dir, const char* mode, uint32_t judge_hash, uint32_t frontier_hash,
                            size_t rows, const uint8_t head[32], uint64_t cursor, int n, int span, uint64_t seed, float demand, int days, int warm,
                            const char* gate_hash = "unknown", const char* sw = "live") {
  FILE* m = std::fopen((dir + "/tape.meta.json.tmp").c_str(), "wb"); if (!m) return false;
  std::fprintf(m, "{\"format\":\"acme-500 durable tape v2\",\"rec_bytes\":%zu,\"rec_ver\":%d,\"seg_rows\":%d,\"alphabet_hash\":%u,\"schema_hash\":%u,"
                  "\"mode\":\"%s\",\"switch\":\"%s\",\"gate_hash\":\"%s\",\"judge_hash\":%u,\"frontier_hash\":%u,\"rows\":%zu,\"chain_head\":\"%s\",\"cursor\":%llu,"
                  "\"args\":{\"n\":%d,\"span\":%d,\"seed\":%llu,\"demand\":%.4g,\"days\":%d,\"warm\":%d}}\n",
               sizeof(Rec), REC_VER, (int)SEG_ROWS, alphabet_hash(), schema_hash(), mode, sw, gate_hash, judge_hash, frontier_hash, rows,
               Blake2b::hex(head, 32).c_str(), (unsigned long long)cursor, n, span, (unsigned long long)seed, demand, days, warm);
  std::fclose(m);
  return atomic_rename(dir + "/tape.meta.json.tmp", dir + "/tape.meta.json");
}

// D2: THE PILL. The world port writes the heartbeat at the close of every
// period, by rename; tools/pill.py reads it. The machine cannot write it.
inline bool pill_write(const std::string& path, uint32_t day, size_t rows, const uint8_t head[32], int sw, uint32_t judge_hash, double wall) {
  FILE* m = std::fopen((path + ".tmp").c_str(), "wb"); if (!m) return false;
  std::fprintf(m, "{\"day\":%u,\"rows\":%zu,\"chain_head\":\"%s\",\"switch\":\"%s\",\"judge\":%u,\"wall\":%.3f}\n",
               day, rows, Blake2b::hex(head, 32).c_str(), switch_name(sw), judge_hash, wall);
  std::fclose(m);
  return atomic_rename(path + ".tmp", path);
}
// D2: THE SWITCH FILE. Read by the port, never by the machine; a change is a
// NOTE row the ledger folds. Returns -1 if the file is absent or unreadable.
inline int switch_file_read(const std::string& path) {
  FILE* f = std::fopen(path.c_str(), "rb"); if (!f) return -1;
  char buf[32] = {0}; const size_t n = std::fread(buf, 1, sizeof buf - 1, f); std::fclose(f);
  size_t k = 0; while (k < n && (buf[k] == ' ' || buf[k] == '\n' || buf[k] == '\r' || buf[k] == '\t')) ++k;
  size_t e = k; while (e < n && buf[e] > ' ') ++e; buf[e] = 0;
  return switch_parse(buf + k);
}

} // namespace acme
