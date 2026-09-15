// ============================================================================
//  acme/calib.h — THE CALIBRATION HARNESS: the heart consumes a curve   [E3c]
//
//  ARCHITECTURE v4 §28.5. For every licence key (a class, under the judge the
//  table is keyed to) the curve |direction| bin → wrong-rate, measured on cells
//  the incumbent, a person keying a draft, a signer, or the machine executed
//  WITH THE MACHINE'S OWN CHOICE: the history's agreement cases (the replay)
//  and, live, every OUTCOME on a cell whose executed choice was the last
//  proposal's. Stored as a monotone step function fitted by pooled adjacent
//  violators (non-increasing in |direction|), with its sample sizes, frozen per
//  term and written to the tape as CALIB rows (one per class per bin) so a
//  fold reads the curve the gate read (O30) and the curve is pinned by the
//  rows themselves. The gate consumes the calibrated wrong-rate where it read
//  the raw direction against the thin margin, when the writ says so
//  (thin_wrong > 0); a key with no measured curve reads rung 0 (unmeasured is
//  never safe); a judge whose raw curve is not monotone on a class is
//  inadmissible on that class (F-CALIB): a wrong-rate that rises with the
//  judge's confidence is a judge that is confidently wrong.
//
//  Kernel side: core.h and firm.h only. The plant never appears here.
// ============================================================================
#pragma once
#include "core.h"
#include "firm.h"

namespace acme {

enum : int { CALIB_NBIN = 8 };
// the bin edges over |direction|: the thin margin (0.55) and the band edge (1.21) are edges
inline const float* calib_edges() { static const float e[CALIB_NBIN + 1] = { 0.f, 0.25f, 0.55f, 0.85f, 1.21f, 1.6f, 2.2f, 3.0f, 1e30f }; return e; }
inline int calib_bin_of(float absdir) {
  const float* e = calib_edges(); int b = 0;
  while (b + 1 < CALIB_NBIN && absdir >= e[b + 1]) ++b;
  return b;
}

struct Calib {
  int NC = 0;
  std::vector<long>  n, bad;         // [c*NBIN+bin] the cells graded, and the wrong ones, cumulative
  std::vector<float> fit;            // [c*NBIN+bin] the frozen monotone fit (wrong-rate), this term
  std::vector<float> raw;            // [c*NBIN+bin] the raw rate at the freeze, for the record
  std::vector<uint8_t> measured;     // [c] at least m_calib cells graded at the freeze
  std::vector<uint8_t> monotone;     // [c] no significant violation of non-increase at the freeze
  int terms_frozen = 0;

  void init(int nc) {
    NC = nc; n.assign((size_t)nc * CALIB_NBIN, 0); bad.assign((size_t)nc * CALIB_NBIN, 0);
    fit.assign((size_t)nc * CALIB_NBIN, 0.f); raw.assign((size_t)nc * CALIB_NBIN, 0.f);
    measured.assign(nc, 0); monotone.assign(nc, 1); terms_frozen = 0;
  }
  void push(int c, float absdir, bool wrong) {
    if (c < 0 || c >= NC) return;
    const size_t k = (size_t)c * CALIB_NBIN + calib_bin_of(absdir);
    ++n[k]; if (wrong) ++bad[k];
  }
  long total(int c) const { long t = 0; for (int b = 0; b < CALIB_NBIN; ++b) t += n[(size_t)c * CALIB_NBIN + b]; return t; }

  // THE FREEZE. Per class: the raw rates; the pooled-adjacent-violators fit,
  // non-increasing in |direction| (a bin with no cells inherits its neighbour's
  // pool); the monotone test on the raw rates: a later bin whose rate exceeds an
  // earlier bin's by more than two standard errors of the difference, both bins
  // with at least ten cells, is a violation, and one violation is enough.
  void freeze(int m_calib) {
    ++terms_frozen;
    for (int c = 0; c < NC; ++c) {
      double r[CALIB_NBIN]; double w[CALIB_NBIN]; long nn[CALIB_NBIN];
      for (int b = 0; b < CALIB_NBIN; ++b) {
        const size_t k = (size_t)c * CALIB_NBIN + b; nn[b] = n[k];
        r[b] = nn[b] ? (double)bad[k] / (double)nn[b] : 0.0; w[b] = (double)nn[b];
        raw[k] = (float)r[b];
      }
      measured[c] = (total(c) >= m_calib) ? 1 : 0;
      // monotone: non-increasing wrong-rate in |direction|. A violation is a
      // later bin above an earlier one by more than three standard errors of
      // the difference AND by more than five points, both bins with at least
      // ten cells: twenty-eight pairs per class over two dozen classes would
      // flag an honest judge by chance at two standard errors alone.
      bool mono = true;
      for (int i = 0; i < CALIB_NBIN && mono; ++i) for (int j = i + 1; j < CALIB_NBIN; ++j) {
        if (nn[i] < 10 || nn[j] < 10) continue;
        const double se = std::sqrt(r[i] * (1 - r[i]) / nn[i] + r[j] * (1 - r[j]) / nn[j]);
        if (r[j] > r[i] + std::max(3.0 * se, 0.05) + 1e-9) { mono = false; break; }
      }
      monotone[c] = mono ? 1 : 0;
      // PAVA, weighted by counts, for a NON-INCREASING sequence: pool while a
      // later block's mean exceeds an earlier one's
      double bv[CALIB_NBIN], bw[CALIB_NBIN]; int bs[CALIB_NBIN]; int nb = 0;
      for (int b = 0; b < CALIB_NBIN; ++b) {
        if (w[b] <= 0.0) continue;                      // an empty bin joins the pool of its neighbours below
        bv[nb] = r[b]; bw[nb] = w[b]; bs[nb] = 1; ++nb;
        while (nb >= 2 && bv[nb - 1] > bv[nb - 2] + 1e-12) {
          const double tw = bw[nb - 1] + bw[nb - 2];
          bv[nb - 2] = (bv[nb - 2] * bw[nb - 2] + bv[nb - 1] * bw[nb - 1]) / tw; bw[nb - 2] = tw; bs[nb - 2] += bs[nb - 1]; --nb;
        }
      }
      // write the fit back over the non-empty bins in order; empty bins take the last value (or the first)
      int pos = 0, blk = 0, used = 0; float last = nb ? (float)bv[0] : 0.f;
      for (int b = 0; b < CALIB_NBIN; ++b) {
        const size_t k = (size_t)c * CALIB_NBIN + b;
        if (w[b] > 0.0 && nb) { fit[k] = (float)bv[blk]; last = fit[k]; if (++used >= bs[blk]) { used = 0; ++blk; } }
        else fit[k] = last;
        ++pos;
      }
      (void)pos;
    }
  }
  float wrong_at(int c, float absdir) const {
    if (c < 0 || c >= NC) return 0.f;
    return fit[(size_t)c * CALIB_NBIN + calib_bin_of(absdir)];
  }
  bool admissible(int c) const { return c >= 0 && c < NC && measured[c] && monotone[c]; }
  // the frozen curve travels as CALIB rows, written by the resident through the
  // ledger's fold (machine.h, Resident::calib_open_term); this struct holds the
  // counts and the fit and writes nothing itself
  // the curve's own pin: BLAKE2b over the frozen fit, counts and flags, beside the judge hash
  uint32_t hash() const {
    Blake2b h;
    if (!fit.empty()) h.update(fit.data(), fit.size() * sizeof(float));
    if (!n.empty()) h.update(n.data(), n.size() * sizeof(long));
    if (!measured.empty()) h.update(measured.data(), measured.size());
    if (!monotone.empty()) h.update(monotone.data(), monotone.size());
    uint8_t d[32]; h.final(d);
    return (uint32_t)d[0] | ((uint32_t)d[1] << 8) | ((uint32_t)d[2] << 16) | ((uint32_t)d[3] << 24);
  }
};

} // namespace acme
