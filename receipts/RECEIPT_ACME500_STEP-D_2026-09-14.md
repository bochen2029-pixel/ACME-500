# Receipt · ACME-500 v2 step D · the tape becomes a store · 2026-09-14

*Claude Opus 5. Implements step D as planned in `docs/ARCHITECTURE_v2.md` §14b, in four sub-steps, each landed with its own battery, twenty lie arms, the tool oracles, the three modes diffed against C1, a commit and a push. Acceptance for the whole step: a kill survives (O18), and every reading identical to C1 under `live`. Branch `v2` of `C:\55555\acme500` (`github.com/bochen2029-pixel/ACME-500`). Built and run under WSL from a fresh copy; raw outputs under `receipts/step-d-2026-09-14/<sub-step>/`.*

## D0 · rows on change; the TICK-driven loop

**What changed.** A person's hold (reasons 1 blocked and 2 no attention) is written when the hold in force changes — the seat or the reason — and not per seat per day; any touch of the cell (a fetch, an act, a mid-case hold, a handoff, a decision, an effect) ends the hold in force, so the next hold is a change and a row. The resident's hold likewise on a change of (reason, band), ended by an effect or an undo. A `STRATUM` row is written when the draw in force changes (kind, canary rate or audit rate) and stands until then. The hold in force and the draw in force are folded fields (`Obligation::hold_reason/hold_seat/mhold_reason/mhold_band`; the ledger's per-cell stratum kind and both rates), compared by O14. The meters count what they counted before (`holds recorded`, `held (a row, with its margin)` are per cell-day, row or not), so no reading moves. Reason-3 holds, a day that ends mid-case, carry state and stay per occurrence.

**The loop is the world port's.** Intake lands (`ARRIVE` rows), a `TICK` is folded (`tick_fold`: the row is written and the ledger's day moves with it), the governor draws, the resident runs its period **on the day the ledger says** — `Resident::period` and `grade` no longer take a day; the governor's `draw_strata` reads the ledger's — then the incumbent works, the world settles, the governor folds the outcomes and steps the ladder. The `TICK` row's value is wall seconds since the last tick, written 0 by the sim and never read by the machine; the dynamic O25 (D3) varies it.

**Acceptance.** Build 0 warnings. `--selftest` **22 of 22** (19.2 s); lie arms 0–19 each exit 0 with the lied-to oracle's own line PASS; O17 both lies, O28 (18 functions), O25a green. **Every reading identical to C1** in `--sim`, `--automate` and `--twin`: the only lines that differ are the tape line, the row histogram and the wall times. O14 holds across the change (58,367 cells, 0 field diffs, the hold in force and the draw in force included); O21 unchanged at χ² 72.2 on 72 cells (the draw in force is read from its row until the next).

**What the tape lost.**

| run | rows, C1 | rows, D0 | HOLD | STRATUM | chain head |
|---|---|---|---|---|---|
| `--sim` | 5,673,669 | **1,607,033** (−72%) | 4,150,074 → 83,438 | — | `3f9a4a26fab68e5c` → `01fa6bcaa35c647c` |
| `--automate` | 4,543,740 | **2,179,225** (−52%) | 2,390,894 → 124,301 | 289,062 → 191,140 | — |
| the battery world | 460,153 | 402,660 | | | |

`PROPOSAL` (138,345), `ACT`, `CONTEXT`, `ARRIVE`, `DECIDE`, `EFFECT`, `ESCALATE`, `OUTCOME` counts are unchanged, as they must be: nothing that carries state was touched. The `STRATUM` saving is smaller than the hold's because the audit rate moves with the class's outcome count every period a wager outcome lands, and the row follows the rate; a rate quantised to the term (step E, with the retained stratum) would make it a row per cell per term.

**A semantic the fold now has to know.** Before D0 a `HOLD` row meant "held today"; after it, a hold stands from its row until the next row about the cell. A fold that wants cell-days held integrates between rows; `minutes_by_via` and the row histogram are unaffected, and no reading in the three modes was ever a hold-row count.

Tree 4,608 lines. Hashes (sha256, first 16): human.h `87f98823fed31429` · ledger.h `905b50496e2bab62` · governor.h `c55d0b7d1c7bfa9b` · solver.h `0a51ac6d9be5e3a4` · machine.h `771c7703ba4ab250` · main.cpp `93e42beeb4c904f7`; core.h, firm.h, world.h, port.h, license.h, report.h, CMakeLists.txt as at C1. Outputs: `receipts/step-d-2026-09-14/d0/` (26 files).
