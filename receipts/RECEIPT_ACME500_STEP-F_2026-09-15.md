# RECEIPT · ACME-500 · STEP F · THE LANES

*The world's side of the machine. Step E closed the licence: the ladder v2 with its retained stratum, its regime detector and its calibration harness, and the account that reads the firm's own thinking. Step F turns the one binary into a set of writers that can stand in separate processes, on separate hardware, under separate keys, without changing a single reading. ARCHITECTURE v4.1 §26 and §27.*

*Every claim here is a reading of a run whose command, output and exit code are under `receipts/step-f-2026-09-15/`. Where this file and a staged output disagree, the output wins and this file is the defect.*

---

## F0a · the tape of tapes: a roster, a phase, a seal, and the merge that folds the same

*Claude Opus 5, 2026-09-15. The first sub-step of F0: the lanes built as a **projection of the one stream** and proven exact, before any of them is moved into its own process. It moves no reading. Built and run under WSL by `tools/run_step.sh step-f-2026-09-15/f0a receipts/step-e-2026-09-15/e4`; raw outputs under `receipts/step-f-2026-09-15/f0a/` (42 files).*

**Why a projection first.** v4 §26 makes every organ a lane: the licensor in a process the machine cannot reach, a judge behind a port, a mouth on a language lane, an executor that mints its own receipts, a counterparty's kernel, an instrument that plants lies. Each is a second writer, and a tape with one chain has one writer. The cheap way to find out whether that design holds is not to split the processes and then debug a protocol; it is to **split the existing stream by writer, merge it back, and measure whether the fold is the same**. If it is, the two-process sim (§26.5, O58) is a re-arrangement of this run rather than a different program. If it is not, the defect is in the design and it costs an afternoon rather than a rewrite. It was not, twice, and both are below.

**What landed.** `lanes.h`, kernel side, including `core.h` and nothing else:

- **The roster**, six lanes in §26.4's order: the world port, the licensor, the judge, the kernel, the human seats, the executor. `lane_of(r)` is a **pure function of the row**: the type says the writer, and where one type has two writers (a hold is written by a seat and by the machine; a context is fetched by both) the seat on the row says which. The record does not move: `Rec` stays forty bytes and the lane is not a field on it.
- **The phase**, nine of them, published as the order of a period: open, arrive, draw, read, gate, human, settle, ladder, close. `phase_of(r)` is a pure function of the row for the same reason.
- **`LaneSet`**, the per-lane chains, and **`seal_period`**, which writes `R_SEAL` at the close of every period: `a` the lanes sealed, `b` the low 32 bits of BLAKE2b over the lanes' heads in lane order, `value` the rows sealed. The seal is itself a row of lane 0 and folds into that lane after it is written, so the next period's merged head covers it.
- **`split_lanes`** (the cold projection, which reproduces the live heads from the rows alone) and **`merge_lanes`** (the merge in the published order).
- **O42** (lie 33) with three claims and the lie that a period's arrivals fold late.

**The correction to §26.2, found by the sim before a line of the two-process build was written.** §26.2 orders the merge by lane: *"every lane l ≥ 1 in lane order, then lane 0's records of period t."* That order cannot hold, because **two writers each write twice in a period**. The world port opens it (TICK, ARRIVE) and closes it (OUTCOME, SEAL); the licensor draws the strata before the machine reads and moves the ladder after the outcomes land. Fold lane 0 last and an ARRIVE row lands after the EFFECT on the cell it opened, and the fold loses the cell. So the merge key is not (lane, sequence) but

> **(day, phase, lane, sequence within the lane)**

with the nine phases part of the published alphabet. A writer may appear in several phases; a phase may hold several lanes; within a phase the lanes fold in lane order, and within a lane the rows fold in the order they were written. §26.2 is amended in v4.1 §39.4 and O42 now holds it.

**Two laws restated rather than invented, each found by an oracle that broke.**

*A NOTE is a row of no period, so it is a row of no seal.* O18 (a kill survives: the durable tape reopens, the checkpoint restores, the run continues bit for bit) failed at the first SEAL after a restore. The cause was not the seal: a restore writes one NOTE between periods, D1's law already says a NOTE is a row of no period and the TICK's row count already excludes it, and that is precisely what made a resumed tape equal an unkilled one. A seal is a period's attestation, so a row of no period is a row of no seal: a NOTE joins its lane's rows, so the merge loses nothing, and never its lane's chain. `seal_covers(r)` is that rule in one line, used by the live set and the cold split alike. With it O18 passes unchanged.

*The seal's head is a chain, and a chain covers the operator's clock.* O25 (the same rows at different wall spacing give identical verdicts) then failed, because the TICK row carries a wall reading that is the operator's number and never the machine's, and the world lane's chain covers every byte of it. That is correct behaviour for a chain and wrong only as a claim: the oracle compares every row byte and now found seventy seals differing. O25 now excludes the SEAL's merged head for the same stated reason it excludes the TICK's wall value, and says so in its own line: neither is a verdict, and no verdict, licence or effect may move. The lie (a resident that reads the wall value) still fires, because a resident that reads the clock changes verdicts and verdicts are rows.

**The reading.** At 500 seats over 260 periods the year's rows divide as:

| lane | rows | what it wrote |
|---|---|---|
| world port | 88,045 | TICK, ARRIVE, OUTCOME, HEADER, SEAL |
| licensor | 51,885 | STRATUM, LICENSE, KAPPA, REGIME |
| judge | 53,849 | PROPOSAL, SENSE, COUNSEL |
| kernel | 53,683 | EFFECT, HOLD, ESCALATE, CONTEXT, UNDO, CALIB, NOTE |
| human | 165,676 | DECIDE, ASSIGN, ACT, MEETING, and the seats' holds |
| executor | 0 | RECEIPT, until F0c mints one |

Seventy seals in the battery's run, every one re-deriving from a cold split at its own row; no row of a sealed period written after its seal; the merge folding to the same ledger, zero field diffs. **160,060 of 413,140 rows sit at a different index under the merge than in the flat stream**, and the fold does not read the index: that number is the honest size of the reordering a multi-writer tape permits, and the fold's invariance across it is the whole claim. The lie moves one period's arrivals into the next and the fold moves with it: 67 field diffs, caught.

**Every default reading is identical to E4**, and the deltas are the seal and its pin:

| | E4 | F0a | why |
|---|---|---|---|
| licensed decision mass, N, κ, bands past rung 0, acted unattended, CPI | 13.1%, 437, 0.064, 11, 4,454, 326.8 | the same, to the digit | the step writes one row a period and reads nothing new |
| the sim's tape | 1,607,033 rows | 1,607,293 | one SEAL per period, 260 periods |
| the automate's tape | 2,136,416 rows | 2,136,676 | the same |
| the alphabet pin | `0xf9bc8d9c` | **`0x1304ca15`** | `SEAL` joins the alphabet, which is the widening the pin exists to make loud |
| the validity record's schema column | `0xf9bc8d9c` | `0x1304ca15` | it is the alphabet pin, and it moved |
| the TICK row's `b` | rows of the period just closed | one more | the seal is a row of the period it closes |

**Acceptance.** Build 0 warnings. `--selftest` **36 of 36** (158.2 s). Lie arms 0–33, six at a time, each exit 0 with the lied-to oracle's own line PASS. O17 both lies, O28, O25a, O30a both arms. O14 folds the tape to 0 diffs with the seals in it; O29 scans them with 0 shape faults; O18 restores and continues bit for bit; O1b is byte-identical. Tree 7,919 lines.

**What F0a did not do.** The lanes are one stream still: `split_lanes` proves the projection is exact, and F0c writes each lane to its own segment file with its lane id in the header. `--replay-from` and O53 (a run replayed with every judge lane absent reproduces every non-judge row bit for bit) are F0b. The licensor as a process with the salt, the stake, STOP and `no-stratum` (O43), the instrument lane and its battery (O44), RESERVE and `--executor-loss` (O55), and the two-process sim (O58) are F0c and after. The executor lane is empty until then. The dependency directory belongs with the lanes' design and is not built.

Hashes (sha256, first 16): lanes.h `eda4414975382956` · core.h `249e512dc6bfd4ee` · checkpoint.h `67c70dffcd20e819` · main.cpp `bee90820b0bb4ecd` · CMakeLists.txt `bd648cfc720bbb49`; calib.h `23a1942c777833c4`, firm.h `f92f13fff89362f0`, gate.h `2f53dc271cd0095e`, governor.h `ce23b6d6328040d0`, ledger.h `7d6ba27be2dd2eac`, license.h `40839b21729bc56a`, machine.h `17fcc8b4e30b479d`, panel.h `17a44e0a8927d3f9`, report.h `f66b03b53edb4a1c`, world.h `56adc32f2f3833d1` as at E4; dump.h, human.h, port.h, solver.h, tapefile.h as at E0. Outputs: `receipts/step-f-2026-09-15/f0a/` (42 files).
