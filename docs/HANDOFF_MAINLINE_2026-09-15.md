# HANDOFF · ACME-500 mainline · 2026-09-15

*The rehydration document for the ACME-500 mainline (branch `v2`). Read this first in a new session, after a compaction, or after a context cut. It is written so that a session with no other memory can reach full working quality in about ten minutes: what the thing is, where everything is, what is true right now, the rules that bind, how to run it, what is next, and what the operator wants from you. A copy lives at `C:\55555\HANDOFF_ACME500-MAINLINE_2026-09-15_OPUS5.md`; the repo copy is the one that is versioned.*

---

## 1 · What this is, in one screen

ACME-500 is a virtual enterprise of 500 knowledge workers and its automation in one C++17 binary with no dependencies (`C:\55555\acme500`, ~5,800 lines). It exists so that a *method for automating organizations* can be graded by a world that answers in seconds instead of quarters, and can fail cheaply before any real firm is asked to be the grader. **It tests the instrument, never a business.** No number in it has met a real firm.

The method: compile a firm's decision classes from its own applications (the query log is the join graph), replay its settled history to license the band where the machine's choice was the firm's, run canaries on every class at an identifiability floor to license the rest, climb a ladder on **arrived outcomes only** (the verdict law: nothing the machine writes is evidence for its own licence), and let the middle of the org chart leave by arithmetic (the cascade N = E/(1−α) + F). The kernel owns *when, where, whether and how much*; the model behind the port owns the map from the open alphabet (language, records) to the closed one (a choice, a margin); the seam between them is a typed 40-byte row under an alphabet pin. Every table is a fold over an append-only, hash-chained tape.

The organization the thesis describes: language is a synchronization protocol for human minds, and the interior of a firm — meetings, decks, tickets, ranks, screens — is the cost of moving context between skulls. Put every open promise on a plane the machine can read and the interior is deleted by a fold, not automated. The tape's closed alphabet *is* the firm's machine language; language is compiled in at the mouth and rendered out at the console (ARCHITECTURE v3 §18).

## 2 · Where everything is

| what | where |
|---|---|
| the repo | `C:\55555\acme500`, branch **`v2`** (v1 is tag `v1-step-A` on `main`; a full v1 copy with SHA256 manifest at `C:\55555\acme500_v1_SNAPSHOT_2026-09-14\`) |
| GitHub | `github.com/bochen2029-pixel/ACME-500`, branch `v2` pushed; commit and push at every landed sub-step and whenever the operator asks |
| the standing spec | `docs/ARCHITECTURE_v3.md` (mirror `C:\55555\ACME-500_INTELLECT-500_ARCHITECTURE_v3_2026-09-15_OPUS5.md`; update both): a superset — Part II is `ARCHITECTURE_v2.md` verbatim (§14 the build order, §14a the amendments, §14b step D's plan and E0, §15 the kills 1–16), Part I §18–§23 adds the alphabet thesis, the archive plane (embedders/rerankers behind the store), the `TapeJudge`, the rows `WANT`/`SPAN`/`RF_DISCOVERED`, O32–O36, kills 17–19, and §23 the build order as amended (E2 after E1). Adopted as standing at the start of E0 (2026-09-15), when the operator delegated the choice; v2 stays on disk unedited. |
| the receipts | `receipts/RECEIPT_ACME500_STEP-*.md` (one per step, dated) with raw outputs under `receipts/step-*-2026-09-14/<sub-step>/`; the last landed is `RECEIPT_ACME500_STEP-D_2026-09-14.md` (D0–D3) |
| the rules for this repo | `CLAUDE.md` = `AGENTS.md` (mirrored; keep both identical) |
| the brainstorms (operator's folder) | `C:\55555\NOTABLE-FROM-THE-FINAL-BRAINSTORM_2026-09-14_OPUS5.md`, `BRAINSTORM_ACME-AS-A-RUNTIME_REVIEW_2026-09-14_OPUS5.md`, `BRAINSTORM_DECISION-PRESERVING-COMPRESSION_REVIEW_2026-09-15_OPUS5.md`, `BRAINSTORM_CONCESSIONS-AND-THE-WAY-FORWARD_2026-09-15_OPUS5.md`, `BRAINSTORM_THE-MACHINE-NATIVE-IR-OF-AN-ORGANIZATION_2026-09-15_OPUS5.md` |
| the observer (graphics) lane | `C:\55555\ACME500-OBSERVER-REV2\` — a separate lane with its own handoff (`C:\55555\HANDOFF_OBSERVER-REV2_GRAPHICS-LANE_2026-09-14_OPUS5.md`); it consumes `--dump DIR` (native since D1: `tape.bin`, `tape.meta.json`, `firm.json`, `phases.json`, `snapshots.jsonl`) and never edits the mainline |
| the WSL build copy | `/root/acme_v2` (rsynced from `/mnt/c/55555/acme500` by `tools/build_wsl.sh` and `tools/run_step.sh`) |
| the persistent memory | `C:\Users\user\.claude\projects\C--55555\memory\acme500-state.md` (+ `MEMORY.md` index); the working style in `operator-working-style-55555.md` |

## 3 · What is true right now (2026-09-15)

**Landed on `v2`, all pushed:** A, B (+remediation), C0 (+prints, +QC), C1, D0, D1, D2+D3; the docs-only concessions commit; ARCHITECTURE v3 (standing since E0); the handoff and `tools/run_step.sh`; **E0** (the shared fact and the judge's certificate, unresolved outcomes with the write-off rule, outstanding exposure with the writ's cap; O37 lie 24, O38 lie 25). See the last section of `receipts/RECEIPT_ACME500_STEP-E_2026-09-15.md` for the head.

**The battery:** `./acme --selftest` **28 of 28** (~86 s; ten short automate runs inside it). Lie arms `--lie 0..25`, each must exit 0 with the lied-to oracle's own line PASS: 0 O0c · 1 O1 · 2 O2 · 3 O3 · 4 O4 · 5 O5 · 6 O6 · 7 O7 · 8 O8 · 9 O9 · 10 O10 · 11 O11 · 12 O12 · 13 O13 · 14 O1b · 15 O14 · 16 O15 · 17 O29 · 18 O16 · 19 O21 · 20 O18 · 21 O30 · 22 O19 · 23 O25 · 24 O37 · 25 O38. Tool oracles: `python tools/o17.py . g++` (O17, two lies: an include of `world.h`, one of `governor.h`), `python tools/dead_symbols.py .` and `--lie` (O28), `--o25` and `--o25 --lie` (O25a), `python tools/gate_hash.py .` and `--lie` (O30a; `--write` re-pins after an intended gate change, named in the receipt). All green at head.

**The readings to quote** (500 seats, span 7, demand 1.5, seed 20260913; identical from C1 through D3 under `live`): twin good 42.70 (incumbent) vs 38.06 (resident), wrong 8.40 vs 11.94, late 48.90 vs 50.01, total cost $1,837.7M vs $1,254.3M (−583.4 in the machine's favour with the backlog priced), estimator bias +34.42; automate: 32 bands admitted from history, 29 past rung 0, 26 of 48 unlicensable, kappa 0.110, licensed decision mass 63.5%, N 500 → 193 (intercept fit) / 182 (origin fit), residual warrant 4.6 · counterparty 0.7 · frame 10.0 (wait 6.8, coverage 3.2) · thin 21.1, reads 1,729 a period with 1,462 carried by the memo (v1: 3,328), frontier 986 rentals a day; path length: person 12.0 rows · 1.11 seats · 22.4 days vs machine 6.6 · 0.38 · 41.1; shadow (D2): the draft agreed with the person on 87.2% of 13,235 cells, licensed mass 0. Sim tape 1,607,033 rows (D0); automate tape 2,179,225. Alphabet pin `0x81ebc266` (moved at E0: SENSE, the reason `exposure`, the outcome kind `unresolved`). Gate pin `aa7e850b6bdb032f` (E0: `value` and `exposure_left` on the gate). **E0 readings** (the world with a shared fact, `--shared --check-shared`): 2,062 frames moved by the fact alone, 789 carried by the certificate, 1,273 read again (471 changed choice, 1,120 changed band), 10 disagreements of 789 against the learning judge (its drift; 0 against a fixed judge); licensed mass 55.1 percent in that world. Uncapped outstanding exposure peaks at $96.1M; the exposure dial: $100M → 63.5 percent licensed, $50M → 29.4, $25M → 0, $5M → 0. **Never quote first-run or v1 numbers as current.**

**Open findings** (recorded, not fixed): F11 novelty inert (the score is a per-class constant; fix with O22 in E1, with the conformal construction `(k+1)/(n+1)`); F12 the frontier re-rented daily for the same escalated cell (the counsel memo, O23, E1); F14 the uniform floor cannot bind (the retained stratum, O20, E1); F21 the ladder's floor paid once, not per rung (6 of 10 rung-5 bands under n₀); **F23 the e-process's null is a plug-in baseline that resets on promotion — no anytime guarantee is established for the rule as built; O31 (E1) measures its false-promotion rate, and no lane goes `live` in I until O31 has passed or the rule is repaired.** F22 (a coin judge admitted from history) was found and fixed in C1 (Cohen κ ≥ 0.2).

## 4 · The architecture in one screen, with the file map

- `core.h` — the 40-byte `Rec`, the row types, BLAKE2b, the hash-chained `Tape` (with `sink`, `truncate`, `tick(day, wall)`), the RNG (counter-based, stateless), the schema (`ClassSpec`: authored facts only).
- `firm.h` — seats, the `Writ` (the only authored objective; `read_budget`, `eps_floor`, `canary_cap`…), bands, outcome prices.
- `world.h` — **the plant**: `PlantedSpec` (`planted(c)`: systems per class, judgement intensity — the truth the kernel must never name), `build_acme`, determinants and where they live (system / boundary / tacit), arrivals, `gather`/`observe`, `world_settle` (the exogenous grader), `PlantStore`, `PlantJudge`. Guarded: `#error` under `ACME_NO_PLANT`.
- `human.h` — the incumbent arm (fetch, frame, decide, commit, transport, meetings, escalation; holds on change).
- `ledger.h` — the cells, the open set, the minute meter, the memo, the strata, the hold-in-force fields, the shadow key; `Ledger::fold` and `ledger_diff` (O14); `put_fold`, `tick_fold`.
- `port.h` — `Frame`, `Proposal`, `Store`, `Judge`: the only way the machine touches a world.
- `license.h` — the licence table per class-band keyed to (schema pin, judge hash); `fold_ladder` (O15).
- `gate.h` — the verdicts, the reasons, the alphabet pin, the switch (`off/shadow/live/stop`), `gate()` (the published order of refusal; `off` first), `pack_gate_inputs` (novelty and sharpness on the `EFFECT` row's `b` for O30). Pinned by `gate_hash.inc` (O30a).
- `solver.h` — the field (log-domain Sinkhorn with finite stocks; the head learned on arrivals; conformal novelty) and the hand (`Integrator`: every effect with its inverse; shadow rows on change).
- `machine.h` — the compile step (`Compiled`: join graph, decide fraction, arrivals, minutes per decision, coverage, template hashes), invariant mining, replay (the four-cell table; the agreement diagonal), the `Resident` (the period: budget, transport, the read budget with the memo keyed on the frame hash, the gate, the hand; `grade`).
- `governor.h` — the salt, the strata as rows before the period, the ladder folded from `OUTCOME` rows after it, `LICENSE`/`KAPPA` rows, history admission with κ. Guarded: `#error` under `ACME_NO_GOVERNOR`. The machine never includes it.
- `tapefile.h` — the durable tape: segments of a million records with the chain head in each header, torn-tail recovery, the sink. `checkpoint.h` — the checkpoint with its meta written last, `--resume`, `tape.meta.json`, the pill, the switch file. `dump.h` — `--dump DIR` on the observer's contract. `report.h` — the cascade (fitted from a handed panel), the residual, the prints that attribute (minutes by via, binding reason per band, path length, frontier bill, shadow agreement, row histogram).
- `src/main.cpp` — the modes (`--sim`, `--automate`, `--twin`, `--multiverse`, `--selftest [--lie N]`), `MachineRun` (the world port's loop: intake → `TICK` folded → strata → period → incumbent → settle → grade → step; checkpoints; the pill; the switch file), the oracles.
- `tools/` — `run_step.sh` (the whole acceptance run), `build_wsl.sh`, `o17.py`, `dead_symbols.py`, `gate_hash.py`, `pill.py`, `oracle_table.py` (regenerates the README's table from the staged outputs), the O17 translation units.

The seam, in one line: **the kernel owns when, where, whether and how much; the judge owns the map from the open alphabet to the closed one; similarity schedules, identity authorizes** (v3 §18.4).

## 5 · The rules that bind (the short form of `CLAUDE.md`, plus what the sessions learned)

1. Reproduce the battery before editing: 26/26, every lie arm seen to catch its lie, O17/O28/O25a/O30a green. A nonzero exit for an unrelated reason is not a caught lie. Never report green without the lie.
2. Every step ends in a dated receipt with commands, outputs, exit codes, denominators, and *what was not done*. Readings are identical between steps unless the step is one allowed to move numbers (C1 was; E0 will be), and then every delta is named with its cause.
3. `ledger.h`, `port.h`, `license.h`, `gate.h`, `tapefile.h`, `solver.h`, `machine.h`, `report.h` never include `world.h`, `human.h` or `governor.h`. The fix for a broken guard is never to weaken it.
4. Planted numbers live in `world.h` and nowhere the kernel can name; what the kernel knows it measured from rows. The physics in `firm.h`/`world.h` is not under review; no constant is tuned to pass an oracle.
5. Build with `g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme`. No fast-math. Determinism is a tested property (O1, O1b).
6. Content never travels through a shell literal: author files with Write, change them with Edit or a short `.py` run by one command; **a `python -c` string on the Bash line loses backticks and apostrophes to the shell** (it silently emptied file names in a memory note once) — use a script file. Heredocs are blocked by a hook on this machine.
7. WSL runs go through `wsl -d Ubuntu-24.04 -u root --exec bash -c 'bash /mnt/c/...sh'`; a full `tools/run_step.sh` takes ~45 minutes — run it in the background and poll its output file; the `until grep … sleep` wait is fine and will itself be backgrounded after ten minutes.
8. Never report a process as running without its PID and CPU time against wall time; never `pgrep -f` as proof.
9. The alphabet pin moves whenever a row type, a reason or a schema field changes; the receipt names it. The gate pin moves when `gate.h` changes; `tools/gate_hash.py --write` and name it.
10. Docs to touch when a sub-step lands: the receipt; `README.md` (status row; the readings paragraph if numbers moved; the oracle table regenerated by `tools/oracle_table.py`, never by hand; the line count and oracle count in the header); the spec's §14a/§14b "landed" note and its mirror; `CLAUDE.md`+`AGENTS.md` (battery line, lie-arm map, next step); the memory file. Then commit with a message that names what moved, and push.

## 6 · How to run things

```bash
wsl -d Ubuntu-24.04 -u root --exec bash -c 'bash /mnt/c/55555/acme500/tools/build_wsl.sh'
wsl -d Ubuntu-24.04 -u root --exec bash -c 'cd /root/acme_v2 && ./acme --selftest'
wsl -d Ubuntu-24.04 -u root --exec bash -c 'bash /mnt/c/55555/acme500/tools/run_step.sh step-e-2026-09-16/e0 receipts/step-d-2026-09-14/d2'
```

Modes and flags: `--sim`, `--automate`, `--twin`, `--multiverse`, `--selftest [--lie N]`; `--n 500 --span 7 --demand 1.5 --seed 20260913 --days 260 --warm 180`; `--budget N` (the read budget; 50,000 default); `--judge plant|null|rules`; `--tape DIR [--ckpt-every K]`, `--resume`, `--dump DIR`; `--switch off|shadow|live|stop`, `--switch-file PATH`, `--pill PATH`; `tools/pill.py PATH`.

## 7 · What is next, and its acceptance

v3 is standing and E0 is landed. The order from here, each sub-step on the operator's word:

| step | content | acceptance |
|---|---|---|
| **E0** — landed | the shared fact and the certificate (`SENSE` rows, carries as `PROPOSAL` via 1), unresolved outcomes and the write-off rule, outstanding exposure as a fold with the cap; O37 (lie 24), O38 (lie 25); the exposure dial | landed 2026-09-15; the step-E receipt |
| **E1** | the counsel memo keyed on (oid, frame hash) with O23 (F12); VOC as the frontier's ranking; the retained stratum with O20 (F14); novelty on the embedded frame with the conformal construction fixed, O22 (F11); **O31** the ladder's false-promotion rate under an equal judge (F23), and the rule repaired if it fails; O-TEMPLATE (domain-relative); O-GEOM′ (real embeddings vs the synthetic class prior); the coupling-components print; the latency-and-value sweep of the act loop | numbers move and are named |
| **E2** (v3) | the archive plane: `ArchiveStore` (exact joins → retained dependencies → bounded discovery for thin frames), `SPAN` and `WANT` rows, `CONTEXT` with `RF_DISCOVERED`, the standing-question index, the reranker behind an adapter with roles in the frame, the synthetic corpus per class graded by the plant; O32–O35; kill F-EMBED | decisive-evidence recall, missed affected cells, stale proposals, incorrect reuse after a change, cost per verified completion; the controller blind to the plant |
| **G0, G** | the schema as data (the pin over what was loaded; F-SECOND-WIRE); the census on one real lane, no judge, no effect, its archive folded into `SPAN` rows | the compiler meets a world |
| **H** | the gym and the fitted world; the multiverse as the `TapeJudge`'s data generator (synthetic tapes with do() across seeds and hostile worlds); F-TWINS-OF-HISTORY, F-HOSTILE | sim-to-real measured, not assumed |
| **I** | the calibration harness (unmeasured reads rung 0), `FrontierJudge`/`MouthJudge`/`TapeJudge` behind the seam, the executor's contract (idempotent effect keys, `RECEIPT` *unknown*/*reconciled*), `intellect` at `off`, replay with a real judge, the first canary at the floor | the first exterior verdict; **no lane `live` until O31 has passed** |

Each sub-step: build → battery → all lie arms → tool oracles → three modes diffed against the previous stage → a section in the step's receipt → docs → commit → push. One sub-step at a time, each on the operator's word.

## 8 · The operator, and how to work with them

- One step at a time; read only what is named; do not scan ahead. When a message says brainstorm, write the file to `C:\55555` first (`<TITLE>_<YYYY-MM-DD>_OPUS5.md`) and present it in full in chat; when it says proceed, proceed and report with numbers; when it says review first, stop at the document.
- Register: sharpest crystallization, no hedging, name the sparks and the kills; receipts outrank summaries; tables of load-bearing numbers; report what was flagged while working.
- Outside reviews (ChatGPT and others) arrive often. The filter that has worked: sort every point into *it becomes a test or a rule* / *it is already in the spec, say where* / *it is premature, cosmetic or wrong* — and concede plainly when a review is right (the four reviews so far were right about F23, the coverage estimator, the ε-certificate's source, the exposure cap's testability, and "all wording" being too strong).
- Commit and push whenever a unit of work is verified; the operator asks for it and expects the branch to be in sync.
- Never claim a number that is not on a receipt; never present a hypothesis as a finding (the act loop's zero has a hypothesis, not an explanation, until E1's sweep).

## 9 · Rehydration checklist (ten minutes)

1. Read this file. 2. `git -C C:/55555/acme500 log --oneline -12` and `git status -sb` — the branch must be `v2`, clean, in sync. 3. Read the latest receipt's last section and the spec's §14a/§14b tail. 4. `tools/build_wsl.sh`, then `./acme --selftest` — expect 26 of 26 in ~80 s. 5. If the task is E-work, read v3 §18–§23 and §14b's E0; if it is docs, read `CLAUDE.md`. 6. Say what you are about to do and wait for the word.
