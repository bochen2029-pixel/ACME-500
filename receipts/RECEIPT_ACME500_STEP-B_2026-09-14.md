# Receipt · ACME-500 v2 step B · the tape carries everything, the ledger is a fold · 2026-09-14

*Claude Opus 5. Implements step B of `docs/SPEC-AMENDMENTS_REV2_ONE-PORT-FOUR-WORLDS.md` (seams S2, S14, S5, S1) as restated in `docs/ARCHITECTURE_v2.md` §3 and §14, on branch `v2`. v1 is untouched: tag `v1-step-A`, and the full copy with its 114-file manifest at `C:\55555\acme500_v1_SNAPSHOT_2026-09-14\`. Built and run under WSL from a fresh copy of the tree. Raw outputs in `receipts/step-b-2026-09-14/` (22 files). The acceptance for this step is that every printed reading of the twin, the automate and the sim is identical to v1, and it is.*

## 0 · Baseline

v1 at `4c8fbc7`/`dda1025`: 17 of 17 with fifteen lie arms; the readings of `receipts/step-a-2026-09-14/`.

## 1 · What changed (commits `430b4cd` and this one)

**The record.** `Rec` v1 (36 bytes; `arm` = the run) → `Rec` v2 (40 bytes): `via`, `firm`, `prov`, `ver` added; `arm` is the decider where one exists and the writer elsewhere (`ARM_HUMAN · ARM_MACHINE · ARM_GOVERNOR · ARM_EXECUTOR`), never the run (F18). Every `put` site was re-stamped.

**The fields that never reached the tape (F19).** `ARRIVE` carries `day_due` in `a` and the dependency's oid in `b`; `EFFECT` carries `via`; `OUTCOME` names the decider (seat, arm) and mirrors `via` in `b`; `DECIDE` and `ESCALATE` carry the band; `UNDO` carries the prior state, seat and decision day it restores; `CONTEXT` carries `RF_LOST` when a fetch decayed away; a day that ends mid-case is a `HOLD` with reason 3 carrying the completeness in hand.

**The rows that were never emitted (F17), now emitted.** `HEADER` (mode × 10 + switch, the schema pin, the record version) as the first row of every tape; `TICK` per period with the previous period's row count; `ACT` per priced human act (fetch minutes ride on `CONTEXT`; frame, decide, commit, transport, rework per obligation; glue per seat) with the reader's completeness on the frame and decide acts; `PROPOSAL` before every gate verdict carrying the judge hash (`0x504C4A31`, the plant's read arithmetic) and the completeness estimate; `KAPPA` per class-band per term; `PATCH` with its inverse (old value in `margin`, new in `value`) before every multiverse rollout. `schema_hash()` is BLAKE2b over the authored class table, low 32 bits.

**The ledger as a fold (`include/acme/ledger.h`).** `Ledger::apply(const Rec&)` implements the §3.2 table; `Ledger::fold(tape)` rebuilds every cell, the open set, `next_id`, the header fields and the minute meter from rows alone. `fold_ladder()` in `machine.h` rebuilds every licence's counts from `OUTCOME` rows in tape order.

**Three oracles, each with a lie.**

| oracle | asserts | lie | result |
|---|---|---|---|
| **O14** fold-to-identity | the cold fold of a 70-day machine-arm tape equals the live world field by field (20 fields per cell, the open set, `next_id`, the schema pin, the version), and the folded minute meter equals the live one per class and act | a struct field set with no row (`hops += 1` on one cell) | 58,367 cells from 414,752 rows, 0 field diffs, 0 minute diffs; lie caught (`--lie 15`) |
| **O15** the ladder is a fold | every licence's `n_machine`, `n_incumbent`, `n_assisted` and their good counts rebuilt from `OUTCOME` rows equal the live ladder's | an outcome counted twice | 72 class-bands, 0 diffs; lie caught (`--lie 16`) |
| **O25** the kernel reads no clock | `machine.h`, `solver.h`, `ledger.h`, `report.h` contain no `chrono`, `time(`, `clock(`, `getenv(` read (`tools/dead_symbols.py --o25`) | a planted `steady_clock` read in a scratch copy | green; lie caught |

O14 earned its keep before it was green: its first two runs found three fold rules I had written wrong (a blocked or no-attention hold is not a touch; the mid-case hold carries the margin; the resident's escalation moves nothing but the state). Each was a defect in the fold, not in the tape, and each would have been invisible without the oracle.

## 2 · The battery after step B

`./acme --selftest` exit 0, **19 passed, 0 failed**, 13.4 s. Seventeen lie arms (0–16), each exit 0 with the lied-to oracle's own line PASS. `dead_symbols.py`: O28 and O25 green on both arms. CMake: `lie_0`…`lie_16`, `dead_symbols`, `dead_symbols_lie`, `no_clock`, `no_clock_lie`.

## 3 · Acceptance · every reading identical

`--twin`, `--automate` and `--sim` diffed line by line against `receipts/step-a-2026-09-14/`, excusing only timing lines and the tape line: **0 differing lines in each**. The tape itself grew as the table says it should — the sim from 5,109,122 to 5,673,669 rows (`TICK`, `ACT`, `PROPOSAL`, `HEADER`, `KAPPA`) — and its chain head changed with the record layout (`c8d8fd36…` → `1c8fe124…`); O1 row count 161,900 → 219,432; O1b 285,623 → 414,752.

## 4 · The tree after step B

3,634 lines (v1: 3,269). Hashes (sha256, first 16): core.h `a2a7665c71af12e5` · world.h `d2627cf12cd7e7a5` · human.h `160eb0f6c2a6a2ef` · solver.h `d428a5b3586ded49` · machine.h `7e96a8feb6afa25b` · ledger.h `7eeecaf67ec2ff42` · main.cpp `247d5d1256d77552` · CMakeLists.txt `e38fec7703e4119c` · tools/dead_symbols.py `77770958ae2a18a2`; firm.h and report.h unchanged.

## 5 · What was not done

The `World` struct still holds `ob` and `open_idx` beside the planted truth; the machine still reads them directly and still calls `observe()` on the plant. Step B proved the ledger *is* a fold (O14) without yet moving consumers onto the folded object; the physical split into `Plant` / `Ledger` / `Store` and the judge port are step C, where numbers are allowed to move and O16/O17/O21 gate them. The `STRATUM`, `COUNSEL`, `RECEIPT` and `CORRECTION` row types are declared and not yet emitted (steps C, E, I). The minutes-by-via, binding-reason and frame-split prints are step C's, once the ledger is the machine's input.
