# ACME-500 · SPEC AMENDMENTS · REV 2 · ONE PORT, FOUR WORLDS

### The design-level changes that make the simulator and the product one program, written to be implemented by a session that has read none of this conversation

**2026-09-14 · Claude Opus 5 · the `C:\55555` session, captured before a context trim.** This document is the spec. It absorbs the transition brief `C:\55555\ACME-500-TO-INTELLECT-500_ONE-MACHINE-FOUR-WORLDS-ONE-TAPE_2026-09-14_OPUS5.md` (another session, four audits, read whole), verifies the claims that decide the order against the source, decides every question that can be decided at the spec level, and changes the brief's order in four places with reasons. Nothing in it has been executed. Every claim about the current code carries a `file:line`, marked **[V]** where this session verified it by grep or read today and **[B]** where it is the brief's receipt not independently re-verified (the brief was written against the same tree hash as below). Every seam carries an invariant and the oracle that catches its violation, and every oracle carries the lie it must be seen to catch.

---

## 0 · The state this spec is written against

| item | value |
|---|---|
| source of record | `C:\55555\acme500\` · git `main` at `4d02132`, clean tree · `https://github.com/bochen2029-pixel/ACME-500` |
| size | 3,257 lines across `core.h firm.h world.h human.h solver.h machine.h report.h main.cpp` |
| source hashes (sha256, first 16) | core.h `605575b5c064cbf6` · firm.h `e405b58b8c4937b9` · world.h `65b584f4185de95c` · human.h `7a91d492d7c57029` · solver.h `de24094d0255ece7` · machine.h `af70957f3dbe95f4` · report.h `8f45d3bc83bab999` · main.cpp `f23e1914361be6ce` · CMakeLists.txt `97168c2e94c9988a` |
| battery | 16 of 16, exit 0; fourteen lie arms, each exit 0 with the lied-to oracle's own line PASS (`receipts/RECEIPT_ACME500_HONEST-INSTRUMENT_2026-09-13.md`) |
| the record | `Rec` is 36 bytes (`core.h:206` **[V]**); 15 record types plus `R_NONE` and `R_N` (17 enumerators, `core.h:169-187`) |
| readings that are the regression baseline | twin: settled 82,479 vs 186,956; good 42.70 vs 38.34; wrong 8.40 vs 11.89; writ cost 236.73 vs 827.68 $M; backlog 1,600.98 vs 420.27; total 1,837.71 vs 1,247.95 (Δ −589.76); open at end 123,927 vs 19,450; canary estimate +27.37 from 12,122 acts vs 78,871 human; assisted 95,963 at 26.0% good; bias +31.73. Automate: coverage mean 0.913 vs planted 0.889; 34 bands to rung 1 from history; 32 past rung 0; 26 unlicensable; kappa 0.242; licensed mass 63.7%; E 412 → 150; N 193 honest / 181 naive; residual 63.7 / 4.7 / 0.7 / 14.3 / 16.7. Multiverse: baseline 834.97 ± 19.38; "explore more" identical to the digit. Sim: 5,109,122 rows, 4,117,431 of them `R_HOLD` (80.6%). |
| open findings, recorded not fixed | **F11** novelty inert (per-class constant completeness) · **F12** the frontier re-rented daily for the same escalated case · **F14** `eps_floor` cannot bind (n₀/V ≥ 0.55 in every class against a 0.35 cap; the gate consults `in_canary` at rung 1 only) |
| build | `g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme` under WSL Ubuntu-24.04 at `/root/acme500`, copied from the Windows tree, never edited there |
| the rules that bind | `C:\55555\acme500\CLAUDE.md` (mirrored `AGENTS.md`): reproduce the battery before editing; a lie arm counts only when the lied oracle is seen to catch it; never report a process running without PID and CPU time; the physics is not tuned to pass an oracle; every step ends in a dated receipt; content through Write/Edit, never shell literals |

**The implementer's first act is to reproduce the row "battery" and the twin/automate readings above, from a fresh WSL copy of the tree at `4d02132`. If any differ, stop and report before editing.**

---

## 1 · The five defects the audits added, verified

These are receipts of the same kind as the seven in the first-run receipt: things the code does that its documents say it does not. They were found by the other session's audits and verified here.

| # | defect | receipt |
|---|---|---|
| **F15** | The documented physics has no caller. `Decider`, `p_good`, `realize_outcome` (`firm.h:274-310`) are described in `firm.h`, `docs/DESIGN.md` and the README as "the one dynamics source both arms call." No call site exists in any file. The grader actually used is `truth_decision`, a sign test on the full weighted signal (`world.h:223`), plus `on_time`, inside `world_settle` (`world.h:291`). | **[V]** `grep` over all sources: zero callers outside `firm.h` |
| **F16** | The rented mind's 72 percent act coin is drawn from the world's seed, `u01(w.seed, 7200 + c, o.id)` (`machine.h:502`). The machine reaches into the plant's RNG. | **[V]** |
| **F17** | Four record types are declared and never emitted: `R_TICK`, `R_KAPPA`, `R_PATCH`, `R_NOTE` have zero `put` sites. The multiverse edits the writ and the planted spec in place with no `R_PATCH` row (`main.cpp:383, 388-390` **[B]**). | **[V]** zero put sites |
| **F18** | `Rec::arm` tags the run, not the decider: `run_machine` passes `arm = 1` to `human_day` and `world_settle` (`main.cpp:159-160`), `world_arrive` always writes 0 (`world.h:193`). The decider is really `seat == -1`. | **[V]** `main.cpp:159` |
| **F19** | Fields the machine depends on never reach the tape: `R_ARRIVE` is written with `a = 0, b = 0` (`world.h:193`) so `day_due` and `dep` are struct-only; `via` exists only on `Obligation` (`world.h:86` **[B]**) and the ladder reads it from the struct (`machine.h:586` **[B]**). A cold fold of the tape cannot rebuild the ladder. | **[V]** `world.h:193`; via added by this session on 09-13 to the struct only |
| **F20** | O1, the determinism oracle, runs `run_human` twice and compares `rec` only, never `chain`; the machine arm has never been under a determinism oracle. | **[V]** `main.cpp` O1 block calls `run_human(A, 40); run_human(B, 40)` only |

Also verified as the brief states **[B→V]**: the machine reads planted `decide_frac` through `FT_DFRAC = sp.decide_frac − 0.2` (`solver.h:103`) and through `would_cost` (`machine.h:480-481`); it computes its own completeness from planted determinant weights via `completeness_from` (`machine.h:422`, `world.h:256-268`); its proposal is the plant's read physics `observe()` with the machine's mask (`machine.h:450`); the salt lives in the `Writ` the resident holds (`firm.h:95`, `machine.h:371-373`); of `Compiled` the resident consumes `join_graph` alone (`C.decide_frac`, `C.coverage`, `C.n_systems` are printed and never read).

---

## 2 · The invariant

One signature, checkable by a build and by an oracle:

> **`period(ledger = fold(tape ≤ h), store, clock) → { proposal rows, verdict rows, effect requests }`**

where `fold` is deterministic and integer-accumulated; the ledger contains nothing not derivable from rows; `store` answers field lookups by key and is the only path to a row's contents; effect requests go to an executor that returns nothing but future rows; and the machine's translation unit does not link the plant.

Three consequences drive every seam in §5:

1. Every field the gate, the ladder, the field or the compile step reads is a row, a fold of rows, or a store lookup.
2. Planted truth is **unlinkable** from the machine, not merely unread. A build target proves it.
3. The clock, the salt, the license table and the switch are inputs the machine cannot write.

Under the signature the four worlds differ only in who appends `ARRIVE`, `CONTEXT`, `DECIDE`(human), `OUTCOME`, `TICK` rows, who answers the store, and who consumes effect requests. **The machine cannot tell which world it is in; that is the property.**

---

## 3 · The mode matrix and the mint table

Two axes, nothing else varies.

| world ↓ · switch → | `off` · watch, judge, record; nothing leaves | `shadow` · verdicts and effect rows flagged shadow; nothing executed | `live` · effects through the executor at the licensed rate |
|---|---|---|---|
| **synthetic** (Plant; planted truth; the battery's home) | `--sim` with the resident inert | the machine replayed on the sim's own history | ACME-500 as today: `--automate`, `--twin`, `--multiverse` |
| **fitted** (Plant with parameters estimated from a real tape) | — | forecast; FORESEE over the fitted firm; prune only | the gym: many fitted-and-randomized firms; trains the value head and the driver; never licenses |
| **replay** (a settled real tape re-decided) | the four-cell table; licenses the agreement band | — | — |
| **real** (CDC in, executor out, outcomes from counterparties) | **INTELLECT day one**: the resident on the stream, inert | drafts diffed against what people do | canaries at r ≥ n₀/V, climbing on arrivals |

What each world may mint:

| world | may | may not | why |
|---|---|---|---|
| synthetic | falsify the instrument; measure the estimator bias; rank structures under planted truth | license anything for a real firm | the grader is authored |
| fitted | forecast one firm; rank patches; train the value head and the driver; prune | promote | a rollout is a view the actor drew of itself |
| replay | license the agreement band; prune classes; set coverage ceilings | license the disagreement band | selective labels |
| real | license the disagreement band at r·V·T ≥ n₀; narrow on any evidence | be bypassed | it is the only exterior |

ACME is (synthetic, live). INTELLECT is (real, off → shadow → live). The name is a build target and a header field.

---

## 4 · Data model amendments, decided

### 4.1 `Rec` v2 · 40 bytes, one static_assert re-pinned

```
struct Rec {              // v2 · 40 bytes · a tape is a scan, not a parse
  uint16_t type;          // RecType
  uint16_t cls;           // decision class
  uint32_t day;           // the TICK index (period). Never wall clock.
  uint32_t oid;           // obligation id; 0 for rows about no obligation
  int32_t  seat;          // >= 0 a seat · -1 the resident · -2 nobody · -3 the governor · -4 the executor
  int32_t  a;             // type-specific, see 4.2
  int32_t  b;             // type-specific
  float    margin;        // type-specific
  float    value;         // type-specific
  uint8_t  arm;           // THE DECIDER: 0 person · 1 machine · 2 governor · 3 executor (was: the run)
  uint8_t  band;          // margin band 0..2 (NBAND = 3)
  uint8_t  flags;         // bit0 reversible · bit1 shadow · bit2 history-admitted · bit3 warrant · bit4 torn-recovered
  uint8_t  via;           // 0 person · 1 unattended act · 2 draft a person keyed · 3 rented mind acted · 4 signer executed verbatim
  uint16_t firm;          // tenant id; 0 in single-tenant. Carried now, multi-tenancy deferred (§5 S13)
  uint8_t  prov;          // provenance: 0 D deterministic · 1 H human-authored · 2 M model-authored · 3 R model-reduced
  uint8_t  ver;           // record version = 2
};
static_assert(sizeof(Rec) == 40, "Rec v2 is 40 bytes; a tape is a scan, not a parse");
```

Decision: widen to 40, do not pack `firm` into `pad`. The chain hashes the 40 bytes. v1 tapes are not read by v2 code; there are no v1 tapes worth keeping (the tape has never been persisted, F17/§1 of the brief).

### 4.2 Record types v2 and their payloads

Types kept: `ARRIVE ASSIGN CONTEXT DECIDE HOLD ESCALATE EFFECT UNDO OUTCOME MEETING TICK LICENSE KAPPA PATCH NOTE`. Types added: `HEADER PROPOSAL STRATUM COUNSEL ACT RECEIPT`. Every type's payload, fixed here so the fold, the observer contract and `--dump` share one table:

| type | seat | a | b | margin | value | band | flags | prov | written by | fold effect on the Ledger |
|---|---|---|---|---|---|---|---|---|---|---|
| `HEADER` | −3 | mode (0 synthetic·1 fitted·2 replay·3 real) ×10 + switch (0 off·1 shadow·2 live) | low 32 bits of BLAKE2b(`tape.meta.json`) | 0 | ver | 0 | 0 | D | the opener | sets mode; the sidecar holds gate hash, judge hashes, serve pins, firm id, schema hash |
| `TICK` | −3 | period index | rows in the previous period | 0 | wall seconds since previous tick (informational; never read by the machine) | 0 | 0 | D | the world port | `ledger.day = a`; ages open cells |
| `ARRIVE` | −2 | **`day_due`** | **`dep` oid or −1** | 0 | money at stake | 0 | 0 | D | the world port | opens a cell: state OPEN, day_open = day |
| `ASSIGN` | seat | 0 | hops | 0 | 0 | 0 | 0 | D | the world port (incumbent arm) | seats the cell: QUEUED |
| `CONTEXT` | seat | system id | 0 | 0 | minutes | 0 | bit1 = machine fetch | D | either arm | `systems_opened |= 1<<a` |
| `ACT` | seat | act kind (0 fetch·1 frame·2 decide·3 commit·4 transport·5 rework·6 glue·7 meeting) | 0 | 0 | minutes | 0 | 0 | D | the incumbent arm (sim); `[survey]` rows in real, `flags bit4` | minutes[seat][cls][kind] += value; the compile step's decide fraction is a fold of these |
| `DECIDE` | seat ≥ 0 | decision | hops | margin | money | band_of(margin) | 0 | H | the incumbent arm | DECIDED by a person; via 0; day_decided = day |
| `PROPOSAL` | −1 | choice | **judge_hash** (int32) | direction | completeness_hat | band_of(direction) | 0 | M | the machine, before its verdict row | recorded; feeds nothing |
| `HOLD` | seat or −1 | reason (`RS_*` for the machine; 1 blocked · 2 no attention for a person) | band | margin/direction | money | band | 0 | H if seat ≥ 0, M if −1 | either arm | held count; tray |
| `ESCALATE` | seat (from) | to seat | escalations | margin | money | band | bit3 warrant | H or M | either arm | seat = a; systems_opened = 0 (cold re-fetch) |
| `EFFECT` | −1 | decision | prev_state | direction | money | band | bit0 reversible · bit1 shadow | M | the machine (gate ACT/DRAFT/WARRANT) | DECIDED by machine; via = via; day_decided |
| `RECEIPT` | −4 | decision | status (0 executed · 1 refused · 2 failed) | 0 | money | band | 0 | D | **the executor only** | effect confirmed; on 1/2 the cell returns to its prior state as an UNDO would |
| `UNDO` | −1 or −4 | decision | 0 | 0 | 0 | 0 | 0 | D | the integrator or executor | restores prev_state |
| `OUTCOME` | **the decider's seat, or −1** | kind (1 good · 2 late · 3 bad) | via (mirror) | margin | writ cost | band | 0 | D | **the world port only** | SETTLED; outcome |
| `MEETING` | owner | attendees | kind (0 one-to-one · 1 status · 2 rollup · 3 cross-wire) | 0 | minutes | 0 | 0 | D | the incumbent arm | readout only |
| `STRATUM` | −3 | 1 canary · 2 audit · 3 retained-control | rate × 1e6 | 0 | 0 | band | 0 | D | **the governor only**, before any proposal on that oid in that period | the period's stratum table; the gate reads `in_canary`/`in_audit`/`to_incumbent` from here |
| `LICENSE` | −3 | rung | band | log E | kappa | band | bit2 history-admitted | D | the governor only | the license table (must equal `license.table`, O15) |
| `KAPPA` | −3 | sup_created (minutes, int) | sup_removed | 0 | kappa | band | 0 | D | the governor, per term | readout |
| `PATCH` | −3 or the operator's seat | kind (0 span · 1 lane bought · 2 writ field · 3 schema field · 4 class collapsed) | param id | **old value (the inverse)** | new value | 0 | 0 | H (operator) or D (governor) | the multiverse / the operator | writ/schema value; the inverse is on the row |
| `COUNSEL` | −1 | judge_hash | **frame_hash** | direction | cost | band | 0 | M | the machine's provider seam | the memo (oid, frame_hash) → proposal |
| `NOTE` | any | code | 0 | 0 | 0 | 0 | 0 | D | anyone | none |

Rule for `prov`: D for facts produced by the world port, the governor, the executor or the ledger mechanics; H for rows a person authored; M for rows a model's proposal authored. **The e-process consumes `OUTCOME` rows only** (D), joined by oid to the `EFFECT`/`DECIDE` row for via, band and judge; M rows are never evidence, they say what was proposed.

### 4.3 `World` → `Plant` + `Ledger` + `Store`

- **`Plant`** (sim only; the planted truth and the generators): `spec[]` (TrueSpec), `tacit[]`, `n_det[]`, `seed`, `demand_scale`, `det_value`, `full_signal`, `truth_decision`, `arrivals_today`; methods `tick(tape, day)`, `arrive(tape, day)`, `settle(ledger, tape, day)` (the only writer of `OUTCOME`; reads the ledger's decisions, computes truth from its own spec, never sees who decided), `human_day(ledger, firm, tape, day)` (the incumbent arm: writes ASSIGN/CONTEXT/ACT/DECIDE/HOLD/ESCALATE/MEETING), `execute(effect) → RECEIPT` (the sim's executor).
- **`Ledger`** (both modes; built only by folding rows): `ob[]` with `id cls day_open day_due day_decided day_settled seat state outcome hops escalations systems_opened decision via band dep margin completeness_hat`, `open_idx`, `next_id`, `day`, `minutes[seat][cls][act]`, `license` (folded from LICENSE rows), `strata` (this period's STRATUM rows), `memos` (oid, frame_hash → proposal), `patches`. `completeness_hat` is the machine's own estimate (§5 S4), never planted.
- **`Store`** (both modes; the data layer): `frame(oid, join_graph) → Frame` returning the recorded field values the machine may see. Sim implementation `PlantStore` returns the plant's `det_value` for determinants in instrumented systems plus the boundary determinant, with tacit ones absent. Real implementation returns the CDC snapshot's fields. The tape records `CONTEXT` (which system was read); the store holds what was read.

### 4.4 `ClassSpec` → `Schema` + `Planted`

- **`Schema`** (authored, ten lines per wire, both modes): `name wire verdict_latency value reversible warrant arrival_per_day base_rate`. `base_rate` is the authored prior for n₀ until outcomes exist (§5 S4).
- **`Planted`** (sim only, held by `Plant`, never by `cls_spec()`): `decide_frac`, the true `n_systems`, tacit mass.
- `cls_spec(c)` returns `Schema`. Any reference to `decide_frac` or the true `n_systems` outside the plant TU is a build error under `-DACME_NO_PLANT`.

### 4.5 Files the machine reads and never writes

| file | writer | reader | content |
|---|---|---|---|
| `governor.salt` | the operator, once | the governor only | the arena's salt. The machine's TU has no read of it |
| `license.table` | the governor's `Ladder::step` | the gate, every period | per (firm, class, band, judge_hash): rung, expiry, n_machine, n0 |
| `intellect.state` | the operator | the gate, every period | `off · shadow · live · stop` |
| `intellect.heartbeat.json` | the machine, atomic, every beat | the operator / the pill | last tick, rows, VRAM if any; staleness > 3 beats = STALLED |
| `firm.json` | `build_acme` in sim; a person plus the template compiler in real | everything | Schema + seats + writ; the one hand-written object |
| `tape.meta.json` | the opener | everything | mode, gate hash, judge hashes, serve pins, firm id, schema hash; its BLAKE2b low 32 bits in the HEADER row |

### 4.6 The judge port

```
struct Frame    { int cls; uint32_t oid; std::vector<float> vals; std::vector<uint8_t> present;
                  float coverage_hat; uint32_t frame_hash; };
struct Proposal { int choice; float direction; float completeness_hat; uint32_t judge_hash; };
struct Judge    { virtual Proposal read(const Frame&) = 0; virtual uint32_t hash() const = 0; };
```

Judges: `PlantJudge` (sim; today's `observe()` arithmetic over `Frame::vals` with the learned `mach_skill`, its noise keyed on **the judge's own seed**, never `w.seed`); `NullJudge` (constant choice) and `RulesJudge` (sign of a fixed weight vector), the two nulls; in INTELLECT `RoutineJudge` (compiled), `EventModelJudge`, `FrontierJudge` behind the provider seam. Every `PROPOSAL`, `EFFECT`, `HOLD` by the machine carries `judge_hash`; the license table is per (class, band, judge_hash). The rented mind's act coin (F16) becomes a property of the judge.

### 4.7 The governor

Its own translation unit (`governor.h/.cpp`), never included by the machine's TU. Holds the salt; each period, for every open oid, draws canary / audit / retained-control and appends `STRATUM` rows **before** the machine's period runs; consumes `OUTCOME` rows; runs the ladder; appends `LICENSE` and `KAPPA` rows and writes `license.table`. In sim it runs in-process from its own TU; in INTELLECT it is a separate process (fusord's switch pattern generalized).

### 4.8 The gate as a library

`gate.h/.cpp` includes `core.h` only; compiled as its own object; BLAKE2b of `gate.cpp`'s bytes is the gate hash printed in `tape.meta.json` and the HEADER row. Published order of refusal unchanged: blocked → warrant reserved → novelty → unlicensed → audit → thin → sharp → irreversible below rung 4 → rung-1 without canary → budget last. It takes the switch as an input: `off` → no `EFFECT` row, no effect request; `shadow` → `EFFECT` with bit1; `live` → effect request.

---

## 5 · The seams

Numbering keeps the brief's S1–S18; S0 and S10b are this session's; S13 is deferred. For each: today, change, invariant, oracle with its lie, and whether printed numbers may move.

**S0 · Determinism over the machine arm (F20).** Today O1 covers `run_human` only and compares `rec` not `chain`. Change: `--automate` twice on one seed → `memcmp` of `rec` **and** `chain` **and** the license table. **O1b · the machine arm is deterministic**; lie: a `std::rand()` or wall-clock read in the resident. *Numbers: none move.* This is the baseline every other oracle needs and it comes first.

**S10b · Delete the dead physics and fix the documents (F15).** `Decider`, `p_good`, `realize_outcome` are deleted from `firm.h`. `docs/DESIGN.md`'s paragraph and the README's "one dynamics source" sentence are rewritten to what the code does: the correct decision is the sign of the full weighted signal over the determinants; a decider sums only the determinants it gathered plus noise scaled by judgment intensity and competence; completeness is load-bearing through which determinants get summed, not through a quality curve. **O28 · no dead physics**: `tools/dead_symbols.py` lists every function defined in `firm.h`/`world.h` with zero call sites outside its definition; the list must be empty; lie: add an uncalled function. *Numbers: none move; no re-run of receipts is owed. Decided here: deletion, not resurrection.*

**S1 · The world port: `World` → `Plant` + `Ledger` + `Store`.** Today `World` holds both truth and run state (`world.h:91-104`), and every consumer reads `World::ob` (the only tape→state fold in the repo is the observer's JavaScript). Change per §4.3. `human_day`, `world_arrive`, `world_settle` become `Plant` methods that append rows; the machine and the report read the `Ledger`. Invariant: `Ledger == fold(tape)` at every period. **O14 · fold-to-identity**: rebuild the ledger cold from the tape after a run and `memcmp` it against the live one (the `Obligation` array, `open_idx`, minutes, license); lie: a struct field set without a row (an `escalations++` with no `ESCALATE` row). *Numbers: none move.*

**S2 · The missing fields on the tape (F17, F18, F19).** `Rec` v2 (§4.1); `ARRIVE` carries `day_due`/`dep`; `EFFECT` carries `via`; `arm` means the decider; `OUTCOME.seat` names who decided (today `o.seat`, which `Integrator::commit` never sets, `solver.h:333` **[B]**); `TICK` emitted per period; `PATCH` emitted for every multiverse patch with its inverse; `KAPPA` per term; `PROPOSAL` before every machine verdict; `HEADER` at open. **O15 · the ladder is a fold**: recompute every `Lic` from `OUTCOME` rows joined to `EFFECT`/`DECIDE` rows and compare to the governor's; lie: an `OUTCOME` counted twice. *Numbers: none move.*

**S3 · The judge port.** Today the machine's proposal is `observe()` on the plant with the machine's mask (`machine.h:450`; the rented mind the same call at competence 0.96, `machine.h:500`; replay `machine.h:255`). Change per §4.6. Invariant: the machine never calls `observe` on a `Plant`. **O16 · the judge is swappable and a null licenses nothing**: run `--automate` with `NullJudge` and with `RulesJudge`; neither may clear the e-process on any band over a full run; lie: a judge that reads `truth_decision` substituted under the null's name, which licenses everything and must be caught. *Numbers: move, because `PlantJudge`'s noise is keyed on its own seed; the receipt prints the deltas against §0.*

**S4 · No planted truth on the machine's path.** Five replacements: `FT_DFRAC` (`solver.h:103`) → `C.decide_frac[c]` folded from `ACT` rows; `would_cost` and review/sign costs (`machine.h:480-481, 520, 531`) → metered minutes per class from `ACT` rows and `C.n_systems[c]` from `CONTEXT` rows; `completeness_from` (`machine.h:422`) → `Frame::coverage_hat` = `C.coverage[c]` × (join-graph systems present in this cell's `CONTEXT` rows / join-graph size); n₀ (`core.h:324`) → the authored `Schema::base_rate` as prior, replaced by the fold over `OUTCOME` rows once a class has ≥ 30 settled outcomes; `unlicensable` and `canary_rate` (`machine.h:359, 381`) → arrivals per term folded from `ARRIVE`. `ClassSpec` split per §4.4. Invariant: `machine.h`, `solver.h`, `report.h`, `gate.cpp` compile under `-DACME_NO_PLANT`, where the plant's section is `#error`. **O17 · the machine links without the plant**: a CMake test target that compiles the machine TU with `-DACME_NO_PLANT` and asserts success, and a second that includes the plant section under the flag and asserts failure (the lie). *Numbers: move; the receipt prints them.*

**S5 · The minute meter on the tape.** Today `decide_frac` is measured from `HumanStats::by_class` (`main.cpp:140`), an in-memory meter. Change: the incumbent arm appends `ACT` rows in sim (§4.2); in real, `ACT` rows carry `flags bit4` and a `[survey]` provenance from the runbook's time-use sample. O13 grades the recovered ranking from rows. *Numbers: none move (the fold equals the meter).*

**S6 · The tape becomes the store.** Adopt fusord's tape discipline: a durable append-only file of 40-byte records in segments, BLAKE2b-256 chain with the head recovered at open, torn-row recovery (`flags bit4` on the recovered row's successor NOTE), a cursor committed as ingested, a checkpoint whose `.meta` carries the cursor, a JSONL projection for the observer via `--dump`. `chaining = false` legal only for `--multiverse` forks and printed as such. **O18 · the tape survives a kill**: run; truncate the file at a random byte inside the last period; reopen; the chain verifies to the last complete row; fold-to-identity against a clean run to that row; lie: a torn row accepted as complete. *Numbers: none move.*

**S7 · The switch and the pill.** Per §4.5 and §4.8. ACME defaults the switch to `live` (its world is the plant); INTELLECT defaults to `off`. **O19 · inert-off**: with `off`, a run produces zero `EFFECT`/`RECEIPT` rows and the incumbent's rows are byte-identical to a run with no machine; lie: an effect written under `off`. *Numbers: none move under `live`.*

**S8 · The gate as a hashed library.** Per §4.8. Invariant: `gate.o` has no symbol from `solver.h`, `machine.h`, `world.h`. O2 continues to prove it never widens over the lattice; the HEADER row carries the hash. *Numbers: none move.*

**S9 · The retained stratum at every rung (F14's structural half).** Today the gate consults `in_canary` at rung 1 only (`solver.h:306` **[B]**); above that the flag relabels an act. Change: at every rung, an oid drawn into the **retained-control** stratum (`STRATUM a = 3`) is routed to the incumbent arm (a person, or in INTELLECT the seat that still exists) and never acted; the control arm never reaches zero and stays the paired baseline for kappa and demotion. `eps_floor` becomes the retained stratum's uniform floor and binds by construction. The 0.35 cap moves into the writ as `canary_cap` so the finding n₀/V > cap prints as a schema fact. The licensing canary rate stays `min(cap, max(floor, n₀/V))`. **O20 · the stratum never reaches zero**: over a run, every licensed class-band shows incumbent decisions ≥ floor × arrivals per term; lie: a rung-5 band with the stratum disabled. *Numbers: move; "licensed" now means "licensed minus the retained stratum," and the receipt says so.*

**S10 · The salt, the seed and the license table leave the machine (F16).** Per §4.5, §4.7. The machine reads `STRATUM` rows and `license.table`; its TU has no read of `governor.salt` or `Plant::seed` and no write of the table. The rented mind's act coin is drawn by `PlantJudge` from its own key. **O21 · the salt is outside**: over a run, a chi-square test of the canary draw against quantile bins of every machine feature is null; lie: a feature-dependent draw. *Numbers: move slightly (the draws are re-keyed); the receipt prints them.*

**S11 · Novelty on the Frame (F11).** Today `nov = novelty(c, |mach_complete − 0.7|)` is a per-class constant. Change: the conformal score is the magnitude of the frame's read signal against the class's calibration sample in sim; the frame embedding or the trunk's hidden state in INTELLECT. **O22 · novelty fires on a planted stranger**: plant obligations whose determinant values are drawn from a shifted distribution; ≥ 90 percent must route to `V_WARRANT/RS_NOVEL`; lie: the shift removed. *Numbers: move.*

**S12 · The counsel memo (F12).** Today a `V_FRONTIER` cell that fails the act draw is re-gated and re-rented daily (`machine.h:494-515`). Change: the frontier's proposal lands as a `COUNSEL` row keyed by (oid, frame_hash); re-ask only when a newer row on the cell changes the frame. **O23 · counsel is memoized on the frame**: identical frame twice → one call; a changed frame on the same oid → a second call; lie: a memo keyed on oid alone. *Numbers: move; the frontier count falls by the repeat fraction.*

**S13 · Tenancy — DEFERRED.** `Rec::firm` is carried and written as 0. N tapes under one governor, `firm.json` per tenant and O24 are not built until a second real wire exists. Decided here.

**S14 · The clock is a row.** Today the period loop is a `for` in `main` (`main.cpp:156-163`) and `human_day`/`world_settle` take `day`. Change: the world port appends `TICK`; the machine runs `period()` when it folds one; sim ticks daily, INTELLECT on a configured period with events ingested between ticks. Invariant: the machine reads time only from `TICK`. **O25 · the world never dilates**: the same rows at different wall spacing yield identical verdict rows; lie: a wall-clock read in the resident. *Numbers: none move.*

**S15 · The fitted world.** `WorldParams` (arrival rates, verdict latencies, base rates, value, seat attention and specialty, the incumbent's decision distribution per class-band, dependency rate, slack) with `Plant::synthesize(seed)` (today's `make_world`/`build_acme`) and `Plant::fit(tape, firm.json)`. `--multiverse` forks either. It prunes; it never promotes. **O26 · the fitted twin forecasts the held-out quarter**: fit on three quarters of a synthetic tape, forecast the fourth, beat the naive rate on arrivals, lateness and good rate; lie: the fit peeks at the fourth quarter. *Later; see §7.*

**S16 · The act plane and the provider seam.** In `live`: GATE → ESCROW (reserve the writ's budget) → EFFECT row with its inverse, fsynced → EXECUTE (the adapter) → `RECEIPT` row minted by the executor, never the machine → SETTLE. Harm dial: `V_DRAFT` = H0, reversible `V_ACT` = H1, `V_WARRANT` = H3, irreversibles never below H2. Every frontier call through one module with one meter, a signed pricebook, escrow before dispatch, a hard budget stop, fingerprints on every output, validated JSON, quarantine never silent drop; in sim the "frontier" is `PlantJudge` at competence 0.96 and the meter charges `w_frontier` as today. **O27 · no un-receipted effect**: every `live` `EFFECT` has a matching `RECEIPT` or `UNDO`; lie: an effect without a receipt. *Later.*

**S17 · `--dump` defines the observer.** `--dump <dir>` writes `firm.json`, `tape.jsonl` (the §4.2 table, one object per row, type as its name), `snapshots.jsonl` (the fold per tick), `phases.json` (phase boundaries derived from rows: first `LICENSE` = LICENSE, first `STRATUM a=1` = CANARY, first `LICENSE a≥2` = CLIMB, first `PATCH kind 4` = COMPILE_OUT), `truth.json` (planted; never loaded by the view). The observer's `contract.js` is regenerated from the §4.2 table; the seven convention mismatches the audit found (minutes in `b` vs `value`, outcome 0..2 vs 1..3, six hold reasons vs eleven, `ARRIVE` seat −1 vs −2, six bands vs three, `margin` decoded as int, `arm` as phase) are resolved in the binary's favour; `golden/gen.js` is retired. *Numbers: none move.*

**S18 · The name.** One library `libacme` (core, ledger fold, machine, gate, ladder reader, judge interface, report), `plant` (the planted section, `PlantJudge`, `PlantStore`, the incumbent arm), `governor`, and two wrappers: `acme` (world synthetic, switch live, plant linked) and `intellect` (world real | replay | fitted, switch off, plant unlinked, O17). The header row prints mode and hashes; the observer's title card reads it.

---

## 6 · The oracle table, v2

Every oracle carries a lie. The existing O0–O13 stay; their lie arms stay; CMake asserts the lied oracle's own line per arm. Added:

| oracle | asserts | the lie it must catch |
|---|---|---|
| O1b | two `--automate` runs, one seed: `rec`, `chain`, license table identical | a `std::rand()` in the resident |
| O14 | cold fold of the tape == live ledger | a struct field set without a row |
| O15 | ladder recomputed from rows == the governor's | an OUTCOME counted twice |
| O16 | `NullJudge` and `RulesJudge` license nothing | a truth-reading judge under the null's name |
| O17 | machine TU builds under `-DACME_NO_PLANT`; plant section under the flag fails | a `#include` reaching `truth_decision` |
| O18 | truncated tape reopens, chain verifies, fold-to-identity holds | a torn row accepted |
| O19 | switch `off` → zero effects, incumbent rows byte-identical to no-machine | an effect under `off` |
| O20 | every licensed band keeps incumbent decisions ≥ floor × arrivals | a rung-5 band with the stratum off |
| O21 | canary draw independent of every machine feature (chi-square null) | a feature-dependent draw |
| O22 | ≥ 90 percent of planted strangers route to WARRANT/NOVEL | the shift removed |
| O23 | identical frame → one counsel call; changed frame → two | a memo keyed on oid only |
| O25 | same rows, different wall spacing → identical verdicts | a wall-clock read |
| O26 | fitted twin beats the naive rate on the held-out quarter | the fit peeks |
| O27 | every live EFFECT has a RECEIPT or UNDO | an effect without a receipt |
| O28 | no function in `firm.h`/`world.h` without a caller | an uncalled function added |

O24 (two firms, one binary) is deferred with S13.

---

## 7 · The order, with acceptance per step

Two rules set it: receipts before doctrine, and never break the loop to add detail. Every step ends with the whole battery green, every lie arm seen to fire, and a dated receipt under `receipts/` that prints the §0 readings beside the new ones.

| step | seams | acceptance | printed numbers |
|---|---|---|---|
| **A** | S0, S10b | O1b and O28 green; `DESIGN.md`/README corrected | **identical** to §0 |
| **B** | S2, S14, S5, S1 | `Rec` v2; every type in §4.2 emitted where the table says; `Plant`/`Ledger`/`Store`; O14, O15 green | **identical** to §0 (bookkeeping only); if any moves, a row was missing and the step is not done |
| **C** | S3, S4, S10 | judge port, `PlantJudge` on its own key, planted truth off the path, governor TU, `STRATUM` rows, O16, O17, O21 green | **move**; the receipt prints each delta and names the seam that moved it |
| **D** | S6, S7, S8 | durable tape, switch, pill, gate library with hash; O18, O19 green | identical to step C under `live` |
| **E** | S9, S11, S12 | retained stratum, novelty on the frame, counsel memo; O20, O22, O23 green; the multiverse's "explore more" row moves a digit | move; the receipt restates "licensed" |
| **F** | S17 | `--dump`; `contract.js` regenerated; `gen.js` retired; the observer draws a real `--automate` dump | none |
| **G** | the census | one real lane's CDC folded into a `Ledger` with no judge: counts, classes, arrivals per term, base rates from any settled outcomes. Can start the day step B lands | none (a first contact) |
| **H** | S15, then the GPU shape | fitted world, O26; then thread-per-corp and warp-per-corp with CPU parity, from the flight computer's execution model | none until O26 |
| **I** | S16, S18 | `intellect` wrapper on the real lane at `off`; then replay with a real judge (the four-cell table); then the first canary at the floor, which is the first wager and is unchanged by all of the above | the first exterior verdict |

Step G is pulled forward from the brief's last place: a census needs no judge and it is the first contact with a world. S13 is deferred out of the order. The GPU port stays behind the port; a port of a machine that still reads the plant would port the leak.

---

## 8 · Kills for the transition design

1. **F-XMODE.** A synthetic tape replayed through `intellect` (world replay, plant unlinked) does not reproduce `acme`'s verdict rows byte for byte → the machine still reads a non-row. Requires O1b first.
2. **F-LINK.** `intellect` links a plant symbol → S4 failed at the build.
3. **F-INERT.** Under `off`, one byte of the world changes or one effect row appears → S7 failed.
4. **F-FOLD.** A cold fold disagrees with the live ledger or ladder on any field → S1/S2 incomplete.
5. **F-STRATUM.** A licensed band's incumbent rows fall to zero over a term → S9 did not bind.
6. **F-SALT.** A planted feature-dependent draw is not caught → the machine can see its own grading.
7. **F-FIT.** The fitted twin's held-out forecast is no better than the naive rate → the gym cannot even prune.
8. **F-NULL.** A null judge clears the e-process on any band → the ladder licenses noise.
9. **F-TRANSFER.** Templates and rates chosen in the randomized gym do not beat hand-set ones on the first real replay.

---

## 9 · What not to do

- Do not start the GPU port, multi-tenancy, or the fitted world before steps A–D land. A port of a leaking machine ports the leak.
- Do not re-run receipts for S10b; nothing calls the deleted code and no number can move.
- Do not tune any constant to pass an oracle; do not touch `truth_decision`, `full_signal`, `observe`'s arithmetic, or the schema's constants except as §4.4 splits them.
- Do not quote any number from the first-run receipt; §0's readings are the baseline.
- Do not let a step end with a number that moved unless the receipt names the seam that moved it.
- Do not accept a nonzero exit as a caught lie; the lied oracle's own line must read PASS.
- Do not report a long run as alive without its PID and CPU time against wall time.
- Do not put content through a shell literal; Write and Edit only. Build under WSL from a fresh copy of the Windows tree.

---

## 10 · Decisions made here, so the implementer does not reopen them

| question | decision |
|---|---|
| record width | 40 bytes, v2, `firm` and `prov` and `ver` explicit; no v1 compatibility |
| the dead physics | delete; fix the two documents; no re-run |
| the machine's completeness | its own estimate from `CONTEXT` rows and the compile step's coverage; never the plant's weights |
| n₀ before outcomes exist | the authored `Schema::base_rate`; folded from outcomes after 30 settled per class |
| the stratum above rung 1 | routed to the incumbent, never acted; this is what "the sample never goes to zero" means |
| the 0.35 cap | a writ field, `canary_cap`, printed |
| the rented mind's coin | the judge's own key |
| the frontier's proposal | a `COUNSEL` row memoized on (oid, frame_hash) |
| who writes `OUTCOME` | the world port only, as today |
| who writes `STRATUM`, `LICENSE`, `KAPPA` | the governor only |
| who writes `RECEIPT` | the executor only |
| the observer's conventions | the binary's, via `--dump`; `gen.js` retired |
| tenancy | deferred; the field is carried |
| the order | A → B → C → D → E → F → G(any time after B) → H → I |

---

## 11 · Pointers

- the brief this absorbs: `C:\55555\ACME-500-TO-INTELLECT-500_ONE-MACHINE-FOUR-WORLDS-ONE-TAPE_2026-09-14_OPUS5.md`
- the receipts: `C:\55555\acme500\receipts\RECEIPT_ACME500_FIRST-RUN_2026-09-13.md`, `…\RECEIPT_ACME500_HONEST-INSTRUMENT_2026-09-13.md`, raw outputs beside them
- the design as it stands: `C:\55555\acme500\docs\DESIGN.md`; the method: `docs\METHOD_TWO-GRADERS-TWO-BANDS.md`; the observer brief: `docs\OBSERVER_BRIEF.md`; the step-1 kickoff (done): `docs\KICKOFF_STEP1_HONEST-INSTRUMENT.md`
- the standing order: `C:\55555\NEXT_2026-09-13_OPUS5.md` (this document is its step 2 kickoff for ACME; steps 2 and 3 of NEXT, the three cheap kills and `--dump`, are unchanged and `--dump` is step F here)
- the wider doctrine this sits under, for context only: `C:\55555\THE-LIMIT_THE-VERDICT-LAW_2026-09-14_OPUS5.md`, `C:\55555\THE-THIN-PLANE_THE-FRAME-ASSEMBLER-IS-A-FOLD_2026-09-14_OPUS5.md`
- fusord's tape, switch and pill, to be copied not rewritten for S6/S7: `C:\fusor1\fusord.cpp` §5–§6
- the memory: `C:\Users\user\.claude\projects\C--55555\memory\acme500-state.md`

---

## 12 · The line

*A simulator becomes the product when the machine cannot tell them apart. ACME-500 has the gate that never widens, the ladder that climbs only on arrivals, the tape that records its silences and the battery that catches its own lies. It still has a machine that reads the world's mind: planted weights, planted judgment, a salt in its own pocket, a coin from the world's seed, and fields that never became rows. Cut the world into a plant, a ledger and a store; make every input a row; put the salt, the license and the switch in files the machine cannot write; give the port four worlds; and prove the unlink with a build. Then ACME is (synthetic, live), INTELLECT is (real, off), the tape is one 40-byte record, the observer draws either, and the only thing that changes on the day a real firm's stream is plugged in is the header line.*
