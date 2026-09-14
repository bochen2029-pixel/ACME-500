# Receipt · ACME-500 v2 step B · the tape carries everything; the ledger is a verified shadow of the world · 2026-09-14

*Claude Opus 5. Implements step B of `docs/SPEC-AMENDMENTS_REV2_ONE-PORT-FOUR-WORLDS.md` (seams S2, S14, S5, S1) as restated in `docs/ARCHITECTURE_v2.md` §3 and §14, on branch `v2` of `C:\55555\acme500` (repository `github.com/bochen2029-pixel/ACME-500`, not yet pushed). v1 is untouched: tag `v1-step-A`, and the full copy with its 114-file manifest at `C:\55555\acme500_v1_SNAPSHOT_2026-09-14\`. Built and run under WSL from a fresh copy of the tree. Raw outputs in `receipts/step-b-2026-09-14/` (22 files) and, after the review of the same day, `receipts/step-b-remediation-2026-09-14/`. The acceptance for this step is that every printed reading of the twin, the automate and the sim is identical to v1, and it is.*

## 0a · Precision, after review (2026-09-14, the paper session's critique)

Four sentences in the first version of this receipt claimed more than landed, and the review was right on each:

- **"The ledger is a fold."** Not yet. `ledger.h` rebuilds every cell from rows and O14 proves the rebuild equals the live world; but no consumer reads the rebuilt ledger. The machine, the report and the invariant miner still read `World::ob`. **The ledger is a verified shadow of the world; the numbers do not yet come from it.** That is why "readings byte-identical" was cheap, and it is what step C0 changes. One exception landed with the remediation: the compile step's minute meter is now folded from `ACT`, `CONTEXT` and `MEETING` rows (`Ledger::fold_minutes`), and `HumanStats` is no longer an input to anything the machine computes; O13 ranks identically from rows.
- **"The clock is a row."** `TICK` is emitted per period by the `for` loop in `main`; the loop is not yet driven by it. The inversion (drain intake, fold a `TICK`, run `period()`) belongs to step D with the durable tape.
- **"O25 proves the kernel reads no clock."** What landed is a source scan of four files for `chrono` / `time(` / `clock(` / `getenv(` with a planted-read lie. It is renamed **O25a**. The spec's O25 is dynamic (the same rows at different wall spacing give identical verdict rows) and cannot run until the loop is TICK-driven.
- **`completeness` in the folded cell.** The fold reads rows only and never calls the plant: for a human decision the value rides on the `ACT` frame and decide rows (`margin`) and on the mid-case `HOLD` (`value`); for the machine it is `PROPOSAL.value`, carried onto the `EFFECT`. The *value* on those rows is still the plant's `completeness_from` (planted weights) at write time; a real firm's tape cannot carry it. In step C1 the cell field becomes `completeness_hat` from the frame's coverage estimate and the plant's value moves to the twin's grader side only.

Also stated for the record: `PROPOSAL.b` carries a placeholder judge hash (`0x504C4A31`, the plant's read arithmetic) until the judge port exists in C1, where O16 requires it to vary; `R_CORRECTION` is declared in `core.h`'s v2 enum with its payload so the alphabet pin already covers it; and `alphabet_hash()` (§1 below) replaced the class-table-only `schema_hash()` in the HEADER row.

**Tape volume, for step D:** 80 percent of rows are per-seat-per-day `HOLD`s, ~385 MB per simulated year at 72 bytes with the chain. The durable tape should write a hold when the reason changes, not every day, with O14 still holding across the change.

## 0 · Baseline

v1 at `4c8fbc7`/`dda1025`: 17 of 17 with fifteen lie arms; the readings of `receipts/step-a-2026-09-14/`.

## 1 · What changed (commits `430b4cd` and this one)

**The record.** `Rec` v1 (36 bytes; `arm` = the run) → `Rec` v2 (40 bytes): `via`, `firm`, `prov`, `ver` added; `arm` is the decider where one exists and the writer elsewhere (`ARM_HUMAN · ARM_MACHINE · ARM_GOVERNOR · ARM_EXECUTOR`), never the run (F18). Every `put` site was re-stamped.

**The fields that never reached the tape (F19).** `ARRIVE` carries `day_due` in `a` and the dependency's oid in `b`; `EFFECT` carries `via`; `OUTCOME` names the decider (seat, arm) and mirrors `via` in `b`; `DECIDE` and `ESCALATE` carry the band; `UNDO` carries the prior state, seat and decision day it restores; `CONTEXT` carries `RF_LOST` when a fetch decayed away; a day that ends mid-case is a `HOLD` with reason 3 carrying the completeness in hand.

**The alphabet pin (remediation).** `alphabet_hash()` in `solver.h` hashes the record version, every record type by name, every verb, every refusal reason, every act kind, the provenance count, the band count, the outcome kinds and the authored class table (`schema_hash()`); adding a reason or a row type changes the HEADER row's pin, which is the widening the pin exists to make loud. O14 asserts the folded pin equals the binary's.

**The rows that were never emitted (F17), now emitted.** `HEADER` (mode × 10 + switch, the alphabet pin, the record version) as the first row of every tape; `TICK` per period with the previous period's row count; `ACT` per priced human act (fetch minutes ride on `CONTEXT`; frame, decide, commit, transport, rework per obligation; glue per seat) with the reader's completeness on the frame and decide acts; `PROPOSAL` before every gate verdict carrying the judge hash (`0x504C4A31`, the plant's read arithmetic) and the completeness estimate; `KAPPA` per class-band per term; `PATCH` with its inverse (old value in `margin`, new in `value`) before every multiverse rollout. `schema_hash()` is BLAKE2b over the authored class table, low 32 bits.

**The ledger as a fold (`include/acme/ledger.h`).** `Ledger::apply(const Rec&)` implements the §3.2 table; `Ledger::fold(tape)` rebuilds every cell, the open set, `next_id`, the header fields and the minute meter from rows alone. `fold_ladder()` in `machine.h` rebuilds every licence's counts from `OUTCOME` rows in tape order.

**Three oracles, each with a lie.**

| oracle | asserts | lie | result |
|---|---|---|---|
| **O14** fold-to-identity | the cold fold of a 70-day machine-arm tape equals the live world field by field (20 fields per cell, the open set, `next_id`, the schema pin, the version), and the folded minute meter equals the live one per class and act | a struct field set with no row (`hops += 1` on one cell) | 58,367 cells from 414,752 rows, 0 field diffs, 0 minute diffs; lie caught (`--lie 15`) |
| **O15** the ladder is a fold | every licence's `n_machine`, `n_incumbent`, `n_assisted` and their good counts rebuilt from `OUTCOME` rows equal the live ladder's | an outcome counted twice | 72 class-bands, 0 diffs; lie caught (`--lie 16`) |
| **O25a** the kernel reads no clock (source scan) | `machine.h`, `solver.h`, `ledger.h`, `report.h` contain no `chrono`, `time(`, `clock(`, `getenv(` read (`tools/dead_symbols.py --o25`) | a planted `steady_clock` read in a scratch copy | green; lie caught |
| **O29** row-shape conformance (remediation) | one scan of the tape; per type, the fields the v2 table says are populated are populated: `ARRIVE.a` a due day, `EFFECT.via` in 1–4, `OUTCOME` names a decider with kind and via, `PROPOSAL.b` a judge hash, `prov` H or M by writer, governor rows by the governor, exactly one `HEADER` first with the right pin, `ver` = 2 everywhere | an `ARRIVE` written with no due day | 414,752 rows, 0 faults; lie caught (`--lie 17`) |

O14 earned its keep before it was green: its first two runs found three fold rules I had written wrong (a blocked or no-attention hold is not a touch; the mid-case hold carries the margin; the resident's escalation moves nothing but the state). Each was a defect in the fold, not in the tape, and each would have been invisible without the oracle.

## 2 · The battery after step B and its remediation

`./acme --selftest` exit 0, **20 passed, 0 failed**. Eighteen lie arms (0–17), each exit 0 with the lied-to oracle's own line PASS (`receipts/step-b-remediation-2026-09-14/lie-*.txt`). `dead_symbols.py`: O28 and O25a green on both arms. CMake: `lie_0`…`lie_17`, `dead_symbols`, `dead_symbols_lie`, `no_clock`, `no_clock_lie`. The README's oracle table is now generated from the run by `tools/oracle_table.py`.

## 3 · Acceptance · every reading identical

`--twin`, `--automate` and `--sim` diffed line by line against `receipts/step-a-2026-09-14/`, excusing only timing lines and the tape line: **0 differing lines in each**. The tape itself grew as the table says it should — the sim from 5,109,122 to 5,673,669 rows (`TICK`, `ACT`, `PROPOSAL`, `HEADER`, `KAPPA`) — and its chain head changed with the record layout (`c8d8fd36…` → `1c8fe124…`); O1 row count 161,900 → 219,432; O1b 285,623 → 414,752.

## 4 · The tree after step B

3,634 lines (v1: 3,269). Hashes (sha256, first 16): core.h `a2a7665c71af12e5` · world.h `d2627cf12cd7e7a5` · human.h `160eb0f6c2a6a2ef` · solver.h `d428a5b3586ded49` · machine.h `7e96a8feb6afa25b` · ledger.h `7eeecaf67ec2ff42` · main.cpp `247d5d1256d77552` · CMakeLists.txt `e38fec7703e4119c` · tools/dead_symbols.py `77770958ae2a18a2`; firm.h and report.h unchanged.

## 5 · What was not done, and how step C is now sequenced

The `World` struct still holds `ob` and `open_idx` beside the planted truth; the machine, the report and the invariant miner still read them directly, and the machine still calls `observe()` on the plant. `ledger.h` still includes `world.h` for the `Obligation` and `World` types, so the fold is not yet linkable without the plant. The `STRATUM`, `COUNSEL`, `RECEIPT` and `CORRECTION` row types are declared and not yet emitted (steps C, E, I).

Step C is split in two, per the review, so that the step allowed to move numbers is the only one that moves them:

- **C0 — consumers onto the fold; numbers identical.** `Obligation` and the ledger leave `world.h`; the machine, the report, the compile step and the invariant miner read the `Ledger`; the `Plant` keeps only the generators and the grader; the machine's translation unit builds under `-DACME_NO_PLANT` (O17). Acceptance: every printed reading byte-identical to v1, because only where a value is read from changes. Any drift here is a fold bug, and C0 is where it is caught.
- **C1 — the judge port; numbers move.** `PlantJudge` on its own noise key; the read budget with the memo keyed on the promise's own rows, the template pin and span contents, never the clock; planted `decide_frac` and completeness off the machine's path (`completeness_hat`); the governor's translation unit writing `STRATUM`; `template_hash` in the licence key; the forgone-dual print (the sum of the shadow prices of the cells the read budget did not reach); O16, O21. Every delta named against C0 and attributable to the judge key and the budget alone.

The minutes-by-via, binding-reason and frame-split prints land in C0, once the ledger is the machine's input.
