// ============================================================================
//  acme/tapefile.h — THE DURABLE TAPE                                      [D1]
//
//  An append-only directory of segments, one million 40-byte records each,
//  written exactly as Rec lies in memory (packed; static_assert 40), so the
//  wire format IS the struct and memcmp stays the theorem. Every segment opens
//  with a 64-byte header carrying the record version, the alphabet pin, its
//  index, the row index of its first record and THE CHAIN HEAD AT ITS START, so
//  a reader recovers the head by re-walking one segment and verifies from
//  genesis on demand.
//
//  A TORN TAIL — a last segment whose length is not header + k x 40 — is the
//  process dying mid-write. It is truncated to the last complete record, and
//  the bytes dropped are reported so the opener can put a NOTE row on the tape
//  saying so. Complete bytes whose chain does not verify are corruption, not a
//  tear, and the open refuses them.
//
//  This file is kernel-side: it reads no clock, knows no plant, and is scanned
//  by O25a. The checkpoint that makes a restore possible is checkpoint.h.
// ============================================================================
#pragma once
#include "core.h"
#include <cstdio>
#include <string>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace acme {

#pragma pack(push, 1)
struct SegHeader {                       // 64 bytes on disk, no implicit padding: the layout is the wire format
  char     magic[8];                     // "ACMETAPE"
  uint16_t rec_ver;                      // REC_VER
  uint16_t hdr_ver;                      // 1
  uint32_t pin;                          // alphabet_hash() at open
  uint32_t seg_index;
  uint32_t pad0;                         // explicit, zero
  uint64_t first_row;                    // row index of this segment's first record
  uint8_t  head_at_start[32];            // the chain head before this segment's first record
};
#pragma pack(pop)
static_assert(sizeof(SegHeader) == 64, "the segment header is 64 bytes");
enum { SEG_ROWS = 1000000 };

inline std::string seg_path(const std::string& dir, uint32_t idx) {
  char b[32]; snprintf(b, sizeof b, "/seg-%06u.tape", idx); return dir + b;
}
inline bool file_size_of(const std::string& p, uint64_t& out) {
  struct stat st; if (stat(p.c_str(), &st) != 0) return false; out = (uint64_t)st.st_size; return true;
}
inline bool truncate_file(const std::string& p, uint64_t len) {
#ifdef _WIN32
  FILE* f = std::fopen(p.c_str(), "r+b"); if (!f) return false;
  const bool ok = _chsize_s(_fileno(f), (long long)len) == 0; std::fclose(f); return ok;
#else
  return truncate(p.c_str(), (off_t)len) == 0;
#endif
}
inline void make_dir(const std::string& p) {
#ifdef _WIN32
  _mkdir(p.c_str());
#else
  mkdir(p.c_str(), 0755);
#endif
}
inline bool atomic_rename(const std::string& from, const std::string& to) {
#ifdef _WIN32
  std::remove(to.c_str());
#endif
  return std::rename(from.c_str(), to.c_str()) == 0;
}

// What an open found: the rows it could read, the head after them, and what
// it had to drop to get a whole tape.
struct Recovered {
  size_t   rows = 0;
  uint8_t  head[32] = {0};
  uint64_t torn_bytes = 0;               // a partial record at the end of the last segment
  bool     corrupt = false;              // a complete record whose chain did not verify
  size_t   corrupt_row = 0;
  uint32_t pin = 0;
};

// The sink: appends every chained record to the current segment, opening a new
// one every SEG_ROWS with the head at that point in its header.
struct TapeFile : TapeSink {
  std::string dir;
  FILE*    cur = nullptr;
  uint32_t cur_seg = 0;
  size_t   rows_in_cur = 0;
  size_t   rows = 0;
  uint32_t pin = 0;

  ~TapeFile() override { close(); }

  // Open for appending after `rows` rows already on disk with `head` after them
  // (0 and genesis for a fresh tape).
  bool open_for_append(const std::string& d, uint32_t alphabet_pin, size_t rows_on_disk, const uint8_t head_after[32]) {
    dir = d; pin = alphabet_pin; rows = rows_on_disk;
    memcpy(last_head, head_after, 32);
    make_dir(dir);
    cur_seg = (uint32_t)(rows / SEG_ROWS); rows_in_cur = rows % SEG_ROWS;
    const std::string p = seg_path(dir, cur_seg);
    uint64_t sz = 0;
    if (rows_in_cur == 0 && !(file_size_of(p, sz) && sz >= sizeof(SegHeader))) {
      cur = std::fopen(p.c_str(), "wb"); if (!cur) return false;
      write_header(cur, cur_seg, rows, head_after);
    } else {
      cur = std::fopen(p.c_str(), "r+b"); if (!cur) return false;
      std::fseek(cur, (long)(sizeof(SegHeader) + rows_in_cur * sizeof(Rec)), SEEK_SET);
    }
    return true;
  }
  void write_header(FILE* f, uint32_t idx, uint64_t first_row, const uint8_t head[32]) {
    SegHeader h{}; memcpy(h.magic, "ACMETAPE", 8); h.rec_ver = REC_VER; h.hdr_ver = 1; h.pin = pin; h.seg_index = idx; h.first_row = first_row;
    memcpy(h.head_at_start, head, 32);
    std::fwrite(&h, sizeof h, 1, f);
  }
  void on_append(const Rec& r, const uint8_t head_after[32]) override {
    if (!cur) return;
    if (rows_in_cur == SEG_ROWS) {                       // roll the segment; the header carries the head before this record,
      std::fclose(cur); ++cur_seg; rows_in_cur = 0;      // which is the head after the previous one
      cur = std::fopen(seg_path(dir, cur_seg).c_str(), "wb");
      write_header(cur, cur_seg, rows, last_head);
    }
    std::fwrite(&r, sizeof(Rec), 1, cur);
    memcpy(last_head, head_after, 32);
    ++rows_in_cur; ++rows;
  }
  uint8_t last_head[32] = {0};
  void flush() { if (cur) std::fflush(cur); }
  void close() { if (cur) { std::fclose(cur); cur = nullptr; } }
  // the committed prefix: everything up to `n` rows stays, the rest is cut
  // (a restore drops the period that was in flight)
  bool cut_to(const std::string& d, size_t n) {
    close(); dir = d;
    const uint32_t last = (uint32_t)(n == 0 ? 0 : (n - 1) / SEG_ROWS);
    for (uint32_t s = last + 1; ; ++s) { const std::string p = seg_path(dir, s); uint64_t sz; if (!file_size_of(p, sz)) break; std::remove(p.c_str()); }
    const size_t keep = n - (size_t)last * SEG_ROWS;
    if (!truncate_file(seg_path(dir, last), sizeof(SegHeader) + keep * sizeof(Rec))) return false;
    if (n == 0) std::remove(seg_path(dir, 0).c_str());
    rows = n;
    return true;
  }
};

// Read every segment into a Tape, re-walk the chain from genesis, truncate a
// torn tail on disk, and report. `accept_torn` is O18's lie: a partial record
// taken as a row.
inline bool tape_load(const std::string& dir, Tape& t, Recovered& out, bool accept_torn = false) {
  t.reset(); t.chaining = true;
  uint8_t head[32] = {0};
  for (uint32_t s = 0; ; ++s) {
    const std::string p = seg_path(dir, s);
    uint64_t sz = 0;
    if (!file_size_of(p, sz)) break;
    FILE* f = std::fopen(p.c_str(), "rb"); if (!f) return false;
    SegHeader h{};
    if (sz < sizeof h || std::fread(&h, sizeof h, 1, f) != 1 || memcmp(h.magic, "ACMETAPE", 8) != 0 || h.rec_ver != REC_VER) { std::fclose(f); return false; }
    if (s == 0) out.pin = h.pin;
    if (memcmp(h.head_at_start, head, 32) != 0) { std::fclose(f); out.corrupt = true; out.corrupt_row = t.rec.size(); return false; }
    const uint64_t body = sz - sizeof h;
    const uint64_t whole = body / sizeof(Rec);
    const uint64_t torn = body - whole * sizeof(Rec);
    for (uint64_t i = 0; i < whole; ++i) {
      Rec r; if (std::fread(&r, sizeof r, 1, f) != 1) { std::fclose(f); return false; }
      t.append(r);
    }
    if (torn) {
      if (accept_torn) {                                    // THE LIE: the fragment, zero-padded, taken as a record
        Rec r{}; const size_t got = std::fread(&r, 1, (size_t)torn, f); (void)got; t.append(r);
        std::fclose(f);
        out.torn_bytes = 0;
      } else {
        std::fclose(f);
        out.torn_bytes = torn;
        truncate_file(p, sizeof h + whole * sizeof(Rec));
      }
    } else std::fclose(f);
    memcpy(head, t.head, 32);
  }
  out.rows = t.rec.size();
  memcpy(out.head, t.head, 32);
  t.recount_ticks();                     // the row after the last TICK, and the notes since, so the next TICK's count is right
  return true;
}

} // namespace acme
