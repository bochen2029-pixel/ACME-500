# Receipt · ACME-500 v2 step E · the second step allowed to move numbers · 2026-09-15

*Claude Opus 5. Step E as planned in `docs/ARCHITECTURE_v3.md` §14b (E0) and §23 (the order as amended: E0 → E1 → E2). v3 became the standing spec at the start of E0, the operator having delegated the choice; its Part II is v2 verbatim. Branch `v2` of `C:\55555\acme500` (`github.com/bochen2029-pixel/ACME-500`). Built and run under WSL from a fresh copy by `tools/run_step.sh step-e-2026-09-15/e0 receipts/step-d-2026-09-14/d2`; raw outputs under `receipts/step-e-2026-09-15/e0/`. Every sub-step: build, the battery, every lie arm, the tool oracles, the three modes diffed against the previous stage, its receipt section, commit, push.*

## E0 · the shared fact and the certificate; unresolved outcomes; the exposure cap

**What E0 is for.** The claim the whole programme rests on is that cognition can track *decision-relevant change* rather than state size: when one fact shared by three hundred open cells moves, the machine should reconsider only where a margin is crossed. E0 builds the smallest world in which that can be measured — a shared fact per class that moves at a planted rate — and the mechanism that meets it: the judge returns, through the port, a certificate of how its read moves with the fact, and the kernel carries a proposal across a change on that certificate when the sign and the band both survive, reading again otherwise. Beside it, the two things the fourth review said the exposure cap could not be tested without: a world that sometimes never answers, with an explicit write-off rule, and outstanding exposure as a fold with a cap the gate reads.

**The plant** (`world.h`). `make_world(seed, shared, flip_rate, unresolved_rate)`. With `shared` on, every class gets one more determinant, in one of its systems, at a fifth of the class's mass, whose value is `shared_x(c, epoch)` — the same for every cell of the class — and whose epoch advances on days drawn at `flip_rate` per class per day (0.025 by default: about two changes a year per class; 39 changes across 24 classes at 500 seats). The truth of a cell is the truth in force on the day it was decided (`truth_decision` reads the epoch of `day_decided`); a person's read and the judge's read see the value in force on the day they read (`gather(…, epoch)`). With `unresolved_rate` on, a decided cell is never graded with that probability; after `Writ::write_off_terms` (3) verdict latencies the world port closes it as `OK_UNRESOLVED` — an explicit `OUTCOME` row, priced as late by the writ, **evidence for nothing** (the governor and the resident's grade skip it), and the only thing that releases the exposure it held. **Every reading at the defaults is identical to D2**: the shared fact and the unanswering world exist only when asked for.

**The port** (`port.h`). `Frame` gains `base_hash` (the record with the shared fact held out), `shared_epoch` and `shared_value`; `Store::frame` takes the day, which names the epoch. `Proposal` gains `sensitivity` and `has_sens`: the judge's own statement of ∂signal/∂fact. The plant's judge can say it (its noise is per cell and does not move with the fact, so the certificate is exact for it); a model judge says nothing until calibration has measured a flip rate for it (step I). The kernel never sees what the fact weighs except as the judge's answer through the port — the reviewer's correction, honoured.

**The rows** (`core.h`). `SENSE` (oid, `a` = the fact's epoch, `b` = 1 if the judge could say, `margin` = the sensitivity, `value` = the fact's value read), written beside every fresh `PROPOSAL` and beside every carry; the memo folds it (`LastProp::has_sens/sens/shared_x/shared_epoch`). A carry is a `PROPOSAL` row with `via` = 1 and the new direction, plus its `SENSE`; a fold re-derives it: direction′ = direction + 4·sens·(0.35 + 0.65·c)·(x′ − x). The reason `exposure` and the outcome kind `unresolved` join the alphabet; the pin moves (`0x2dc5517b` → `0x81ebc266`), the gate pin moves with `GateIn`'s two new inputs (`d1115a674638eba9` → `aa7e850b6bdb032f`, `tools/gate_hash.py --write`, named here).

**The kernel** (`machine.h`). In the read loop, a memo whose `base_hash` still matches while the fact's epoch differs is *moved by the fact alone*: if the certificate exists and the new direction keeps its sign and its band **with a margin of 10⁻³ at the sign and at both band edges** (a direction within that of an edge is read again, so float rounding between a carry and a fresh read can never differ), the proposal is carried without a read; otherwise the cell is read again, and the read's choice and band are compared with the old ones. `--check-shared` is a measurement mode: every carry is also re-read against the judge and disagreements are counted. The gate takes `value` and `exposure_left` and holds an act — and a frontier rental, which is the wager too — with reason `exposure` when the outstanding total would pass the cap; the check sits after the budget's, so exhaustion of either produces a hold and never an act (O2 gains the axis). `Ledger::outstanding[c]`/`outstanding_total` is a fold of `EFFECT` (via 1 and 3, not shadow) and `OUTCOME`/`UNDO` rows; the hand and the world port keep the live ledger equal to it; O14 compares it. Three ledgers as the review asked: reserved (the gate's check, the instant before the effect), committed (the effect row), outstanding (until the verdict or the write-off).

**O37 · the shared fact** (lie arm 24). The short automate under a shared world (`flip_rate` 0.05, `--check-shared`) with a judge whose competence does not learn, so the certificate is exact and 0 disagreements is the requirement:

```
  [PASS] O37  ... (33 changes of a shared fact; 515 frames moved by it: 324 carried by the certificate, all 324
               re-derived from rows, 324 checked against the judge with 0 disagreements; 191 read again, of which
               31 changed choice and 180 changed band)
```

Every carry re-derives from its `PROPOSAL` and `SENSE` rows; the judge's own read agrees with every carry on sign and band; 191 of 515 were read again because a margin would cross, and of those the choice actually changed on 31 and the band on 180. The lie carries the old proposal as if the fact had not moved: 491 carries, 0 re-derivable, caught.

**O38 · outstanding exposure** (lie arm 25). The short automate with 5 percent of cells never answered and a cap of $300k:

```
  [PASS] O38  ... (cap $300k; the fold's outstanding peaked at $300k, 0 rows over the cap; 10846 acts held for exposure;
               491 write-offs, each a row; the ladder counted 37 wager outcomes against 37 answered; the live ledger's
               outstanding equals the fold's)
```

The fold's outstanding never exceeds the cap at any row of the tape; acts were held for it; every unanswered cell was closed by an explicit write-off row; the ladder counted exactly the answered wager outcomes and not one write-off. The lie lets one act in a hundred past the cap: the fold peaks at $517k with 172,294 rows over the cap, caught.

**Two corrections found on the way.** The frontier's act (via 3) is the wager and consumes exposure like an unattended act; the first cut checked only the `ACT` verdict and the fold peaked at $18M under a $300k cap. And a learning judge's fresh read moves for a reason that is not the fact — its noise shrinks as its competence learns — so the certificate's disagreement count against a *learning* judge is a reading of the judge's drift, not a defect of the carry; O37 therefore stands a fixed-competence judge behind the port, and the automate below reports the drift as what it is.

**Acceptance (the baseline).** Build 0 warnings. `--selftest` **28 of 28** (85.7 s); lie arms 0–25 each exit 0 with the lied-to oracle's own line PASS; O17 both lies, O28 (21 functions), O25a, O30a both arms (pin `aa7e850b6bdb032f`). `--sim`, `--automate`, `--twin` at the defaults: **every reading identical to D2**; the lines that differ are the tape line (the sim's chain head `01fa6bcaa35c647c` → `bf3dc6680ebf4d42`: the alphabet pin moved and the `HEADER` row carries it; the row count is the same 1,607,033), the pin line and the wall times. Tree 6,148 lines.

**The readings that move, each named.** 500 seats, span 7, demand 1.5, 260 days, warm 180.

*The shared fact* (`--automate --shared --check-shared`, 41.6 s): 39 changes seen by the machine; **2,062 frames moved by the fact alone; 789 carried by the certificate without a read; 1,273 read again — of which the choice changed on 471 and the band on 1,120**. So on this world 38 percent of the frames a shared change touched needed no cognition at all, and of the 62 percent that did, 37 percent changed their decision. The certificate checked against the learning judge: 789 checks, 10 disagreements (1.3 percent) — the judge's drift, per the correction above; 0 against a fixed judge (O37). Reads 1,755 a period (1,708 fresh, 47 floor), 1,487 carried by the memo. The shared world is a different world and its other readings move with it: acted unattended 14,323 (15,672 at D2), licensed decision mass 55.1 percent (63.5), the twin under it good 41.05 vs 37.38, wrong 8.00 vs 11.69, cost $1,838.6M vs $1,258.1M (−580.5). A fifth of every class's decision mass now moves under the deciders' feet; that it costs the machine eight points of licensed mass is the price of a world with change in it, and the honest baseline for E2's archive plane.

*Outstanding exposure, uncapped:* the machine at D2's readings holds **$96.1M of unattended decisions in flight at peak** ($68.7M at the end). Nobody had that number. *The exposure dial* (`--exposure-cap`, no shared fact, no unanswering):

| cap | acted unattended | acts held for exposure | licensed decision mass | N |
|---|---|---|---|---|
| none | 15,672 | 0 | 63.5 % | 150 |
| $100M | 15,672 | 0 (peak $96.1M) | 63.5 % | 150 |
| $50M | 6,431 | 15,761 | 29.4 % | 291 |
| $25M | 2,169 | 78,527 | 0.0 % | 412 |
| $5M | 640 | 173,725 | 0.0 % | 412 |

This is λ ≲ B/(b·L) made visible: **the licence follows the exposure cap.** At ACME's values (b ≈ $15k a wager, L 7–90 days) a cap of $25M throttles the wager below the ladder's floor and nothing licenses in a year; at $50M half the mass licenses; the cap the D2 readings implicitly ran under was above $96M. The levers the review named — smaller commitments, staged execution, faster feedback — are levers on b and L, and this table is where they would show.

*With an unanswering world* (`--unresolved 0.03` with the $5M cap): 3,757 cells written off as unresolved over the year, each a row, priced as late, evidence for nothing; outstanding held at the cap; acted unattended 754. The act loop's zero (the roundtable's finding) now has its hypothesis test's instrument: the sweep over L and b is E1's.

**What E0 did not do.** A sampled shadow; the archive plane (E2: the shared fact here is one typed determinant, not a span with paraphrases and superseded versions); the routing cost per span (E2, once `SPAN`/`WANT` rows exist); per-class exposure caps (one total cap here); the sweep of verdict latency against value for the act loop (E1); a certificate from a model judge (I). The certificate's tolerance (10⁻³) is a constant of this plant's arithmetic and is stated as such.

Hashes (sha256, first 16): core.h `f4926f040acbf32e` · firm.h `a91ca2a90e036bf2` · world.h `0308ae6cd8962efb` · human.h `f68615894272a641` · ledger.h `2e739e7fce127250` · port.h `21ae7be54af91ef4` · gate.h `8305eefd67a6f549` · solver.h `3381079ef92dd95b` · machine.h `3af1f5cc9377c44c` · governor.h `c4235dfd4cf83518` · report.h `3bc31d106165bf4f` · checkpoint.h `1c1fc084cea088ad` · main.cpp `b1c944f3687fc05e` · CMakeLists.txt `0610e3a9a9c0564a`; license.h, tapefile.h, dump.h as at D2. Outputs: `receipts/step-e-2026-09-15/e0/` (41 files, the three cap runs included).

## E3a · the ladder as built, measured: O31 before the repair

*Claude Fable 5.1, 2026-09-15. The first sub-step of E3 under ARCHITECTURE v4.1 §28 and §39.2, and the "before" its receipt requires: the false-promotion rate of the rule as built, measured before a line of it changes. E3a moves no default reading. Built and run under WSL by `tools/run_step.sh step-e-2026-09-15/e3a receipts/step-e-2026-09-15/e0`; raw outputs under `receipts/step-e-2026-09-15/e3a/` (the acceptance run's 34 files and the five O31 readings).*

**What E3a is for.** F23 (v3 §14a item 8) says the e-process as built has no anytime guarantee: its null is a plug-in incumbent rate that moves with every outcome, the process resets on every promotion so the error compounds across rungs, and n₀ stands as a condition inside a test it was never part of. Nobody had the number. E3a builds the instrument that gives it and reads it, so that E3b's repair has a before and an after on the same instrument.

**The instrument** (`world.h`, `BoundaryJudge`; `main.cpp`, `--o31`). A judge the plant holds exactly on a stated null: its direction is the plant's arithmetic at a fixed competence (0.75), so its bands are populated like a real judge's; its choice is the truth with a probability chosen per class-band so that its good rate lands on a target: the incumbent's own rate in the band as the rule's plug-in reads it (`--at bar`, the null of the rule as built), or the rate a delta worse on the failure side, g₀ = 1 − (1 − ḡ)(1 + δ) (`--at g0`). A cell already past its due day cannot be good whatever is chosen, so the coin is spent on on-time reads only, scaled by the late share of the wager's own acted cells, folded from the ledger once a day. It is the plant's judge and reads the plant's ledger; the kernel never sees it. `--shift X` lifts the target above the boundary, which is how the mode is seen to discriminate. Every band past the thin band is admitted to rung 1 as a test (`Governor::admit_all_for_test`, a LICENSE row with 5 on its via byte), the wager runs on every class at once, and the bands a promotion ever took past the test rung are counted, with a Wilson interval against the ladder's α = 1/20; the judge's achieved good rate is printed beside the target it aimed at when each coin was spent, so the distance from the boundary is a reading.

**Two corrections to the instrument on the way, both printed.** The first late-share estimate (over first reads) was dominated by the warm backlog and clipped the coin to certainty for weeks while the fresh arrivals it acted on were on time: nine points above target. The second (over the wager's acted cells, refreshed daily) sits on the target at 200 seats and two to five points above it at 500. So the `--at bar` reading at 500 seats is an upper bound on the rate at the bar (the judge there landed above the incumbent, and some of its promotions are true), and the `--at g0` reading, where the judge sat eight points below the incumbent's rate, is the number to quote.

**The readings** (`o31_500_at_*.txt`, `o31_200_at_*.txt`; 500 seats: warm 180, 260 days, five seeded firms; 200 seats: warm 40, 130 days, three):

| the judge | seats | bands run | promoted past the test rung | rate (Wilson 95%) | the judge's good rate against its target |
|---|---|---|---|---|---|
| a delta worse than the incumbent's rate (`g0`) | 500 | 190 | 26 | **0.137 (0.095 to 0.193)** | 0.263 against 0.239 |
| at the incumbent's own rate (`bar`) | 500 | 190 | 54 | 0.284 (0.225 to 0.352), an upper bound | 0.398 against 0.343 |
| a quarter above g₀ (`--shift 0.25`, the lie's dial) | 500 | 114 | 33 | 0.290 (0.214 to 0.379) | 0.407 against 0.382 |
| a delta worse (`g0`) | 200 | 113 | 17 | 0.150 (0.096 to 0.228) | 0.350 against 0.334 |
| at the incumbent's own rate (`bar`) | 200 | 113 | 20 | 0.177 (0.118 to 0.258) | 0.470 against 0.472 |

**The finding.** The rule as built promotes a judge that is worse than the incumbent by the writ's own tolerance on 13.7 percent of the bands it runs on (interval 9.5 to 19.3), against a stated 5 percent, at 500 seats over a year; at the battery's scale, 15.0 percent. The stated level is exceeded by a factor of about three, on every seed. F23 is no longer a review's argument; it is a reading, and it is the number E3b's repaired rule must bring to its level on the same instrument.

**Also in E3a.** `tools/run_step.sh` runs the lie arms `LIE_JOBS` at a time (default six of the box's sixteen threads; each arm's exit code beside its output; the arms took eight minutes against forty), `--judge-comp X` stands the plant judge at a fixed competence behind the port for the stub sweep of E3b, and the frontier judge is overridable so the boundary judge can stand at the rented mind's port too (a frontier at competence 0.96 would have contaminated the count).

**Acceptance (the baseline reproduced).** Build 0 warnings. `--selftest` **28 of 28** (93.3 s); lie arms 0–25 each exit 0 with the lied-to oracle's own line PASS; O17 both lies, O28 (21 functions), O25a, O30a both arms (pin `aa7e850b6bdb032f`, unmoved: `gate.h` is untouched). `--sim`, `--automate`, `--twin` at the defaults: **every reading identical to E0**; the only differing lines are wall times (the sim's rows per second, the replay's milliseconds, the resident's period, the twin's seconds). Alphabet pin `0x81ebc266`, unmoved: no row type or reason was added. Tree 6,328 lines.

**What E3a did not do.** Change the ladder (E3b); O31 as a battery oracle, which is stated against the repaired rule and lands with it (E3b); O31c with an installment channel (F1b, when the channel exists); the retained stratum (E3b); the calibration harness (E3c).

Hashes (sha256, first 16): world.h `2dcba7c10c4f102f` · governor.h `969ff4d717ccfc14` · main.cpp `50e39ee62546468d`; every other file as at E0. Outputs: `receipts/step-e-2026-09-15/e3a/` (39 files).
