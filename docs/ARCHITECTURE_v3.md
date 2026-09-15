# ACME-500 / INTELLECT-500 · ARCHITECTURE v3
### One machine, four worlds, one tape — and one alphabet. The organization's machine language is the closed alphabet under a pin; language is compiled in at the mouth and rendered out at the console; similarity schedules and identity authorizes; the archive plane behind the store is where embedders and rerankers live, and a judge that reads rows is one more judge behind the seam.

**Version 3.0 · 2026-09-15 · Claude Opus 5 · the `C:\55555` session.** A **superset** of `ARCHITECTURE_v2.md`: Part II of this file is v2 as landed, verbatim and unedited (its own §0–§17, its amendments §14a–§14b, its kills 1–16). Part I is what v3 adds, numbered §18 onward so every cross-reference into v2 holds. Nothing in v2 is overwritten; where v3 amends a v2 statement it says so and leaves the original standing below. The steps landed on branch `v2` at the time of writing: A, B, C0, C1, D0–D3 (`receipts/RECEIPT_ACME500_STEP-D_2026-09-14.md`), 26 of 26 in the battery with twenty-four lie arms, four tool oracles, every reading identical to C1 under `live`.

**What v3 is for.** Four outside reviews and three brainstorms in `C:\55555` (2026-09-14/15) converged on one direction from different starts: the corporation's internal medium need not be human language; the firm's exact state should be typed and its working state may be learned; retrieval over an archive should be event-driven rather than question-driven; a discovered relationship should become a maintained dependency; and the model that judges should be one that reads the firm's own record rather than prose about it. v3 fits those into the architecture as built — as rows, interfaces, oracles and steps — **without training a model from scratch**: frozen pretrained embedders and rerankers behind adapters, a small sequence model over rows trained where the counterfactual exists, and the plant as the grader of all of it.

**Tags** as in v2: **[M]** measured with a file · **[P]** planted · **[BUDGET]** arithmetic on stated inputs · **[AS BUILT]** code on this disk · **[v3]** new in this version.

**Reading order for the review:** §18 (the thesis restated as the alphabet), §19 (the archive plane: embedders and rerankers behind the store), §20 (the judges extended: the TapeJudge), §21 (the rows v3 adds), §22 (oracles and kills), §23 (the build order amended), then Part II for everything v2 already settled.

---

## §18 · The thesis, restated as the alphabet **[v3]**

### 18.1 The machine-native IR of an organization is the closed alphabet, not a latent

A `Rec` is a structured token: type, class, day, oid, seat, arm, a, b, margin, value, band, flags, via, provenance — fourteen typed fields in forty bytes, embeddable as a sum of field embeddings, requiring no vocabulary of words. A cell's rows from `ARRIVE` to `OUTCOME` are a sentence; a period between two `TICK`s is a document; the firm's year is 2.2 million tokens at 500 seats **[M, D1]**; the alphabet pin is the vocabulary version. O29 is the grammar checker; O14 says the semantics is a fold and nothing else.

So the question the brainstorms asked — *what is the machine-native intermediate representation of an organization?* — has an answer in the repository: **the closed alphabet under the pin, the fold that gives every row its meaning, and the seam that carries a scalar margin, a choice and three hashes.** That alphabet is the corporation's ISA. The learned latent the reviews reach for lives *behind the port*, in whichever judge stands there, and is replaceable precisely because the alphabet is not. This is §2.1's thin waist and §1.6's seam rule named as the thesis.

### 18.2 Language at the perimeter, receding by promotion

Language enters at the mouth (§6.3), which compiles the open alphabet into the closed one as rows with `prov = M` — derived claims with provenance, never evidence — and leaves at the console (the observer of §9.3, the operator's views of §12.4), which renders rows into pictures and prose after the fact. Between the two there is no sentence in the operating loop. What has no field yet (the scared dog, the cracked driveway) is read by the mouth and reasoned about by the frontier; when it recurs, it earns a row type under a new pin, the licence re-keys, and the receipt names the promotion. **The perimeter recedes one promoted row type at a time**, under the verdict law, and is never assumed to have receded. This is §12.3's compile-down motion applied to the alphabet itself.

### 18.3 Three representations, and where each lives as built

| representation | what it holds | where it lives in ACME | may it be approximate? |
|---|---|---|---|
| exact state | identities, quantities, deadlines, commitments, permissions, observations, effects, receipts, outcomes | the tape and its fold (`ledger.h`), the archive's spans by content hash | never |
| learned working state | compressed relationships, forecasts, interpretations, uncertainty | behind the port: the judge's own state (§5.1), the counsel memo, the cognitive index (§3.6) | yes, and it is never the record |
| constitutional state | the writ, the licence, the gate, the salt, the switch | `firm.h`, `license.h`, `gate.h` (hashed, O30a), `governor.h` | never; deterministic and pinned |

### 18.4 The law

> **Similarity schedules; identity authorizes.** An embedding may decide *where and when* cognition is spent — which cells a new span touches, which precedent a judge is shown, how novel a frame is, what to read next. A hash decides *what may happen* — the memo's validity, the licence key, the effect's re-derivation. No approximate object is ever the key to an authorization.

Corollaries that decide the rest of this document: the frame hash stays exact (a memo keyed on "close enough" would carry a proposal across a change that flips the decision, and O30 would stop being a proof); the gate reads no vector; the ladder is keyed on hashes; every use of an embedder or a reranker below is on the *scheduling* side of the seam, and every result it produces that matters to a decision is written down as a row before it is used again.

### 18.5 What "next token" is for an organization

The `arm` byte partitions the tape into **policy rows** (`HUMAN`, `MACHINE`: the firm's actions) and **world rows** (`GOVERNOR` as written by the world port: `ARRIVE`, `OUTCOME`). A model that predicts every next row learns the incumbent's meetings and holds (F8: the fossil teaches what people did). The world model the thesis needs predicts **world rows given policy rows** — P(`OUTCOME` ∣ frame, effect) and P(next arrivals ∣ state) — and the fold computes the deterministic part of the transition exactly and for free, so nothing the fold knows is ever learned. "Next token" for an organization is the world's next row under the firm's action: the verdict law's own object, the only thing a licence may be earned on.

### 18.6 Two corrections the thesis carries with it

- **Latent is not compressed.** A row is 40 bytes; a 1,024-dimensional float16 embedding of the same fact is 2 KB. The compression an organization needs is in *the number of times a fact is interpreted*, and the reads-per-period print (3,328 → 1,729 **[M, C1]**) already measures it. A latent adds prediction, not compression. The document says "fewer re-interpretations", never "smaller symbols".
- **Latent is not auditable.** O30 re-derives every effect from rows; a decision whose inputs live only in a judge's state cannot be re-derived, so that state may propose and may never be the record.

---

## §19 · The archive plane behind the store: embedders and rerankers **[v3]**

v2 §3.6 named the archive plane and the frame `C(i, t) = A[T_c ⊕ W_b]`; `Store::frame` (§5.1) returns it. v3 specifies what stands behind that call, with frozen pretrained models and no training from scratch.

### 19.1 The store as a three-layer read

`ArchiveStore : Store`. For a cell i of class c, `frame()` assembles spans in this order, and the order is the design:

1. **Exact joins** — the template `T_c` (the join graph the compile step recovered, §4.6): the systems and keys this class reads. Deterministic, free, always.
2. **Retained dependencies** — the inverse index of `CONTEXT` rows (§19.3): every span this cell has been shown before, and every span a discovery once linked to it. A fold, no vector.
3. **Bounded semantic discovery** — only when the frame is *thin*: `completeness_hat` under a threshold τ_c, or a `WANT` row standing (§19.4). A read plan over the archive's index (everywhy's coarse scan → rescoring → graph propagation → bounded read plan, as it exists) returns candidate spans; typed checks (identity, period, amount) filter them; the reranker (§19.5) orders the survivors for the cell's named question; the top-k enter the frame under the walk budget `b`.

Layer 3 is a fetch and it is charged like one: it counts against the read budget (§4.2) and writes `CONTEXT` rows for what it fetched. The frame hash covers every span in the frame by content, layers 1–3 alike, so a discovered span that later changes invalidates the memo exactly.

### 19.2 Index identity is content, and versions are new spans

`span_id = H(content)`. A superseded document is a *new* span; the old one stays addressable for the frames that were built on it (replay must reconstruct what was known, §18.3 "exact state"). The index refreshes its vector on a content change and never keeps a stale vector under an old location. The rule for any index adopted from outside (everywhy's `doc_id#seq` skips existing entries and would keep an edited document's old chunk): **encode unchanged content once; propagate every change explicitly.** Oracle O34 (§22) plants an edit under an unchanged id and requires the dependent memo to invalidate.

### 19.3 Discovery becomes a dependency: the `CONTEXT` row with `RF_DISCOVERED`

When layer 3 finds a span and the judge's proposal used it, the kernel writes a `CONTEXT` row for (cell, span): seat −1, arm `MACHINE`, `a` = the system the span belongs to, `b` = the span's content hash (low 32 bits), `value` = 0 minutes, `prov = M`, flags `RF_DISCOVERED`. From then on the fold's inverse index routes that span's successors to that cell with no vector in the path — the same rows the compile step already reads as the join graph, now carrying links the machine found rather than links people left. The pattern:

> **discover the link with a vector → check it with typed rules → retain it as a row → route through the fold.**

The vector search runs once per discovery; the tape keeps the link; the fold maintains the index. Language-free after the first read. O33 (§22) counts vector consultations per (span, cell) after the first and requires zero.

### 19.4 Standing questions are the want row, inverted

§3.6 named the *want row*: the machine records which determinant a frame is missing. v3 makes it a row type, `WANT` (§21), written on change like a hold: (oid, class, `a` = the system or determinant kind wanted, `b` = the question template id, `margin` = the decision margin at stake, `value` = the class value at stake). The fold keeps a **standing-question index**: (system, template) → cells waiting. Then retrieval runs in both directions with one matrix:

| trigger | operation |
|---|---|
| a cell's frame is thin | find spans that answer its standing questions (layer 3) |
| a span arrives | find the cells whose standing questions it may answer: exact keys through the fold's index first, then S = Qᵀd over the question and span embeddings for the candidates the keys missed |

A hit in the second direction is a *candidate* only: typed checks decide applicability, the reranker orders, the judge reads, and a used span becomes a `CONTEXT` row (§19.3). The system remembers what it is waiting to learn, and nobody polls. E0's shared-fact experiment (§14b) gains a second column: the routing cost per span, reads per span landed.

### 19.5 The reranker's job is narrow, and it is never the field's

A reranker ranks evidence **for a named question** — does the latest agreement permit this action; has the condition been satisfied; is there evidence against the interpretation; is this span about the same entity and period. It runs on a bounded shortlist after exact and lexical lookup; its score is a relevance estimate and never a probability of correctness; it does not establish completeness, truth, urgency or authority, and it does not rank the field's queue — selecting evidence and scheduling attention are different problems, and only the field (§4.2, and VOC in E) schedules. The frame it feeds has **slots with roles** — current facts · governing rules · exceptions and amendments · conflicting evidence · precedent · missing requirements — so that a frame is an argument the judge can read and a fold can audit, not ten passages that sound related.

Baselines, frozen: Qwen3-Embedding-0.6B (everywhy's) and Qwen3-Reranker-0.6B, both with task instructions, behind adapters with their own `judge_hash`-style fingerprints (a golden-set fingerprint with a TTL, as §5.2 requires of any rented model). Late-interaction (ColBERT-style) indexing is the alternative to profile if reranking becomes the bottleneck on stable corpora; it is not chosen in advance.

### 19.6 Learning what helps a decision, from the plant

Relevance is not decision-usefulness: a span can be on topic and change nothing; a dull sentence with a changed date can decide the case. The plant can label the difference, because it alone can settle a cell with and without a span. Step E2 (§23) generates, per class, a synthetic corpus — determinants rendered as typed spans with content hashes; a shared fact; the same determinant under a second span id (a paraphrase); near-duplicates that do not apply (wrong entity, wrong period); superseded versions; corrections; distractors — and grades every retrieval by the hidden truth into *beneficial · harmful · immaterial* changes to the decision. Those labels train, later and only if the baseline warrants it, a small scorer over features (embedding relations, exact-entity matches, missing required fields, source age, the decision margin, prior results from similar evidence), and later still a fine-tuned reranker or adapter. **The controller stays blind to the plant during operation** (O17 extended: the store's adapter compiles without `world.h`); the plant's knowledge enters grading and separated training data only.

### 19.7 Procedures with conditions, and case memory

Two more indices over the same machinery, both hints and never keys:

- **Routines**: the `RoutineJudge`'s compiled artifacts (§5.2, §12.3) indexed with their applicability conditions, required facts, source and policy versions, and the cases they were evaluated on. Similarity locates a candidate routine; its conditions, checked against exact state, decide whether it applies; the gate decides whether its effect may proceed. The reusable object is a method with conditions, never the previous case's answer.
- **Cases**: settled cells as episodes — the frame at decision time, the spans used, the proposal, the effect, the outcome, including the failures — retrievable *respecting simulated time* (no future frames), shown to the judge as a frame slot (precedent). A precedent informs a proposal; it confers nothing on a licence (§1.2), and the reviewer's caution stands: similar wording does not establish equivalent circumstances.

### 19.8 The class geometry, replaced by a measurement

`Compiled::class_geom` (§4.6) is a synthetic cosine prior over the class name today. v3's first and cheapest experiment replaces it with real embeddings of the class descriptions and compares the head's held-out loss against the synthetic prior and against random (O-GEOM, §22). A day's work with a clean answer to whether pretrained semantic structure helps ACME anywhere.

---

## §20 · The judges, extended: a judge that reads rows **[v3]**

§5.2 lists six judge kinds. v3 adds the seventh, which is the epiphany's proper landing:

| judge | world | what it is |
|---|---|---|
| `TapeJudge` **[v3]** | fitted, real | a small structured-token transformer whose input is the cell's own row history plus the typed determinants of its frame (values, units, sources, ages, span hashes — never prose) and whose output is the proposal the seam already carries: choice, signal, `completeness_hat`, **a sensitivity per shared fact** (∂signal/∂z, so the kernel can apply the ε-rule of §14b E0 from the port's answer and never from the plant's weights), its hash. Its vocabulary is the alphabet pin: a promoted row type re-keys the licence and the rungs are re-earned — model replacement without truth replacement. |

**Where it learns.** A real tape carries one policy's actions and never the counterfactual; a model trained on it alone learns the incumbent. The plant carries do(): the multiverse (§10.2, §7.2) can run the same cell under different actions and settle each. So the plan is **pretrain where do() exists — synthetic tapes across seeds and hostile worlds (kill 14) — and calibrate where it does not — the real tape, on observed actions only**, with F-TWINS-OF-HISTORY (kill 16) as the test that catches a model claiming an intervention the history never identified. This is sim-to-real for organizations, and it is the first job the CUDA lattice has that is not ACME's own search: **the lattice is the judge's data generator.**

**What it is not.** Not the controller. It proposes; the field schedules; the gate authorizes (O30 re-derives its effects like any other's); the ladder licenses it band by band on arrivals. Every reply's "one model runs the company" collapses into the seam, and that is what the seam is for. The corpus problem is stated plainly: one firm's tape is small and carries one policy; the honest first milestone is a `TapeJudge` that transfers from synthetic tapes to one real tape without losing calibration, measurable in step I with the harness and not before.

**The `EventModelJudge` of §5.2/§7.3** is the same animal at the period grain (the canvas); the `TapeJudge` is at the cell grain. They share the vocabulary and may share weights; the distinction is what they are asked.

---

## §21 · The rows v3 adds **[v3]**

Every addition moves the alphabet pin, and the receipt that lands it names the move. All are folded by the ledger; none is evidence.

| row | fields | fold effect | written by |
|---|---|---|---|
| `WANT` | oid, cls; `a` = system or determinant kind wanted; `b` = question template id; `margin` = the margin at stake; `value` = the class value | the standing-question index: (a, b) → cells; cleared by the next `CONTEXT` for that system on that cell, or by the cell's disposition; on change, like a hold | the machine (`prov = M`) |
| `CONTEXT` + `RF_DISCOVERED` (flag bit 64) | as `CONTEXT`; seat −1, arm `MACHINE`; `b` = span content hash (low 32) | the inverse index span → cells, beside the human fetches; the join graph as the compile step reads it, now with links the machine found | the machine (`prov = M`) |
| `SPAN` | oid 0; `a` = system; `b` = span content hash; `value` = bytes; flags: `RF_SUPERSEDES` (bit 128) with `margin` = the superseded hash as float bits | the archive's registration journal: which spans exist, which supersede which; the store's index identity is this row's hash | the world port at ingest (`prov = D`) |

`RECEIPT` gains the two statuses the fourth review asked for and D2 designed: `b` = 3 *unknown* (the executor cannot say whether the world performed it) and `b` = 4 *reconciled* (the world was queried and answered); an effect's idempotency key is `H(oid, day, choice, judge_hash)`, derivable by any fold, so the record does not change. Built in I; named here so the alphabet is planned once.

---

## §22 · Oracles and kills added **[v3]**

| oracle | asserts | the lie | step |
|---|---|---|---|
| O-GEOM′ | real embeddings of the class descriptions beat the synthetic cosine prior and random on the head's held-out loss, or the prior is deleted | the embeddings replaced by the prior under the embeddings' name | E1 |
| O32 | on the synthetic corpus, the archive store's decisive-evidence recall ≥ the exact-join baseline on every class, at a stated cost, with the controller blind to the plant | the store reading the plant's determinant weights (an include of `world.h` in the adapter: the O17 guard extended) | E2 |
| O33 | after the first discovery of a (span, cell) link, no vector search is consulted for that link again: the fold routes | an index that ignores `CONTEXT` rows with `RF_DISCOVERED` | E2 |
| O34 | index identity is content: an edited span under an unchanged location refreshes its vector, invalidates every memo whose frame held it, and leaves the old span addressable to the frames built on it | the `doc_id#seq` skip | E2 |
| O35 | the standing-question index wakes exactly the cells whose `WANT` rows a span answers, and a `WANT` row is cleared by the `CONTEXT` row that answers it | a span routed by similarity alone, past the exact keys | E2 |
| O36 | the `TapeJudge`'s calibration on a real tape's observed actions is within the harness's tolerance of its calibration on synthetic tapes with do(), or the transfer is declared failed | synthetic outcomes leaked into the calibration set | I |

Kills, continuing v2's numbering:

17. **F-EMBED.** If the archive plane with embeddings and reranking does not beat exact joins plus retained dependencies on decisive-evidence recall *at equal cost*, the embeddings are cost, not capability, and they leave the read path; the test needs relationships the controller must discover, because where every dependency is explicit, structured computation wins and this document says so in advance.
18. **F-SIM2REAL.** If the `TapeJudge` pretrained on synthetic tapes does not hold its calibration on the first real tape (O36), the organizational world model is a synthetic artifact and is not attached to any lane; the plant embodies the thesis (kill 14), and a judge that learned the plant's thesis rather than the firm's dynamics must be caught before it proposes anything real.
19. **F-VECTOR-KEY.** If any authorization path — the memo, the licence, O30's re-derivation — is found keyed on an embedding rather than a hash, that path is a defect of the same kind as F14 (the lineage choosing its own arena) and is repaired before the next step.

---

## §23 · The build order, amended **[v3]**

The order of §14 stands through D. What v3 changes is the content of E and I, and the role of H:

| step | v3 content | acceptance |
|---|---|---|
| **E0** | as §14b: the shared-fact experiment with outstanding exposure, incremental updates against full reads against changed decisions counted separately, unresolved outcomes, `exposure_cap` with three ledgers; the routing cost per span as its second column | the thesis in one print: reconsideration only where a margin is crossed. **Landed 2026-09-15** (the step-E receipt): at 500 seats 2,062 frames moved by a shared fact alone, 789 carried by the judge's certificate without a read, 1,273 read again with 471 changed choices; O37 with lie 24, O38 with lie 25; the uncapped machine holds $96.1M in flight at peak and the licence follows the cap ($50M → 29.4 percent, $25M → nothing) |
| **E1** | the counsel memo (O23), VOC as the frontier's ranking, the retained stratum (O20, F14), novelty on the frame with the conformal construction fixed (O22, F11) — **with the frame embedded by the frozen embedder**, which is the first vector in the kernel's scheduling path and the first test of §18.4 — O31, O-TEMPLATE, **O-GEOM′ (§19.8)** | numbers move and are named |
| **E2 [v3]** | the archive plane: `ArchiveStore`, `SPAN` and `WANT` rows, `CONTEXT` with `RF_DISCOVERED`, the standing-question index, the reranker behind its adapter with roles in the frame, the synthetic corpus per class and its grading; O32–O35; kill F-EMBED run | decisive-evidence recall, missed affected cells, stale proposals, incorrect reuse after a change, cost per verified completion, all printed; the controller blind to the plant |
| **G0, G** | as §14b: the schema as data; the census on one real lane, no judge, no effect — now also folding the lane's archive into `SPAN` rows | the compiler meets a world |
| **H** | the gym and the fitted world as §14 — **and the multiverse as the `TapeJudge`'s data generator**: synthetic tapes with do() across seeds and hostile worlds; F-TWINS-OF-HISTORY run | sim-to-real measured, not assumed |
| **I** | as §14 with the fourth review's items — plus the `TapeJudge` behind the port, calibrated on the real tape (O36, F-SIM2REAL), and the `RECEIPT` statuses of §21 | the first exterior verdict; no lane `live` until O31 has passed |

**The sentence.** *The organization's machine language is its closed alphabet under a pin; the fold is its grammar; the seam is its calling convention. Language is compiled in at the mouth and rendered out at the console, and the perimeter recedes one promoted row type at a time. Similarity schedules; identity authorizes. A model that reads rows and proposes is one more judge behind the seam, licensed like any other on the world's rows — and the plant, which alone can run the counterfactual, is its teacher.*

---
---

# Part II · ARCHITECTURE v2 as landed, verbatim

*Everything below this line is `docs/ARCHITECTURE_v2.md` at commit `10ee83c` on branch `v2`, included unedited so that this file is a superset. Its §16 is Notation and its §17 is The line; v3's own sections are §18–§23 above. Its kills 1–16 are the kills v3's 17–19 continue.*

---

# ACME-500 / INTELLECT-500 · ARCHITECTURE v2
### One machine, four worlds, one tape. The simulator that trains the kernel and the product that runs the firm are the same program; the only thing that changes on the day a real organization is plugged in is the header line.

**Version 2.0 · 2026-09-14 · Claude Opus 5 · the `C:\55555` session.** Supersedes `acme500/docs/DESIGN.md` (v1, the design as built through step A of the REV 2 spec) and absorbs `docs/SPEC-AMENDMENTS_REV2_ONE-PORT-FOUR-WORLDS.md` (the transition amendments), the roundtable final of 2026-09-14 (`ROUNDTABLE_THE-FLAT-ORGANIZATION_2026-09-14.md`), the thin-plane and verdict-law derivations, and the reads of `fusord.cpp`, the flight computer, the ledger lattice, `org_solver.cu` and OrgSolver Converged. Nothing in v1 is overwritten; v1 remains the receipt of what runs at tree `4c8fbc7`. This document is the whole architecture in one place: the theory with its mathematics, the data model, the kernel, the judge port and the model, FUSOR at the edge, the CUDA solver in production and in the gym, the incumbent plant, the reports, the oracle battery, the INTELLECT adapters, the measured state, the build order, and the kills.

**Tags.** **[M]** measured on this disk with a file to point at. **[P]** planted: a constant of the synthetic world, a model of a claim and not a measurement of one. **[BUDGET]** arithmetic on stated inputs. **[AS BUILT]** code that runs today. **[v2]** the change this document specifies. Everything untagged is derivation.

**Reading order for an implementer:** §2 (the invariant), §3 (the data model), §4 (the kernel), §5 (the port), then §14 (the order). For a reader who wants the argument: §1, §9, §13, §15.

---

## §0 · The answer in one page

> **Preserve the information required for authorized decisions. Reuse established judgments while their validity conditions hold; spend fresh cognition where it can improve a decision or test those conditions.** (The operating rule, 2026-09-15; the memo keyed on the frame hash and the ε-floor re-read are its two halves as built.)

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

**Flattening has a computable criterion, and it is a lower bound.** A write is scaffolding if nothing external reads it — no foreign key to a counterparty, no filing, no payment rail, no delivery, no still-active human seat — *and* no authorized effect's frame is downstream of it, transitively (an inspection result or a fraud note is read by no counterparty and decides a later effect). The dependency walk from every `EFFECT`/`DECIDE` back through the rows its frame read establishes potential relevance, not semantic irreducibility: a field that sits in a frame unused is retained, a dependency exercised only under another policy is missed. So S is a conservative retention analysis; stronger deletion needs behavioural verification. The flattening dividend per wire is

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

> **The kernel owns WHEN, WHERE, WHETHER and HOW MUCH. The model owns the map from the open alphabet to the closed one. The seam is a typed row carrying a scalar margin, a provenance and three hashes.** In short: zero-copy context, proof-carrying action — state and cognition may converge (the memo, the warm cell already read); judgment and authority never do (the gate, the licence, O30).

The closed alphabet (the event schema, hash-pinned) says what may cross; the proposal's margin says what the kernel needs from whoever answers; the judge hash, the template hash and the schema hash say what was asked, with what context, under which contract. The model's only property the kernel ever consumes is its **calibration**, margin → wrong-rate per licence key, measured on the shadow tape and never assumed. That is what lets the kernel be trained headless on a stub judge whose competence is swept, and what lets a model be swapped without losing the evidence: the rungs are re-earned on the same tape.

### 1.7 Two clocks: the edge is resident, the center is a tick

The world is **read** at reply speed and **graded** at outcome speed. Where the world arrives in words, the reader must be resident: a judgment made by a called function is a judgment about the moment the function was called, and the free channels of ingest (the sentence boundary, the surprisal, the un-say) exist only inside an owned continuous loop (§6). Where the plane is folded and the licence is computed, the center must be a **tick**: O1b, determinism over the machine arm, holds because the center folds on TICK rows, and a resident center is not bit-replayable. The read budget with a frame-hash memo (§4.2) is what decode-on-delta means at the center. The radius of any single mind is `r = c·T/2` for its period `T`; a firm larger than one period's light-cone is loci with one germ crossing, not one trunk.

### 1.8 SABRE

SABRE was the first time the *state* of an airline fit in one place: one true record per seat, one writer, readable everywhere, and overbooking became a policy instead of an accident. The field is SABRE for promises: one true record of every open obligation with its pressure and its price, current the way a balance is current. What SABRE could not hold — the judgment at the counter — is the judge, and it now has a port. SABRE made the state global. This makes the judgment global.

---

## §2 · The invariant and the mode matrix

### 2.1 The signature

> **`period(ledger = fold(tape ≤ h), store, clock) → { proposal rows, verdict rows, effect requests }`**

`fold` is deterministic and integer-accumulated; the ledger contains nothing not derivable from rows; `store` answers field lookups by key and is the only path to a row's contents; effect requests go to an executor that returns nothing but future rows; the machine's translation unit does not link the plant. The typed row under the alphabet pin is the programme's thin waist, in the sense IP is the internet's: above it the judge can change (six kinds in §5.2), below it the systems of record can change (the ingest map of §12.1), and the invariant in the middle is one 40-byte record carrying identity, state, proposal, authority and receipt. The kernel is to the judge what a hypervisor is to a CPU: it schedules where judgment is spent, holds the state, enforces privilege, and does no judging. Three consequences: (1) every field the gate, the ladder, the field or the compile step reads is a row, a fold of rows, or a store lookup; (2) planted truth is unlinkable from the machine, proved by a build target; (3) the clock, the salt, the licence table and the switch are inputs the machine cannot write.

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
- A row with `prov = M` is a *derived claim with provenance*: it preserves what a model read from external evidence and creates no corroboration by being written; it is never evidence for a licence.
- **Ledger** (both modes; built only by folding rows): `ob[]` with `id cls day_open day_due day_decided day_settled seat state outcome hops escalations systems_opened decision via band dep margin completeness_hat`; `open_idx`; `day`; `minutes[seat][cls][act]`; the licence table folded from `LICENSE`; this period's strata; the counsel memos; the applied patches. `completeness_hat` is the machine's own estimate (§4.6), never the plant's weights.
- **Store** (both modes; the data layer): `frame(oid, join_graph) → Frame`. `PlantStore` returns the plant's determinant values for instrumented systems plus the boundary determinant, tacit ones absent; `CdcStore` returns the systems-of-record snapshot. The tape records `CONTEXT` (which system was read); the store holds what was read.
- **Schema** (authored, ten lines per wire, both modes): `name wire verdict_latency value reversible warrant counterparty arrival_per_day base_rate`, plus in INTELLECT the ingest predicates (§12.1). `base_rate` is the prior for n₀ until 30 outcomes exist; `counterparty` is the commercial fact that the other side demands a person (C1: it was a rule over planted judgement intensity before).
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

The plan `P` is never materialised; the duals `u[N]`, `v[M]` are, in O(N+M), and they are **prices**: `v_j` is the shadow price of column j, what one more unit of that seat or that stock is worth to the firm this period, and it is positive when the capacity binds strictly (complementary slackness gives that direction; a binding capacity with a zero price is the degenerate case, and oracle O5 asserts the typical one). The duals are T-scaled multipliers of the column capacities, in cost units only up to the temperature. Iterate to a published curve (O3: 60 iterations 1.0e−2 FAIL, 150 3.3e−4, 400 4.7e−6 plateau **[M, ledger lattice]**), finish on the equality constraint (ending on the column pass fails O1 at 6.2e−2 **[M]**). Storage int8, compute fp32, fixed order of summation, no atomics, no fast-math: two runs are `memcmp`-identical.

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

**Ripeness and residual.** Ripeness = days to due against the class's verdict latency; residual = the cell's deviation from its class centroid, stored not computed. The field's ranking as built is ripeness (lateness), and the value-of-computation form of §9.3's E lever is where expected gain would enter; a (1 − 1/e) bound for greedy would need the value to be submodular, which complementary information sources break, so none is claimed. The ε-floor is constitutional: every lane sampled at ≥ ε·λ so no edge starves and the machine may not choose how much it is checked.

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

**History admission.** A band with `agree_total ≥ 40` replayed instances and `p_agree ≥ firm_base − 0.01` is admitted to rung 1 without a canary, **provided the judge's agreement with the firm on the class is beyond chance: Cohen's κ ≥ 0.2 over the replayed instances** (C1, F22). Without that clause a coin admitted itself on the half of history it happened to agree with, at the firm's own good rate: 8 class-bands in the small battery world **[M]**; with it, none, and the plant's judge (κ ≈ 0.75) loses nothing. The agreement band is licensed by history (§1.3); the disagreement band has no such path by construction.

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

**What the coverage estimator is, and is not** (2026-09-15). The compile step's coverage is one minus the wrong-rate on the agreement diagonal. That statistic is *realized correctness on agreement cases*, P(wrong ∣ agree), and it identifies information coverage only under a measurement model that is not established: two independent readers each wrong 10 percent of the time with every determinant recorded read 1 − 0.01/0.82 = 98.8 percent against a true 100. O7's PASS (mae 0.05, corr 0.94 against the planted tacit mass) means it tracks coverage *in this plant*, where reader noise is small against the missing-determinant effect and the two readers' errors on recorded determinants are independent; in a real firm both readers trust the same system of record, and a wrong stored value fools both and reads as "unrecorded". Missing facts, wrong stored facts and faulty reasoning are three explanations the statistic cannot separate; kill F-HOSTILE carries the wrong-stored-value world.

### 4.8 Novelty and calibration

**Novelty** is conformal: a score over the frame — the magnitude of the read signal against the class's calibration sample in sim, the frame embedding or the trunk's hidden state in INTELLECT — with the conservative rank p-value p = (k + 1)/(n + 1), k the calibration scores at least as large as s, small for a stranger; the novelty score is 1 − p, and above 0.97 the case is outside the population the licence was earned on and goes to a warrant seat. (The code as built computes 1 − k/n, an empirical percentile that disagrees with 1 − p at the threshold's edge — k = 1 of 64 reads 0.984 against 0.969 — and is corrected with O22 in step E, where the score itself is replaced; F11.) v1's score was a per-class constant (v1's F11); v2's has a tail, and oracle O22 plants strangers from a shifted distribution and requires ≥ 90% routed.

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

A judgment made by a called function is a judgment about the instant the function was called: the recurrent state cannot be rewound, so a delayed judgment is a judgment about now. The channels that come already paid for — the boundary mass that says a thought finished, the surprisal of the arriving word, the un-say that kills a line mid-word when the world answers first, the hold that is a row — are by-products of a forward pass the loop was running anyway; none is free, each is a marginal cost on a computation already bought, and each transport of one across a modality is proven, never assumed. They exist only inside an owned continuous loop over one trunk. A served endpoint asked to reuse its own cache re-encodes the entire context **[M, F-PERSIST]**; a poll pays full inference at cadence while decode-on-delta pays ingest at the world's rate. This is the one place where "no new architecture" is false: the model is unchanged; the serving is new, and it cannot be bought behind an API.

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
- **Cognitive path length** per class and per arm **[M, D1]**: the rows between `ARRIVE` and the terminal `DECIDE`/`EFFECT` (fetches, frames, handoffs, escalations, holds in force), the seats passed through, and the days. The print that says whether the interior was deleted or moved (kill F-PATH). At 500 seats: cells a person decided 12.0 rows · 1.11 seats · 22.4 days; cells the machine disposed of 6.6 · 0.38 · 41.1 — shorter in rows and seats on every class, longer in days because the machine takes the cells people never reached (`claim.settle.auth`: 56.7 rows and 5.0 seats through the warrant chain against 9.3 and 0.84).
- The **frontier bill by value** **[M, D1]**: 78,869 rentals, 61 percent on classes worth $9k–$20k and 0.8 percent on the five worth $60k–$150k; the rented mind is spent on volume, not consequence, until step E ranks its queue by margin thinness × value.

---

## §10 · The twin and the multiverse

### 10.1 The twin

Two firms on one seeded world with nothing shared: the incumbent arm (500 people, the calendar, the cone) and the resident arm (the same people plus the machine at `live`). Because arrivals, determinants and luck are keyed on `(seed, stream, oid)`, the comparison is a paired counterfactual and not two noisy samples. It measures the one number a real programme can never get:

> **bias = (within-firm canary estimate) − (true paired effect)**

where the within-firm estimate compares unattended acts to human decisions inside the resident arm, and the true effect compares the arms. At demand 1.5: true effect −4.36 points of good rate; within-firm +27.37; **bias +31.73** **[M]**. Two mechanisms, both growing with the canary rate: the machine is routed the cases it was licensed for (selection), and the humans in the same firm are working a queue the machine already drained (interference). Budget for a bias of about this size in any real programme; only a twin can print it.

Across demand 1.5 / 1.0 / 0.7 **[M, roundtable]**: net −589.8 / −294.5 / −158.5 M; outcomes worse at every demand (+591 / +326 / +184 M); late gap +0.86 / +5.90 / +5.85; cycle gap +6.05 / +7.93 / +10.21 days; the sign of the total flips at `w_unplaced` ≈ 0.30–0.32 at every demand. The honest sentence: at a planted price for an unplaced obligation above about 0.3 of its value, a drafting machine that never leaves the queue unworked beats a firm that cannot keep up, while losing on correctness, lateness and cycle at every demand.

### 10.2 The multiverse

Best-of-N continuations under edited constraints, sharing one prefix and one physics: ten structural patches (buy a lane on a class, thin margin, span, kappa cap, ε floor, baseline), six rollouts each, 200 days, reweighted by the writ with pessimism. Every patch carries an inverse and lands as a `PATCH` row **[v2]**. Result **[M]**: the three lane-buying patches take the top three places in both runs, the best by $6.6M on an $8M spread — under one standard error, a direction and not a verdict; with the queue priced, acting sooner beats acting later. On the GPU this is §7.2; on the CPU it is 35–44 minutes for sixty rollouts. It prunes; it never promotes: the multiverse says this configuration looks better, and only the world says the machine has earned the right to run it.

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
| O16 | `NullJudge` and `RulesJudge` license nothing, from history or the wager | a truth-reading judge under the null's name | green, C1 |
| O17 | the machine TU builds under `-DACME_NO_PLANT -DACME_NO_GOVERNOR`; one include of the plant or of the governor under it fails | an include reaching `world.h`; one reaching `governor.h` | green, C0 and C1 |
| O18 | a truncated tape reopens, the chain verifies, fold-to-identity holds | a torn row accepted | step D |
| O19 | switch `off` → zero effects; incumbent rows byte-identical to no-machine | an effect under `off` | step D |
| O20 | every licensed band keeps incumbent decisions ≥ ε_floor × arrivals | a rung-5 band with the stratum off | step E |
| O21 | the canary draw is independent of the machine's band: the governor drew before the judge read, one trial per cell, χ² against the rates on the STRATUM rows | a band-dependent draw | green, C1 |
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
| **D** | HOLD and STRATUM rows on change, not per period; the TICK-driven loop; the durable tape with checkpoint and `--dump` on the observer's contract; switch and pill; the gate as a hashed library; O18, O19, the dynamic O25, O30 (every EFFECT re-derives from rows), O30a | a kill survives | **landed 2026-09-15**: identical under `live`; §14b |
| **E** | the retained stratum at every rung; novelty on the frame; the counsel memo; `canary_cap` in the writ; O20, O22, O23 | the multiverse's "explore more" row moves | move; "licensed" restated |
| **F** | `--dump` defines the observer; the golden retires | the observer draws a real automate | none |
| **G** (any time after B) | the census: one real lane folded, no judge | first contact with a world | none |
| **H** | the fitted world; then the warp-per-corp port with CPU parity; O26 | the gym prunes | none until O26 |
| **I** | the calibration harness (a licence key with no measured curve reads rung 0: unmeasured is never safe); `FrontierJudge` and `MouthJudge` behind the seam; `intellect` at `off`; replay with a real judge; the first canary at the floor | the first exterior verdict | the number that matters |

The greenfield arm (`via` 2 → 1: every draft an unattended act) runs on the fitted world in H; it is the only run that measures the act loop's product.

---

## §14a · Amendment, 2026-09-14, after step B and its review

Step B landed on branch `v2` (`receipts/RECEIPT_ACME500_STEP-B_2026-09-14.md`). The paper session's review of it changed this document in five places, all of them precision rather than direction:

1. **The ledger is a verified shadow until C0.** `ledger.h` folds the tape and O14 proves the fold equals the live world, but no consumer reads the fold yet. §14's step C splits into **C0** (consumers onto the fold, the `Plant`/`Ledger`/`Store` split in code, the machine's TU under `-DACME_NO_PLANT`, O17; every reading byte-identical to v1) and **C1** (the judge port, `PlantJudge` on its own key, the read budget, planted truth off the path, the governor's TU and `STRATUM`, O16, O21; numbers move and each delta is named against C0). The minutes-by-via, binding-reason and frame-split prints belong to C0.
2. **C1 carries three items §14 omitted:** the frame-hash rule of §3.6 (the memo key is the promise's own rows, the template pin and span *contents*, never the clock); `template_hash` in the licence key (§4.5); and the forgone-dual print, the sum of the shadow prices of the cells the read budget did not reach, which is nearly free once the duals exist.
3. **`TICK` is emitted, not yet obeyed.** The loop in `main` still drives the period; the inversion (drain intake, fold a `TICK`, run `period()`) lands in step D with the durable tape, and only then can the dynamic **O25** run. What runs today is **O25a**, a source scan of the kernel's files for any clock or environment read, with its own lie.
4. **The pin is the alphabet.** `alphabet_hash()` covers the record version, every record type, verb, reason, act kind and provenance, the band count, the outcome kinds and the class table; the HEADER row carries it and O14 asserts it. `schema_hash()` (the class table alone) is one of its inputs.
5. **O29, row-shape conformance**, is added to §11: one scan of the tape asserting per type that the fields §3.2 says are populated are populated; its lie is an `ARRIVE` with no due day. It is the oracle that catches table drift for the rest of the programme.

Also recorded: `completeness` in the folded cell is read from rows (the frame and decide `ACT` rows, the mid-case `HOLD`, the `PROPOSAL`), never from the plant; the value on those rows is still the plant's at write time and becomes `completeness_hat` in C1. `PROPOSAL.b` is a placeholder judge hash until C1. The compile step's decide fraction is now a fold of `ACT` rows (S5 landed in the remediation; O13 identical). For step D: a `HOLD` row when the reason changes, not per seat per day, with O14 holding across the change.

6. **C0 landed** (`receipts/RECEIPT_ACME500_STEP-C0_2026-09-14.md`, commit `fa71d23` and its addendum on `v2`). `ledger.h` holds the cells and includes nothing from the plant; `port.h` declares `Store::frame` and `Judge::read`; `PlantStore`/`PlantJudge` at the end of `world.h` carry v1's arithmetic behind the seam, the coin still on the world seed by design (F16 closes in C1); `Resident`, `replay`, the invariant miner, `score_arm` and `residual_of` read the `Ledger` and never a `World`; O17 is a build fact with a lie (`tools/o17.py`, CMake `links_without_plant`). Every v1 reading byte-identical, the sim tape byte-identical (chain head `b01905e496dba40e`). The §9.3 prints that attribute — minutes by via (both twin arms and the resident arm of `--automate`), the binding reason per class-band with its lever, the frame residual split into `wait` and `coverage`, the per-type row histogram — print as added lines under the same numbers. Terms-per-rung and the per-class coverage price remain unprinted until the flatten (step E) exists to print them beside. The binding-reason print found **F21**: `Ladder::step` checks the floor against cumulative `n_machine`, so n₀ is paid once and never per rung (n₀/8 on a history-admitted band 2), and 8 of the 10 bands at rung 5 hold fewer wager outcomes than n₀; §1.4's *terms per rung* is kept for the first rung only. Recorded, not fixed; the fix (n₀ per rung, or a floor that scales with the rung) is a design decision and belongs to a numbered step.

7. **C1 landed** (`receipts/RECEIPT_ACME500_STEP-C1_2026-09-14.md`), the first step to move a number, every delta named there. What stands: `Judge::read(const Frame&)` with the judge's competence and noise on its own key (F16 closed); the kernel's completeness from the compile step; `ClassSpec` (authored, with `counterparty`) split from `PlantedSpec` (`world.h`, `planted(c)`), `build_acme` in the plant, the cascade fitted from a handed panel, kappa's denominator metered per decided cell from rows; the read budget (`Writ::read_budget`, 50,000 [BUDGET]) with the memo keyed on the frame hash and the reason `unread`; `governor.h` as its own TU with the salt, writing STRATUM rows before the period and LICENSE/KAPPA after, the ladder keyed to the judge (`license.h`); O16 and O21 with lies, O17 with a second lie for the governor. **Readings at C1** (500 seats, demand 1.5) **[M]**: twin good 42.70 vs 38.06, wrong 8.40 vs 11.94, late 48.90 vs 50.01, total cost $1,837.7M vs $1,254.3M (−583.4), bias +34.42; automate: 32 bands from history, 29 past rung 0, 26 of 48 unlicensable, kappa 0.110, licensed mass 63.5%, N 193 / 182, residual warrant 4.6 · counterparty 0.7 · frame 10.0 (wait 6.8, coverage 3.2) · thin 21.1, reads 1,729 a period with 1,462 carried by the memo (v1: 3,328), frontier 986 a day. The [M] figures elsewhere in this document are v1/C0 readings unless marked C1; the largest single move is kappa 0.242 → 0.110, the metered round trip being 2.2× the plant's formula. **F22**, found by O16 and fixed: history admission licensed a coin judge on 8 class-bands of the battery world until agreement beyond chance (κ ≥ 0.2) became a condition (§4.5). Measured and recorded: the price of a small read budget — at 4,000 a day against the 84,000-cell warm backlog, the twin loses seven points of good rate and the queue win takes a fortnight (kill F-READS now has a curve to draw).

8. **Five claims of this document, re-graded after two outside reviews (2026-09-15)** — documentation mismatch, unsupported guarantee, or unresolved algorithmic issue: (i) coverage from the agreement diagonal: an *unsupported guarantee* in general, a documentation mismatch in the sim (§4.7 now says what it measures); (ii) the e-process with a measured incumbent baseline that resets on promotion and selects its bands: an *unresolved algorithmic issue in the authorization mechanism* — **F23**; no anytime guarantee is established for the rule as built, n₀ is a power calculation and not part of one; **O31** (step E) measures the ladder's false-promotion rate under a judge whose good rate equals the incumbent's, and **no lane goes `live` in step I until O31 has passed or the promotion rule is repaired**; (iii) greedy 1 − 1/e: a documentation mismatch, deleted from §4.2; (iv) Sinkhorn duals nonzero exactly when binding: a documentation mismatch plus an interpretation now stated; (v) the conformal p-value's direction: the text was inverted and the code was not, and the code's percentile is a small construction mismatch corrected with O22 in E. "All wording, none a defect in the code" was the earlier claim and it was too strong.

---

## §14b · Step D, the plan · 2026-09-14

D is the step that makes the tape a store instead of a log and puts the machine behind a switch it cannot write. Its acceptance is two-sided: **a kill survives** (O18) and **every reading identical to C1 under `live`** (D moves no number; the tape may lose rows, and the chain head moves with them, named). Four sub-steps, each with its own build, battery, twenty lie arms, tool oracles, the three modes diffed against C1, a receipt section, a commit and a push. The receipt is `receipts/RECEIPT_ACME500_STEP-D_2026-09-14.md`.

**D0 · rows on change; the TICK-driven loop.** The human arm's `HOLD` rows for reasons 1 and 2 (blocked, no attention) are 73 percent of the sim's rows and carry no state; they are written when the cell's (holder, reason) changes and when it is first held, not per seat per day. The machine's `HOLD` rows likewise on a change of (reason, band); `STRATUM` rows on a change of (kind, canary rate, audit rate). Reason-3 holds (a day that ends mid-case) carry state and stay per occurrence. The ledger's fold is unchanged in meaning (a hold moves nothing), O14 holds across the change, O29's shapes hold. The loop inverts: the world port appends `TICK`; the machine's `on_tick()` folds it, reads its day from the ledger and runs the period; `main` drives intake → tick → machine → incumbent → settle → governor. The `TICK` row's value is wall seconds since the last tick, written 0 by the sim. **Acceptance:** every reading identical to C1; row counts fall and are printed; the chain head moves and is named; O1b, O14, O15, O29 green.

**D1 · the durable tape, the checkpoint, `--dump`.** `Tape` gains a file backend: segments of 40-byte records (`tape.000.seg`, one million rows each) with a 64-byte segment header (magic, record version, alphabet pin, segment index, chain head at the segment's start); the chain head is recovered at open by reading the last segment's header and re-walking its records; a torn tail is truncated to the last complete record and a `NOTE` row with `RF_TORN_OR_SURVEY` appended; `tape.meta.json` (the opener: gate hash, judge hashes, template hashes, firm id, alphabet pin, rows, chain head, cursor). A checkpoint of the ledger (cells, open set, memo, strata, minute meter, the licence table) is written with a `.meta` last by atomic rename carrying the cursor and chain head; restore loads it and folds the rows after the cursor. `--dump DIR` writes the observer lane's contract natively (`tape.bin`, `tape.meta.json`, `firm.json`, `phases.json`, `snapshots.jsonl`, as `ACME500-OBSERVER-REV2/tools/dump_patch/dump.h` defines them), retiring the patch. **O18 · a kill survives:** run N periods to a durable tape, kill mid-period (the battery truncates the file mid-record and drops the in-flight checkpoint), reopen: the chain verifies from genesis, the checkpoint plus the tail fold equals the fold from genesis, and the run continued from the restore is byte-identical to an unkilled run. The lie: a torn row accepted as a row. **Acceptance:** O18 both arms; the dump's `tape.bin` byte-identical to the in-memory tape; readings identical. **Landed** (the step-D receipt, D1): O18 with lie arm 20 (the lie moved to a restore that keeps the period in flight, since a torn record is inert under the cursor rule); a real `SIGKILL` 30 s into the 500-seat run, restored at day 199 with 61,644 rows of the periods in flight dropped and the continued run equal to the unkilled one row for row, one NOTE aside; the dump on the observer's contract, `tape.bin` byte-identical to the segment bodies.

**D2 · the switch, the pill, the gate as a hashed library; O19, O30.** `Switch {off, shadow, live, stop}` read from `intellect.state` (the sim: `--switch`), never written by the machine, folded into the gate's published order after the budget: `off` → `HOLD RS_SWITCH_OFF`; `shadow` → the `EFFECT` row carries `RF_SHADOW` and the hand mutates nothing, the incumbent proceeds and the draft is diffed against what the person then did (a print: shadow agreement per class); `live` → as C1; `stop` → the period returns before it reads. `gate.cpp`: the gate compiled as its own object including `core.h` only, its BLAKE2b over its own source printed in `tape.meta.json` and asserted by the fold. `intellect.heartbeat.json` written atomically every period (tick, rows, chain head); `tools/pill.py` reads it and prints STALLED past three beats. O2 gains a sharpness axis: placement pressure may narrow and never widen. **O19 · off is invisible:** a run at `off` writes no `EFFECT`, and the incumbent arm's rows (every human-written type, in order) are byte-identical to a run with no machine at all; the lie: an effect under `off`. **O30 · every EFFECT re-derives from rows** (proof-carrying autonomy): for each `EFFECT`, the gate library re-run on the cell's last `PROPOSAL`, its `STRATUM` row of that period, the licence table folded from `LICENSE` rows to that day, the class's schema line and the switch reproduces the verdict, the via and the band; the lie: an `EFFECT` whose `PROPOSAL` is missing. **Acceptance:** O19, O30 both arms; readings at `live` identical to D1; the `off` run's incumbent rows equal the sim's.

**D3 · the dynamic O25.** Two runs whose `TICK` values (wall spacing) differ, everything else equal: the verdict, effect, hold and licence rows are identical with the `TICK` values masked. The lie: a resident that reads the `TICK` value. With O25 dynamic, O25a stays as the source scan.

**D2 and D3 landed** (the step-D receipt): `gate.h` pinned by `tools/gate_hash.py` (O30a, pin `d1115a674638eba9`); the switch on the HEADER and NOTE rows, folded, the gate's first check (`switch-off` joins the reasons; the alphabet pin moved); shadow effects as rows on change with the shadow verdict in force a fold field; the pill; O30 (48,000 effects re-derived, all agree; lie: a proposal without its row), O19 (0 effects under off, 288,051 incumbent rows byte-identical to a run with no machine; lie: one cell in five hundred let through), O25 dynamic (70 jittered TICKs, every other byte identical; lie: a resident that reads the clock), O2 with the pressure and switch axes. Shadow at 500 seats: 124,190 shadow effects, the draft agreed with the person on 87.2 percent of 13,235 cells, licensed mass 0. Every reading identical to C1 under `live`. Step D is closed; a kill survives and `live` is unchanged.

**E0, before the rest of E — the shared-fact experiment with outstanding exposure.** A shared determinant per class whose value can move mid-run (one row moves three hundred frames); the judge returning, through the port, a sensitivity per shared fact with its proposal (an arithmetic judge can; a model judge cannot until calibration measures a flip rate); the kernel re-deriving which cells' margins *and bands* the change crosses from the returned sensitivity and the memo — never from the plant's weights — and reading only those; an outcome kind *unresolved* with a rate and an explicit write-off rule; contention for an exposure allowance; `Writ::exposure_cap` with a hold reason and three ledgers (reserved before execution, committed by a receipt, outstanding until the outcome). The E0 receipt counts, separately: source interpretations · affected cases · incremental updates · full reads · changed dispositions and changed authorizations · discrepancies against the full reference · outstanding exposure over time. Then E1: the counsel memo (O23), VOC as the frontier's ranking, the retained stratum (O20, F14), novelty on the frame with the conformal construction fixed (O22, F11), O31, O-TEMPLATE (domain-relative: the compact representation preserves the declared outputs against the full reference over a stated domain; unique in this plant because each determinant lives in one system), the coupling-components print, the latency-and-value sweep of the act loop.

**Not in D:** the counsel memo and the retained stratum (E), `--dump` beyond the observer's contract (F), tenancy, the executor's `RECEIPT` in a real lane (I; the sim's executor is the plant's `world_settle`, and O27 waits for a lane with a counterparty). The observer lane re-runs its fold on D1's dump before D2 lands, so the contract is proven by the page that consumes it.

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
14. **F-HOSTILE** (registered 2026-09-14 after an outside read of the code). The synthetic world embodies the thesis: `p_tacit = 0.02 + 0.38 · decide_frac` plants that judgement-heavy work is the unrecorded work, seat skill is planted per specialty, arrivals are seasonal Poisson, dependencies are 9 percent. So ACME proves that the instrument recovers a planted structure and exploits it, not that real firms have that structure. The kill: a family of hostile worlds in the gym (step H) that scramble those relations one at a time — tacit mass independent of, or inverse to, judgement intensity; the heaviest determinant always tacit; skills uncorrelated with specialisation; bursty arrivals; dependency chains — under which the thesis is false, and the instrument must **report** that world (coverage falls, licensed mass falls, kappa demotes) rather than win in it. An instrument that licenses the same mass in a hostile world is reading the plant, and O17 is not enough. A second variant is *wrong stored values in a system both readers trust*, under which the agreement-diagonal estimator reads "unrecorded" for what is recorded and wrong (§4.7). One variant is adversarial people: seats whose tacit share rises as instrumentation lands and whose escalations multiply once the machine is live, the organization making itself less legible to the meter; the instrument must read the falling coverage and say so.
15. **F-PATH** (registered 2026-09-14). On every class the resident has licensed, its cognitive path length (§9.3) must be shorter than the incumbent's on the same class — fewer rows, fewer seats, fewer days from arrival to disposition. A licensed class with a path as long as the person's (escalated to the same warrant seat every period, re-rented daily) is a round trip relabelled, not an interior deleted, and the licensed-mass figure must not count it as automation.

---

## §16 · Notation

`c` class · `oid` obligation · `d` determinant · `w_d` its weight · `x_d` its value · `p` base failure rate · `δ` non-inferiority tolerance · `n₀(p, δ)` the identifiability sample · `r` canary rate · `V` decisions per term · `T` term · `L_c` verdict latency · `E` decision seats · `α` support coefficient · `F` fixed floor · `N` headcount · `κ` supervision created / removed · `u, v` transport duals · `P_ij` the plan (never materialised) · `supp` support spectrum · `S` flattening dividend · `A / D` agreement / disagreement bands · `via` how a decision left the building · `[M] [P] [BUDGET]` measured, planted, arithmetic.

---

## §17 · The line

*One machine, four worlds, one tape. The interior is deleted by a fold and the edge is kept because it is where the verdict arrives. The kernel owns when, where, whether and how much, has no words in it, is trained on ten thousand synthetic firms per card, and replays bit for bit. The model owns the map from the world's language to the closed alphabet, stands at every cell the field selects, reads the world at the edge, compiles the templates once, and enters through one port as a proposal with a margin, three hashes and a measured curve. The CUDA solver prices every seat and every promise in microseconds and integrates a firm per thread in the gym. FUSOR is resident where the world speaks and the center is a tick because the tape must replay. The draft loop wins the queue on the first day and needs no licence; the act loop earns its licence from the world at n₀(p, δ)/(rV) per rung, and the only levers are the tolerance the writ admits and the volume the tape pools. The floor by law is a seat or two in a hundred. The floor by structure is what the machine cannot yet see. ACME is this program with a planted world behind the port. INTELLECT is this program with a real one. The tape is the proof.*

---
*v2.0 · 2026-09-14 · Claude Opus 5. Supersedes DESIGN.md (v1) and absorbs the REV 2 spec and the roundtable final; overwrites nothing. Nothing in this file is a measurement except the rows marked [M], and every one of those has a file.*

16. **F-TWINS-OF-HISTORY** (registered 2026-09-15, for step H). Two plant configurations that produce byte-identical tapes and respond differently to one structural patch (in one, escalation's manager reads better; in the other, escalation only marks the hard cases and the manager reads no better). The fitted model may identify an action justified in both worlds, state the assumption behind a conditional recommendation, name the observation or experiment that distinguishes them, or decline to certify the consequence; the failure is claiming the tape identifies what it does not. Universal abstention is not required and would punish robust decisions. The admissible hypotheses and the controller's information are explicit inputs of the test, and the plant does not leak which world is active. Ten million rollouts cannot resolve an ambiguity every rollout inherited; the multiverse's job is to search for improvements and to identify the observations that distinguish them from artifacts (§10.2).
