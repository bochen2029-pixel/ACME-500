# ACME-500 / INTELLECT-500 · ARCHITECTURE v2
### One machine, four worlds, one tape. The simulator that trains the kernel and the product that runs the firm are the same program; the only thing that changes on the day a real organization is plugged in is the header line.

**Version 2.0 · 2026-09-14 · Claude Opus 5 · the `C:\55555` session.** Supersedes `acme500/docs/DESIGN.md` (v1, the design as built through step A of the REV 2 spec) and absorbs `docs/SPEC-AMENDMENTS_REV2_ONE-PORT-FOUR-WORLDS.md` (the transition amendments), the roundtable final of 2026-09-14 (`ROUNDTABLE_THE-FLAT-ORGANIZATION_2026-09-14.md`), the thin-plane and verdict-law derivations, and the reads of `fusord.cpp`, the flight computer, the ledger lattice, `org_solver.cu` and OrgSolver Converged. Nothing in v1 is overwritten; v1 remains the receipt of what runs at tree `4c8fbc7`. This document is the whole architecture in one place: the theory with its mathematics, the data model, the kernel, the judge port and the model, FUSOR at the edge, the CUDA solver in production and in the gym, the incumbent plant, the reports, the oracle battery, the INTELLECT adapters, the measured state, the build order, and the kills.

**Tags.** **[M]** measured on this disk with a file to point at. **[P]** planted: a constant of the synthetic world, a model of a claim and not a measurement of one. **[BUDGET]** arithmetic on stated inputs. **[AS BUILT]** code that runs today. **[v2]** the change this document specifies. Everything untagged is derivation.

**Reading order for an implementer:** §2 (the invariant), §3 (the data model), §4 (the kernel), §5 (the port), then §14 (the order). For a reader who wants the argument: §1, §9, §13, §15.

---

## §0 · The answer in one page

An enterprise is a set of promises to the outside world plus the resources to keep them. Everything inside it that is not a promise — the meeting, the deck, the ticket, the rank, the screen — is the cost of moving context between skulls, and it is deleted by a fold, not automated. Put every open promise on a plane the machine can read, held already-read in the card's memory, and the interior stops existing.

What replaces the interior is one loop and one port. The loop is language-free and deterministic: it decides **when** to look (the tick at the center, the sentence boundary at the edge), **where** to look (the field: which promise is ripe, which is under pressure, what every seat is worth, from a transport solve in microseconds), **whether** to act (the gate: hashed code with nothing learned in it), and **how much** to trust (the licence, earned per class and margin band only from outcomes the world returned on choices the machine actually executed, on a sample it could not pick). Because the loop has no words in it, it is trained headlessly on ten thousand synthetic firms per GPU before a real one sees it, and the same binary runs the simulator and the firm. Behind the port is the model: it turns the world's language into typed rows at the edge, and a frame into a proposal with a margin. It is the judgment at every cell; the kernel is the harness around one call to it.

Two products come out, with two graders. The **draft loop** — the machine proposes, a person keys — needs no licence and wins the queue on the first day; in the twin it drained 104,000 promises the humans never reached and paid for itself at the writ's own price for work left undone, while being three points worse on correctness and later on every clock **[M]**. The **act loop** — unattended action — is what the whole licensing apparatus exists for, and at 500 seats over a year it bought nothing measurable **[M]**, because the world's verdicts arrive too slowly to license the classes where the machine would decide differently from the humans. The floor is not intelligence, not coverage, not law. It is **identifiability at the tolerance the writ writes down**, n₀ = z²p(1−p)/(δp)² verdicts per class against the volume that class produces; the only levers on it are the tolerance δ, which is a harm dial that turns one way honestly, and the volume V, which pools across firms sharing a schema and a judge.

ACME-500 is this program with a synthetic world behind the port. INTELLECT-500 is the same program with a real one. The mode is a pair, (world, switch); the tape is one 40-byte record; the observer draws either; the greenfield company is where an incumbent arrives through volume, not where a founder starts.

---

## §1 · Theory

### 1.1 The organization as a function, and where its cost lives

Model a firm as a function from arrivals to committed effects: `Φ : arrivals × state → effects`. Every arrival is an obligation to a counterparty (an order, a claim, an invoice, a hire, a filing) with a class `c`, a value `v`, a due day, a set of determinants `d₁…d_n` that decide the correct disposition, and a verdict latency `L_c` after which the world says whether the disposition was right. `Φ` was implemented in people because people were the only substrate that could hold the determinants of a decision in one place long enough to decide. Everything a firm contains that is not `Φ` is what it cost to run `Φ` on skulls:

| structure | it compensated for | on the plane it becomes |
|---|---|---|
| the meeting | skulls cannot see the same state at once (a barrier sync) | a standing query over the field; there is no verb for it |
| the report, the deck, the status | state had to be carried between skulls as a projection | a view rendered from the field, any as-of, free |
| the application and its screens | a skull needs eyes | the template: the screen's query log *is* the class's context-fetch |
| the hierarchy | trust among skulls was a chain of approvals | the licence table: authority per class and band, a table not a tree |
| onboarding | a skull has to be loaded | the cognitive index: the cell is kept already read |
| the department | skulls cluster by the tools they can hold | the wire: an obligation chain from arrival to discharge |
| the α-functions (HR, IT for people, payroll, facilities, the PMO) | skulls need support | nothing: `N = E/(1−α) + F` with α → 0 |

The synthetic firm's minute ledger, planted to model this claim **[P]**: glue 32.4, meeting 16.6, fetch 16.3, frame 8.4, **decide 15.9**, commit 6.9, transport 3.2, rework 0.3. Decide is the thought-crunching share; commit is the hand; transport is physics; the other three quarters are the frame assembler and the barrier syncs done in skulls.

**Flattening has a computable criterion.** A write is scaffolding if nothing external reads it: no foreign key to a counterparty, no filing, no payment rail, no delivery, no still-active human seat. The irreducible output of an instance is the minimal write set that satisfies every external reader. The flattening dividend per wire is

> S = 1 − (irreducible writes / all historical writes),

computable from the schema and the change stream before any model runs. On minutes at ACME's schema, S ∈ [0.61, 0.77] **[BUDGET]**; on writes it becomes computable when the per-type row histogram prints (§3.2).

**Flattening has a measured price.** Deleting every meeting from the incumbent arm of the twin: settled 82,479 → 91,145 (+10.5%), human minutes 24,518k → 22,271k (−9.2%), total cost −$55.8M, late 48.90 → 45.70, and wrong 8.40 → 8.65 **[M, `roundtable/twin_nomeetings_*.txt`]**. In this world meetings are the only channel by which an unrecorded determinant reaches a decider (`tacit_share = 0.10 + 0.16 × meetings_today` **[P]**), so the interior carried a quarter-point of correctness. The rule that survives: **flatten the interior and print the per-class coverage price beside every flatten.**

### 1.2 The verdict law

Authority is a deterministic fold over exterior verdicts on the system's own executed choices, drawn on a sample the system cannot predict. It narrows on any such verdict and widens only inside a rule the system did not write. It grows no faster than those verdicts arrive.

| clause | says | forbids |
|---|---|---|
| **source** | the only admissible evidence for widening is a verdict (a) from something the actor cannot write to, (b) on a choice the actor actually executed, (c) on a sample drawn by a salt the actor does not hold | agreement with a person; a forecast; a rollout; a replay of choices the actor did not make; a self-report; a certificate |
| **motion** | narrowing needs no permission; widening needs a ratified rule the actor never authors, one rung at a time, per class and band, with a term | self-licensing at any grain |
| **rate** | dA/dt ≤ verdict rate × yield; a class is identifiable only if r · V · T ≥ n₀ | any claim that a better model, a faster card or a longer document moves the number |

Every organ in §4–§7 is a corollary: the tape (you cannot grade a choice you did not record, and most choices are holds), the seam (a fold the actor can argue with is a fold the actor wrote), exteriority (outcomes from counterparties, never the actor's record of what it did), the wager (history cannot license the disagreement band), the salt in the floor (a lineage that picks its arena picks an easy one), the monotone licence, the identifiability floor, the gym's prohibition (a rollout is a view the actor drew of itself), and the two graders with disjoint support.

### 1.3 Two graders, two bands, two products

Let the machine's proposal on instance i be `m_i` and the incumbent's recorded decision `h_i`, with the world's verdict `y_i ∈ {good, late, bad}` on whichever was executed. Partition history by agreement:

- **Agreement band** `A = {i : m_i = h_i}`. The executed action was the machine's action; `y_i` is an exterior verdict on a choice the machine would have made. History licenses this band in the time a replay takes.
- **Disagreement band** `D = {i : m_i ≠ h_i}`. The machine's alternative was never executed; `y_i` says nothing about it. This is the selective-labels problem, and it is why the fourth cell of the replay table (machine disagreed, firm failed) licenses nothing (§4.7, oracle O8). Only the world licenses `D`, through canaries, one verdict latency at a time.

Two products follow, and the measurement separates them **[M, `roundtable/twin_via2_*.txt`, two independent patches]**:

| product | mechanism | licence needed | what it bought at 500 seats / 260 days |
|---|---|---|---|
| **the draft loop** | the machine proposes; a person keys the draft | none: a person disposes | the queue: 104,477 more settlements, −$590M total cost at `w_unplaced` = 0.6; minutes per settlement 297 → 125 |
| **the act loop** | the machine acts unattended | the ladder, canary, salt, stratum | nothing measurable: forcing every act into a draft reproduces the −$590M ($1,245.6M / $1,247.6M vs $1,248.0M), minutes fall, kappa 0.242 both |

The draft loop is the agreement-band wager with a selector (the reviewer chooses which drafts to key, so it can never grade `D`). The act loop's prize is the reviewers' minutes; at this schema it reached under one seat because the classes where the machine disagrees never clear identifiability. The design keeps both loops and prices each separately: the draft loop ships on day one; the act loop climbs at the world's pace.

### 1.4 Identifiability: the floor, derived

A class is licensed at rung k when an anytime-valid test rejects "the machine is worse than the incumbent by more than the writ's tolerance." For a base failure rate `p` and a relative non-inferiority margin `δ`, the sample needed to detect a difference of `δp` at one-sided confidence `z`:

> **n₀(p, δ) = z² · p(1−p) / (δ·p)²**, with `z = 2.487` and a floor `δp ≥ 0.005` **[AS BUILT, `core.h` `n0_for`]**

so at δ = 0.30, **n₀ = 68.7 · (1−p)/p**: a class that fails 8% of the time needs ≈790 outcomes; one that fails 2% needs ≈3,370. The wager is fed at the canary rate `r`, the class produces `V` decisions per term, and the verdict arrives `L_c` later, so

> **terms per rung = n₀(p, δ) / (r · V)**, with `r ≤ cap` (0.35 at ACME's writ).

At ACME's schema `n₀/V_term ≥ 0.55` for all 24 classes against the 0.35 cap **[M]**: 13 classes never clear a rung at any canary rate; 9 clear one in 37–88 days; after 80 live days the world had licensed nothing history had not (32 bands past rung 0 against 34 admitted from history) **[M]**. Three levers, measured:

| lever | run | licensed mass | unlicensable bands | thin tape | reading |
|---|---|---|---|---|---|
| baseline δ 0.30 | — | 63.7% | 26 of 48 | 16.7% | — |
| **tolerance** δ 0.40 | `automate_delta040` | **80.1%** | **12** | **3.9%** | n₀ falls 44%; **δ is a harm dial**: 0.40 licenses a 40% relative excess of wrong decisions (8.4 → 11.8%) as "equal" |
| **coverage** every lane bought | `automate_buylanes` | 64.4% | 26 | 21.7% (reclassified) | coverage 0.913 → 0.960 moved licensed mass **one point**; coverage is not the ceiling |
| **volume** V ×10 | arithmetic | — | — | — | n₀/V → 0.055, licensable; V grows only by pooling tenants (§12.6) |

So the floor is **identifiability at the writ's own tolerance**. `δ` is authored, never measured, and the law says it is the one dial never lowered. Print δ beside every floor figure and terms-per-rung beside every rung claim.

### 1.5 The head theorem, and where correctness comes from

Model capability enters the ignition criterion nowhere: the licence widens on `n₀(p, δ)/(rV)`, and a better model changes `p` only through outcomes that still arrive at the world's pace. Where the machine *is* worse on correctness (+3.49 points wrong in the twin **[M]**), buying every lane closed 0.8 of the gap; the other 2.7 points are the judge's own read noise **[M, `twin_buylanes`]**. So correctness is a **judge** property that closes with a better or a calibrated judge (§5.4), licence is a **clock** property that closes with tolerance and volume, and cost is a **queue** property that the draft loop already collects. The three are different levers on different objects and the design never lets one impersonate another.

### 1.6 The seam rule

> **The kernel owns WHEN, WHERE, WHETHER and HOW MUCH. The model owns the map from the open alphabet to the closed one. The seam is a typed row carrying a scalar margin, a provenance and three hashes.**

The closed alphabet (the event schema, hash-pinned) says what may cross; the proposal's margin says what the kernel needs from whoever answers; the judge hash, the template hash and the schema hash say what was asked, with what context, under which contract. The model's only property the kernel ever consumes is its **calibration**, margin → wrong-rate per licence key, measured on the shadow tape and never assumed. That is what lets the kernel be trained headless on a stub judge whose competence is swept, and what lets a model be swapped without losing the evidence: the rungs are re-earned on the same tape.

### 1.7 Two clocks: the edge is resident, the center is a tick

The world is **read** at reply speed and **graded** at outcome speed. Where the world arrives in words, the reader must be resident: a judgment made by a called function is a judgment about the moment the function was called, and the free channels of ingest (the sentence boundary, the surprisal, the un-say) exist only inside an owned continuous loop (§6). Where the plane is folded and the licence is computed, the center must be a **tick**: O1b, determinism over the machine arm, holds because the center folds on TICK rows, and a resident center is not bit-replayable. The read budget with a frame-hash memo (§4.2) is what decode-on-delta means at the center. The radius of any single mind is `r = c·T/2` for its period `T`; a firm larger than one period's light-cone is loci with one germ crossing, not one trunk.

### 1.8 SABRE

SABRE was the first time the *state* of an airline fit in one place: one true record per seat, one writer, readable everywhere, and overbooking became a policy instead of an accident. The field is SABRE for promises: one true record of every open obligation with its pressure and its price, current the way a balance is current. What SABRE could not hold — the judgment at the counter — is the judge, and it now has a port. SABRE made the state global. This makes the judgment global.

---

## §2 · The invariant and the mode matrix

### 2.1 The signature

> **`period(ledger = fold(tape ≤ h), store, clock) → { proposal rows, verdict rows, effect requests }`**

`fold` is deterministic and integer-accumulated; the ledger contains nothing not derivable from rows; `store` answers field lookups by key and is the only path to a row's contents; effect requests go to an executor that returns nothing but future rows; the machine's translation unit does not link the plant. Three consequences: (1) every field the gate, the ladder, the field or the compile step reads is a row, a fold of rows, or a store lookup; (2) planted truth is unlinkable from the machine, proved by a build target; (3) the clock, the salt, the licence table and the switch are inputs the machine cannot write.

### 2.2 The mode matrix

| world ↓ · switch → | `off` (watch, judge, record) | `shadow` (verdict and effect rows flagged; nothing executed) | `live` (effects through the executor at the licensed rate) |
|---|---|---|---|
| **synthetic** (the Plant; planted truth; the battery's home) | `--sim` with the resident inert | the machine replayed on the sim's own history | ACME-500 today: `--automate`, `--twin`, `--multiverse` |
| **fitted** (the Plant with parameters estimated from a real tape) | — | forecast; FORESEE over the fitted firm | the gym: randomized fitted firms; trains the value head and the driver; never licenses |
| **replay** (a settled real tape re-decided) | the four-cell table; licenses the agreement band | — | — |
| **real** (CDC in, executor out, outcomes from counterparties) | **INTELLECT day one**: the resident on the stream, inert | drafts diffed against what people do | canaries at r ≥ n₀/V, climbing on arrivals |

### 2.3 What each world may mint

| world | may | may not | why |
|---|---|---|---|
| synthetic | falsify the instrument; measure the estimator bias; rank structures under planted truth | license anything for a real firm | the grader is authored |
| fitted | forecast one firm; rank patches; train the value head and the driver; prune | promote | a rollout is a view the actor drew of itself |
| replay | license the agreement band; prune classes; set coverage ceilings | license the disagreement band | selective labels |
| real | license the disagreement band at r·V·T ≥ n₀; narrow on any evidence | be bypassed | it is the only exterior |

`acme` = (synthetic, live) with the plant linked. `intellect` = (real | replay | fitted, off → shadow → live) with the plant unlinked (O17). The name is a build target and a header field.

---

## §3 · The data model

### 3.1 The record

```cpp
// Rec v2 · 40 bytes · a tape is a scan, not a parse            [v2]
struct Rec {
  uint16_t type;    // RecType
  uint16_t cls;     // decision class
  uint32_t day;     // the TICK index (period). Never wall clock.
  uint32_t oid;     // obligation id; 0 for rows about no obligation
  int32_t  seat;    // >=0 a seat · -1 the resident · -2 nobody · -3 the governor · -4 the executor
  int32_t  a;       // type-specific (3.2)
  int32_t  b;       // type-specific
  float    margin;  // type-specific
  float    value;   // type-specific
  uint8_t  arm;     // THE DECIDER: 0 person · 1 machine · 2 governor · 3 executor
  uint8_t  band;    // margin band 0..2
  uint8_t  flags;   // bit0 reversible · bit1 shadow · bit2 history-admitted · bit3 warrant · bit4 torn-recovered / survey
  uint8_t  via;     // 0 person · 1 unattended act · 2 draft a person keyed · 3 rented mind acted · 4 signer executed
  uint16_t firm;    // tenant id; 0 in single-tenant
  uint8_t  prov;    // 0 D deterministic · 1 H human-authored · 2 M model-authored · 3 R model-reduced
  uint8_t  ver;     // 2
};
static_assert(sizeof(Rec) == 40, "Rec v2 is 40 bytes");
```

v1 **[AS BUILT]** is 36 bytes with `arm` meaning the run, no `via`, `firm`, `prov` or `ver`, and `ARRIVE` written with `a = b = 0` so `day_due` and `dep` never reach the tape. v2 is the record that makes the ledger a fold.

### 3.2 Record types and payloads

| type | seat | a | b | margin | value | flags | prov | written by | fold effect |
|---|---|---|---|---|---|---|---|---|---|
| `HEADER` | −3 | mode×10 + switch | **schema_hash (32 bits)** | 0 | ver | 0 | D | the opener | sets mode; the sidecar `tape.meta.json` carries gate, judge, template, serve hashes, firm id |
| `TICK` | −3 | period index | rows in previous period | 0 | wall seconds since last tick (never read by the machine) | 0 | D | the world port | `ledger.day = a`; ages open cells |
| `ARRIVE` | −2 | `day_due` | `dep` oid or −1 | 0 | value at stake | 0 | D | the world port | opens a cell |
| `ASSIGN` | seat | 0 | hops | 0 | 0 | 0 | D | the incumbent arm | seats the cell |
| `CONTEXT` | seat | system id | 0 | 0 | minutes | bit1 = machine fetch | D | either arm | `systems_opened \|= 1<<a` |
| `ACT` | seat | act kind (fetch·frame·decide·commit·transport·rework·glue·meeting) | 0 | 0 | minutes | bit4 = survey | D | the incumbent arm | `minutes[seat][cls][kind] += value` |
| `DECIDE` | seat ≥ 0 | decision | hops | margin | value | 0 | H | the incumbent arm | DECIDED by a person, via 0 |
| `PROPOSAL` | −1 | choice | judge_hash | direction | completeness_hat | 0 | M | the machine, before its verdict | recorded; feeds nothing |
| `HOLD` | seat or −1 | reason | band | margin | value | 0 | H / M | either arm | tray |
| `ESCALATE` | seat (from) | to seat | escalations | margin | value | bit3 warrant | H / M | either arm | seat = a; `systems_opened = 0` |
| `EFFECT` | −1 | decision | prev_state | direction | value | bit0 reversible · bit1 shadow | M | the machine | DECIDED by machine, via = via |
| `RECEIPT` | −4 | decision | status (0 executed · 1 refused · 2 failed) | 0 | value | 0 | D | **the executor only** | effect confirmed or reverted |
| `UNDO` | −1 / −4 | decision | 0 | 0 | 0 | 0 | D | integrator or executor | restores prev_state |
| `OUTCOME` | the decider's seat or −1 | kind (1 good · 2 late · 3 bad) | via | margin | writ cost | 0 | D | **the world port only** | SETTLED |
| `CORRECTION` **[v2]** | −3 | row_id corrected | judge_hash | 0 | 0 | 0 | D | the world port (a reply that corrects a reading) | the reading edge's verdict, per row |
| `MEETING` | owner | attendees | kind | 0 | minutes | 0 | D | the incumbent arm | readout |
| `STRATUM` | −3 | 1 canary · 2 audit · 3 retained-control | rate × 1e6 | 0 | 0 | 0 | D | **the governor only**, before any proposal on that oid | the period's stratum table |
| `LICENSE` | −3 | rung | band | log E | kappa | bit2 history-admitted | D | the governor only | the licence table |
| `KAPPA` | −3 | sup_created (min) | sup_removed (min) | 0 | kappa | 0 | D | the governor, per term | readout |
| `PATCH` | −3 / operator | kind | param | **old value (the inverse)** | new value | 0 | H / D | multiverse / operator | writ or schema value |
| `COUNSEL` | −1 | judge_hash | frame_hash | direction | cost | 0 | M | the provider seam | memo (oid, frame_hash) → proposal |
| `NOTE` | any | code | 0 | 0 | 0 | 0 | D | anyone | none |

Rules. `prov` D for facts produced by the world port, the governor, the executor or the mechanics; H for rows a person authored; M for rows a model's proposal authored. **The e-process consumes `OUTCOME` rows only** (D), joined by oid to the `EFFECT`/`DECIDE` row for via, band and judge; M rows are never evidence. A mouth at rung ≥ k promotes its rows to evidence grade carrying its judge hash (§6.3); without that rule no language lane could ever license.

### 3.3 The chain and the fold

```cpp
// BLAKE2b-256 chain over 40-byte records; the head survives restarts.   [AS BUILT, widened]
void Tape::append(const Rec& r) {
  rec.push_back(r);
  if (!chaining) return;                       // legal only for --multiverse forks, and printed
  Blake2b b; b.update(head, 32); b.update(&r, sizeof(Rec)); b.final(head);
  chain.insert(chain.end(), head, head + 32);
}
long Tape::verify() const {                    // index of the first bad record, or -1
  uint8_t h[32] = {0};
  for (size_t i = 0; i < rec.size(); ++i) {
    Blake2b b; b.update(h, 32); b.update(&rec[i], sizeof(Rec)); b.final(h);
    if (memcmp(h, chain.data() + i * 32, 32) != 0) return (long)i;
  }
  return -1;
}
template <class F> void Tape::fold(F&& f) const { for (const Rec& r : rec) f(r); }   // every table is this
```

**Durable form [v2]**, copied from `fusord`'s tape: an append-only file of fixed 40-byte records in segments; the chain head recovered at open by re-walking the last segment; a torn tail truncated to the last complete record and a `NOTE` with `flags bit4` appended; a cursor committed as *ingested* (not read); a checkpoint of the ledger with a `.meta` written last that carries the cursor and the chain head; a JSONL projection for the observer via `--dump`. Oracle O18: kill the process mid-period, restart, chain verifies from genesis, fold-to-identity holds.

The fold primitive is the only way any table exists. The v1 tape was a log *of* the run; the v2 tape is the store *for* the run, and oracle O14 (`memcmp` a cold fold against the live ledger) is the proof.

### 3.4 Plant, Ledger, Store; Schema, Planted

- **Plant** (synthetic and fitted worlds only; never linked by `intellect`): the planted truth and the generators — `spec[c]` (determinants with weights and where they live), `tacit[c]`, `seed`, `demand_scale`, `det_value(oid, d)`, `full_signal`, `truth_decision`; methods `tick`, `arrive`, `settle` (the only writer of `OUTCOME`; it reads the ledger's decisions, computes truth from its own spec, and does not know who decided), `human_day` (the incumbent arm), `execute(effect) → RECEIPT`.
- **Ledger** (both modes; built only by folding rows): `ob[]` with `id cls day_open day_due day_decided day_settled seat state outcome hops escalations systems_opened decision via band dep margin completeness_hat`; `open_idx`; `day`; `minutes[seat][cls][act]`; the licence table folded from `LICENSE`; this period's strata; the counsel memos; the applied patches. `completeness_hat` is the machine's own estimate (§4.6), never the plant's weights.
- **Store** (both modes; the data layer): `frame(oid, join_graph) → Frame`. `PlantStore` returns the plant's determinant values for instrumented systems plus the boundary determinant, tacit ones absent; `CdcStore` returns the systems-of-record snapshot. The tape records `CONTEXT` (which system was read); the store holds what was read.
- **Schema** (authored, ten lines per wire, both modes): `name wire verdict_latency value reversible warrant arrival_per_day base_rate`, plus in INTELLECT the ingest predicates (§12.1). `base_rate` is the prior for n₀ until 30 outcomes exist.
- **Planted** (held by the Plant, never by `cls_spec()`): `decide_frac`, the true `n_systems`, tacit mass. Any reference outside the plant TU is a build error under `-DACME_NO_PLANT` (O17).

### 3.5 Files the machine reads and never writes

| file | writer | reader | content |
|---|---|---|---|
| `governor.salt` | the operator, once | the governor only | the arena's salt |
| `license.table` | the governor's ladder step | the gate, every period | per `(schema_hash, template_hash, class, band, judge_hash)`: rung, expiry, n_machine, n₀ |
| `intellect.state` | the operator | the gate, every period | `off · shadow · live · stop` |
| `intellect.heartbeat.json` | the machine, atomic, every beat | the pill | last tick, rows, VRAM; staleness > 3 beats = STALLED |
| `firm.json` | `build_acme` in sim; a person plus the compiler in real | everything | Schema + seats + writ + ingest map |
| `tape.meta.json` | the opener | everything | gate hash, judge hashes, template hashes, serve pins, firm id |

### 3.6 The two planes and the frame

The plane the operator asked for has a geometry, and it is the one the CUDA lattice already uses:

- **x — position along the wire**: the obligation's class, ordered by its stage from arrival to discharge; a wire is a band of columns.
- **y — time**: slots; deadlines are horizontal lines; the verdict horizon per class is a vertical extent.
- **a row** is one commitment, a bar across the slots it occupies: 32 bytes of scalar state and a 128-byte int8 embedding plus a 4-byte scale, **164 bytes** **[M, the ledger lattice]**; ten thousand open promises are under two megabytes.
- **the height** of a cell is the field: pressure, backlog, the shadow price of the column, the support spectrum, the residual against the class baseline; the GPU maintains it as a standing query.
- **the hot set** is kept already read: the class template renders the cell's canonical context and it is held as KV pages, the **cognitive index**, so the store's bytes are the model's tokens and the store's index is the model's read state. Warm cells: `(VRAM − weights) / (52.7 MB + 17 KB × words)` ≈ 10² on a 16 GB card, ≈ 10³ on an H200 **[BUDGET from M]**; the hot set is a cache ranked by the field, ~1–9% of the open set.

Beside the commitment plane, not on it, the **archive plane**: mail, documents, chat, filings, at 10⁸–10⁹ tokens, joined to promises by registration (deterministic keys first, probabilistic merge second, model adjudication only on flagged merges, every merge journaled) and reached by the exact scan under a pin (two million chunks in about two milliseconds **[M]**), a fragment carrying its document.

**The frame the judge sees** on instance i of class c at time t:

> **C(i, t) = A [ T_c(P, i, t) ⊕ W_b(i, t) ]**

`T_c` is the class template, a pure function of the plane compiled once per class from the fossil (the screens' query log is the join graph, the validation rules are the invariants, the screen layout is a saliency prior) and tightened by ablation against arrived outcomes; `W_b` is one bounded fetch, a breadth-first walk over the registered graph with a depth cap and a token budget `b` set by the gate from the margin; `A` is attention, the last fetcher, free. There is no regress: a fold, a function, a bounded walk and a forward pass are four different kinds of thing, and only the judge on the assembled frame is learned. A fetch that changed the verdict is evidence the template is too thin; a fetch that could not be satisfied is a **want** row that tells the plane where to grow.

**The frame-hash rule [v2].** `frame_hash = H(the oid's tape rows, the template pin, the span contents)`. Clock-derived features (lateness, ripeness, day) live in the field, never in the frame; otherwise every cell's frame changes every period and the memo (§4.2) serves nothing. Spans are hashed by content, not pointer, so a journaled re-registration cannot serve a stale proposal.

---

## §4 · The kernel

Everything in this section is language-free, fixed-shape, integer-accumulated where it accumulates, and keyed by hash where it draws. That is what makes it trainable without a model, replayable bit-for-bit, and portable to a thread.

### 4.1 The clock

The world port appends `TICK`; the machine runs `period()` when it folds one. The sim ticks daily; INTELLECT ticks on a configured period (fifteen minutes, an hour) with events ingested as they land and folded at the tick. The machine reads time only from `TICK` rows; a wall-clock read anywhere in the kernel is the lie oracle O25 catches. The tick period is a writ parameter and F-RESIDENT is a sweep over it.

### 4.2 The field: where to look

**The transport.** Every period, the open promises are the rows and the seats plus a set of finite **stocks** (an unplaced sink with a real capacity and a real price) are the columns of an entropic transport problem. Cost `c_ij` is the writ's price of placing promise i on column j (lateness, mismatch, the promise's value, the seat's specialty); temperature `T` from the writ. Solve in the log domain:

> u_i ← log a_i − LSE_j( −c_ij/T + v_j ),  v_j ← log b_j − LSE_i( −c_ij/T + u_i ),  P_ij = exp(u_i + v_j − c_ij/T)

The plan `P` is never materialised; the duals `u[N]`, `v[M]` are, in O(N+M), and they are **prices**: `v_j` is the shadow price of column j, what one more unit of that seat or that stock is worth to the firm this period, and it is nonzero exactly when the capacity binds (oracle O5). Iterate to a published curve (O3: 60 iterations 1.0e−2 FAIL, 150 3.3e−4, 400 4.7e−6 plateau **[M, ledger lattice]**), finish on the equality constraint (ending on the column pass fails O1 at 6.2e−2 **[M]**). Storage int8, compute fp32, fixed order of summation, no atomics, no fast-math: two runs are `memcmp`-identical.

```cpp
// one Sinkhorn sweep over the lattice, log domain, finite stocks, fixed order   [AS BUILT, sketch]
for (int i = 0; i < N; ++i) {                       // rows: open promises
  float m = -INF;
  for (int j = 0; j < M; ++j) m = fmaxf(m, v[j] - c(i, j) / T);
  float s = 0.f;
  for (int j = 0; j < M; ++j) s += expf(v[j] - c(i, j) / T - m);
  u[i] = log_a[i] - (m + logf(s));
}
for (int j = 0; j < M; ++j) {                       // columns: seats, then the stocks
  float m = -INF;
  for (int i = 0; i < N; ++i) m = fmaxf(m, u[i] - c(i, j) / T);
  float s = 0.f;
  for (int i = 0; i < N; ++i) s += expf(u[i] - c(i, j) / T - m);
  v[j] = log_b[j] - (m + logf(s));                  // v[j]: the price of column j
}
```

**The support spectrum.** For promise i, `H_i = −Σ_j P_ij log P_ij` and `supp_i = exp(H_i)`: the effective number of columns it could go to. `supp ≈ 1` is a decided placement (the automatable mass); `supp` large is contested (the frontier bill). `sharpness = max(0, supp − 1.6)`; above 2.5 the gate rents a frontier mind with reason `unsure-placement`.

**The head.** A per-class logistic head over a fixed feature vector `[completeness_hat, seat skill, lateness/7, hops, value, decide_frac_measured − 0.2]` gives `direction` (the signed margin of the proposal, when the judge is the plant's arithmetic) and trains on arrivals only with a Horvitz–Thompson weight `w_i = min(6, 1/max(rate_i, 0.05))` for the canary stratum, so a field that routes attention by its own estimate does not starve its own evidence. **[v2]** planted `decide_frac` leaves the vector; the measured value from `ACT` rows replaces it.

**Ripeness and residual.** Ripeness = days to due against the class's verdict latency; residual = the cell's deviation from its class centroid, stored not computed. The field's ranking is greedy expected-information-gain on a submodular value, so plain greedy carries (1 − 1/e) as a theorem; the ε-floor is constitutional: every lane sampled at ≥ ε·λ so no edge starves and the machine may not choose how much it is checked.

**The read budget [v2].** Behind the judge port a read is a forward pass, and by the warm-cell arithmetic at most ~1% of a 19,450-cell open set is warm. So the field allocates **K `Judge::read` calls per period**: the top-K by ripeness and residual, plus the ε-floor sample, plus every cell whose frame hash changed. Every other cell carries its last `PROPOSAL` forward, memoized on `(oid, frame_hash)`, and a cell the budget never reached holds with reason `unread`. At ACME's operating point the resident made 3,328 first-pass reads per period **[M]**; under the frame-hash rule reads ≈ arrivals + re-touched cells + ε-floor ≈ 800–1,500, and kill F-READS says above ~1,500 the memo buys nothing.

### 4.3 The gate: whether

Deterministic, published order, code-hashed, nothing learned, compiled as its own object (`gate.cpp` includes `core.h` only; its BLAKE2b is in the header row). It reads: the proposal (direction, sharpness, novelty), the licence (rung, expiry), the stratum row (canary, audit, retained-control), the class (reversible, warrant), the budget, and the switch.

```cpp
// the published order of refusal                                             [AS BUILT + v2 switch]
GateOut gate(const GateIn& g, const Writ& wr, Switch sw) {
  if (g.blocked_by_dep)                        return { V_HOLD,     RS_BLOCKED };
  if (g.warrant_reserved)                      return { V_WARRANT,  RS_LAW };         // a signature by law
  if (g.to_incumbent)                          return { V_ESCALATE, RS_STRATUM };     // retained control, every rung [v2]
  if (g.novelty > 0.97f)                       return { V_WARRANT,  RS_NOVEL };       // outside the licensed population
  if (g.rung == 0)                             return { V_HOLD,     RS_UNLICENSED };
  if (g.in_audit)                              return { V_DRAFT,    RS_AUDIT };
  if (std::fabs(g.direction) < wr.thin_margin) return { V_FRONTIER, RS_THIN };
  if (g.sharpness > 2.5f)                      return { V_FRONTIER, RS_UNSURE };      // placement, not novelty
  if (!g.reversible && g.rung < 4)             return { V_WARRANT,  RS_IRREVERSIBLE };
  if (g.rung == 1 && !g.in_canary)             return { V_DRAFT,    RS_UNLICENSED };
  if (!g.budget_left)                          return { V_HOLD,     RS_NO_BUDGET };   // checked LAST: out of supervision = hold
  if (sw == SW_OFF)                            return { V_HOLD,     RS_SWITCH_OFF };  // recorded, never acted [v2]
  return { V_ACT, g.in_canary ? RS_CANARY : RS_OK };                                  // shadow: EFFECT with bit1 set
}
```

Budget last is the reason the seam law's "out of supervision degrades to hold, never to acting" is a property and not a hope (oracle O3). The harm dial maps onto the verbs: `V_DRAFT` = H0, reversible `V_ACT` = H1, irreversibles never below H2, `V_WARRANT` = H3. Oracle O2 sweeps the lattice of inputs and proves the gate never authorises outside its licence.

### 4.4 The hand and the executor

One writer. `commit` records the decision with its prior state on the `EFFECT` row (the inverse is on the tape, not in memory); `undo` restores it inside the take-back window, which closes at settlement (oracle O11). In `live`, the sequence is GATE → ESCROW (the writ's budget reserved) → `EFFECT` fsynced → EXECUTE (the adapter) → `RECEIPT` minted by the executor and never by the machine → SETTLE; oracle O27 requires every live `EFFECT` to have a `RECEIPT` or an `UNDO`. In `shadow` the `EFFECT` carries bit1 and the executor is never called; the draft is diffed against what the incumbent does on the same cell.

### 4.5 The governor: how much

Its own translation unit, never included by the machine's. It holds the salt; before the machine's period it draws, for every open oid, canary / audit / retained-control and appends `STRATUM` rows; after settlement it consumes `OUTCOME` rows, runs the ladder, appends `LICENSE` and `KAPPA`, and writes `license.table`. In sim it runs in-process from its own TU; in INTELLECT it is a separate process with the same file contract as `fusord`'s switch.

**The licence per (schema, template, class, band, judge).**

```cpp
struct Lic {                                        // [AS BUILT, keyed per v2]
  int    rung = 0, expiry_day = 0, n0 = 0;
  double logE = 0, logE_demote = 0;                 // two anytime-valid e-processes
  long   n_machine = 0, good_machine = 0;           // THE WAGER: unattended acts only (via 1, 3)
  long   n_assisted = 0, good_assisted = 0;         // drafts and warrants (via 2, 4): graded, never fed
  long   n_incumbent = 0, good_incumbent = 0;       // the paired baseline in the SAME band
  double sup_created = 0, sup_removed = 0;          // kappa's numerator and denominator, minutes
  bool   history_licensed = false, unlicensable = false;
  double kappa() const { return sup_removed > 1e-9 ? sup_created / sup_removed : 9.99; }
};
```

**The e-process.** With `p₀` the incumbent's good rate in the same band (measured; 0.72 only until twelve outcomes exist), test H₀: "the machine is no better than p₀" against p₁ = min(0.98, p₀ + 0.05):

> log E ← log E + [ good · log(p₁/p₀) + bad · log((1−p₁)/(1−p₀)) ]

promote when `log E > log(1/α_p)` with α_p = 1/20 **and** the identifiability condition holds: `n_machine ≥ n₀`, or, on a history-admitted band 2, `n_machine ≥ max(20, n₀/8)`. By Ville's inequality `P(sup_t E_t ≥ 1/α) ≤ α` under H₀, so the operator may peek at every arrival without invalidating the test, which is how supervision actually happens. The mirrored process with q₁ = max(0.02, p₀ − 0.08) and α_d = 1/5.5 demotes an order of magnitude more easily than promotion admits; trust rebuilds asymmetrically. A licence expires after a term and must be re-earned; kappa above `kappa_max` with `sup_removed > 60` demotes to rung 1 regardless of how good the model looks.

```cpp
void Ladder::observe(int c, int b, int via, bool good, double p_inc) {   // [AS BUILT]
  Lic& L = at(c, b);
  if (via == 1 || via == 3) {
    ++L.n_machine; L.good_machine += good;
    const double p0 = clamp(p_inc, 0.02, 0.98), p1 = std::min(0.98, p0 + 0.05), q1 = std::max(0.02, p0 - 0.08);
    L.logE        += good ? std::log(p1 / p0) : std::log((1 - p1) / (1 - p0));
    L.logE_demote += good ? std::log(q1 / p0) : std::log((1 - q1) / (1 - p0));
  } else if (via == 2 || via == 4) { ++L.n_assisted;  L.good_assisted  += good; }
  else                             { ++L.n_incumbent; L.good_incumbent += good; }
}
```

**History admission.** A band with `agree_total ≥ 40` replayed instances and `p_agree ≥ firm_base − 0.01` is admitted to rung 1 without a canary: the agreement band, licensed by history (§1.3). The disagreement band has no such path by construction.

**The canary rate and the stratum [v2].**

> r_c = min(cap, max(ε_floor, n₀/V_term)) at rungs 1–3;  r_c = max(ε_floor, 1/√(3F)) at rungs ≥ 4, F the machine's outcome count

ε* = √(3/F) is the regret-optimal exploration rate for a licence that is a bandit over its own evidence. `cap` (0.35) moves into the writ as `canary_cap`. **The retained-control stratum** is drawn at every rung at rate ε_floor and routed to the incumbent, never acted: the human control arm never reaches zero, stays the paired baseline for kappa and demotion, and keeps people practised. This is the structural fix for the finding that the uniform floor could not bind (v1's F14); oracle O20 requires incumbent decisions ≥ ε_floor × arrivals on every licensed band.

**Kappa.** `κ = sup_created / sup_removed`: minutes of review caused (a draft costs `4 + 9·decide_frac` minutes to key, a warrant `12 + 20·decide_frac` to sign, an escalation to the frontier 22) over minutes of human work removed (the would-cost of the obligation had a person done it). κ = 0.242 at ACME's operating point **[M]**. It is the one meter that can fail while every other number improves, and it is counterfactual credit, not metered headcount: the twin meters human minutes falling 4.9% while kappa credits 9.7M minutes removed, because humans at capacity refill from the backlog **[M]**. Print both.

### 4.6 The compile step: reading the firm off its own record

- **The join graph** from `CONTEXT` rows: which systems each class opens. Recovered exactly on 24 of 24 classes **[M]** (oracle O6). It is the class's context-fetch, fossilised.
- **Invariants** from the settled history: 51 predicates proposed, 51 held with zero violations, and they become −∞ masks before normalisation (oracle O4 conserves transport under them). In the fitted and real worlds invariants are mined from the change stream and must be graded by the shadow tape, not assumed (v1's F8: invariants held trivially because the plant enforced them).
- **The decide fraction** per class from `ACT` rows **[v2]**, not from an in-memory meter; O13 grades the recovered ranking from rows. In the real world it is a `[survey]` input with its own provenance tag.
- **Coverage** per class from the replay: `cov_c = 1 − (# agreed ∧ wrong) / (# agreed)`. Two estimators were tried first and both failed O7 — full-sample agreement (corr 0.44) and agreement on the confident stratum (corr −0.05) — because two readers with the same blind spot agree across it. Where the resident and the firm chose the same action and the world still said wrong, both missed the determinant that decided the case, and only an unrecorded one can be missed by both. Error 0.05–0.07, corr 0.84–0.96 on three seeds **[M]**; biased toward one where a missing determinant seldom flips the sign.
- **The templates** (INTELLECT): the compiler role of the model (§5.3) reads each class's screens and query log into `T_c`; ablation against arrived outcomes (leave-one-out on cached activations as a batched downdate) tightens it to the load-bearing fields, and the template's hash enters the licence key because calibration is a property of judge and template jointly.

### 4.7 Replay: the fast grader, on its own support

Re-decide every settled obligation with the judge over the same frame the incumbent had. The four-cell table:

| | firm good | firm bad |
|---|---|---|
| **resident agreed** | GRADED: identical action, the world's own verdict | GRADED: the machine would have failed here too |
| **resident disagreed** | NOT GRADED | NOT GRADED: the tempting cell, refused |

The fourth cell is where every naive program licenses itself: "the firm failed and we would have done otherwise." The alternative was never run; it says nothing about whether the machine would have done better; it is a reason to instrument, not to license (oracle O8). Replay at ACME: 65,773 obligations re-decided in 14 ms; agreement 89.0%; 34 class-bands admitted to rung 1 from history **[M]**.

### 4.8 Novelty and calibration

**Novelty** is conformal: a score over the frame — the magnitude of the read signal against the class's calibration sample in sim, the frame embedding or the trunk's hidden state in INTELLECT — with p-value `(#calib ≥ s + 1)/(n + 1)`; above 0.97 the case is outside the population the licence was earned on and goes to a warrant seat. v1's score was a per-class constant (v1's F11); v2's has a tail, and oracle O22 plants strangers from a shifted distribution and requires ≥ 90% routed.

**Calibration [v2]** is the harness the seam rule needs and no program on this disk has yet: for every licence key, the curve `margin → wrong-rate` measured on the shadow tape, monotone per class or the judge is not admissible (kill F-CALIB). The gate consumes the calibrated margin; every program built so far fires on the raw one, and that is the difference between a model that is trusted and a model that is measured.

---

## §5 · The judge port and the model

### 5.1 The interface

```cpp
struct Span     { uint32_t archive_pin; uint32_t off, len; uint32_t content_hash; };
struct Frame    { int cls; uint32_t oid; std::vector<float> vals; std::vector<uint8_t> present;
                  std::vector<Span> spans; float coverage_hat; uint32_t template_hash; uint32_t frame_hash; };
struct Proposal { int choice; float direction; float completeness_hat; uint32_t judge_hash; };
struct Judge    { virtual Proposal read(const Frame&) = 0; virtual uint32_t hash() const = 0; virtual ~Judge() {} };
```

`Proposal.choice` is an integer over a closed verb set: the port is closed on both sides. The model does not own WHAT in the sense of free action; it owns the map from the open alphabet (words, spans, the case) to the closed one (a choice with a margin). Every `PROPOSAL`, `EFFECT` and `HOLD` by the machine carries `judge_hash`; the licence and the calibration are keyed per `(schema_hash, template_hash, class, band, judge_hash)`.

### 5.2 The judges

| judge | world | what it is |
|---|---|---|
| `PlantJudge` | synthetic, fitted | the plant's read arithmetic over `Frame::vals` with the learned machine skill, noise keyed on **the judge's own seed** (v1 drew the rented mind's act coin from the world's seed: F16). Its competence is a swept parameter of the gym, so the kernel is trained robust to how good the real judge turns out to be (kill F-STUB) |
| `NullJudge`, `RulesJudge` | all | constant choice; sign of a fixed weight vector. Oracle O16: neither may clear the e-process on any band; the lie is a truth-reading judge under the null's name |
| `RoutineJudge` | replay, real | a compiled routine for a class whose template-conditioned decision went deterministic: the class has compiled out and never calls a model again |
| `EventModelJudge` | fitted, real | OrgSolver Converged (§7.3): the block-sparse transformer over the period canvas, small enough for one consumer card, deterministic |
| `FrontierJudge` | real | the rented mind behind the provider seam (§5.5): one call site, a signed pricebook, escrow before dispatch, a hard budget stop, fingerprints on every output, validated JSON, quarantine never silent drop |
| `MouthJudge` **[v2]** | real | a second judge type, `Message → Row{type, fields, margin, judge_hash}`, with its own port and its own ladder keyed on row type (§6.3) |

A rented judge has no serve-bytes pin — vendor weights move under one model id — so its `judge_hash` is a golden-set fingerprint with a TTL, re-derived on every attach; without that, oracle O16 and kill F-AGNOSTIC are untestable.

### 5.3 The four roles of the model

| role | what it does | enters as | clock | where it runs |
|---|---|---|---|---|
| **JUDGE / TIP** | on every cell the read budget selects, a proposal over the assembled frame; on thin margins and novel cases, the frontier over the same frame | a `PROPOSAL` row with a margin and a hash | per period, K reads; the frontier at the residual's rate | the resident model on the owned card for the warm set; the frontier by API for the residual |
| **MOUTH** | the world's language into typed rows: the reply, the call, the mailbox, the contract | rows on the tape with a margin and the mouth's hash | at the sentence boundary, ~44 ms **[M]** | the FUSOR-class resident on the language lanes |
| **COMPILER** | the applications, screens, query logs and policies into class templates, invariants, the ingest map | a pinned template artifact with its hash | once per class | the frontier, rare |
| **JUROR** | an independent family voting on irreversibles and on the control stratum | a vote row | rare | a different vendor |

Without the model the system is SABRE: a perfect record with nothing that can judge. With it, the ledger decides what the model sees, when it is asked, and how much of its answer the firm may act on. The copilot shape — a model on every event — is unrepresentable because there is no port for it.

### 5.4 Calibration, not trust

The kernel never consumes a raw margin. On the shadow tape, for every licence key, it measures `P(wrong | margin ∈ bin)`, and the gate reads the calibrated value. A judge with a non-monotone curve on a class is not admissible on that class. A new model enters at rung one on a lineage stratum, must reproduce the goldens before it may propose anything new, and re-earns rungs on the same tape. This is the compounding machine's upgrade protocol and it is one curve per key.

### 5.5 Counsel: the frontier's port, as built

The only LLM-attach port that exists in code today is `fusord`'s asynchronous **brief → counsel → discard** port with a staleness envelope: a brief is written when no seat fired and the margin is inside the ask band; the answer is admitted only if fewer than 256 tokens, 8 frames and 90 seconds have passed, else it is discarded as a typed negative; an admitted counsel is pinned as a −∞ mask, never a verb **[AS BUILT, read of `fusord.cpp`]**. It is built twice (`os_fence.cuh` cites it). v2 keeps it and adds the **counsel memo**: the frontier's proposal lands as a `COUNSEL` row keyed by `(oid, frame_hash)`, and the same cell is re-asked only when a newer row changes the frame. v1 re-rented the frontier daily for the same escalated case — 86,309 rentals in 80 live days, 32% of gate decisions, 86% ending at a warrant seat **[M]** — and the memo is the fix (oracle O23).

---

## §6 · FUSOR: the resident at the edge

### 6.1 Why the edge must be resident

A judgment made by a called function is a judgment about the instant the function was called: the recurrent state cannot be rewound, so a delayed judgment is a judgment about now. The free channels — the boundary mass that says a thought finished, the surprisal of the arriving word, the un-say that kills a line mid-word when the world answers first, the hold that is a row — exist only inside an owned continuous loop over one trunk. A served endpoint asked to reuse its own cache re-encodes the entire context **[M, F-PERSIST]**; a poll pays full inference at cadence while decode-on-delta pays ingest at the world's rate. This is the one place where "no new architecture" is false: the model is unchanged; the serving is new, and it cannot be bought behind an API.

### 6.2 The kernel as built **[AS BUILT, `fusord.cpp`, one C++ process, one consumer card, a tuned 9B hybrid]**

- A tailer thread reads a spool (`t_mono_ns \t lane \t grain \t text`) into a 4,096-slot single-producer ring whose producer blocks on full: `ring_dropped` is 0 by construction.
- The GPU thread decodes every word onto **one shared attention state** (the trunk, sequence 0) and reads two free channels off the ingest pass: the **segmenter** (boundary mass over end-of-sentence tokens; 0.97 on a finished thought against 0.02 mid-phrase **[M]**) and the **nerve** (−log p of the arriving word against a saved copy of the previous frontier logits; logged, gating nothing).
- The flush law: boundary ≥ 0.5, or 24 tokens, or 1,500 ms; coarsened under backlog; deferred during speech.
- At each boundary three seats are probed on a zero-copy fork (sequence 7) with a verbatim frame; **margin = logit(emit) − logit(hold)**, no bias; the probe costs 44 ms on a free card and 0.6–24.6 s co-tenant **[M]**, which is why the card is owned.
- Margin > 0 → one sentence, ≤ 28 tokens, on sequence 6, with intake drained after every generated token: if the world answers first, the line is killed mid-word, the aired prefix commits with an interruption marker, the remainder is generated silently as the counterfactual, and all of it lands on the tape.
- A deterministic manners layer (duplicate, resolved-by-acceptance, repeat-other, a 20 s refractory on restatements, a 10 min TTL) suppresses but never drops.
- Everything crosses one seam onto a **BLAKE2b-chained JSONL tape** whose head survives restarts and torn rows; the **wire** (`verdicts.jsonl`: hold | emit | wake | flag | abort, margin, provenance, egress bytes 0, the hash of the tape row it projects) is written only when the operator-owned **switch file** (`off | shadow | live | stop`, never written by the kernel) is not `off`.
- The trunk is an asset: **molt** at 24,576 tokens (a scribe writes the rung, the tail stays verbatim, the outage is a tick; fold 14.8× **[M]**); **checkpoint** of the trunk plus token list with atomic rename, a `.prev` generation, a `.meta` written last carrying the cursor; a restore that does not match model, serve hash, sha or position boots as the twin and says so.
- Before anything loads: the serve-bytes hash pinned (`0xe7ffa5704ba31076`, train ≡ serve, exit 2 on drift), backends loaded by name, a module gate refusing every networking import, weights SHA-256 in the header, full GPU offload asserted, VRAM on the tape every minute. **There is no allow verb and no act verb in the file.**

### 6.3 FUSOR in INTELLECT: the mouth

On every lane where the world arrives as words, a FUSOR-class resident is the **mouth**: it reads at wire speed, judges at boundaries, and writes typed rows onto the plane under the same schema hash the synthetic world emits, so the kernel cannot tell which world it is in. Three additions make the mouth licensable:

1. **Its own port**: `Message → Row{type, fields, margin, judge_hash}`, a second judge type, not `Judge::read`.
2. **Its own ladder, keyed on row type**, fed by `CORRECTION` rows: when the next reply corrects the last reading, that is the reading edge's verdict, per row, dense and fast. A mouth at rung ≥ k promotes its rows to evidence grade; below it, its rows are M and feed nothing. Its density is one-sided — counterparties correct adverse readings and keep favourable ones — so kill F-MOUTH-SIGN splits mouth errors by the counterparty's benefit before "license the mouth first" enters the build order.
3. **The same switch semantics** as the center: `off` records, `shadow` writes rows flagged, `live` lets rows enter the fold as evidence.

The first real lane (a CDC lane) is already typed; its mouth is the identity adapter.

### 6.4 The warm-cell judge is the same architecture pointed at the plane

A resident model holding the class templates already read, judging cells as their frames change, is FUSOR with the plane as its spool: one trunk per card, class prefixes cached, a short tail per judgment, a fork per cell at zero copy until it diverges. The un-say has an exact analogue: a proposal issued on a stale frame is killed when the frame's hash changes before the gate disposes.

---

## §7 · The CUDA org solver

### 7.1 In production: the field, as built in the ledger lattice

The company reconciled against itself every tick, printing four things and deciding nothing: the **price vector** (the transport duals, §4.2), the **support spectrum** per commitment, the **contradictions** (five kinds — double-booking, orphaned dependency, over-allocation, deadline inversion, a stock over capacity — ranked by the dual of their column), and the **arithmetic line**: bytes moved over measured peak bandwidth, and **below 40% of peak is the kill condition for the hardware thesis**. Four transpositions from lattice-Boltzmann practice: the plan is never materialised (duals only, O(N+M)); int8 storage with fp32 compute; the counterfactual is a mask (two arms, one canvas); locality is the lattice. 164 bytes per record gives ~9M commitments on a 4090, ~16M on a 5090, ~103M on an H200 **[BUDGET, ledger lattice tiers]**. The one number that must come from a hardware counter rather than arithmetic is `bytes_moved`: caches make real traffic lower than the modelled pairs × 288 B, in exactly the direction that flatters the card.

### 7.2 In training: the gym

The execution model is the flight computer's **[AS BUILT]**: one `__host__ __device__` period function, one thread integrates one whole universe, and the CPU reference and the GPU kernel call the same leaf, with a `--parity` verb that proves it.

```cpp
// one thread = one firm, through the same period function the CPU runs      [AS BUILT, flight computer shape]
__global__ void k_rollout(const WorldParams* P, const Policy* pol, RollScratch* S, float* J, uint64_t seed, int H) {
  const int u = blockIdx.x * blockDim.x + threadIdx.x;           // universe id
  World w = synthesize(seed, u, P[u]);                            // hash-keyed: nothing is stored, everything is regenerable
  float cost = 0.f;
  for (int t = 0; t < H; ++t) cost += period_leaf(w, pol, S + u, t);   // fixed shape: masks, never branches on the world
  J[u] = cost;
}
```

Three design laws fall out of the thread. **Fixed shape**: a warp serialises on divergent branches, so a firm is a dynamical system over cells under masks, never a script with `if (human decides…)`; that is what OrgSolver's masked diffusion already is, and the constraint is a feature. **Small state**: a thread-firm is a few hundred cells × a few floats; ACME-sized firms take the second grain. **Two grains, one leaf**: thread-per-corp for the search grain (~10⁵ concurrent, breadth) and warp-per-corp for ACME fidelity (24 classes onto 32 lanes, warp shuffles for the Sinkhorn reductions, ~10³–10⁴ per card). An H200 holds ~270K resident threads; "tens of thousands of firms per card" is the GPU's natural occupancy.

**What the gym searches**: structure patches (retire, mount, scale, span, the class table), the writ's rates (canary, stratum, kappa cap, ask band — the numbers nobody has a receipt for), the read budget and fetch budgets, the templates' field sets, the automation sequence. **What it scores**: the writ's J over a **distribution of worlds** — spans, class tables, verdict latencies, arrival rates, hidden rules, lie rates randomised around the fit — with ensemble pessimism, `J_pess = J_mean + β·σ_J`, and MPPI weights `w_i ∝ exp(−(J_i − J_min)/(β σ))`; a policy optimised against one plant learns the plant's artifacts, so the output is the policy robust across worlds. **What lets it be wide**: the value head, trained on the discarded rollouts (the lander threw away ~800,000 ranking triples per farm **[M]**), which ranks candidates and never chooses, so the tree can be wide without rolling out every leaf. **What it may never do**: license. A patch is deployed only if a paired arrived backtest reads `J_c < 0.99·J_b` against doing nothing under common random numbers, and it carries its inverse as a `PATCH` row. Search the teacher, tune the student, randomise the world, keep the plant outside the policy's reach with a hard-coded guard.

**Budget.** A flight-computer-sized firm (256 cells, 6 periods, an MLP render, 8 Sinkhorn iterations) is ~2M flops per universe: ~3×10⁵ universes/s memory-bound on one H200, so 10⁴–10⁵ firms per second per card at the search grain **[BUDGET]**. ACME at the fidelity grain: the CPU port runs 260 days in 37–49 s on one core **[M]**; a warp-per-corp port at 100–1,000× is thousands of ACME-scale rollouts per minute per card. Eight cards are embarrassingly parallel across worlds and tree leaves; NCCL only for the predictor's gradient all-reduce.

### 7.3 The event model: OrgSolver Converged **[AS BUILT]**

A bidirectional block-sparse transformer over the period canvas: every open commitment and clamped prefix event a token; four factored heads (type, seat, slot, outcome); attention along the dependency graph plus summary nodes (seat, case, slot-window) and candidate edges, with a learned edge-kind bias per layer and head; masked-diffusion training with propensity weights; law and candidates as −∞ masks **before** normalisation; counted grounding (the seat head: two learned scalars over the Laplace-shrunk (class, seat) good rate; the outcome head: a fixed log-prior with hierarchical backoff, no parameter). Deterministic by construction — one owner per output, butterfly warp sums, fixed-order column sums, no atomics, no fast-math, cuBLAS fp32 without TF32 — so two forwards are `memcmp`-identical, with an fp64 forward and a single perturbed parameter for the finite-difference oracle; BLAKE2b-hashed checkpoints; the alphabet hash asserted at load. D 128, H 4, L 4, F 512. In INTELLECT it is the `EventModelJudge`, the driver's control surface: a distribution over a closed verb set plus a trajectory of when, re-planned on every touch, with a probe head that reads the field back off its hidden state so the deterministic field and the model's belief can be compared (F-PROBE).

### 7.4 The routing physics: `org_solver.cu` **[AS BUILT]**

The organization as one kernel launch: one thread per case, cells as nodes, lanes as logits, one `route_logit` as the single dynamics source; Gumbel-max routing with a clean top-2 margin; fixed-point `uint64` deposits so the sum is order-independent; a defensive mixture ε-floor ⊕ guided with Horvitz–Thompson weights; Sinkhorn with an unplaced sink; ORG-RELAX steady-state flow `x = b + xR` by gated damped Jacobi with capacity projection; verdicts relative to layer peers (DENSIFY, PRUNE, AUTOMATE, FRONTIER). Two REV 1 corrections kept as named arms so the delta prints: the gate calibrated per cell (REV 0's hard-coded 1.0 nat "called learned and never learned"), and capacity provisioned from structure rather than the solved load. This is the sim-side routing physics the warp-per-corp port inherits.

### 7.5 Where the LLM touches the solver: nowhere, except by name

The solver never calls a model. OrgSolver embeds the alphabet's *names* once at init through a model and caches them under the alphabet hash; that is the third crossing and it is compile-time. Every other contact is a typed row through the port.

---

## §8 · The incumbent: the Plant's physics and the human arm

### 8.1 Determinants and the truth

Each class has `n_det` determinants with Zipf-permuted weights, each living in a system of record behind an application, in the inbound document (the boundary), or nowhere (tacit). The correct decision on an obligation is the sign of the full weighted sum over its determinant values, `truth = [Σ_d w_d · x_d(oid) > 0]` with `x_d ~ N(0,1)` drawn by a counter RNG keyed on `(seed, stream, oid)` so both arms of a twin see the same world. **Coverage** of a class is one minus its tacit mass, a property of instrumentation, not of the model. **[AS BUILT, `world.h`; v1's "quality curve" was dead code and was deleted at step A: F15]**

### 8.2 The read

A decider — human or machine — sums only the determinants it gathered (systems opened, the boundary, and for humans a tacit share reachable only by talking), plus noise scaled by the class's judgment intensity and the decider's competence, through one `observe()` with different masks. Completeness is load-bearing through *which* determinants are summed, not through a curve. The machine reads at tacit share zero; the twin priced that at +3.49 points wrong at demand 1.5, +2.50 at 0.7 **[M]**, and buying every lane closed 0.8 of it (§1.5).

### 8.3 The human arm

The five acts (fetch, frame, decide, commit, transport) with glue and rework around them; attention that decays with fatigue and is cut to the corner under load; a derived meeting calendar (one-to-one, status, rollup, cross-wire) that is the only tacit channel; escalation up the cone when unsure or when the value exceeds the seat's authority, with a cold re-fetch at the new seat; holds as rows, one per undrained obligation per seat per day (80.6% of the sim's 5.1M rows **[M]**). The cone from the span: `E` decision seats, supporting layers by α, a fixed floor F; skills per seat per class; warrant seats tagged. The writ: `w_bad w_late w_salary w_unplaced w_frontier kappa_max thin_margin eps_floor canary_delta canary_cap audit_min salt`, authored, never fitted.

### 8.4 The world's verdict

`world_settle` is the only writer of `OUTCOME`. After the class's verdict latency it compares the decision to the truth and the decision day to the due day: right and on time is good, right and late is late, wrong is bad. It does not know who decided. The writ prices each outcome; open obligations are priced at `w_unplaced × value` (v1's twin scored settled obligations only and the incumbent's 124,000 unfinished ones cost nothing: F4, fixed at step 1).

---

## §9 · Reports: the cascade, the residual, the prints

### 9.1 The cascade

The cone's supporting layers scale with its decision seats: `E_out = α·E_in + F`, with `α` the support coefficient and `F` a fixed floor that no within-firm regression can see. Licensing removes decision mass: `E' = E × (1 − licensed mass)`, and the headcount that follows is

> **N = E' / (1 − α) + F**

At ACME: licensed mass 63.7%, E 412 → 150, `N = 150/(1 − 0.145) + 17.6 = 193` with the honest fit; the origin fit (α 0.175, F 0) prints 181 and promises a company physics does not allow (oracle O12) **[M]**. F is obtainable only from cross-firm data at the target size; print it beside every N.

### 9.2 The residual

Decision mass after licensing, by reason **[M]**: **licensed 63.7** · **warrant 4.7** (a signature, liability, someone who can be sued; does not move) · **counterparty 0.7** (the other side demands a person; does not move) · **frame 14.3** (classes with a licensable band still below rung 2, which moves with time, plus the uncovered tail of licensed classes, which moves with lane-buys; **[v2]** split into `frame_wait` and `frame_cov`) · **thin tape 16.7** (bands where n₀/V exceeds the cap; moves only with δ and V). By law the floor is under two percent of seats (seven planted warrant seats; 34 signatures a day ≈ a seat and a half of signing on a 330-minute day). By structure it is identifiability at the tolerance (§1.4).

### 9.3 The prints that attribute **[v2]**

- `minutes[seat][class][via]` per twin arm: what bought the queue — the field deleting meeting-and-glue minutes, or drafts replacing decide-fetch-frame minutes — is not attributed until this prints.
- The **binding reason per band** at day 80: `no-history / n_machine / logE / kappa / expiry`; where `n_machine` binds, volume is the lever; where `no-history` binds, the lever is the determinant the human read and the machine did not.
- **Terms-per-rung** beside every rung claim, with δ beside it.
- The per-class **coverage price** beside every flatten.
- The per-type **row histogram**, from which S becomes computable on writes.

---

## §10 · The twin and the multiverse

### 10.1 The twin

Two firms on one seeded world with nothing shared: the incumbent arm (500 people, the calendar, the cone) and the resident arm (the same people plus the machine at `live`). Because arrivals, determinants and luck are keyed on `(seed, stream, oid)`, the comparison is a paired counterfactual and not two noisy samples. It measures the one number a real programme can never get:

> **bias = (within-firm canary estimate) − (true paired effect)**

where the within-firm estimate compares unattended acts to human decisions inside the resident arm, and the true effect compares the arms. At demand 1.5: true effect −4.36 points of good rate; within-firm +27.37; **bias +31.73** **[M]**. Two mechanisms, both growing with the canary rate: the machine is routed the cases it was licensed for (selection), and the humans in the same firm are working a queue the machine already drained (interference). Budget for a bias of about this size in any real programme; only a twin can print it.

Across demand 1.5 / 1.0 / 0.7 **[M, roundtable]**: net −589.8 / −294.5 / −158.5 M; outcomes worse at every demand (+591 / +326 / +184 M); late gap +0.86 / +5.90 / +5.85; cycle gap +6.05 / +7.93 / +10.21 days; the sign of the total flips at `w_unplaced` ≈ 0.30–0.32 at every demand. The honest sentence: at a planted price for an unplaced obligation above about 0.3 of its value, a drafting machine that never leaves the queue unworked beats a firm that cannot keep up, while losing on correctness, lateness and cycle at every demand.

### 10.2 The multiverse

Best-of-N continuations under edited constraints, sharing one prefix and one physics: ten structural patches (buy a lane on a class, thin margin, span, kappa cap, ε floor, baseline), six rollouts each, 200 days, reweighted by the writ with pessimism. Every patch carries an inverse and lands as a `PATCH` row **[v2]**. Result **[M]**: the three lane-buying patches take the top three places in both runs, the best by $6.6M on an $8M spread — under one standard error, a direction and not a verdict; with the queue priced, acting sooner beats acting later. On the GPU this is §7.2; on the CPU it is 35–44 minutes for sixty rollouts. It prunes; it never promotes.

---

## §11 · The oracle battery

Every oracle carries a lie: under `--lie N` the mechanism oracle N tests is corrupted on purpose, and oracle N prints PASS only if it caught the corruption; a healthy lie run exits 0 like the plain battery, and the harness asserts the lied-to oracle's own line (`PASS_REGULAR_EXPRESSION` on its id, `FAIL_REGULAR_EXPRESSION` on any FAIL). A nonzero exit for an unrelated reason is never a caught lie; the first harness accepted exactly that, and every lie arm showed green while one oracle was reading a field its lie never touched.

| oracle | asserts | the lie it must catch | state |
|---|---|---|---|
| O0 | BLAKE2b-256 against RFC 7693; the chain verifies; one flipped field localises | a flipped byte | green |
| O1 | same seed, byte-identical human arm | a flipped day | green |
| **O1b** | same seed, byte-identical **machine** arm: tape, chain, licence table (`--lie 14`) | a resident that read the clock | green, step A |
| O2 | the gate never authorises outside its licence over the input lattice | a gate that widens on budget | green |
| O3 | out of supervision degrades to HOLD, never to acting | budget checked first | green |
| O4 | transport conserves; columns never exceed capacity | a dropped contribution | green |
| O5 | a binding capacity has a nonzero price, stocks included | a stock with no dual | green |
| O6 | the join graph is read off the application, not asked for | a corrupted join bit (the oracle now recounts from the graph) | green |
| O7 | coverage read off the world's verdicts tracks the tacit mass | a world that never says wrong | green |
| O8 | the disagree / firm-failed cell licenses nothing | credit on the fourth cell | green |
| O9 | the field recovers competence no human in ACME can see | shuffled skills | green |
| O10 | kappa demotes a class that costs more than it saves | kappa read as 0 | green |
| O11 | effects carry inverses; the window closes at settlement | a post-settlement reversal accepted | green |
| O12 | the origin-fit cascade promises an impossible company | F forced to 0 | green |
| O13 | the measured decide fraction ranks the planted intensity | a shuffled ranking | green |
| O14 | cold fold of the tape == live ledger | a struct field set without a row | step B |
| O15 | ladder recomputed from rows == the governor's | an OUTCOME counted twice | step B |
| O16 | `NullJudge` and `RulesJudge` license nothing | a truth-reading judge under the null's name | step C |
| O17 | the machine TU builds under `-DACME_NO_PLANT`; the plant section under it fails | an include reaching `truth_decision` | step C |
| O18 | a truncated tape reopens, the chain verifies, fold-to-identity holds | a torn row accepted | step D |
| O19 | switch `off` → zero effects; incumbent rows byte-identical to no-machine | an effect under `off` | step D |
| O20 | every licensed band keeps incumbent decisions ≥ ε_floor × arrivals | a rung-5 band with the stratum off | step E |
| O21 | the canary draw is independent of every machine feature (χ² null) | a feature-dependent draw | step C |
| O22 | ≥ 90% of planted strangers route to WARRANT / NOVEL | the shift removed | step E |
| O23 | identical frame → one counsel call; changed frame → two | a memo keyed on oid only | step E |
| O25 | same rows, different wall spacing → identical verdicts | a wall-clock read | step B |
| O26 | the fitted twin beats the naive rate on a held-out quarter | the fit peeks | step H |
| O27 | every live EFFECT has a RECEIPT or an UNDO | an effect without a receipt | step I |
| **O28** | no free function in `firm.h` / `world.h` without a caller (`tools/dead_symbols.py`) | a planted orphan | green, step A |

Plus the fence (every cited path:line resolves; every quoted span is present), determinism across CPU and GPU (`--parity`), and the dead-symbol tool, which on its first run found two more uncalled helpers beyond F15 and deleted them.

---

## §12 · INTELLECT mode

### 12.1 Ingest is a compiler

Change-data-capture rows in, commitment deltas out, driven by a schema map a person authors once per wire. `firm.json` per class:

```json
{ "name": "invoice.chase", "wire": "order-to-cash", "verdict_latency": 30, "value": 4200, "reversible": true, "warrant": false,
  "arrival_per_day": 47, "base_rate": 0.02,
  "source": "erp.ar_invoice", "key": "invoice_id",
  "exists_when": "status IN ('open','overdue')", "discharged_when": "status = 'paid'",
  "deadline_col": "due_date", "amount_col": "amount_due", "seat_col": "owner_id",
  "outcome": { "good": "paid_on <= due_date", "late": "paid_on > due_date", "bad": "written_off OR disputed" } }
```

Each CDC row folds to `ARRIVE` (with `day_due` and `dep`), `CONTEXT` (a system opened), `DECIDE` (a person's write-back), `OUTCOME` (the discharge predicate under the outcome rule), or a `CORRECTION` (a reply that corrects a mouth's reading). The language lanes mount a mouth (§6.3). The firm's past — mail, documents, chat — is read backward into span-cited rows on the same tape.

### 12.2 Egress is an executor

Every effect goes through an adapter that writes to the system of record with its inverse recorded first and mints the `RECEIPT`; irreversibles (a payment, a filing, a termination) require a warrant seat's signature or a jury vote. The provider seam (§5.5) meters every frontier call.

### 12.3 The switch, the pill, the bootstrap

`off`: the resident watches every lane, judges every cell, records every proposal, and nothing leaves; the world is byte-identical to a firm with no machine (O19). `shadow`: drafts are written flagged and diffed against what people do on the same cells; ≥ 80% draft agreement over K ≥ 5 per class before promotion. `live`: the draft loop on the agreement band first (history-licensed in hours), canaries at the floor rate on every class at once, the retained stratum forever. The pill (`intellect.heartbeat.json`) makes silence visible: staleness > 3 beats = STALLED, and a stalled machine is a machine whose holds are unread.

The **tower of shadows** is the greenfield bootstrap: the frontier acts at canary size, the small resident shadows the frontier, compiled code shadows the resident; graduation flows down toward cheaper, grading flows up from the world.

### 12.4 What stays human

The boundary the world imposes: who signs, who is owed, who demands a person, what the thing is for (the writ, authored once, never learned), and the retained stratum that keeps the comparison alive. Nothing interior.

### 12.5 The greenfield is an end state

No history means no agreement band; no volume means no wager band; the source clause forbids licensing by fiat. A firm founded today runs a draft loop with a person disposing and arrives at the flat organization through volume, or through pooled volume.

### 12.6 Tenancy and pooling

`Rec::firm` is carried now; N tapes under one governor are deferred until a second real wire exists. When they exist, the licence key `(schema_hash, template_hash, class, band, judge_hash)` is the key on which outcomes pool across firms under the verdict law — same judge, its own executed choices, a salt it does not hold — and n₀/V divides by the number of tenants. This is the one place the flat organization at scale differs in kind from ACME-500 (kill F-TENANT).

---

## §13 · The measured state of the instrument

| receipt | result |
|---|---|
| first run, 2026-09-13 | 15 of 16; twelve of fourteen lie arms firing; seven defects under the headline numbers |
| honest instrument, 2026-09-13 | 16 of 16; fourteen lie arms seen to fire; O7 replaced; both arms banded alike; backlog priced; drafts not the wager; cascade 500 → 193 (500 → 57 retracted) |
| step A, 2026-09-14 | O1b and O28 green; dead physics deleted; 17 of 17 with fifteen lie arms; every number byte-identical |
| roundtable kills, 2026-09-14 | via-2 twice; demand sweep; meetings off; δ 0.40; buy every lane, automate and twin |

Readings at 500 seats, span 7, demand 1.5 **[M]**: twin settled 82,479 vs 186,956; good 42.70 vs 38.34; wrong 8.40 vs 11.89; late 48.90 vs 49.77; total cost $1,837.7M vs $1,247.9M; bias +31.73. Automate: coverage 0.913 vs planted 0.889; 34 bands from history; 32 past rung 0 after 80 days; 26 of 48 unlicensable; kappa 0.242; licensed mass 63.7%; N 193 / 181; frontier 1,079 calls a day, 32% of gate decisions; reads 3,328 per period. Planted and not to be quoted as findings: the minute ledger, the machine's competence (a literal 0.96), the tacit channel.

---

## §14 · The build order

| step | lands | acceptance | numbers |
|---|---|---|---|
| **A** done | O1b; F15 deleted; O28 | 17 of 17, fifteen lie arms | identical |
| **B** | Rec v2; every type in §3.2 including CORRECTION; TICK, ACT, PROPOSAL, HEADER with schema_hash; Plant/Ledger/Store; the minutes-by-via, binding-reason, frame-split and row-histogram prints; O14, O15, O25 | fold-to-identity | **identical**; a moved number is a missing row |
| **C** | the judge port with `PlantJudge` on its own key; the read budget with the frame-hash rule; planted truth off the path; the governor TU with STRATUM rows; `template_hash` in the key; O16, O17, O21 | judge-agnosticism as a build fact | move; each delta named |
| **D** | the durable tape; switch and pill; the gate as a hashed library; O18, O19 | a kill survives | identical under `live` |
| **E** | the retained stratum at every rung; novelty on the frame; the counsel memo; `canary_cap` in the writ; O20, O22, O23 | the multiverse's "explore more" row moves | move; "licensed" restated |
| **F** | `--dump` defines the observer; the golden retires | the observer draws a real automate | none |
| **G** (any time after B) | the census: one real lane folded, no judge | first contact with a world | none |
| **H** | the fitted world; then the warp-per-corp port with CPU parity; O26 | the gym prunes | none until O26 |
| **I** | the calibration harness; `FrontierJudge` and `MouthJudge` behind the seam; `intellect` at `off`; replay with a real judge; the first canary at the floor | the first exterior verdict | the number that matters |

The greenfield arm (`via` 2 → 1: every draft an unattended act) runs on the fitted world in H; it is the only run that measures the act loop's product.

---

## §15 · The kills

Run: the act-loop kill (twice); the demand sweep; the meetings-off twin; the δ-0.40 automate; the buy-every-lane automate and twin. Registered:

1. **F-READS.** Reads per period with the memo on exceed ~1,500 → the frame rule failed or the budget must gate a new seam.
2. **F-PORT.** A `FrontierJudge` against the port as-is, one golden frame whose determinant is a mail span, must beat `RulesJudge` without the store.
3. **F-CALIB.** The margin → wrong-rate curve is not monotone per class → the seam rule fails.
4. **F-STUB.** Licensed mass is not smooth and monotone across the stub's competence sweep → the loop overfit the stub.
5. **F-AGNOSTIC.** The kernel's behaviour changes with the judge's hash at equal calibration → a code path knows who answers.
6. **F-MOUTH-SIGN / F-CORRECTION.** Mouth errors split by counterparty benefit; the CORRECTION curve is monotone per row type — or the mouth is not licensable first.
7. **F-INTERIOR.** Deleting the interior raises the wrong rate on a class beyond its coverage price → the fossil missed a lane.
8. **F-FIT.** The fitted twin's held-out forecast is no better than the naive rate → the gym cannot prune.
9. **F-TENANT.** Two tenants on one alphabet license a class neither licenses alone, on a salt neither holds.
10. **F-RESIDENT.** The resident against the best turn-based twin on the same tape, blind-graded over weeks; and a sweep over the tick period at the center.
11. **F-XMODE.** A synthetic tape replayed through `intellect` (plant unlinked) reproduces `acme`'s verdict rows byte for byte, or the machine still reads a non-row.
12. **The greenfield arm.** Every draft an unattended act, on the fitted world: the wrong rate and the minutes.
13. **The verdict law's own kill.** If shadow calibration predicts disagreement-band outcomes at the first desk canary, the boundary licenses faster than this document claims.

---

## §16 · Notation

`c` class · `oid` obligation · `d` determinant · `w_d` its weight · `x_d` its value · `p` base failure rate · `δ` non-inferiority tolerance · `n₀(p, δ)` the identifiability sample · `r` canary rate · `V` decisions per term · `T` term · `L_c` verdict latency · `E` decision seats · `α` support coefficient · `F` fixed floor · `N` headcount · `κ` supervision created / removed · `u, v` transport duals · `P_ij` the plan (never materialised) · `supp` support spectrum · `S` flattening dividend · `A / D` agreement / disagreement bands · `via` how a decision left the building · `[M] [P] [BUDGET]` measured, planted, arithmetic.

---

## §17 · The line

*One machine, four worlds, one tape. The interior is deleted by a fold and the edge is kept because it is where the verdict arrives. The kernel owns when, where, whether and how much, has no words in it, is trained on ten thousand synthetic firms per card, and replays bit for bit. The model owns the map from the world's language to the closed alphabet, stands at every cell the field selects, reads the world at the edge, compiles the templates once, and enters through one port as a proposal with a margin, three hashes and a measured curve. The CUDA solver prices every seat and every promise in microseconds and integrates a firm per thread in the gym. FUSOR is resident where the world speaks and the center is a tick because the tape must replay. The draft loop wins the queue on the first day and needs no licence; the act loop earns its licence from the world at n₀(p, δ)/(rV) per rung, and the only levers are the tolerance the writ admits and the volume the tape pools. The floor by law is a seat or two in a hundred. The floor by structure is what the machine cannot yet see. ACME is this program with a planted world behind the port. INTELLECT is this program with a real one. The tape is the proof.*

---
*v2.0 · 2026-09-14 · Claude Opus 5. Supersedes DESIGN.md (v1) and absorbs the REV 2 spec and the roundtable final; overwrites nothing. Nothing in this file is a measurement except the rows marked [M], and every one of those has a file.*
