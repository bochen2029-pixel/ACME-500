# ACME-500

**A virtual enterprise of 500 knowledge workers, and its automation, in one C++17 binary with no dependencies. It exists so that a method for automating organizations can be graded by a world that answers in seconds instead of quarters, and can fail cheaply before any real firm is asked to be the grader.**

3,180 lines. Five modes. Sixteen oracles, each carrying a lie that it must be seen to catch. Every number it prints is a reading, and the readings that turned out wrong are printed as retractions in the receipts.

```
g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme
./acme --selftest
```

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

The readings as they stand, synthetic firm, one writ, 500 seats, demand 1.5:

- **The twin.** The resident settles 187k obligations to the incumbent's 82k and ends with 19k open against 124k. It is 3.5 points worse on correctness and 4.4 points worse on the good rate. With the backlog priced at the writ's own stock price, total cost is $1,248M against $1,838M, a delta of −$590M in the machine's favour; with the backlog free, as the first run scored it, the sign is the other way. The within-firm canary estimate reads +27.4 points against a true paired effect of −4.4: **bias +31.7**.
- **The cascade.** Licensed decision mass 63.7 percent. Headcount 500 → 193 with the intercept fitted, 181 with the origin fit that promises a company physics does not allow. The first run printed 500 → 57; that number rested on two of the seven defects and is retracted.
- **The residual.** Warrant 4.7, counterparty 0.7, frame 14.3, thin tape 16.7 percent. The first two do not move when models improve.
- **The multiverse.** Ten structural patches, six rollouts each. The three that buy a lane, moving a class's unrecorded determinants into a system of record, take the top three places every time. With the queue priced, acting sooner beats acting later; with it free, the reverse.

Three findings are recorded and not fixed: the novelty score has no variance once lateness is removed from it; the frontier is re-rented daily for the same escalated case; the writ's uniform floor cannot bind at this schema because the identifiability floor exceeds the canary cap in every class. They are in the receipt with their lines.

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

| | oracle | reading, 2026-09-14 |
|---|---|---|
| O0 | BLAKE2b-256 against RFC 7693; the tape chain verifies; one flipped field localises | PASS |
| O1 | same seed, byte-identical tape | PASS, 161,900 rows by memcmp |
| O2 | the gate never authorises outside its licence | PASS, 8,640 lattice points |
| O3 | out of supervision degrades to HOLD, never to acting | PASS |
| O4 | transport conserves; columns never exceed capacity | PASS, error 1.3e−8 |
| O5 | a binding capacity has a nonzero price, stocks included | PASS |
| O6 | the join graph is read off the application, not asked for | PASS, 24 of 24 |
| O7 | coverage read off the world's verdicts tracks the tacit mass | PASS, error 0.052, corr 0.945 |
| O8 | the disagree / firm-failed cell licenses nothing | PASS |
| O9 | the field recovers competence no human in ACME can see | PASS, within-class corr +0.241 |
| O10 | kappa demotes a class that costs more than it saves | PASS |
| O11 | effects carry inverses; the window closes at settlement | PASS |
| O12 | the origin-fit cascade promises an impossible company | PASS |
| O13 | the measured decide fraction ranks the planted intensity | PASS, partial Spearman 0.963 |

## How it is built

- **The tape is the only truth.** Append-only, BLAKE2b-chained, fixed-width records; every table is a fold over it. A hold is a row, because an organization fails by omission and an omission that leaves no trace cannot be graded. The interior, the meeting, the deck, the ticket, is recorded only so it can be priced and deleted.
- **The RNG is stateless.** Counter-based, keyed on seed, stream and index, so two arms of a twin get the same arrivals, determinants and luck on every obligation id.
- **One dynamics source.** The goodness of a decision is defined in one function that both arms call. It does not take "human or machine"; a decider is a competence and a completeness. The load-bearing variable is completeness: who can assemble the context, at what cost, at what rate.
- **Determinants live somewhere.** In a system of record behind an application, in the inbound document, or nowhere. Coverage of a class is one minus its tacit mass, and it is a property of instrumentation, which is buyable, not of the model.
- **Two graders with disjoint support.** History grades the band where the machine's choice was the firm's, with zero latency; the world grades the rest, one term late, through the canary. The fourth cell of the table, where the machine disagreed and the firm failed, is refused: the machine's alternative was never run.
- **The gate contains nothing learned.** Five verdicts, a published order of refusal, budget checked last so that running out of supervision produces a hold and never an act. Direction, sharpness and novelty are three numbers, not entropy.
- **κ.** Supervision created over supervision removed, per class-band. The one meter that can fail while every other number improves.

The full design, with the arithmetic of n₀, the ladder, the cascade fitted both ways, and the honest paragraph about meetings, is [docs/DESIGN.md](docs/DESIGN.md).

## The observer

[`observer/`](observer/) is a three.js page that folds the tape and draws the firm as a headquarters: departments as floors, pods of desks with the lead at the head, glass offices, the executive floor, the annex, the resident's basement, the dock where outcomes land. It is a pure observer of a frozen contract and runs from a golden with no backend. Its own README says what is missing and why.

## Layout

```
src/main.cpp              five modes and the oracle battery
include/acme/core.h       the RNG, fixed-point accumulators, BLAKE2b, the tape, the schema
include/acme/firm.h       the cone from the span, the α/E split, skills, the writ, the one dynamics source
include/acme/world.h      determinants and where they live, arrivals, the read, the exogenous grader
include/acme/human.h      the five acts, the glue, the derived meeting calendar, escalation
include/acme/solver.h     the field, log-domain Sinkhorn with finite stocks, the gate, the hand
include/acme/machine.h    compile, invariant mining, replay, the two graders, the ladder, the resident
include/acme/report.h     the cascade fitted both ways, the residual, the paired arms
receipts/                 every run, with commands, outputs, exit codes and what was not done
docs/                     the design, the method, the observer contract, the step-1 brief
observer/                 the visualization
```

## Working on it

`CLAUDE.md` binds any agent session in this tree and is worth a human's read too: reproduce the baseline battery before editing; a lie arm counts only when the lied-to oracle is seen to catch it; never report a process as running without its PID and CPU time; the physics in `firm.h` and `world.h` is not under review and no constant is tuned to pass an oracle; every step ends in a dated receipt.

The next steps, in order: a `--dump` verb so the observer reads the binary's own tape; the three cheap kills in the wider programme; and then the only grader that matters, one real wire.

## License

MIT. See [LICENSE](LICENSE).
