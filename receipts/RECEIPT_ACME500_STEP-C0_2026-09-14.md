# Receipt · ACME-500 v2 step C0 · consumers onto the fold; the machine links without the plant · 2026-09-14

*Claude Opus 5. Implements step C0 as sequenced in `docs/ARCHITECTURE_v2.md` §14a after the step-B review: the fold stops being a shadow and becomes the thing the numbers come from, with every printed reading byte-identical to v1 because only where a value is read from changed. Branch `v2` of `C:\55555\acme500` (repository `github.com/bochen2029-pixel/ACME-500`, branch `v2`). Built and run under WSL from a fresh copy. Raw outputs in `receipts/step-c0-2026-09-14/` (24 run outputs, plus the table the README is regenerated from; the addendum's 25 are under `addendum-prints/`).*

## 0 · What C0 changed

**The types moved.** `Obligation`, `ObState`, `decay`, `TimeLedger` and `HumanStats` left `world.h`/`human.h` for `ledger.h`, which now includes nothing from the plant. `World` is the **Plant**: seed, planted spec, tacit mass, determinant counts, class count, demand scale, and the generators and the grader (`make_world`, `arrivals_today`, `world_arrive`, `det_value`, `full_signal`, `truth_decision`, `observe`, `completeness_from`, `world_settle`). The cells, the open set and `next_id` live in the **Ledger**, which the plant's arm and the hand mutate directly while writing the rows that describe the mutation, and which the fold rebuilds (O14).

**The port (`port.h`).** `Store::frame(oid, cls, mask) → Frame` and `Judge::read(frame, competence, noise_key) → Proposal`, plus `Judge::act_coin`. The plant implements both at the end of `world.h` (`PlantStore`, `PlantJudge`) with exactly the arithmetic v1 computed inline: `completeness_from` behind `Frame::coverage_hat`, `observe()` behind `read`, and the rented mind's coin still drawn from the world's seed behind `act_coin` (F16 is preserved on purpose in C0 so no number moves; C1 keys it on the judge).

**Every consumer reads the Ledger.** `Resident::period(Ledger&, Firm&, Tape&, day, const Store&, Judge&)` and `Resident::grade(Ledger&, …)`; `replay(const Ledger&, …, const Store&, Judge&)`; `mine_invariants(tape, const Ledger&, NC)`; `score_arm(const Ledger&, Firm)`; `residual_of(lad, demand_scale, C)`; `Integrator::commit/unwind(Ledger&, …)`; `Resident::init(NC, NS, demand_scale, …)` never holds a `World`. `machine.h`, `solver.h`, `report.h` and `ledger.h` include neither `world.h` nor `human.h`.

**O17, a build fact.** `world.h` carries `#ifdef ACME_NO_PLANT → #error`. `tools/o17_machine_tu.cpp` includes every kernel header under the flag, instantiates the period, the replay, the residual, the arm score and the invariant miner against a null store and a null judge, and must compile; `tools/o17_lie_tu.cpp` adds one include of `world.h` and must be refused by the guard. `tools/o17.py` runs both and prints the two battery-style lines; CMake test `links_without_plant`.

```
  [PASS] O17  the machine links without the plant: ledger.h, port.h, solver.h, machine.h, report.h compile under -DACME_NO_PLANT
  [PASS] O17  the machine links without the plant: the lie TU (one include of world.h) was refused by the guard   (lie arm)
```

## 1 · Acceptance · the tape is byte-identical, the readings are byte-identical

`--twin`, `--automate` and `--sim` diffed against the v1 readings (`receipts/step-a-2026-09-14/`): **0 differing lines** in twin and sim; **1** in automate, the replay's wall time (13 ms → 14 ms). The sim's chain head is `b01905e496dba40e…`, identical to the step-B remediation's: **C0 changed no row.** Battery **20 of 20**; eighteen lie arms (0–17), each exit 0 with its lied-to oracle's line PASS; O28, O25a and O17 green on both arms.

## 2 · The tree after C0

3,827 lines. Hashes (sha256, first 16): core.h `63728b6de2b48a78` · firm.h `75b625c084626c65` · world.h `1a92cf7d375ccc21` · human.h `c115d72fa01b8c1a` · ledger.h `43e538c3ab80e49e` · port.h `5192b2fe8ef027ea` · solver.h `67c1bf92bc228f4f` · machine.h `60310876b6468075` · report.h `fb352388176efa5b` · main.cpp `502639365e4323fa` · CMakeLists.txt `b114d7c99f61b572` · tools/o17.py `9e7ca1727b97809c` · tools/o17_machine_tu.cpp `3ccf542dd43d4060` · tools/o17_lie_tu.cpp `7d9f011fe005b62d`.

## 3 · What C0 did not do, precisely

- The machine still reads planted values **through the port**: `Frame::coverage_hat` is the plant's completeness for the mask, `Judge::read` is the plant's read physics with the machine's competence, and `act_coin` is the world's seed. C0 moved the reads behind the seam without changing what stands behind it; C1 changes what stands behind it and names every delta.
- `cls_spec(c).decide_frac` (planted, in `core.h`'s class table) is still read by the head (`FT_DFRAC`), by `would_cost` and by the counterparty rule; the `Schema`/`Planted` split of the class table is C1.
- The three prints the review asked for in C0 — minutes by via, the binding reason per band, the frame split into wait and coverage — were not in commit `fa71d23`; they land in the addendum below (§4), same day.
- `STRATUM`, `COUNSEL`, `RECEIPT`, `CORRECTION` are declared and not emitted.
- `Ledger` still keeps `idx_of_oid` as a dense vector sized by the largest oid; a real lane's ids will need a map.

## 4 · Addendum, same day · the prints that attribute (§9.3), as added lines only

Four prints, all folds over the tape, the ledger and the ladder, none an input to anything the machine computes (`report.h` §5). Rebuilt and re-run in full: build with 0 warnings, battery **20 of 20**, eighteen lie arms each exit 0 with the lied-to oracle's own line PASS and no FAIL line, O17/O28/O25a green on both arms. Against the staged C0 outputs: `--selftest` **0** differing lines; `--sim` 3 lines added, 1 timing line changed; `--automate` 59 added, 1 timing line changed; `--twin` 19 added, 1 timing line changed; **no reading line changed**; sim chain head still `b01905e496dba40e`. Outputs in `receipts/step-c0-2026-09-14/addendum-prints/` (25 files). Tree 3,993 lines; changed hashes: ledger.h `f4cb303a541d2cda` (a const `at_oid`), report.h `0b22be1db42f7361`, main.cpp `ede73a17e7fc20fc`; every other file as in §2.

**Minutes by via** (`minutes_by_via`: every human minute on ACT, fetch-on-CONTEXT and MEETING rows, attributed to how the cell it was spent on left the building; printed for both twin arms and for the resident arm of `--automate`). Twin, thousands of minutes:

| | incumbent arm | resident arm |
|---|---|---|
| on cells a person decided (via 0) | 9,411.7 (38.4%) | 8,592.8 (36.8%) |
| on cells the resident acted on (via 1) | — | 21.1 |
| on cells the resident drafted and a person keyed (via 2) | — | 1,815.3 (7.8%) |
| on cells a rented mind acted on (via 3) | — | 299.0 |
| on cells a signer executed (via 4) | — | 582.3 |
| on cells never decided, open at the end | 3,093.7 (12.6%) | 2.2 |
| no cell: glue 7,944.3 + meetings 4,068.3 | 12,012.6 (49.0%) | 12,012.6 (51.5%) |
| all | 24,518.0 | 23,325.3 |

Two readings the arm table above it could not give. First, glue and meeting minutes are **identical in both arms** to the tenth of a minute: the resident bought nothing on the calendar, and the whole 1,192.7k-minute gap between the arms is fetch, frame, decide, commit and transport on cells. Second, **2,717.7k minutes (11.7 percent) of the resident arm's human minutes were spent on cells the machine then closed** — cases people had fetched, framed and weighed (604.1k decide minutes on the via-2 cells alone) before the resident's next period took them. That is the interference mechanism of the twin's bias paragraph, in minutes. The incumbent's 3,093.7k minutes on cells still open at the end is the backlog's other price, beside the $590M the writ charges for it.

**The binding reason per band** (`binding_reason`: the first thing in the ladder's own order that keeps each class-band from one rung wider; at day 259, the end of the default run, which is day 80 of live). Band 0 is the thin band, drafted or rented at every rung, so nothing on the ladder applies to it (24). Of the 48 bands past it: **at-top 10**, **no-history 14** (9 in band 1, 5 in band 2: never admitted, the lever is the determinant the human read and the machine did not), **n_machine 18** (14 in band 1, 4 in band 2: fewer wager outcomes than the floor, the lever is volume), **logE 4** (past the floor, the e-process below 1/α), **kappa 2** (both bands of `collections.chase`, the one class whose licence costs more attention than it saves). 26 of 48 carry the `unlicensable` label (F14), which the ladder never reads; the print shows it as a qualifier, not a reason.

**F21, found by the print, recorded and not fixed here.** 8 of the 10 bands at rung 5 have **fewer wager outcomes than n₀** (`quote.price` 721 against 1,077; `quote.approve.disc` 218 against 1,306; `order.credit.check` 312 against 1,650; `dispute.resolve` 205 against 314; `claim.coverage.det` 554 against 695; `po.match.3way` 615 against 2,223; `spend.approve` 330 against 1,306; `recon.break.clear` 632 against 791). The floor in `Ladder::step` is checked against cumulative `n_machine`, so it is paid once and never per rung, and on a history-admitted band 2 it is n₀/8; §1.4's *terms per rung = n₀/(r·V)* is therefore a promise the code keeps for the first rung only. Whether the fix is n₀ per rung, or a floor that scales with the rung, is a design decision for the operator; C0 moves no number.

**The frame residual, split.** frame 14.3 = **wait 10.8** (a licensable band still below rung 2; moves with time) + **coverage 3.5** (the uninstrumented tail of licensed classes; moves with lane-buys). The lane-buy lever of §1.4 moved licensed mass by one point; the coverage tail it acts on is 3.5 points of decision mass, and the 10.8 it cannot touch waits on the ladder.

**The row histogram.** `--automate`: HOLD 2,363,532 of 4,357,488 rows (54 percent), PROPOSAL 266,202, ACT 487,139, CONTEXT 409,695, EFFECT 125,368, OUTCOME 186,956, ESCALATE 107,149, ARRIVE 206,406, ASSIGN 102,603, DECIDE 79,775, MEETING 22,100, KAPPA 216, LICENSE 86, TICK 260, HEADER 1. `--sim`: HOLD 4,150,074 of 5,673,669 (73 percent). Step D's rule — a HOLD row when the reason changes, not per seat per day — has its denominator.

Not printed, still: terms-per-rung beside each rung claim and the per-class coverage price beside each flatten (§9.3), which wait for the flatten of step E.
