// O17 · the machine links without the plant and without the governor. This
// translation unit includes everything the kernel is made of and nothing from
// the plant or the governor, and it is compiled with -DACME_NO_PLANT
// -DACME_NO_GOVERNOR, under which world.h and governor.h refuse to compile.
// If this file stops compiling, a kernel header has reached one of them.
#define ACME_NO_PLANT 1
#define ACME_NO_GOVERNOR 1
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/ledger.h"
#include "acme/port.h"
#include "acme/license.h"
#include "acme/gate.h"      // D2: the gate is its own header
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/report.h"
#include "acme/tapefile.h"   // D1: the durable tape is kernel-side

namespace acme {
// A null judge and a null store, so the kernel's period is instantiated
// against the port alone.
struct NullStore : Store {
  Frame frame(uint32_t oid, int cls, uint32_t mask) const override { Frame f; f.oid = oid; f.cls = cls; f.systems_mask = mask; return f; }
};
struct NullJudge : Judge {
  Proposal read(const Frame& f) override { Proposal p; p.choice = 0; p.signal = 0.f; p.completeness_hat = f.coverage_hat; p.judge_hash = hash(); return p; }
  bool act_coin(int, uint32_t, float) override { return false; }
  uint32_t hash() const override { return 0x4E554C4Cu; }   // 'NULL'
};
}

int acme_o17_instantiate() {
  using namespace acme;
  // the kernel cannot build a firm (the generator is the plant's); it is handed one
  Firm f; f.span = 7;
  for (int i = 0; i < 50; ++i) { Seat s{}; s.id = i; s.kind = SK_IC; s.fn = FN_E; s.wire = 0; s.boss = -1; s.attention = 330.f; s.attn_left = 330.f; s.spec = 1u; f.seat.push_back(s); }
  Ledger L; L.init(n_classes());
  Tape t; t.header(0, 2, alphabet_hash()); t.tick(0);
  Compiled C = compile_from_tape(t, L.NC, 1, true);
  Resident r; r.init(L.NC, f.size(), C, f.writ, 1);
  Ladder lic; lic.init(L.NC, alphabet_hash(), 0x4E554C4Cu);
  NullStore store; NullJudge judge, frontier;
  L.sw = SW_LIVE;
  r.period(L, f, t, store, judge, frontier, lic);
  r.grade(L, judge, frontier);
  const ReplayOut rp = replay(L, f, r.fd, C, lic, f.writ, store, judge);
  const Residual res = residual_of(lic, C);
  const ArmResult ar = score_arm(L, f);
  const Ladder folded = fold_ladder(t, L.NC, 0);
  return (int)(rp.n.size() + (size_t)res.total + (size_t)ar.settled + mine_invariants(t, L, L.NC).size() + folded.lic.size());
}
