# Receipt · ACME-500 REV 2 step A · determinism over the machine arm, the dead physics deleted · 2026-09-14

*Claude Opus 5. Implements step A of `docs/SPEC-AMENDMENTS_REV2_ONE-PORT-FOUR-WORLDS.md` (seams S0 and S10b). Built and run under WSL Ubuntu-24.04 at `/root/acme500` from a fresh copy of the Windows tree. Raw outputs in `receipts/step-a-2026-09-14/`. The acceptance for this step is that every printed number in the twin, the automate and the sim reads identically to the spec's §0, and it does.*

## 0 · Baseline reproduced before editing

From the tree at `4d02132`: all nine source hashes matched the spec's §0 to the byte; `--selftest` 16 of 16, exit 0; fourteen lie arms exit 0 with the lied-to oracle's line PASS; the twin, automate and sim readings matched §0 to the digit (twin Δ total −589.76 $M, bias +31.73, licensed mass 63.7 percent, N 193 / 181, coverage 0.913 vs 0.889, kappa 0.242, 12,122 / 77,874 wager, 95,963 assisted).

## 1 · S10b · the dead physics deleted (F15)

`Decider`, `p_good` and `realize_outcome` were removed from `firm.h`. The section banner now says where the physics lives (`world.h`: `full_signal` / `truth_decision`, `observe`, `world_settle`) and what stood there. `README.md`'s "one dynamics source" bullet and its layout line, and `docs/DESIGN.md`'s layout line, were rewritten to describe the grader the code uses.

**O28 · no dead physics** was added as `tools/dead_symbols.py`: it lists every free function defined at column 0 in `firm.h` and `world.h` with zero call sites outside its own definition line, and its `--lie` plants `acme_planted_orphan` in a scratch copy and passes only if it catches it and nothing else is dead. Its first run found two more callers-of-nothing beyond F15: `seat_kind_name` (`firm.h:49`) and `outcome_name` (`firm.h:294`), both name-table helpers nothing printed. Both were deleted; they return the day something calls them.

```
  [PASS] O28  no dead physics: every free function in firm.h/world.h has a caller (13 functions)
  [PASS] O28  no dead physics: the planted orphan was caught, and nothing else is dead   (lie arm)
```

CMake runs both as `dead_symbols` and `dead_symbols_lie` when a Python 3 interpreter is found.

## 2 · S0 · determinism over the machine arm (F20)

O1 ran the human arm only and compared `rec` without `chain`. **O1b** runs `--automate`'s `run_machine` twice on one seed (200 seats, span 7, seed 7, warm 40, days 70) and requires the tape records, the BLAKE2b chain and the licence table (every `Lic` field-wise) to be identical. Its lie, `--lie 14`, is a resident that read the clock: the first `EFFECT` row's margin of the second run is perturbed by a `steady_clock`-derived amount after the run, the same post-hoc style as O1's own lie.

```
  [PASS] O1b  same seed produces a byte-identical MACHINE arm: tape, chain, licence table (285623 rows; rec same, chain same, licence table same)
  [PASS] O1b  ... (285623 rows; rec DIFF, chain same, licence table same)     under --lie 14: caught
```

Cost: `--selftest` wall time 5 s → 11.4 s.

## 3 · The battery after step A

`./acme --selftest` exit 0, **17 passed, 0 failed**. Fifteen lie arms, each with its lied-to oracle's line checked:

```
lie  0 O0c PASS   lie  4 O4  PASS   lie  8 O8  PASS   lie 12 O12 PASS
lie  1 O1  PASS   lie  5 O5  PASS   lie  9 O9  PASS   lie 13 O13 PASS
lie  2 O2  PASS   lie  6 O6  PASS   lie 10 O10 PASS   lie 14 O1b PASS
lie  3 O3  PASS   lie  7 O7  PASS   lie 11 O11 PASS
```

CMake: `lie_0` … `lie_14` with `LIE_LINE_14 = O1b`, plus `dead_symbols` and `dead_symbols_lie`.

## 4 · Acceptance · every number identical

`--twin`, `--automate` and `--sim` were diffed line by line against the honest-instrument receipt's outputs. Every differing line is a timing (`14 ms` → `13 ms`; `3.7 s, 1376172 rows/s` → `2.4 s, 2168297 rows/s`) or the trailing blank line the receipt's `time` wrapper left. The sim's chain head is unchanged at `c8d8fd363b282e76…`, so the tape is byte-identical after the deletions, which is what "no caller" must mean.

## 5 · The tree after step A

3,269 lines. Hashes (sha256, first 16): firm.h `75b625c084626c65` · main.cpp `f11149abb7402e80` · CMakeLists.txt `9cd072ef5512dc25` · tools/dead_symbols.py `f533364c55dd26bd`; core.h, world.h, human.h, solver.h, machine.h, report.h unchanged from §0.

## 6 · What was not done

Nothing beyond S0 and S10b. No row, struct, physics constant or oracle threshold changed. Step B (S2, S14, S5, S1: `Rec` v2, the missing fields on the tape, the tick as a row, the minute meter as rows, the Plant/Ledger/Store split, O14 and O15) is next, and its acceptance is the same: every printed number identical to §0.
