// ============================================================================
//  acme/port.h — THE PORT: the only way the machine touches a world   [v2, C1]
//
//  Two interfaces and nothing else cross between the kernel and whatever world
//  stands behind it. The STORE answers "what does this cell's record hold" as a
//  Frame; the JUDGE answers "what would you do with it" as a Proposal with a
//  margin and a hash. The kernel links against these declarations only; the
//  plant implements them in world.h (PlantStore, PlantJudge), a real lane
//  implements them from CDC and a model. The machine cannot tell which.
//
//  C1: the judge owns its competence and its noise; neither crosses the seam.
//  The kernel stamps its own coverage estimate on the frame (the compile
//  step's, never the plant's), reads the judge only when the frame's hash
//  changed or the floor sample says so, and carries the last proposal forward
//  otherwise. The seam rule: the kernel owns when, where, whether and how
//  much; the judge owns the map from the open alphabet to the closed one.
// ============================================================================
#pragma once
#include "core.h"

namespace acme {

// What the machine may see of one cell.
struct Frame {
  int      cls = 0;
  uint32_t oid = 0;
  uint32_t systems_mask = 0;     // the join graph the machine reads with
  bool     boundary = true;      // the inbound document is always in hand
  float    coverage_hat = 0.f;   // THE KERNEL'S estimate of what this frame holds of the decision:
                                 // the compile step's coverage for the class (1.0 until measured)
  uint32_t frame_hash = 0;       // the store's hash of the record's content: the cell's own rows,
                                 // the span contents; never the clock. The kernel folds the
                                 // template pin in before it keys the memo on it.
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
  virtual Proposal read(const Frame& f) = 0;
  // the rented mind's act coin (F16): drawn by the judge on its own key, never by the kernel
  virtual bool act_coin(int cls, uint32_t oid, float p) = 0;
  // the kernel tells the judge what the world said about a cell it decided. Public
  // information, never evidence for a licence; a judge may ignore it.
  virtual void observe(int cls, bool right) { (void)cls; (void)right; }
  virtual uint32_t hash() const = 0;
};

} // namespace acme
