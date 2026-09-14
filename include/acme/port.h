// ============================================================================
//  acme/port.h — THE PORT: the only way the machine touches a world   [v2, C0]
//
//  Two interfaces and nothing else cross between the kernel and whatever world
//  stands behind it. The STORE answers "what does this cell's record hold" as a
//  Frame; the JUDGE answers "what would you do with it" as a Proposal with a
//  margin and a hash. The kernel links against these declarations only; the
//  plant implements them in world.h (PlantStore, PlantJudge), a real lane
//  implements them from CDC and a model. The machine cannot tell which.
//
//  C0 installs the port with the plant's exact arithmetic behind it, so every
//  printed number is unchanged; C1 changes what stands behind it and the
//  numbers are allowed to move, each delta named.
// ============================================================================
#pragma once
#include "core.h"

namespace acme {

// What the machine may see of one cell. In C0 `coverage_hat` is the plant's
// own completeness for the mask; in C1 it becomes the compile step's coverage
// times the fraction of the join graph present in the cell's CONTEXT rows.
struct Frame {
  int      cls = 0;
  uint32_t oid = 0;
  uint32_t systems_mask = 0;     // the join graph the machine reads with
  bool     boundary = true;      // the inbound document is always in hand
  float    coverage_hat = 0.f;
  uint32_t frame_hash = 0;       // f(the cell's own rows, the template pin, span contents); never the clock
};

struct Proposal {
  int      choice = 0;
  float    signal = 0.f;         // the raw read; the machine scales it into a direction
  float    completeness_hat = 0.f;
  uint32_t judge_hash = 0;
};

struct Store {
  virtual ~Store() {}
  virtual Frame frame(uint32_t oid, int cls, uint32_t systems_mask) const = 0;
};

struct Judge {
  virtual ~Judge() {}
  // competence and noise_key are the C0 shape of the call: the plant's read
  // arithmetic needs both to reproduce v1 bit for bit. In C1 they become the
  // judge's own property and leave the signature.
  virtual Proposal read(const Frame& f, float competence, uint64_t noise_key) = 0;
  // the rented mind's act coin (F16): drawn by the judge, not by the kernel
  virtual bool act_coin(int cls, uint32_t oid, float p) = 0;
  virtual uint32_t hash() const = 0;
};

} // namespace acme
