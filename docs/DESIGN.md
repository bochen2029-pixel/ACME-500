# ACME-500

A virtual enterprise of 500 white-collar knowledge workers, and its automation,
in one C++17 binary. No dependencies. No UI. 3,180 lines.

It is a **demonstrator** because the automation is not a script — it is a
resident, a field, a gate and a ladder, and every number it prints is a reading
rather than a target. It is a **digital twin** because the physics is one
function that both arms call, so the comparison between them is a counterfactual
rather than two stories.

```
g++ -O2 -std=c++17 -Iinclude -Wall -Wextra src/main.cpp -o acme
# or:  cmake -B build && cmake --build build && ctest --test-dir build
```

```
./acme --sim         [--days 260]                 run ACME as it is
./acme --twin        [--days 260]                 both arms, one seed, one physics
./acme --automate    [--warm 180 --days 400]      the whole programme
./acme --multiverse  [--futures 64]               policy search over structure
./acme --selftest    [--lie N]                    the oracles; each carries a lie
```

Common flags: `--n 500 --span 7 --demand 1.5 --seed 20260913`.

---

## The thesis, as mechanism

**An organisation is a function from events to effects that was implemented in
people because people were the only substrate that could hold context.** Not a
set of tasks. So the unit is the decision class, never the task, and the thing
to automate is the context assembly, not the workflow.

Five acts per obligation, and exactly one is the job:

| act | what it is |
|---|---|
| fetch | open N applications, read the fields |
| frame | render what you found for another human's eyes |
| **decide** | **the judgement** |
| commit | type the answer into another application |
| transport | the handoff, the escalation, the status meeting |

plus **glue** — reading the thread to stay current, re-reading yesterday's case,
answering a question someone asked because they could not see your screen. Glue
is a per-*seat* cost, not a per-item cost, and it is the largest single line.

`--sim` at 500 seats, span 7, demand 1.5, 60 days:

```
glue      1,881,943 min      fetch    1,298,842      decide  1,270,923
meeting     964,216          frame      698,504      commit    563,132
transport   236,092          rework      27,745

DECIDE FRACTION = 0.183
```

Nobody typed those in. They fall out of the acts.

### Coordination is derived, not authored

The meeting calendar is generated from the cone. A manager's one-to-ones are
`span × 30 min`; a team status is `(span+1) × 45 min`; a rollup's prep scales
with `depth`; cross-wire touches scale with live dependencies. **Change the span
and the whole load moves.** The middle layer is a coarsening ratio — the span of
human attention, ~7 — and coordination is what a coarse grid costs when the
smoother is a person who cannot see the other cells. A GPU's coarsening ratio is
N, which is why the middle stops being *necessary* rather than becoming
*automated*.

### The one honest thing about meetings

A meeting is not pure waste in this model. It is the **only** channel through
which tacit determinants — facts that live in no system — reach the person
deciding. Deleting meetings costs coverage, and the simulation makes you pay for
that rather than hand-waving it. What a meeting cannot justify is the other 90%
of its content, which is state already in a database being read aloud to people
who could have looked.

### The ceiling is instrumentation, not capability

Every obligation carries determinants. Each lives in a **system** (behind an
application UI), at the **boundary** (in the inbound document), or is **tacit**
(a phone call, a relationship — recorded nowhere). The correct decision is the
sign of the weighted sum of all of them. Nobody sees the whole sum.

```
coverage(class) = 1 − tacit mass of that class
```

A human assembles context by opening applications, at a cost in minutes, and by
remembering — which decays. The resident assembles it by query, free, but only
for what is instrumented. **Same physics function, different inputs.** So the
premise that a frontier model with the right context beats an expert is taken as
settled and is not what this tests. What it tests is who can assemble the
context, at what cost, at what rate.

---

## The automation

### Two graders with disjoint support

**History** grades with zero latency and restricted support. Where the
resident's choice on a settled instance *is* the choice the firm made, the action
is identical and its outcome is on the tape. *Agreement only names the band; the
arrival grades it.*

**The world** grades with full support and one term of latency. The only way to
learn what happens when the machine chooses differently is to let it choose, on
a fraction of live instances it cannot predict, and wait.

So the unit of a licence is the **class-band**, never the class. And the two
bands need different canaries for different reasons: in the agreement band the
canary grades **execution** (history graded the decision, not the machine's
ability to carry it out); in the disagreement band it grades the **decision**.
Both run from day one. Two calendars fall out — the compression case licenses in
weeks, the beat-the-firm case in terms.

### The fourth cell is refused

|  | firm good | firm bad |
|---|---|---|
| resident agreed | graded — identical action, the world's verdict | graded |
| resident disagreed | no label, ever | **no label, ever** |

The bottom-right is the tempting one and this program refuses it. The firm's
choice failed, but the resident's alternative was never executed, so it says
nothing about whether the resident would have done better. It is a reason to
instrument, not a reason to license. `O8` enforces it.

### n₀ is a function, not a constant

```
n0(c) = (z_α + z_β)² · p(1−p) / δ²      δ = max(p · δ_rel, floor)
```

`leave.approve` at p = 0.02 needs **3,368** machine-chosen outcomes.
`po.match.3way` needs **2,223**. The consequence is counterintuitive and the
program prints it: **the highest-stakes classes are the slowest to license**,
because the thing you must detect is rare. Bands that cannot clear n₀ inside a
term at any tolerable canary rate are marked `unlicensable` and printed as human
rather than held in shadow forever.

### The gate

Five verdicts — `HOLD / ACT / DRAFT / FRONTIER / WARRANT`. No `allow`: the gate
can only narrow what the licence already permits. Nothing in it is learned. The
**order of refusal is published and fixed**, and the fact that budget is checked
*last* is the safety property: running out of supervision produces a hold, never
an act. A hold is a verdict and it is a row, because an organisation fails by
omission and an omission that leaves no trace cannot be graded.

Three numbers, not entropy: **direction** (signed log-odds), **sharpness**
(excess spread over this class's own baseline), **novelty** (conformal, computed
*without* asking the model how confident it feels). Entropy is unsigned — it says
how unsure the field is and never which way it leans.

### κ

Supervision created over supervision removed, per class-band. A class whose
licence costs more attention than it saves is demoted the same day, regardless of
how good the model looks. It is the only meter in the program that can fail while
every other number improves.

### The cascade

```
N = E / (1 − α) + F
```

Automation does not remove the fraction of tasks it automates. It reduces E, and
the multiplier removes everything standing on top. The α-functions leave whether
or not anyone automates what they do — **after** the E-work is licensed, because
deleting them first breaks the people still doing it.

`fit_cascade()` fits it **both ways** and prints the error, because the honest
version of this argument shows its own bias: a within-firm regression cannot see
`F`. You fit α across a range of N the firm has never left, then use it to
predict a size the firm has never been. The origin fit omits the intercept and
promises a smaller company than physics allows. You get F from cross-firm data at
the target size, never from the payroll you have.

### The residual — and only two columns move

`warrant` (a signature, liability), `counterparty` (the other side demands a
person), `frame` (uninstrumented determinants, classes that are two classes),
`thin tape` (the world answers too slowly). The first two do not move when models
improve. That is the honest answer to "why not a hundred percent," and it is a
legal and instrumentation fact rather than a capability one.

---

## What the twin can do that a real programme cannot

`--twin` runs two ACMEs on one seed with **nothing shared** — same arrivals, same
determinants, same luck keyed on every obligation id. It computes the exact
paired effect, then computes what a within-firm canary *would have reported*, and
prints the difference.

That gap is not noise. Two mechanisms drive it and both grow with the canary
rate: the machine is routed the cases it was licensed for (**selection**), and
the humans in the same firm are working a queue the machine already drained
(**interference**). So `r` has an **interior optimum**, not just the floor
`n₀/V` that the identifiability arithmetic gives. Raise r to clear n₀ faster and
you contaminate the comparison; lower it to protect the comparison and you never
clear n₀.

A real deployment can never measure this — its canary and its control share one
ledger. A simulation is the only place the bias term can be obtained at all.

---

## Findings the build forced

**The machine is slightly *worse* on accuracy.** Human 92.4%, machine-read 92.1%.
It loses the tacit determinants and wins on cost, lateness and allocation. Left
in rather than tuned away — it is the honest shape and it is what makes the
residual columns mean anything.

**Competence must be graded on correctness, not good-rate.** Correlating seat
skill with good-rate gives noise (c0 −0.097, c3 −0.172, c8 −0.021). Switching to
`outcome != BAD` flips it (c0 +0.261, c2 +0.396, c5 +0.468). Lateness is a
property of the queue; correctness is a property of the decider. A busy expert
and an idle novice look identical until you separate them.

**Competence is a residual, not a hit rate.** Updating skill against the raw
outcome recovers the org's workload distribution and calls it talent. The update
has to be against what the head already predicted from completeness.

**The field ranks people the org chart cannot.** `skill[]` exists in the firm and
*nothing in the assignment path reads it* — a lead sees remaining attention and a
specialty tag, which is what a real lead has. The field recovers the ordering
from outcomes alone at within-class corr **+0.241**.

---

## Oracle battery — 15 of 16

Every oracle carries a lie: `--selftest --lie N` corrupts the mechanism oracle N
claims to test, and that oracle **must** fail. An oracle that passes its own lie
is broken and says so.

| | oracle | reading |
|---|---|---|
| O0a | BLAKE2b-256("abc") vs RFC 7693 | PASS |
| O0b/c | tape chain verifies; one flipped field localises | PASS |
| O1 | same seed → byte-identical tape | PASS (161,900 rows, memcmp) |
| O2 | the gate never authorises outside its licence | PASS (8,640 lattice points) |
| O3 | out of supervision degrades to HOLD, never to acting | PASS |
| O4 | transport conserves; columns never exceed capacity | PASS (err 1.3e−8) |
| O5 | a binding capacity has a nonzero price, stocks included | PASS |
| O6 | join graph read off the application, not asked for | PASS (24/24 exactly) |
| **O7** | **coverage from agreement tracks the tacit mass** | **FAIL — see below** |
| O8 | the disagree/firm-failed cell licenses nothing | PASS |
| O9 | the field recovers competence no human in ACME can see | PASS (+0.241 over 9 classes) |
| O10 | κ demotes a class that costs more than it saves | PASS |
| O11 | effects carry inverses; window closes at settlement | PASS |
| O12 | the origin-fit cascade promises an impossible company | PASS |
| O13 | measured decide fraction ranks the planted intensity | PASS (partial Spearman 0.963) |

### O7, open, with the diagnosis

Two estimators, neither good enough:

| estimator | mean abs err | corr across classes |
|---|---|---|
| full-sample agreement | 0.103 | 0.439 |
| confident stratum only | **0.071** | **−0.052** |

Restricting to instances where both readers landed far from the fence — where
noise cannot explain a disagreement — improved the level and destroyed the
ranking. That combination is diagnostic: the classes with the most tacit mass are
also the ones where fewest instances clear the confidence bar, so the surviving
sample is selected in a way that cancels the signal. The fix is to reweight the
stratum by its selection probability, or to revert to the noisier but unbiased
full-sample estimate. **Currently the second estimator is in the code and the
oracle fails.** It is left failing rather than have the bar moved to meet it.

### Not yet run

The four demo modes are written but have not been exercised end to end. The lie
arms (`--lie 0..13`) have not been verified to fail. Both are one command each;
neither has been done.

---

## Files

```
include/acme/core.h      331   counter RNG, fixed-point accumulators, BLAKE2b,
                               the hash-chained tape, the schema (6 wires, 24 classes)
include/acme/firm.h      310   the cone from the span, α/E/warrant tagging, skills,
                               specialisation, the writ, the outcome vocabulary and its
                               price (the "one dynamics source" that stood here until
                               2026-09-14 had no caller and was deleted: F15; the physics
                               is world.h's observe / truth_decision / world_settle)
include/acme/world.h     299   determinants and where they live, arrivals, the read,
                               context decay, the exogenous grader
include/acme/human.h     371   the five acts, the glue, the derived meeting calendar,
                               escalation as a cold re-fetch, the day loop
include/acme/solver.h    351   the field (learned head + residual competence),
                               log-domain Sinkhorn with finite stocks, the gate, the hand
include/acme/machine.h   572   compile from the query log, invariant mining, replay,
                               the two graders, the ladder, κ, the resident period
include/acme/report.h    184   the α/E regression fitted both ways, the residual,
                               the paired arms, the canary bias, the counters
src/main.cpp             762   five modes and the oracle battery
```

Inherited, with attribution: the tape, the switch and the emit gate from
`fusord.cpp`; the alphabet, the masked decode and the oracles-with-lies discipline
from `org_solver.cu`; the duals-only transport, the finite-stock T17 fix and
two-arms-one-canvas from `ledger_lattice.cu`.

---

## What is not claimed

**This world is synthetic with planted truth. It tests the instrument, never a
business.** Every constant in the schema is a guess made in the open so it can be
argued with. No number here has met a real firm.

The first real receipt is the one the crystallisation named and it has not
changed: replay a historical quarter of one core wire, print invariant violations
(must read zero), the licensed fraction of decision mass at rung 1, the
thin-margin rate, and trunk geometry against random geometry on held-out
next-event likelihood.

The kill for the whole method is one bet: **if replay agreement in no band
predicts outcome accuracy after the first live quarter, history is not a grader
for this firm** and the ladder has to be climbed live, one term per rung.

---

## 2026-09-13 · step 1, the honest instrument (appended, not rewritten)

The first run of this program (`receipts/RECEIPT_ACME500_FIRST-RUN_2026-09-13.md`)
found seven defects under the numbers above. Step 1 fixed them
(`receipts/RECEIPT_ACME500_HONEST-INSTRUMENT_2026-09-13.md`). What changed:

- **The battery is 16 of 16 and every lie arm is seen to fire.** A caught lie
  prints PASS on the lied-to oracle's own line and the run exits 0; CMake asserts
  that line per oracle and no longer accepts a nonzero exit for any reason.
- **O7 is green with a different estimator.** Coverage is read off the world,
  not off agreement: one minus the wrong-rate on the agreement diagonal. Two
  readers with the same blind spot agree across it; the arrival does not. Mean
  error 0.05 to 0.07, correlation 0.84 to 0.96 on three seeds. Its lie is a
  world that never says wrong.
- **Both arms are banded by the same rule**, so the wager is paired against the
  firm's rate in the same band and never against a default.
- **The backlog is on the books.** With open obligations priced at `w_unplaced`,
  the twin's cost delta reverses sign: −$590M in the machine's favour, where the
  section above printed +$591M against it. The machine is still 3.5 points
  worse on correctness; it buys the queue.
- **Drafts are not the wager.** Only unattended acts feed the e-process;
  drafts and warrants are graded as assisted and printed beside the counter.
- **The numbers above are superseded.** Licensed decision mass 63.7 percent, not
  91.8; the cascade reads 500 → 193, not 500 → 57; the within-firm estimator
  bias is +31.7 points, not −10.9. All of it is a reading of a synthetic firm.

Three things were found and not fixed: the novelty score has no variance once
lateness is removed from it; the frontier is re-rented daily for the same
escalated case; and the writ's `eps_floor`, now read, cannot bind, because
n₀ / V exceeds the 0.35 canary cap in every class of this schema and the gate
consults the canary flag at rung 1 only. The multiverse, re-run with the
backlog in its objective, keeps the three lane-buying patches on top and swaps
the two thin-margin patches end for end: with the queue priced, acting sooner
beats acting later. All of it is in the receipt.
