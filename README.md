# ACME-500

**A virtual enterprise of 500 knowledge workers, and its automation, in one C++17 binary with no dependencies. It exists so that a method for automating organizations can be graded by a world that answers in seconds instead of quarters, and can fail cheaply before any real firm is asked to be the grader.**

4,551 lines on branch `v2` (v1 is tag `v1-step-A`). Five modes. Twenty-two oracles in the binary and three on the source tree, each carrying a lie that it must be seen to catch. Every number it prints is a reading, and the readings that turned out wrong are printed as retractions in the receipts.

```
g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme
./acme --selftest
```

![ACME-500 at the CASCADE phase: the headquarters with its department floors, the executive floor under the sign, the annex at left, the dock with outcome marks, the ledger skyline, and the resident's basement](observer/shots/08-CASCADE.png)

*The observer at day 59 of the golden, phase CASCADE. Every desk lamp, token, tray, glass room and darkened floor is driven by a row on the tape; nothing is animated on its own.*

![A department floor: pods of desks with the lead at the head, glass offices on the window wall, the lift core, the ceiling strips](observer/shots/cam-WING.png)

*One department floor. Pods of eight with the lead at the head, managers in glass offices on the window wall, tokens on the desks sized by value, trays stacking the holds.*

## Why it exists

An organization is treated here as a function from events to effects that was implemented in people because people were the only substrate that could hold context. The method that automates it ([docs/METHOD_TWO-GRADERS-TWO-BANDS.md](docs/METHOD_TWO-GRADERS-TWO-BANDS.md)) compiles the firm's decision classes from its own applications, replays its settled history to license the band where the machine's choice was the firm's, runs canaries on every class at the identifiability floor to license the rest, climbs a ladder on arrived outcomes only, and lets the middle of the org chart leave by arithmetic. Every step in that sentence is a claim, and a real firm grades claims at the pace the world answers: a chaser in days, an invoice in a quarter, a hire in a year.

ACME-500 is a firm with planted truth. The tacit mass of each decision class, the competence of each seat, the support coefficient and the fixed floor of the headcount cascade, the judgment intensity of each class: all planted, none shown to the instrument. The instrument must recover them from the tape, and an oracle checks that it did. That makes three things possible that a deployment cannot do:

1. **Falsify the method before a customer does.** The first run found seven defects under its own headline numbers, retracted the headline, and found that its coverage estimator's premise was false. Those receipts are in this repository, unedited.
2. **Measure the number a real programme can never get.** A real firm's canary and its control share one ledger, so the within-firm estimate of the machine's effect is biased by selection and interference and nobody can say by how much. A twin runs two firms on one seed with nothing shared and prints the bias. Here it is +31.7 points.
3. **Show the instrument running**, with its refusals, its holds, its kappa, and its own retractions, so that what a buyer sees is a reading and not a promise.

It is a falsifier first, a gym second, and a demonstrator last. **It tests the instrument, never a business.** No number here has met a real firm.

## Status, 2026-09-14

| receipt | result |
|---|---|
| [first run](receipts/RECEIPT_ACME500_FIRST-RUN_2026-09-13.md) | 15 of 16 oracles; twelve of fourteen lie arms catching their lie; seven defects found with lines, most of them under the headline numbers |
| [honest instrument](receipts/RECEIPT_ACME500_HONEST-INSTRUMENT_2026-09-13.md) | 16 of 16; all fourteen lie arms seen to fire; the coverage estimator replaced by one graded by the world (error 0.05 to 0.07, correlation 0.84 to 0.96 on three seeds); the twin's cost sign reversed by pricing the backlog; the cascade restated |
| [step A](receipts/RECEIPT_ACME500_STEP-A_2026-09-14.md) (v2 begins) | O1b, determinism over the machine arm; the dead physics deleted (F15); O28, a tool oracle for uncalled functions; 17 of 17 with fifteen lie arms; every reading identical |
| [step B](receipts/RECEIPT_ACME500_STEP-B_2026-09-14.md) and its remediation | the record is v2 (40 bytes, `via`, `arm` = the decider); the rows the fold was missing; `ledger.h` folds the tape into every cell and the minute meter; O14 fold-to-identity, O15 the ladder is a fold, O25a no clock in the kernel, O29 row shape; the alphabet pin; 20 of 20 with eighteen lie arms; every reading identical |
| [step C0](receipts/RECEIPT_ACME500_STEP-C0_2026-09-14.md) | every consumer reads the folded ledger; the port (`Store::frame`, `Judge::read`) with the plant behind it; O17, the machine links without the plant; the prints that attribute (minutes by via, the binding reason per band, the frame split, the row histogram); every reading identical; F21 found and recorded |
| [step C1](receipts/RECEIPT_ACME500_STEP-C1_2026-09-14.md) | the judge on its own key with its own competence (F16); the kernel's completeness from the compile step; the planted half of the class table out of the kernel's reach (`planted(c)`, `build_acme` in the plant, the cascade fitted from a handed panel, kappa's denominator metered from rows); the read budget with the memo keyed on the frame; the governor in its own translation unit writing STRATUM, LICENSE and KAPPA rows; the licence keyed to the judge; O16, O21, O17 with a second lie; 22 of 22 with twenty lie arms; the first step to move a number, every delta named; F22 found and fixed |

The readings as they stand after step C1, synthetic firm, one writ, 500 seats, demand 1.5 (the C1 receipt names what moved from v1 and why):

- **The twin.** The resident settles 187k obligations to the incumbent's 82k and ends with 19k open against 124k. It is 3.5 points worse on correctness and 4.6 points worse on the good rate. With the backlog priced at the writ's own stock price, total cost is $1,254M against $1,838M, a delta of −$583M in the machine's favour; with the backlog free, as the first run scored it, the sign is the other way. The within-firm canary estimate reads +29.8 points against a true paired effect of −4.6: **bias +34.4**.
- **The reads.** With the memo keyed on the frame, the resident reads 1,729 cells a period against 3,328 when it re-read every open cell every day; 1,462 a period are carried forward. Behind the port a read is a forward pass, and the read budget is a dial: at 4,000 a day against the 84,000-cell warm backlog the machine wins the queue in a fortnight instead of a day and loses seven points of good rate on the way.
- **The cascade.** Licensed decision mass 63.5 percent. Headcount 500 → 193 with the intercept fitted, 182 with the origin fit that promises a company physics does not allow. The first run printed 500 → 57; that number rested on two of the seven defects and is retracted.
- **Kappa.** 0.110: 2.3M minutes of review caused against 21.3M minutes of person-work removed, the denominator metered from the firm's own rows per decided cell rather than priced by the plant's formula (which read 0.242).
- **The residual.** Warrant 4.6, counterparty 0.7, frame 10.0 (of which 6.8 waits on the ladder and 3.2 on instrumentation), thin tape 21.1 percent. The first two do not move when models improve.
- **The multiverse** (last run at v1). Ten structural patches, six rollouts each. The three that buy a lane, moving a class's unrecorded determinants into a system of record, take the top three places every time. With the queue priced, acting sooner beats acting later; with it free, the reverse.

Four findings are recorded and not fixed: the novelty score has no variance once lateness is removed from it; the frontier is re-rented daily for the same escalated case; the writ's uniform floor cannot bind at this schema because the identifiability floor exceeds the canary cap in every class; and the ladder's floor is paid once, not per rung, so six of the ten bands at the top rung got there with fewer wager outcomes than n₀ (F21, found by the binding-reason print in [the C0 receipt](receipts/RECEIPT_ACME500_STEP-C0_2026-09-14.md)). A fifth, F22, was found and fixed in C1: history admission licensed a coin on the half of history it happened to agree with, until agreement beyond chance became a condition. They are in the receipts with their lines.

## The five modes

| mode | what it does | wall time, one core |
|---|---|---|
| `--sim [--days 260]` | ACME as it is: 500 skulls passing notes. Prints where the minutes went by act: glue, meeting, fetch, frame, decide, commit, transport, rework. The decide fraction is measured, never asserted. | 4 s |
| `--twin [--days 260]` | both arms on one seeded world with nothing shared: the exact paired counterfactual, and the bias a within-firm canary would have reported | 49 s |
| `--automate [--warm 180 --days 260]` | the whole programme: instrument the boundary, compile from the query log, replay the history, license the agreement bands, canary everything at the floor, climb on arrivals, kappa, the cascade, the residual, the counter on the wall | 47 s |
| `--multiverse [--futures 64]` | policy search over structure: many ACMEs under edited constraints, writ-reweighted with ensemble pessimism; every patch carries an inverse | 35 min |
| `--selftest [--lie N]` | the oracle battery; `--lie N` corrupts the mechanism oracle N tests and that oracle must be seen to catch it | 5 s |

Common flags: `--n 500 --span 7 --demand 1.5 --seed 20260913`.

## The oracle battery

Every oracle carries a lie. Under `--lie N` the mechanism oracle N claims to test is corrupted on purpose, and oracle N prints PASS only if it caught the corruption; a healthy lie run exits 0 like the plain battery. CMake asserts the caught line per oracle (`PASS_REGULAR_EXPRESSION` on that oracle's id, `FAIL_REGULAR_EXPRESSION` on any FAIL) and never accepts a nonzero exit for an unrelated reason as a caught lie. That last clause was learned the hard way: the first harness marked every lie arm `WILL_FAIL`, one oracle was red for its own reasons, and all fourteen lie tests showed green while one of them was planting its lie in a field its oracle never read.

| oracle | asserts | reading |
|---|---|---|
| O0a | BLAKE2b-256("abc") matches RFC 7693 | PASS |
| O0b | tape chain verifies over 500 rows | PASS |
| O0c | a single flipped field localises to its row | PASS, localised at row -1 |
| O1 | same seed produces a byte-identical tape | PASS, 219432 rows, memcmp |
| O1b | same seed produces a byte-identical MACHINE arm: tape, chain, licence table | PASS, 460153 rows; rec same, chain same, licence table same |
| O14 | the ledger folds from the tape: cold fold == the live ledger every consumer reads | PASS, 58367 cells folded from 460153 rows; 0 field diffs; 0 minute diffs; alphabet pin matches |
| O29 | every row carries what the v2 table says it carries | PASS, 460153 rows scanned, 0 shape faults |
| O15 | the ladder is a fold of OUTCOME rows: counts rebuilt == live | PASS, 72 class-bands; 0 count diffs against the live ladder |
| O16 | a null judge and a rules judge license nothing, from history or the wager | PASS, null judge: top rung 0, 0 LICENSE rows, 0 wager outcomes; rules judge: top rung 0, 0 rows, 0 outcomes |
| O21 | the canary draw is independent of the machine's band | PASS, chi-square 72.2 on 72 class-band cells against a 99.9% point of 114.9 |
| O2 | the gate never authorises outside its licence | PASS, 8640 lattice points, 390 reached ACT |
| O3 | out of supervision degrades to HOLD, never to acting | PASS |
| O4 | transport conserves: rows ship, columns never exceed | PASS, max column overshoot 0.00e+00, placed+stock err 1.30e-08 |
| O5 | a binding capacity has a nonzero price, stocks included | PASS, binding seat price 29.6598, unplaced-stock price 28.0206 |
| O6 | the join graph is read off the application, not asked for | PASS, 24 of 24 classes, exactly |
| O7 | coverage read off the world's verdicts tracks the tacit mass | PASS, mean |err| 0.054, corr 0.946 over 19 classes |
| O8 | the disagree/firm-failed cell licenses nothing | PASS, 2174 cells available, 21611 credited, 21611 agreements |
| O9 | the field recovers competence no human in ACME can see | PASS, mean within-class corr 0.242 over 9 classes, 209 seat-class pairs |
| O10 | kappa demotes a class that costs more than it saves | PASS, kappa 2.25 -> rung 1 |
| O11 | effects carry inverses; the window closes at settlement | PASS, 60 effects committed and unwound; post-settlement reversal refused |
| O12 | the origin-fit cascade promises an impossible company | PASS, alpha 0.145 vs 0.197 planted; F 17.6 vs 9.0; gap at E=200 is 9 seats |
| O13 | the measured decide fraction ranks the planted intensity | PASS, partial Spearman 0.963 over 24 classes, n_systems controlled; the LEVEL is never recovered |
| O28 | no dead physics: every free function in firm.h/world.h has a caller | PASS, 18 functions |
| O25a | the kernel reads no clock: machine.h, solver.h, ledger.h, report.h, port.h, license.h read time only from TICK rows | PASS |
| O17 | the machine links without the plant or the governor: ledger.h, port.h, license.h, solver.h, machine.h, report.h compile under -DACME_NO_PLANT -DACME_NO_GOVERNOR | PASS |

*Regenerated from `receipts/step-c1-2026-09-14/selftest.txt`, `o28_o25a.txt` and `o17.txt` by `tools/oracle_table.py`, never edited by hand. O25a is a source scan (no clock read in the kernel's files); the dynamic O25 (same rows at different wall spacing give identical verdicts) waits for the TICK-driven loop of step D.*

## How it is built

- **The tape is the only truth.** Append-only, BLAKE2b-chained, fixed-width records; every table is a fold over it. A hold is a row, because an organization fails by omission and an omission that leaves no trace cannot be graded. The interior, the meeting, the deck, the ticket, is recorded only so it can be priced and deleted.
- **The RNG is stateless.** Counter-based, keyed on seed, stream and index, so two arms of a twin get the same arrivals, determinants and luck on every obligation id.
- **One read function, one grader.** The correct decision is the sign of the full weighted sum over a class's determinants. A decider, human or machine, sums only the determinants it gathered, plus noise scaled by the class's judgement intensity and its own competence, through one `observe()` with different masks; the world settles the outcome by comparing the decision to that truth and the day to the due day, without knowing who decided. The load-bearing variable is completeness, through which determinants get summed: who can assemble the context, at what cost, at what rate. (An earlier "quality curve" described here until 2026-09-14 had no caller and was deleted; finding F15.)
- **Determinants live somewhere.** In a system of record behind an application, in the inbound document, or nowhere. Coverage of a class is one minus its tacit mass, and it is a property of instrumentation, which is buyable, not of the model.
- **Two graders with disjoint support.** History grades the band where the machine's choice was the firm's, with zero latency; the world grades the rest, one term late, through the canary. The fourth cell of the table, where the machine disagreed and the firm failed, is refused: the machine's alternative was never run.
- **The gate contains nothing learned.** Five verdicts, a published order of refusal, budget checked last so that running out of supervision produces a hold and never an act. Direction, sharpness and novelty are three numbers, not entropy.
- **κ.** Supervision created over supervision removed, per class-band. The one meter that can fail while every other number improves.

The standing architecture, with the arithmetic of n₀, the verdict law, the data model, the kernel, the judge port, the build order and the kills, is [docs/ARCHITECTURE_v2.md](docs/ARCHITECTURE_v2.md); its §14 is the order of steps and §14a the amendments after review. [docs/DESIGN.md](docs/DESIGN.md) is v1's design, kept as the receipt of what ran at tag `v1-step-A`.

## The observer

[`observer/`](observer/) is a three.js page that folds the tape and draws the firm as a headquarters: departments as floors, pods of desks with the lead at the head, glass offices, the executive floor, the annex, the resident's basement, the dock where outcomes land. It is a pure observer of a frozen contract and runs from a golden with no backend.

### The eight phases, one camera

The director cuts on the phase boundaries in `phases.json` and on nothing else. Day 0 to day 54 of the golden, ORBIT camera, headless.

| INSTRUMENT | COMPILE | REPLAY | LICENSE |
|---|---|---|---|
| ![](observer/shots/01-INSTRUMENT.png) | ![](observer/shots/02-COMPILE.png) | ![](observer/shots/03-REPLAY.png) | ![](observer/shots/04-LICENSE.png) |
| the tape is spooled; nothing is claimed | the join graph draws itself over the rack | the tower is run against its own history | rungs are issued per class and band |

| CANARY | CLIMB | COMPILE_OUT | CASCADE |
|---|---|---|---|
| ![](observer/shots/05-CANARY.png) | ![](observer/shots/06-CLIMB.png) | ![](observer/shots/07-COMPILE_OUT.png) | ![](observer/shots/08-CASCADE.png) |
| machine verdicts run beside live desks | licensed bands climb; floors go quiet | classes collapse into routine blocks | the annex empties; residual is what is left |

### The close cameras

| the dock | the basement |
|---|---|
| ![The dock: tokens waiting in the bay, outcome marks on the apron graded good, late and bad, the chute out](observer/shots/cam-DOCK.png) | ![The resident's basement: the license grid lit by rung, the admitted glyphs, the tape spool, the kappa gauge, the systems rack, the routine block](observer/shots/cam-BASEMENT.png) |
| the boundary. Tokens wait in the bay; outcomes land on the apron as marks, green, amber and red, rimmed by who decided; effects leave by the chute. | the resident. The 24 × 6 license grid lit by rung, the purple glyphs where history admitted a band, the spool, the kappa gauge, the rack, and the routine block that grows as classes compile out. |

| the executive floor | the annex |
|---|---|
| ![The roof and the executive floor, with the boardroom and the glass offices](observer/shots/cam-TOP.png) | ![The annex during the cascade, its upper floors dark, the tower lit behind it](observer/shots/cam-ANNEX.png) |
| the top: the boardroom at the front, the offices along the back, the sign, the rooftop plant. | the α-functions in their podium, emptying floor by floor during the cascade, after the wings and never before. |

![Isometric view of the whole plant](observer/shots/cam-ISO.png)

*Isometric. The tower, the annex with its bridge, the dock, the ledger skyline.*

## Layout

```
src/main.cpp              five modes and the oracle battery
include/acme/core.h       the RNG, fixed-point accumulators, BLAKE2b, the 40-byte record and the tape, the schema and its pin
include/acme/firm.h       the cone from the span, the α/E split, skills, the writ, the outcome vocabulary and its price
include/acme/ledger.h     THE LEDGER: the cells, the open set and the minute meter, as a fold of the tape; includes nothing from the plant
include/acme/port.h       THE PORT: Store::frame and Judge::read, the only way the machine touches a world
include/acme/license.h    THE LICENCE TABLE: per class-band, keyed per judge; the gate reads it, the governor writes it
include/acme/governor.h   THE GOVERNOR: the salt, the strata as rows, the ladder as LICENSE and KAPPA rows; the machine never includes it
include/acme/world.h      THE PLANT: the planted half of the class table, build_acme, determinants and where they live, arrivals, the read, the grader; PlantStore, PlantJudge
include/acme/human.h      the incumbent arm: the five acts, the glue, the derived meeting calendar, escalation
include/acme/solver.h     the field, log-domain Sinkhorn with finite stocks, the gate, the hand
include/acme/machine.h    compile, invariant mining, replay, the read budget and the memo, the resident (links without the plant or the governor)
include/acme/report.h     the cascade fitted from a handed panel, the residual, the paired arms, the prints that attribute
tools/                    the tool oracles: dead_symbols.py (O28, O25a), o17.py (O17); oracle_table.py regenerates the table above
receipts/                 every run, with commands, outputs, exit codes and what was not done
docs/                     ARCHITECTURE_v2.md (standing), DESIGN.md (v1), the method, the observer contract, the briefs
observer/                 the visualization
```

## Working on it

`CLAUDE.md` binds any agent session in this tree and is worth a human's read too: reproduce the baseline battery before editing; a lie arm counts only when the lied-to oracle is seen to catch it; never report a process as running without its PID and CPU time; the physics in `firm.h` and `world.h` is not under review and no constant is tuned to pass an oracle; every step ends in a dated receipt.

The next steps are §14 of the architecture, in order: C1 (the judge on its own key, the read budget with the frame-hash memo, planted truth off the machine's path, the governor's own translation unit writing `STRATUM` rows, O16 and O21; the first step allowed to move a number, with every delta named), then D (the durable tape, the switch and the pill, the TICK-driven loop), E, F (`--dump`), G (one real lane, folded), H, and I, the only grader that matters: one real wire.

## License

MIT. See [LICENSE](LICENSE).
