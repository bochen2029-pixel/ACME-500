# Receipt · ACME-500 first run · 2026-09-13

*Claude Opus 5. Source: `C:\Users\user\Downloads\acme500.zip` (identical to the flat copy and the two-part bundle in `files (1).zip`; every file compared byte for byte). Built and run under WSL Ubuntu-24.04 at `/root/acme500`. Every number below carries the command that printed it. The README said the four modes and the fourteen lie arms had never been run; they have now.*

## 1 · Build

```
g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme
```
Zero warnings. 5.3 s. Binary 181,944 bytes. 3,180 source lines, as the README states.

## 2 · The oracle battery

```
./acme --selftest        exit=1        5.3 s
```
15 passed, 1 failed. Every reading matches the README's table to the printed digit:

| oracle | reading |
|---|---|
| O0a BLAKE2b("abc") | PASS |
| O0b/c chain, flipped field localises | PASS |
| O1 same seed, byte-identical tape | PASS, 161,900 rows |
| O2 gate never widens | PASS, 8,640 lattice points, 390 reach ACT |
| O3 no budget degrades to HOLD | PASS |
| O4 transport conserves | PASS, overshoot 0, placed+stock err 1.3e−8 |
| O5 binding capacity has a price | PASS, seat 29.66, unplaced stock 28.02 |
| O6 join graph from the query log | PASS, 24 of 24 |
| **O7 coverage tracks tacit mass** | **FAIL, mean abs err 0.071, corr −0.052 over 19 classes** |
| O8 the fourth cell licenses nothing | PASS, 2,193 cells available, 21,592 credited = 21,592 agreements |
| O9 competence recovered from outcomes | PASS, within-class corr +0.241 over 9 classes, 209 pairs |
| O10 kappa demotes | PASS, 2.25 → rung 1 |
| O11 inverses, window closes at settlement | PASS, 60 committed and unwound |
| O12 origin-fit cascade understates F | PASS, α 0.145 vs 0.197 planted, F 17.6 vs 9.0 |
| O13 decide fraction ranks planted intensity | PASS, partial Spearman 0.963 over 24 |

## 3 · The fourteen lie arms, run for the first time

```
for n in 0..13: ./acme --selftest --lie $n
```

| lie | exit | oracles printing FAIL |
|---|---|---|
| 0 | 1 | O7 |
| 1 | 1 | O7 |
| 2 | 1 | O7 |
| 3 | 1 | O7 |
| 4 | 1 | O7 |
| 5 | 1 | O7 |
| **6** | 1 | **O6**, O7 |
| **7** | **0** | none |
| 8 | 1 | O7 |
| 9 | 1 | O7 |
| 10 | 1 | O7 |
| 11 | 1 | O7 |
| 12 | 1 | O7 |
| 13 | 1 | O7 |

How to read it. In the code, a lied-to oracle prints PASS when it catches its lie: every check is `ck(LIE == N ? !ok : ok, …)` (`main.cpp` lines 461, 473, 499, 514, 535, 552, 566, 588, 607, 656, 669, 693, 705, 746). So a healthy lie run prints all PASS and exits 0. The harness says the opposite: `CMakeLists.txt` marks every lie test `WILL_FAIL TRUE`, and the battery's last line prints "it must appear as one of the failures." Three consequences:

- **Twelve lie arms caught their lie** (0 to 5, 8 to 13): the lied-to oracle printed PASS. Those are real negative controls, now seen to work.
- **Every lie run exits 1 for the wrong reason**: O7 is red in the baseline, so the exit code never depends on the lie. Under CTest all fourteen lie tests would show green today, including the one below, because `WILL_FAIL` is satisfied by O7 and not by the lie. The program built to refuse certification-as-truth has a green CI on a broken lie arm.
- **Lie 7 is uninformative** while O7 fails on its own: the lied-to oracle "catches" the lie for the same reason it fails the baseline.

## 4 · Findings from the run, each with its line

**F1 · O6's lie is planted in a field O6 never reads.** The lie sets `C.join_graph[3] = 1u` (`main.cpp:560`); the oracle compares `C.n_systems[c]` to the schema (`main.cpp:563`); `n_systems` was computed inside `compile_from_tape` from the uncorrupted mask (`machine.h:87`) before the lie landed. The lie is invisible, O6 passes its own lie, and prints FAIL under the code's convention. One-line fix: recompute `n_systems` from `join_graph` inside the oracle, or corrupt `n_systems`.

**F2 · The incumbent's band is never assigned, so the wager is graded against a constant.** `o.band` is written only in `Integrator::commit` (`solver.h:330`); `human_work` never sets it; `Resident::grade` reads `o.band` for every settled obligation (`machine.h:551`), so every human outcome lands in band 0. `p_incumbent(c, b)` for bands 1 and 2 then returns the 0.72 default (`machine.h:206`, `n_incumbent < 12`). The ladder table confirms it empirically: `good_h` reads 0.000 for every class at band 2. The paired comparison the README describes is not paired in the code; the e-process compares the machine's band-2 rate against 0.72 for every class.

**F3 · The twin's headline omits the incumbent's backlog.** `score_arm` skips every obligation not `OB_SETTLED` (`report.h:133`). At the end of the twin the incumbent holds 123,927 open obligations and the resident 19,144. The writ prices a stock (`w_unplaced = 0.6`, `firm.h:88`) and the solver charges it (`machine.h:419`), but the arm score does not. The printed true effect, −4.41 points of good-outcome rate and +$591M writ cost, is computed on a denominator that excludes 124,000 unfinished obligations on one side. Priced at `w_unplaced × value`, the incumbent's backlog is on the order of $1 to 2 billion, which reverses the sign of the cost delta. The report and the solver disagree on the cost function, which is the disagreement `solver.h` says the callback design exists to prevent.

**F4 · The wager ran on drafts, not on a canary.** Over 80 live days: acted unattended 18,840, of which canary 402; drafted 96,635; the counter on the wall reads 111,784 machine-chosen outcomes. `V_DRAFT` commits the machine's choice with `by_machine = 1` (`machine.h:509`, `solver.h:330`), so drafts are the wager. The identifiability floor r ≥ n₀/V governed 402 instances; the machine's choices were executed on about 60 percent of settled instances through DRAFT. That is also the mechanism behind the twin's −10.85 point estimator bias: the "canary arm" was the majority of the work, so selection and interference were maximal, not marginal. The README's sentence "the compression case licenses in weeks" is achieved by drafts at rate one, not by canaries at the floor.

**F5 · Novelty is mostly lateness.** The conformal score is `|completeness − 0.7| + 0.3 · |lateness in weeks|` (`machine.h:447`). At demand 1.5 the backlog is many weeks late, so 60,221 gate decisions went to warrant seats as `novel-case`, and 44,403 more were held for `no-adjudication-budget`. The safety property held (budget checked last, holds not acts). The vocabulary mislabels a late case as an unfamiliar one.

**F6 · The operating point is a firm that is drowning.** At demand 1.5 the incumbent settles 82,479 obligations in 260 days with 42.7 percent good, 48.9 percent late, 8.4 percent wrong, mean cycle 51.9 days, 54.2 percent of deadlines breached, and 123,927 still open. Both arms are graded mostly on lateness. The README's accuracy headline, human 92.4 against machine 92.1, is not what this run shows: correctness (one minus wrong) is 91.6 percent for the incumbent and 88.3 percent for the resident, 3.3 points worse, on the twin's own settled sets.

**F7 · The cascade headline rests on F2 and F4.** `--automate` prints licensed decision mass 91.8 percent, E falling from 412 to 34, N = 57 with the honest fit and 41 with the naive one. That fraction comes from `residual_of` counting a class licensed if any band reached rung 2 (`report.h:99`); rungs climbed against a constant baseline (F2) on a wager that was really drafts (F4). Do not cite 500 → 57 until both are fixed and the twin is re-run.

**F8 · All 51 invariants held trivially.** The simulator enforces no-double-decide and dependency order by construction, so invariant mining has no negative instance in this world. It is not tested here.

**F9 · README defaults differ from the binary.** `--automate` defaults to 260 days (`Args`), the README says 400. The run above was 180 warm + 80 live.

**What stands.** Determinism to the byte over 161,900 rows; the keyed chain localising one flipped field; the gate lattice never widening in 8,640 points; transport conservation to 1e−8; finite stock prices; the join graph recovered 24 of 24 from the query log; the fourth cell refused; competence recovered from outcomes at +0.241 where no human in the firm can see it; kappa demotion; inverses with the window closing at settlement; the origin-fit cascade understating F; the decide fraction ranked at 0.963. Twelve of those thirteen have a negative control that was seen to fire today. Those are receipts.

## 5 · The modes

```
./acme --sim          exit=0   4.3 s    5,109,122 rows, chain OK, 1.64M rows/s
./acme --automate     exit=0   37.4 s   3,504,996 rows, chain VERIFIED, resident period 440 ms × 80
./acme --twin         exit=0   37.5 s
./acme --multiverse   exit=0   44m20s   see 5a
```

Sim, the ledger at 500 seats, span 7, demand 1.5, 260 days, 24,518 thousand minutes: glue 32.4, meeting 16.6, fetch 16.3, frame 8.4, decide 15.9, commit 6.9, transport 3.2, rework 0.3 percent. The decide fraction is 15.9 percent. 38,089 escalations, 0.44 per decision; 21,914 meetings; 4,117,431 holds recorded.

Automate, phase by phase: join graph 24 of 24 exactly; 51 invariants proposed, 51 held; replay re-decided 65,773 settled obligations in 13 ms; four-cell table 32,682 agreed-good, 25,844 agreed-not-good, 2,114 disagreed-firm-good, 5,133 disagreed-firm-bad; agreement-band mass 89.0 percent; estimated mean coverage 0.936 against planted 0.889, error +0.047; 34 class-bands licensed to rung 1 by history; 21 class-bands past rung 0 after 80 live days; 26 bands unlicensable; kappa 0.233 (2,326k minutes caused, 9,989k removed); machine-chosen outcomes 111,784 against n₀ 77,874.

Twin, paired arms on one seed: settled 82,479 vs 187,262; good 42.70 vs 38.29 percent; wrong 8.40 vs 11.67; late 48.90 vs 50.04; breaches 54.17 vs 57.93; cycle 51.94 vs 58.03 days; writ cost $236.73M vs $827.63M; human minutes 24,518k vs 22,846k; open at end 123,927 vs 19,144. Estimator bias −10.85 points (true −4.41, within-firm canary estimate −15.26). See F3 and F4 before reading any of those as a verdict on the method.

## 5a · The multiverse, landed

```
./acme --multiverse     exit=0    real 44m20s, user 44m14s    10 patches × 6 rollouts × 200 days, 2,660 s
```

Ranked by the pessimistic cost J + 0.5σ, in $M over 200 days, writ cost plus salary plus frontier calls:

| rank | J mean | sd | J pess | weight | patch |
|---|---|---|---|---|---|
| 0 | 494.41 | 13.63 | 501.23 | 0.9647 | buy the lane on claim.coverage |
| 1 | 501.42 | 16.51 | 509.67 | 0.0142 | buy the lane on dispute.resolve |
| 2 | 501.73 | 17.94 | 510.70 | 0.0085 | buy the lane on fraud.refer |
| 3 | 503.53 | 18.84 | 512.95 | 0.0027 | thin margin 0.90, act later |
| 4 | 503.73 | 18.82 | 513.14 | 0.0025 | span 10, flatter cone |
| 5 | 504.19 | 18.18 | 513.28 | 0.0023 | span 5, deeper cone |
| 6 | 504.66 | 18.24 | 513.78 | 0.0018 | baseline, do nothing |
| 7 | 504.66 | 18.24 | 513.78 | 0.0018 | eps floor 0.10, explore more |
| 8 | 505.36 | 18.56 | 514.64 | 0.0012 | kappa cap 0.60, stricter brake |
| 9 | 508.47 | 18.14 | 517.55 | 0.0003 | thin margin 0.30, act sooner |

What it says. The three patches that buy a lane, moving a class's tacit determinants into a system of record, occupy the top three places; the best beats doing nothing by $10.2M on a spread of about $7M over six rollouts, so about one and a half standard errors, not a verdict. Acting sooner on thin margins is the worst patch and acting later is better than baseline, which is the shape F6 predicts for a machine that is slightly worse on correctness. The two span patches sit inside noise of the baseline.

**F10 · A writ parameter that nothing reads.** The "explore more" patch reproduces the baseline to every printed digit: 504.66, 18.24, 513.78. `Writ::eps_floor` is declared (`firm.h:92`), set by `eval_patch` (`main.cpp:370`), and consumed nowhere; the canary rate comes from n₀ over arrivals and the audit rate from 1/√(3F), neither of which reads it. The comment beside it, "a floor the kernel may raise, never lower," describes a floor that does not exist in the code. The multiverse measured the absence exactly, which is the right way to find it.

**How the first "run" of this mode went, for the record.** My first launch, a `nohup … &` inside a `wsl --exec` session, died with that session; no process, no file. I then reported it "still running" twice on the strength of `pgrep -c -f "acme --multiverse"` returning 1, which was the command line of the shell I ran the check from. A background wait loop polled for a file that would never appear for six and a half hours. The second launch was verified by the binary's own PID, CPU time equal to wall time, and one saturated core in Task Manager, before anything was claimed.

## 6 · What was not done

O7 was not repaired. No source was edited. No number here has met a real firm; the world is synthetic with planted truth and tests the instrument.

## 7 · The order of repair, for whoever picks it up

1. Pick one lie-arm convention. The code's is the better one: a lie run exits 0 if and only if the lied-to oracle caught its lie and everything else passed; CMake then drops `WILL_FAIL` and asserts the lied oracle's own line instead. Fix O6's lie (F1). Until O7 is green, lie 7 proves nothing.
2. Assign the incumbent's band from its own margin at decide time (F2), so `p_incumbent` is the firm's rate in that band.
3. Charge open obligations in `score_arm` at `w_unplaced × value` (F3), so the report and the solver price the same thing.
4. Decide what a draft is (F4). If a draft is a wager at rate one, say so and drop the canary arithmetic for rung 1; if the canary rate is meant to govern, gate drafts at the floor rate too.
5. Take lateness out of the novelty score (F5).
6. Re-run twin and automate. Expect the headline sign to move.
7. Then O7.
