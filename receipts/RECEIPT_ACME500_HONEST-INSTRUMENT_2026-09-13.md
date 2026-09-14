# Receipt · ACME-500 step 1 · the honest instrument · 2026-09-13

*Claude Opus 5. Implements `C:\55555\KICKOFF_ACME500-STEP1_HONEST-INSTRUMENT_2026-09-13.md`. Source of record `C:\55555\acme500\`; built and run under WSL Ubuntu-24.04 at `/root/acme500`. Raw outputs of every run below are in `receipts\honest-instrument-2026-09-13\`. Every number carries its command. The physics in `firm.h` and `world.h` was not touched; no constant was tuned to pass an oracle.*

## 0 · Baseline reproduced before editing

Rebuilt from the Windows tree: 15 passed, 1 failed, O7 red at mean abs err 0.071, corr −0.052 over 19 classes; lie 7 exit 0, every other lie exit 1, lie 6 with O6 printing FAIL. Identical to the first-run receipt.

## 1 · The fixes, in the kickoff's order

| # | defect | change | acceptance, measured |
|---|---|---|---|
| 1 | lie-arm convention inverted between code and harness | the code's convention kept (a caught lie prints PASS); the two prints now say so; CMake drops `WILL_FAIL` and asserts, per lie, `PASS_REGULAR_EXPRESSION "\[PASS\] <oracle> "` and `FAIL_REGULAR_EXPRESSION "\[FAIL\]"` | every lie run exits 0 with its oracle's own line PASS; see §2 |
| 2 | O6's lie planted in `join_graph`, oracle read `n_systems` | the oracle recounts systems from `join_graph` (`__builtin_popcount`) | clean 24 of 24; under `--lie 6` 23 of 24, caught |
| 3 | incumbent's band never assigned; wager graded against a constant 0.72 | `band_of` and `NBAND` moved to `firm.h` beside the writ; `human_work` sets `o.band` from its own margin at decide time | `good_h` at band 2 reads 0.195, 0.240, 0.144, 0.221 (was 0.000 everywhere); `p_incumbent` is measured |
| 4 | twin scored settled obligations only; backlog free | `ArmResult` gains `open_n`, `backlog_cost` at `w_unplaced × value`, `total_cost()`; sim, twin and the multiverse objective use it | twin prints backlog $1,600.98M vs $420.27M; see §3 |
| 5 | drafts counted as the wager | `Obligation::via` (0 human, 1 act, 2 draft, 3 rented mind, 4 warrant); `Integrator::commit` records it; `Ladder::observe` feeds the e-process from via 1 and 3 only, counts 2 and 4 as `n_assisted`; the Horvitz–Thompson weight applies to via 1 only; ladder table, counter and twin estimate split accordingly | wager 12,122 outcomes against n₀ 77,874; assisted 95,963 graded and never fed |
| 6 | lateness inside the novelty score | the conformal score is `\|completeness − 0.7\|` alone, in the score and its calibration | `novel-case` 60,221 → 0; see F11 |
| 6b | sharpness and novelty printed under one word | `RS_UNSURE`, "unsure-placement", for the support-spectrum branch that rents a frontier mind | 86,309 now print as `unsure-placement` |
| 7 | `eps_floor` declared, set, read nowhere | the canary draw's two hard-coded 0.02 floors read `wr.eps_floor` | canary instances 402 → 537 at the 0.03 default; **the multiverse acceptance is NOT met**, the floor is read but cannot bind at this schema, see §5 and F14 |

## 2 · O7, the coverage estimator

The shipped estimator read coverage off agreement on the stratum where both readers were far from the fence. A diagnostic on the O7 world (`o7dbg.cpp`, seed 13, 300 seats, 150 days) showed that stratum agreeing at 0.96 to 1.00 in every class whatever its tacit mass, because a machine missing a heavy determinant lands near the fence and is filtered out: the stratum discards exactly the informative cases. It also showed the premise failing underneath: the humans' mean completeness on tacit classes is 0.64 to 0.92, so they hold 10 to 40 percent of the tacit mass themselves, and two readers with the same blind spot agree across it.

Seven estimators computable from the tape alone were scored against the planted truth on three seeds (`o7dbg2.cpp`, classes with n ≥ 60):

| estimator | seed 13 mae / corr | seed 29 | seed 41 |
|---|---|---|---|
| E1 full-sample agreement | 0.103 / +0.439 | 0.132 / +0.564 | 0.105 / +0.704 |
| E2 agreement, human confident | 0.074 / +0.187 | 0.074 / −0.070 | 0.065 / +0.457 |
| E3 agreement, both confident (shipped) | 0.071 / −0.052 | 0.065 / +0.074 | 0.067 / +0.405 |
| **E4 1 − wrong-rate among agreed** | **0.052 / +0.945** | **0.068 / +0.963** | **0.052 / +0.841** |
| E5 E4 on the human-confident stratum | 0.063 / +0.866 | 0.056 / +0.849 | 0.048 / +0.865 |
| E6 mean \|z\| ratio | 0.064 / +0.688 | 0.092 / +0.737 | 0.066 / +0.781 |
| E7 1 − 2 × disagreement, human confident | 0.096 / +0.187 | 0.103 / −0.070 | 0.087 / +0.457 |

E4 is installed. Where the resident and the firm chose the same action and the world still said wrong, both missed the determinant that decided the case, and the only determinants both can miss are the unrecorded ones. It uses the agreement diagonal only, which is what O8 permits, and nothing planted. It is biased toward one where a missing determinant seldom flips the sign (class 17: truth 0.570, estimate 0.791), and the code says so. The oracle's thresholds were not moved: mae < 0.12, corr > 0.45, met with margin on all three seeds.

The lie arm had to change with it. The old lie moved one determinant to tacit after the outcomes had settled; against E4 the oracle still tracked (0.055 / 0.932) and printed FAIL, meaning the lie never reached the mechanism. The new lie is a world that never says wrong: under `--lie 7` every settled outcome is set GOOD, the estimator reads corr 0.000, and the oracle catches it.

**Battery after step 1:** `./acme --selftest` exit 0, **16 passed, 0 failed**. Fourteen lie arms, each run with its lied-to oracle's line checked:

```
lie  0 exit=0  O0c [PASS]      lie  7 exit=0  O7  [PASS]
lie  1 exit=0  O1  [PASS]      lie  8 exit=0  O8  [PASS]
lie  2 exit=0  O2  [PASS]      lie  9 exit=0  O9  [PASS]
lie  3 exit=0  O3  [PASS]      lie 10 exit=0  O10 [PASS]
lie  4 exit=0  O4  [PASS]      lie 11 exit=0  O11 [PASS]
lie  5 exit=0  O5  [PASS]      lie 12 exit=0  O12 [PASS]
lie  6 exit=0  O6  [PASS]      lie 13 exit=0  O13 [PASS]
```

## 3 · The twin, with the backlog on the books

`./acme --twin`, 500 seats, span 7, demand 1.5, 260 days, 49 s.

| | incumbent | resident | delta |
|---|---|---|---|
| settled | 82,479 | 186,956 | +104,477 |
| good % | 42.70 | 38.34 | −4.36 |
| wrong % | 8.40 | 11.89 | +3.49 |
| late % | 48.90 | 49.77 | +0.86 |
| writ cost of outcomes, $M | 236.73 | 827.68 | +590.95 |
| backlog at w_unplaced, $M | 1,600.98 | 420.27 | −1,180.71 |
| **total cost, $M** | **1,837.71** | **1,247.95** | **−589.76** |
| still open at the end | 123,927 | 19,450 | −104,477 |

The first run's headline was +$591M against the machine, computed with 124,000 of the incumbent's unfinished obligations off the books. Priced at the writ's own stock price, the sign reverses: −$590M in the machine's favour. The machine is still 3.5 points worse on correctness and 4.4 on the good rate; what it buys is the backlog, which the writ says is worth more.

The within-firm estimate, now computed from unattended acts only: +27.37 points (12,122 acts against 78,871 human-decided), against a true paired effect of −4.36; estimator bias **+31.73**. The first run printed −10.85 with drafts in the numerator. The 95,963 assisted decisions, drafts a person keyed and warrants a signer executed, ran 26.0 percent good and are printed beside the estimate, not inside it.

## 4 · The automate, restated

`./acme --automate`, 180 warm + 80 live days, 47 s. Join graph 24 of 24; coverage mean 0.913 against planted 0.889, error +0.024 (was +0.047); 34 class-bands licensed to rung 1 from history; 32 class-bands past rung 0 after 80 live days; 26 unlicensable; kappa 0.242.

Refusal reasons: ok 3,633; unlicensed 62,466; irreversible 9,305; no-adjudication-budget 63,324; blocked-by-dep 3,219; audit-sample 37,409; canary 537; unsure-placement 86,309; novel-case 0.

**The cascade, restated.** Licensed decision mass **63.7 percent** (was 91.8); E falls from 412 to 150; N = **193** with the honest fit, 181 with the origin fit. The first run's 500 → 57 is retracted; the number that replaces it is 500 → 193, and it is a reading of a synthetic firm under one writ, not a claim about any business. Residual: licensed 63.7, warrant 4.7, counterparty 0.7, frame 14.3, thin tape 16.7 percent.

## 5 · The multiverse, landed

```
./acme --multiverse     exit=0    real 34m56s, user 34m51s    10 patches × 6 rollouts × 200 days, 2,096 s
```

The objective now includes the backlog, so every J is about $330M higher than the first run's. Ranked by J + 0.5σ, $M over 200 days:

| rank | J mean | sd | J pess | weight | patch | first run's rank |
|---|---|---|---|---|---|---|
| 0 | 828.36 | 12.39 | 834.56 | 0.906 | buy the lane on claim.coverage | 0 |
| 1 | 832.32 | 17.62 | 841.13 | 0.034 | buy the lane on dispute.resolve | 1 |
| 2 | 832.04 | 18.93 | 841.51 | 0.028 | buy the lane on fraud.refer | 2 |
| 3 | 834.25 | 20.64 | 844.57 | 0.006 | thin margin 0.30, act sooner | 9 |
| 4 | 834.97 | 19.38 | 844.66 | 0.006 | baseline | 6 |
| 5 | 834.97 | 19.38 | 844.66 | 0.006 | eps floor 0.10, explore more | 7 |
| 6 | 835.22 | 19.30 | 844.88 | 0.005 | kappa cap 0.60 | 8 |
| 7 | 834.67 | 20.64 | 844.99 | 0.005 | span 10 | 4 |
| 8 | 835.74 | 19.66 | 845.57 | 0.004 | span 5 | 5 |
| 9 | 837.68 | 20.51 | 847.93 | 0.001 | thin margin 0.90, act later | 3 |

What moved and why. The three lane-buying patches keep the top three places; the best beats doing nothing by $6.6M on a spread of about $8M over six rollouts, under one standard error, a direction and not a verdict. The two thin-margin patches swapped ends: acting sooner was the worst patch when the backlog was free and is now third; acting later was third and is now worst. That is the twin's finding in the search: with the queue priced, draining it is worth more than the correctness it costs. The span patches stay inside noise.

**Fix 7's acceptance is not met, and the receipt says so.** "Explore more" still reproduces the baseline to the digit: 834.97, 19.38, 844.66. `eps_floor` is now read (`machine.h` `canary_rate`), but it cannot bind at this schema, for two reasons that are both structural:

- The canary rate at rungs 1 to 3 is `min(0.35, max(eps_floor, n₀ / V_term))`, and `n₀ / V_term` is at least 0.55 for every one of the 24 classes (the smallest is `claim.intake.triage` at 913 over 1,650; `invoice.issue` is 3,368 over 1,410). The identifiability floor exceeds the 0.35 cap everywhere, so every class runs its canary at the cap and no uniform floor below it can matter. The README's own sentence that the rarest-failure classes are the slowest to license has this corollary: at these base rates and volumes, no class can clear n₀ inside a term at any rate, which is why 26 bands print as unlicensable.
- Above rung 1 the canary flag only labels an act as `canary` rather than `ok`; the gate acts either way (`solver.h` `gate`, the rung-1 line is the only one that consults `in_canary`). So at rung 4 and above, where `eps_floor` could bind against 1/√(3F), it changes a reason code and no decision.

This is recorded as **F14** and not fixed: the fix belongs to the schema and the gate, not to a constant.

## 6 · New findings

**F14 · The uniform floor cannot bind at this schema.** See §5. The multiverse measured it exactly, twice, for two different reasons; the first time the parameter was unread, the second time it is read and dominated.

**F11 · The novelty instrument is inert in this world.** With lateness removed, the conformal score is a function of the resident's completeness, and the resident's completeness is a per-class constant (it reads every instrumented system, every time, with no decay). A constant has no tail, so `novelty > 0.97` never fires and `novel-case` reads 0. The first run's 60,221 "novel" cases were lateness wearing the wrong word. A novelty score with variance needs a per-instance input, the natural one being the magnitude of the resident's read signal relative to its calibration sample; that is a design change and is not made here.

**F12 · The frontier is re-rented daily for the same case.** An obligation that reaches `V_FRONTIER` and fails the rented mind's 72 percent act draw is escalated, stays in the open set, and is re-gated the next period, where the same sharpness sends it to the frontier again. The 86,309 frontier calls over 80 days are dominated by repeats on aged, escalated obligations; the frontier bill scales with backlog age rather than with novel cases. Not fixed here.

**F13 · The good rate is a lateness rate at this operating point.** In band 2 the firm's own good rate is 0.14 to 0.24 while its wrong rate is 8.4 percent; nearly all of the gap is lateness at demand 1.5. The license is graded on the writ's metric by design, and on that metric the resident's unattended acts, which are on time by construction, read 0.75 to 0.96. The e-process is honest about what it compares; the writ decides that timeliness counts.

## 7 · What was not done

F11, F12 and F14 are recorded, not fixed; fix 7's multiverse acceptance failed for the reason F14 gives. The physics was not touched. `README.md` was appended, not rewritten. Nothing was committed; there is no git here. No number in this receipt has met a real firm.
