// ============================================================================
//  acme/dump.h — --dump DIR: the observer's contract, written by the binary   [D1]
//
//  Taken over from the graphics lane's dump patch (ACME500-OBSERVER-REV2/
//  tools/dump_patch/dump.h, 2026-09-14) so their patch retires and the page
//  that consumes these files proves the format. OBSERVATION ONLY: this header
//  reads the run's objects and writes the files the observer folds; it draws no
//  random number, writes no row, moves no ledger cell, and the run's stdout is
//  byte-identical with or without it. Main-side: it reads the plant.
//
//   tape.bin         the records, verbatim: n × 40 bytes, little-endian x86 layout
//   tape.meta.json   n, the chain head, the pins, the args, the histogram, the readings
//   firm.json        the schema and the seats (the Schema half; no planted parameter; C1: no n_systems)
//   phases.json      phase boundaries derived from rows, computed here so the page's
//                    own derivation has something to be checked against
//   snapshots.jsonl  the ledger's own state at the close of every day (the binary's
//                    fold, so the page's fold at day N can be tested for identity)
#pragma once
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/world.h"
#include "acme/ledger.h"
#include "acme/machine.h"
#include "acme/report.h"
#include "acme/license.h"   // C1: the ladder lives here, keyed to the judge
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

namespace acme {

inline const char* g_dump_dir = nullptr;
#ifndef ACME_COMMIT
#define ACME_COMMIT "unknown"   // -DACME_COMMIT=\"<hash>\": the commit the dump is of
#endif
inline std::vector<std::string>& dump_snaps() { static std::vector<std::string> v; return v; }

inline const char* dump_kind(int k) { static const char* n[] = { "IC", "lead", "manager", "director", "VP", "C" }; return (k >= 0 && k < SK_N) ? n[k] : "?"; }
inline const char* dump_fn(int k)   { static const char* n[] = { "E", "alpha", "warrant" }; return (k >= 0 && k < FN_N) ? n[k] : "?"; }

// One line per day, at the close of the day, from the LEDGER (not from the tape):
// what the page's fold must reproduce. The open set is summarised by its size,
// its sum and its xor so the check is exact without listing thousands of oids.
inline void dump_snapshot(const Tape& tape, const Ledger& L, const Firm& f, uint32_t day,
                          const Ladder* lad, const MachineStats* st) {
  if (!g_dump_dir) return;
  const ArmResult ar = score_arm(L, f);
  uint32_t osum = 0, oxor = 0;
  for (const Obligation& o : L.ob) if (o.state != OB_SETTLED) { osum += o.id; oxor ^= o.id; }
  std::string rungs; int licensed = 0, unlic = 0; long n_mach = 0, n_asst = 0;
  if (lad) for (int c = 0; c < lad->NC; ++c) for (int b = 0; b < NBAND; ++b) {
    const Lic& x = lad->at(c, b);
    rungs += (char)('0' + (x.rung < 0 ? 0 : (x.rung > 9 ? 9 : x.rung)));
    if (x.rung > 0) ++licensed;
    if (x.unlicensable) ++unlic;
    n_mach += x.n_machine; n_asst += x.n_assisted;
  }
  char buf[1024];
  snprintf(buf, sizeof buf,
    "{\"day\":%u,\"rows\":%zu,\"open\":%ld,\"open_sum\":%u,\"open_xor\":%u,"
    "\"settled\":%ld,\"good\":%ld,\"late\":%ld,\"bad\":%ld,\"breached\":%ld,"
    "\"licensed_bands\":%d,\"unlicensable\":%d,\"n_machine\":%ld,\"n_assisted\":%ld,"
    "\"acted\":%llu,\"drafted\":%llu,\"frontier\":%llu,\"warrant\":%llu,\"held\":%llu,"
    "\"canary\":%llu,\"audit\":%llu,\"undone\":%llu,\"sup_created\":%.1f,\"sup_removed\":%.1f,"
    "\"reads\":%llu,\"memo_hits\":%llu,\"unread\":%llu,\"rungs\":\"%s\"}",
    day, tape.size(), ar.open_n, osum, oxor, ar.settled, ar.good, ar.late, ar.bad, ar.breached,
    licensed, unlic, n_mach, n_asst,
    (unsigned long long)(st ? st->acted : 0), (unsigned long long)(st ? st->drafted : 0),
    (unsigned long long)(st ? st->frontier : 0), (unsigned long long)(st ? st->warrant : 0),
    (unsigned long long)(st ? st->held : 0), (unsigned long long)(st ? st->canary : 0),
    (unsigned long long)(st ? st->audit : 0), (unsigned long long)(st ? st->undone : 0),
    st ? st->sup_created_min : 0.0, st ? st->sup_removed_min : 0.0,
    (unsigned long long)(st ? st->reads : 0), (unsigned long long)(st ? st->memo_hits : 0), (unsigned long long)(st ? st->unread : 0), rungs.c_str());
  dump_snaps().push_back(buf);
}

inline void dump_firm(FILE* o, const Firm& f, int NC, uint64_t seed, float demand) {
  std::fprintf(o, "{\"seed\":%llu,\"span\":%d,\"n\":%d,\"demand\":%.4g,\"payroll\":%.0f,",
               (unsigned long long)seed, f.span, f.size(), demand, f.payroll());
  std::fprintf(o, "\"counts\":{\"IC\":%d,\"lead\":%d,\"manager\":%d,\"director\":%d,\"VP\":%d,\"C\":%d,\"E\":%d,\"alpha\":%d,\"warrant\":%d},",
               f.n_ic, f.n_lead, f.n_mgr, f.n_dir, f.n_vp, f.n_c, f.count_fn(FN_E), f.count_fn(FN_ALPHA), f.count_fn(FN_WARRANT));
  std::fprintf(o, "\"wires\":[");
  for (int w = 0; w < W_N; ++w) std::fprintf(o, "%s{\"id\":%d,\"name\":\"%s\"}", w ? "," : "", w, wire_name(w));
  std::fprintf(o, "],\"classes\":[");
  for (int c = 0; c < NC; ++c) {
    const ClassSpec& s = cls_spec(c);
    /* C1: the Schema half only — n_systems is measured by the kernel now (see meta.compiled), counterparty is authored */
    std::fprintf(o, "%s{\"id\":%d,\"name\":\"%s\",\"wire\":%d,\"verdict_latency\":%d,\"base_rate\":%.4g,"
                    "\"arrival_per_day\":%.4g,\"value\":%.0f,\"reversible\":%d,\"warrant\":%d,\"counterparty\":%d,\"n0\":%d}",
                 c ? "," : "", c, s.name, (int)s.wire, (int)s.verdict_latency, s.base_rate,
                 s.arrival_per_day, s.value, (int)s.reversible, (int)s.warrant, (int)s.counterparty, n0_for(c));
  }
  std::fprintf(o, "],\"seats\":[");
  for (int i = 0; i < f.size(); ++i) {
    const Seat& s = f.seat[i];
    std::fprintf(o, "%s{\"id\":%d,\"kind\":\"%s\",\"fn\":\"%s\",\"wire\":%d,\"boss\":%d,\"depth\":%d,\"salary\":%.0f,\"attention\":%.0f,\"spec\":[",
                 i ? "," : "", s.id, dump_kind(s.kind), dump_fn(s.fn), (int)s.wire, s.boss, s.depth, s.salary, s.attention);
    bool first = true;
    for (int c = 0; c < NC && c < 32; ++c) if (s.spec & (1u << c)) { std::fprintf(o, "%s%d", first ? "" : ",", c); first = false; }
    std::fprintf(o, "]}");
  }
  const Writ& w = f.writ;
  std::fprintf(o, "],\"writ\":{\"w_bad\":%.4g,\"w_late\":%.4g,\"w_salary\":%.4g,\"w_unplaced\":%.4g,\"w_frontier\":%.4g,"
                  "\"kappa_max\":%.4g,\"thin_margin\":%.4g,\"eps_floor\":%.4g,\"canary_delta\":%.4g,\"audit_min\":%d,\"read_budget\":%d}}\n",
               w.w_bad, w.w_late, w.w_salary, w.w_unplaced, w.w_frontier, w.kappa_max, w.thin_margin, w.eps_floor, w.canary_delta, w.audit_min, w.read_budget);
}

// Phase boundaries, derived from rows where a row exists and named as a close
// where the binary only computes (the cascade is arithmetic at the end; no row).
inline void dump_phases(FILE* o, const Tape& tape, int days, int warm) {
  long first_license = -1, first_climb = -1, first_canary = -1, first_stratum = -1, first_collapse = -1;
  tape.fold([&](const Rec& r) {
    if (r.type == R_LICENSE && first_license < 0) first_license = r.day;
    if (r.type == R_LICENSE && r.a >= 2 && first_climb < 0) first_climb = r.day;
    if (r.type == R_EFFECT && (r.via == 1 || r.via == 3) && first_canary < 0) first_canary = r.day;
    if (r.type == R_STRATUM && r.a == 1 && first_stratum < 0) first_stratum = r.day;
    if (r.type == R_PATCH && r.a == 4 && first_collapse < 0) first_collapse = r.day;
  });
  std::fprintf(o, "[\n {\"day\":0,\"phase\":\"INSTRUMENT\",\"source\":\"start: the boundary log; nobody uses anything\"}");
  if (warm < days) {
    std::fprintf(o, ",\n {\"day\":%d,\"phase\":\"COMPILE\",\"source\":\"close of the warm history: compile_from_tape\"}", warm);
    std::fprintf(o, ",\n {\"day\":%d,\"phase\":\"REPLAY\",\"source\":\"close of the warm history: replay on its own support\"}", warm);
  }
  if (first_license >= 0) std::fprintf(o, ",\n {\"day\":%ld,\"phase\":\"LICENSE\",\"source\":\"row: first LICENSE\"}", first_license);
  if (first_stratum >= 0)      std::fprintf(o, ",\n {\"day\":%ld,\"phase\":\"CANARY\",\"source\":\"row: first STRATUM a=1\"}", first_stratum);
  else if (first_canary >= 0)  std::fprintf(o, ",\n {\"day\":%ld,\"phase\":\"CANARY\",\"source\":\"row: first EFFECT via 1|3 (no STRATUM rows before C1)\"}", first_canary);
  if (first_climb >= 0)   std::fprintf(o, ",\n {\"day\":%ld,\"phase\":\"CLIMB\",\"source\":\"row: first LICENSE a>=2\"}", first_climb);
  if (first_collapse >= 0) std::fprintf(o, ",\n {\"day\":%ld,\"phase\":\"COMPILE_OUT\",\"source\":\"row: first PATCH a=4\"}", first_collapse);
  std::fprintf(o, ",\n {\"day\":%d,\"phase\":\"CASCADE\",\"source\":\"close: N = E/(1-alpha) + F, computed at the end; no row\"}\n]\n", days - 1);
}

inline void dump_write(const char* dir, const Tape& tape, const Ledger& L, const Firm& f, const World& w,
                       const Compiled* C, const Ladder* lad, const MachineStats* st, const Residual* res,
                       int n, int span, uint64_t seed, float demand, int days, int warm) {
  mkdir(dir, 0755);
  std::string base = std::string(dir) + "/";
  // 1. the records, verbatim
  {
    FILE* o = std::fopen((base + "tape.bin").c_str(), "wb");
    if (!o) { std::fprintf(stderr, "dump: cannot open %s\n", (base + "tape.bin").c_str()); return; }
    if (!tape.rec.empty()) std::fwrite(tape.rec.data(), sizeof(Rec), tape.rec.size(), o);
    std::fclose(o);
  }
  // 2. the meta: the pins, the head, the readings
  {
    unsigned long long hist[R_N] = {};
    tape.fold([&](const Rec& r) { if (r.type < R_N) ++hist[r.type]; });
    int hdr_mode = -1, hdr_sw = -1; uint32_t hdr_schema = 0; int hdr_ver = 0;
    if (!tape.rec.empty() && tape.rec[0].type == R_HEADER) {
      hdr_mode = tape.rec[0].a / 10; hdr_sw = tape.rec[0].a % 10; hdr_schema = (uint32_t)tape.rec[0].b; hdr_ver = (int)tape.rec[0].value;
    }
    const ArmResult ar = score_arm(L, f);
    FILE* o = std::fopen((base + "tape.meta.json").c_str(), "wb");
    std::fprintf(o, "{\"format\":\"acme-500 tape v2\",\"rec_bytes\":%zu,\"rec_ver\":%d,\"endian\":\"little\","
                    "\"chain\":\"blake2b-256(prev_head[32] || rec[40]); head starts as 32 zero bytes\","
                    "\"n_rows\":%zu,\"chain_head\":\"%s\",\"chain_ok\":%s,",
                 sizeof(Rec), REC_VER, tape.size(), tape.head_hex().c_str(), tape.verify() < 0 ? "true" : "false");
    std::fprintf(o, "\"header\":{\"mode\":%d,\"switch\":%d,\"schema_hash\":%u,\"ver\":%d},\"alphabet_hash\":%u,\"schema_hash\":%u,",
                 hdr_mode, hdr_sw, hdr_schema, hdr_ver, alphabet_hash(), schema_hash());
    std::fprintf(o, "\"args\":{\"n\":%d,\"span\":%d,\"seed\":%llu,\"demand\":%.4g,\"days\":%d,\"warm\":%d,\"NC\":%d},",
                 n, span, (unsigned long long)seed, demand, days, warm, w.NC);
    std::fprintf(o, "\"commit\":\"%s\",", ACME_COMMIT);
    std::fprintf(o, "\"histogram\":{");
    bool first = true;
    for (int t = 0; t < R_N; ++t) if (hist[t]) { std::fprintf(o, "%s\"%s\":%llu", first ? "" : ",", rec_type_name(t), hist[t]); first = false; }
    std::fprintf(o, "},\"readings\":{\"settled\":%ld,\"good\":%ld,\"late\":%ld,\"bad\":%ld,\"breached\":%ld,\"open\":%ld,"
                    "\"writ_cost\":%.0f,\"backlog_cost\":%.0f,\"mean_cycle\":%.3f",
                 ar.settled, ar.good, ar.late, ar.bad, ar.breached, ar.open_n, ar.writ_cost, ar.backlog_cost, ar.mean_cycle());
    if (st) std::fprintf(o, ",\"acted\":%llu,\"drafted\":%llu,\"frontier\":%llu,\"warrant\":%llu,\"held\":%llu,\"canary\":%llu,\"audit\":%llu,\"undone\":%llu,"
                            "\"kappa\":%.4f,\"sup_created_min\":%.0f,\"sup_removed_min\":%.0f",
                         (unsigned long long)st->acted, (unsigned long long)st->drafted, (unsigned long long)st->frontier,
                         (unsigned long long)st->warrant, (unsigned long long)st->held, (unsigned long long)st->canary,
                         (unsigned long long)st->audit, (unsigned long long)st->undone, st->kappa(), st->sup_created_min, st->sup_removed_min);
    if (st) std::fprintf(o, ",\"reads\":%llu,\"reads_fresh\":%llu,\"reads_floor\":%llu,\"memo_hits\":%llu,\"unread\":%llu,\"budget_bound\":%d,\"forgone_dual\":%.1f,\"frontier_calls\":%.0f",
                         (unsigned long long)st->reads, (unsigned long long)st->reads_fresh, (unsigned long long)st->reads_floor,
                         (unsigned long long)st->memo_hits, (unsigned long long)st->unread, st->budget_bound, st->forgone_dual, st->frontier_calls);
    if (lad) {
      int licensed = 0, unlic = 0; long n_mach = 0, n_asst = 0, need = 0;
      for (int c = 0; c < lad->NC; ++c) for (int b = 0; b < NBAND; ++b) {
        const Lic& x = lad->at(c, b);
        if (x.rung > 0) ++licensed;
        if (x.unlicensable) ++unlic;
        n_mach += x.n_machine; n_asst += x.n_assisted; need += x.n0;
      }
      std::fprintf(o, ",\"licensed_bands\":%d,\"unlicensable_bands\":%d,\"n_machine\":%ld,\"n_assisted\":%ld,\"n0_total\":%ld", licensed, unlic, n_mach, n_asst, need);
      std::fprintf(o, ",\"ladder_key\":{\"schema_pin\":%u,\"judge_hash\":%u}", lad->schema_pin, lad->judge_hash);
    }
    if (res) {
      const double T = res->total > 0 ? res->total : 1e-9;
      std::fprintf(o, ",\"residual\":{\"licensed\":%.4f,\"warrant\":%.4f,\"counterparty\":%.4f,\"frame\":%.4f,\"frame_wait\":%.4f,\"frame_cov\":%.4f,\"thin_tape\":%.4f}",
                   res->licensed / T, res->warrant / T, res->counterparty / T, res->frame / T, res->frame_wait / T, res->frame_cov / T, res->thin_tape / T);
    }
    std::fprintf(o, "}");
    if (lad) {
      std::fprintf(o, ",\"ladder\":[");
      bool f1 = true;
      for (int c = 0; c < lad->NC; ++c) for (int b = 0; b < NBAND; ++b) {
        const Lic& x = lad->at(c, b);
        std::fprintf(o, "%s{\"cls\":%d,\"band\":%d,\"rung\":%d,\"expiry_day\":%d,\"logE\":%.4f,\"n_machine\":%ld,\"good_machine\":%ld,"
                        "\"n_assisted\":%ld,\"good_assisted\":%ld,\"n_incumbent\":%ld,\"good_incumbent\":%ld,\"n0\":%d,\"history\":%s,\"unlicensable\":%s,\"kappa\":%.4f}",
                     f1 ? "" : ",", c, b, x.rung, x.expiry_day, x.logE, x.n_machine, x.good_machine, x.n_assisted, x.good_assisted,
                     x.n_incumbent, x.good_incumbent, x.n0, x.history_licensed ? "true" : "false", x.unlicensable ? "true" : "false", x.kappa());
        f1 = false;
      }
      std::fprintf(o, "]");
    }
    if (C) {
      std::fprintf(o, ",\"compiled\":[");
      for (int c = 0; c < C->NC; ++c)
        std::fprintf(o, "%s{\"cls\":%d,\"n_systems\":%d,\"join_graph\":%u,\"coverage\":%.4f,\"decide_frac\":%.4f,"
                        "\"arrivals_per_day\":%.4f,\"minutes_per_decision\":%.2f,\"template_hash\":%u}",
                     c ? "," : "", c, C->n_systems[c], C->join_graph[c], C->coverage[c], C->decide_frac[c],
                     c < (int)C->arrivals_per_day.size() ? C->arrivals_per_day[c] : 0.f,
                     c < (int)C->minutes_per_decision.size() ? C->minutes_per_decision[c] : 0.f,
                     c < (int)C->template_hash.size() ? C->template_hash[c] : 0u);
      std::fprintf(o, "]");
    }
    std::fprintf(o, "}\n");
    std::fclose(o);
  }
  // 3. the firm
  { FILE* o = std::fopen((base + "firm.json").c_str(), "wb"); dump_firm(o, f, w.NC, seed, demand); std::fclose(o); }
  // 4. the phases
  { FILE* o = std::fopen((base + "phases.json").c_str(), "wb"); dump_phases(o, tape, days, warm); std::fclose(o); }
  // 5. the snapshots
  {
    FILE* o = std::fopen((base + "snapshots.jsonl").c_str(), "wb");
    for (const std::string& s : dump_snaps()) { std::fputs(s.c_str(), o); std::fputc('\n', o); }
    std::fclose(o);
  }
  // stderr, so the run's stdout stays byte-identical to the unpatched binary's
  std::fprintf(stderr, "dump: %zu rows -> %s (tape.bin %zu bytes, %zu snapshots)\n",
               tape.size(), dir, tape.size() * sizeof(Rec), dump_snaps().size());
}

} // namespace acme
