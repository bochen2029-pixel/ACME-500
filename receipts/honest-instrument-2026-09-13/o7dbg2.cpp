// O7 candidates: several coverage estimators computable from the tape alone,
// scored against the planted truth on the O7 world. Not part of the build.
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/world.h"
#include "acme/human.h"
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/report.h"
#include <cstdio>
#include <cmath>
#include <vector>
using namespace acme;

struct Est { const char* name; std::vector<double> v; };
static void score(const World& w, const std::vector<long>& n, Est& e) {
  double num = 0, den = 0, sx = 0, sy = 0, sxy = 0, sxx = 0, syy = 0; int k = 0;
  for (int c = 0; c < w.NC; ++c) {
    if (n[c] < 60 || e.v[c] < 0) continue;
    const double x = 1.0 - w.tacit[c], y = e.v[c];
    num += std::fabs(x - y); den += 1; sx += x; sy += y; sxy += x * y; sxx += x * x; syy += y * y; ++k;
  }
  const double mae = den ? num / den : 1.0;
  const double r = (k * sxy - sx * sy) / std::max(1e-9, std::sqrt((k * sxx - sx * sx) * (k * syy - sy * sy)));
  std::printf("  %-52s mae %.3f  corr %+.3f  (%d classes)\n", e.name, mae, r, k);
}

int main(int argc, char** argv) {
  const uint64_t seed = argc > 1 ? (uint64_t)atoi(argv[1]) : 13;
  World w = make_world(seed); w.demand_scale = 1.5f;
  Firm f = build_acme(300, 7, seed);
  Tape tape; HumanStats hs; hs.init(w.NC);
  auto rep = reports_of(f);
  for (int d = 0; d < 150; ++d) { world_arrive(w, tape, d); human_day(w, f, tape, hs, rep, d, 0); world_settle(w, f, tape, d, 0); }
  Compiled C = compile_from_tape(tape, hs.by_class.data(), w.NC, seed, true);
  Field fd; fd.init(w.NC, f.size(), seed);
  const int NC = w.NC;
  std::vector<long> n(NC, 0), agree(NC, 0), hconf(NC, 0), hconf_agree(NC, 0), both(NC, 0), both_agree(NC, 0);
  std::vector<long> agreed_hconf(NC, 0), agreed_hconf_wrong(NC, 0), agreed(NC, 0), agreed_wrong(NC, 0);
  std::vector<double> absz(NC, 0), absm(NC, 0), zsum(NC, 0), msum(NC, 0);
  for (const Obligation& o : w.ob) {
    if (o.state != OB_SETTLED || o.by_machine) continue;
    const int c = o.cls;
    const Read mr = observe(w, o, C.join_graph[c], true, 0.f, fd.mach_skill_of(c), 0x22ULL);
    const float z = 4.0f * mr.signal * (0.35f + 0.65f * mr.completeness);
    const bool ag = (mr.choice == o.decision);
    const bool hc = std::fabs(o.margin) > 1.1f, mc = std::fabs(z) > 1.1f;
    const bool wrong = (o.outcome == OK_BAD);
    ++n[c]; if (ag) ++agree[c];
    if (hc) { ++hconf[c]; if (ag) ++hconf_agree[c]; }
    if (hc && mc) { ++both[c]; if (ag) ++both_agree[c]; }
    if (ag) { ++agreed[c]; if (wrong) ++agreed_wrong[c]; }
    if (ag && hc) { ++agreed_hconf[c]; if (wrong) ++agreed_hconf_wrong[c]; }
    absz[c] += std::fabs(z); absm[c] += std::fabs(o.margin);
  }
  std::vector<Est> E;
  Est e1{ "E1 full-sample agreement (v0 fallback)", std::vector<double>(NC, -1) };
  Est e2{ "E2 agreement | human confident only", std::vector<double>(NC, -1) };
  Est e3{ "E3 agreement | both confident (current, fails)", std::vector<double>(NC, -1) };
  Est e4{ "E4 1 - wrong-rate among agreed", std::vector<double>(NC, -1) };
  Est e5{ "E5 1 - wrong-rate among agreed & human-confident", std::vector<double>(NC, -1) };
  Est e6{ "E6 mean |z_machine| / mean |margin_human|", std::vector<double>(NC, -1) };
  Est e7{ "E7 1 - 2 * disagreement | human confident", std::vector<double>(NC, -1) };
  for (int c = 0; c < NC; ++c) {
    if (n[c]) e1.v[c] = (double)agree[c] / n[c];
    if (hconf[c] >= 30) e2.v[c] = (double)hconf_agree[c] / hconf[c];
    if (both[c] >= 30) e3.v[c] = (double)both_agree[c] / both[c];
    if (agreed[c] >= 30) e4.v[c] = 1.0 - (double)agreed_wrong[c] / agreed[c];
    if (agreed_hconf[c] >= 30) e5.v[c] = 1.0 - (double)agreed_hconf_wrong[c] / agreed_hconf[c];
    if (n[c] && absm[c] > 0) e6.v[c] = std::min(1.0, (absz[c] / n[c]) / (absm[c] / n[c]));
    if (hconf[c] >= 30) e7.v[c] = 1.0 - 2.0 * (1.0 - (double)hconf_agree[c] / hconf[c]);
  }
  std::printf("seed %llu\n", (unsigned long long)seed);
  for (Est* e : { &e1, &e2, &e3, &e4, &e5, &e6, &e7 }) score(w, n, *e);
  std::printf("\n  c  truth   E1     E2     E4     E5     E6     E7\n");
  for (int c = 0; c < NC; ++c) if (n[c] >= 60)
    std::printf(" %2d  %.3f  %.3f  %.3f  %.3f  %.3f  %.3f  %.3f\n", c, 1.0 - w.tacit[c], e1.v[c], e2.v[c], e4.v[c], e5.v[c], e6.v[c], e7.v[c]);
  return 0;
}
