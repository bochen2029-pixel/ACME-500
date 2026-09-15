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
  // E0: THE SHARED FACT. A record may hold a fact shared by many cells of its
  // class (a supplier's date, a rate, a policy line). The store names its
  // epoch and its value; base_hash is the record's hash with the shared fact
  // held out, so the kernel can tell "only the shared fact moved" from "the
  // record changed". The kernel never sees what the fact weighs.
  uint32_t base_hash = 0;
  uint32_t shared_epoch = 0;
  float    shared_value = 0.f;
};

struct Proposal {
  int      choice = 0;
  float    signal = 0.f;         // the raw read; the machine scales it into a direction
  float    completeness_hat = 0.f;
  uint32_t judge_hash = 0;
  // E0: THE CERTIFICATE. d signal / d shared_value, if the judge can say (an
  // arithmetic judge can; a model judge cannot until calibration has measured
  // a flip rate for it). With it the kernel carries a proposal across a change
  // of the shared fact when the sign and the band survive, and reads only when
  // they do not. Never the plant's weight as the kernel would see it: the
  // judge's own answer, through the port.
  float    sensitivity = 0.f;
  bool     has_sens = false;
};

struct Store {
  virtual ~Store() {}
  virtual Frame frame(uint32_t oid, int cls, uint32_t systems_mask, uint32_t day) const = 0;   // E0: the day names the shared fact's epoch
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
