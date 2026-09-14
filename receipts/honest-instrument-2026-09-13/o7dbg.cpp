// O7 diagnostic: per class, the planted truth against both coverage estimators.
// Same world, seed and warm-up as the O7 oracle in main.cpp. Not part of the build.
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/world.h"
#include "acme/human.h"
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/report.h"
#include <cstdio>
using namespace acme;

int main() {
  World w = make_world(13); w.demand_scale = 1.5f;
  Firm f = build_acme(300, 7, 13);
  Tape tape; HumanStats hs; hs.init(w.NC);
  auto rep = reports_of(f);
  for (int d = 0; d < 150; ++d) { world_arrive(w, tape, d); human_day(w, f, tape, hs, rep, d, 0); world_settle(w, f, tape, d, 0); }
  Compiled C = compile_from_tape(tape, hs.by_class.data(), w.NC, 13, true);
  Field fd; fd.init(w.NC, f.size(), 13); Ladder lad; lad.init(w.NC);
  ReplayOut RP = replay(w, f, fd, C, lad, f.writ);
  std::printf("%3s %6s %6s %6s %6s %6s %6s %6s %6s %6s  %s\n", "c", "n", "conf", "tacit", "truth", "full", "cstrat", "agr_g", "agr_b", "dis", "human completeness (mean)");
  for (int c = 0; c < w.NC; ++c) {
    double hc = 0; long hn = 0;
    for (const Obligation& o : w.ob) if (o.state == OB_SETTLED && o.cls == c) { hc += o.completeness; ++hn; }
    std::printf("%3d %6ld %6ld %6.3f %6.3f %6.3f %6.3f %6ld %6ld %6ld  %.3f\n", c, RP.n[c], RP.conf_n[c], w.tacit[c], 1.0 - w.tacit[c],
                RP.band_mass[c], RP.conf_n[c] ? (double)RP.conf_agree[c] / RP.conf_n[c] : -1.0,
                RP.agree_good[c], RP.agree_bad[c], RP.disagree[c], hn ? hc / hn : 0.0);
  }
  return 0;
}
