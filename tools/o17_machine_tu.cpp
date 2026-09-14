// O17 · the machine links without the plant. This translation unit includes
// everything the kernel is made of and nothing from the plant, and it is
// compiled with -DACME_NO_PLANT, under which world.h refuses to compile.
// If this file stops compiling, a kernel header has reached the plant.
#define ACME_NO_PLANT 1
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/ledger.h"
#include "acme/port.h"
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/report.h"

namespace acme {
// A null judge and a null store, so the kernel's period is instantiated
// against the port alone.
struct NullStore : Store {
  Frame frame(uint32_t oid, int cls, uint32_t mask) const override { Frame f; f.oid = oid; f.cls = cls; f.systems_mask = mask; return f; }
};
struct NullJudge : Judge {
  Proposal read(const Frame& f, float, uint64_t) override { Proposal p; p.choice = 0; p.signal = 0.f; p.completeness_hat = f.coverage_hat; p.judge_hash = hash(); return p; }
  bool act_coin(int, uint32_t, float) override { return false; }
  uint32_t hash() const override { return 0x4E554C4Cu; }   // 'NULL'
};
}

int acme_o17_instantiate() {
  using namespace acme;
  Firm f = build_acme(50, 7, 1);
  Ledger L; L.init(n_classes());
  Tape t; t.header(0, 2, alphabet_hash()); t.tick(0);
  Compiled C = compile_from_tape(t, L.NC, 1, true);
  Resident r; r.init(L.NC, f.size(), 1.0f, C, f.writ, 1);
  NullStore store; NullJudge judge;
  r.period(L, f, t, 0, store, judge);
  r.grade(L, f, t, 0);
  const ReplayOut rp = replay(L, f, r.fd, C, r.lad, f.writ, store, judge);
  const Residual res = residual_of(r.lad, 1.0f, C);
  const ArmResult ar = score_arm(L, f);
  return (int)(rp.n.size() + (size_t)res.total + (size_t)ar.settled + mine_invariants(t, L, L.NC).size());
}
