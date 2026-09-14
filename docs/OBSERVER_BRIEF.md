# ACME-500 world · the brief for Claude Design

*2026-09-13 · Claude Opus 5 · the visualization layer for ACME-500, briefed the way the fighter cockpit was: a pure observer of a frozen contract, replayable with no backend, self-verifiable without a human. The contract below is read off `core.h`, `firm.h`, `world.h`, `solver.h` and `machine.h`, and every input is named exactly like the record. What the backend must add to honor it is at the end.*

## The principle to give the designer

The world is a pure observer of the tape. The designer never simulates the organization. The view reads `firm.json` once, folds `tape.jsonl` rows into a state snapshot at the scrub position, and reacts to events. The only computation it owns is that fold, and the backend ships the same fold as `snapshots.jsonl` so the designer can check theirs against it. If the contract is good, the graphics survive every rewrite of the solver, the gate and the ladder, which is the point: those will be rewritten, because the first run found five defects in them.

Four rules, the same four that made the cockpit's renderer drop in without redesign:

1. **Contract first, world second.** The record types in `core.h` are the vocabulary, named exactly. The golden sample is a 60-day run at 200 seats, four files, hashed.
2. **Frames stated once.** The building is the cone. One placement function from a seat to a position, with frozen test vectors. Floors are depth, wings are wires, the annex is the α-functions, the basement is the resident. three.js is Y-up; the contract's vertical axis is the floor index.
3. **Everything visual is keyed to a row or a field.** A desk lamp lights from `CONTEXT`, a token climbs a stair from `ESCALATE`, a room fills from `MEETING`, a tile changes color from `LICENSE`. Nothing is animation-keyed. If a row does not drive it, it does not move.
4. **Self-verifiable without a human.** Dev hooks, a headless mode, the golden replay, and a stub so the page runs cold. An agent drives it and screenshots it before any backend exists.

## The contract, sketched

Flat, boring, in the units the code uses: days, minutes, dollars, integers.

**`firm.json`, static per run.** `seed`, `span`, `n`, `wires[]` (six names), `classes[]` (`id`, `name`, `wire`, `n_systems`, `verdict_latency`, `base_rate`, `arrival_per_day`, `value`, `reversible`, `warrant`), `seats[]` (`id`, `kind` IC | lead | manager | director | VP | C, `fn` E | alpha | warrant, `wire` 0 to 5 or 6 for cross-cutting, `boss`, `depth`, `salary`, `attention` minutes per day, `spec` list of class ids), `writ` (`thin_margin`, `eps_floor`, `kappa_max`, `audit_min`, `canary_delta`). Planted truth is excluded: no `decide_frac`, no `tacit`, no `skill[]`. The view shows only what the firm can observe about itself.

**`tape.jsonl`, the events, one row per line, the `Rec` struct verbatim:** `type`, `cls`, `day`, `oid`, `seat` (−1 the resident, −2 nobody), `a`, `b`, `margin`, `value`, `arm` (0 incumbent, 1 machine), `band`, `flags`. Types and what `a`, `b` mean:

| type | a | b | note |
|---|---|---|---|
| ARRIVE | | | `value` = money at stake |
| ASSIGN | | hops | |
| CONTEXT | system id | | `value` = minutes |
| DECIDE | decision | hops | `margin` signed |
| HOLD | reason | band | the majority of rows |
| ESCALATE | to seat | escalations | `flags` = 1 warrant |
| EFFECT | decision | previous state | `flags` = reversible |
| UNDO | decision | | |
| OUTCOME | 1 good, 2 late, 3 bad | by_machine | `value` = writ cost |
| MEETING | attendees | 0 one-to-one, 1 status, 2 rollup, 3 cross-wire | `seat` = owner, `value` = minutes |
| TICK | | | |
| LICENSE | rung | band | `margin` = log E, `value` = kappa, `flags` = 1 history-admitted |
| KAPPA | | | |
| PATCH | | | a structural change with its inverse |

**`snapshots.jsonl`, one state per day, the backend's own fold.** `day`, `dow`, `phase`, `seats[id]` (`attn_left`, `meetings_today`, `fatigue`, `holding` list of oids), `open[]` obligations (`oid`, `cls`, `seat`, `state` OPEN | QUEUED | INPROG | DECIDED | ESCALATED, `day_open`, `day_due`, `hops`, `completeness`, `margin`, `systems_opened`, `by_machine`, `band`, `dep`), `ledger` minutes today and cumulative (`glue`, `meeting`, `fetch`, `frame`, `decide`, `commit`, `transport`, `rework`), `machine` (`license[cls][band]` = `rung`, `n_machine`, `n0`, `kappa`, `history_licensed`, `unlicensable`; counters `acted`, `drafted`, `frontier`, `warrant`, `held`, `canary`, `audit`, `undone`; `kappa`; `counter` = machine-chosen outcomes received and needed), `cascade` (`alpha_hat`, `F_hat`, `E_now`, `E_new`, `N_pred`, `headcount`), `residual` (`licensed`, `warrant`, `counterparty`, `frame`, `thin_tape`).

**`phases.json`, the cutovers:** `[{day, phase, note}]` with `phase` in INSTRUMENT, COMPILE, REPLAY, LICENSE, CANARY, CLIMB, COMPILE_OUT, CASCADE. The director cuts on these and on nothing else.

**Commands, the only inbound channel, a closed enum:** `seek(day)`, `speed(x)`, `cam(preset)`, `select(seat | oid | cls)`, `layer(name, on)`, `cutover(phase)`. The view never writes the tape.

## The look, in one paragraph

Sims and SimCity for the enterprise. A cutaway tower with the front wall removed, six wings around a core, floors by depth so the cone reads as a building that is wide at the ground and narrow at the top; the α-functions in an annex beside it; the resident in a green-lit basement with the tape as a physical spool, the systems of record as a rack, and the field as a 24 by 3 grid of class-band tiles lit by rung. Tiny figures walk to glass meeting rooms and back. Obligation tokens arrive at a loading dock, are carried to desks, dim into trays when held, climb stairs when escalated, and leave through a chute at the dock when an effect goes out, with a ghost that stays until the outcome lands. The ledger is a skyline of eight bars on the plaza, and the decide bar is the only one lit warm. The estate's register throughout: dark plate, mono captions, purple for measured numbers, green for the tape and the machine, no logos, a typographic title card. As the program runs, the basement brightens and the floors go dark, wing by wing, and then the annex empties floor by floor after the wings do.

## What to hand the designer, pasted whole

```text
BRIEF — ACME-500 world, pure observer

You are building the visual and interface layer only. You never simulate the firm.
You read firm.json once, fold tape.jsonl into the state at the scrub position, and
react to rows. The backend ships its own fold as snapshots.jsonl; yours must match it.
THREE injected not imported, zero binary assets, every input named exactly as the
record, named meshes. Instanced geometry: the golden has 1.2 million rows and a full
year has five million; you animate a day's deltas, never a row at a time.

1. THE CONTRACT (v0, frozen with a golden sample; days, minutes, dollars, integers)
   firm.json: seed, span, n, wires[6], classes[24] {id, name, wire, n_systems,
     verdict_latency, base_rate, arrival_per_day, value, reversible, warrant},
     seats[n] {id, kind IC|lead|manager|director|VP|C, fn E|alpha|warrant, wire 0-6,
     boss, depth, salary, attention, spec[]}, writ {thin_margin, eps_floor, kappa_max,
     audit_min, canary_delta}. No planted truth is in this file and none is shown.
   tape.jsonl rows: {type, cls, day, oid, seat, a, b, margin, value, arm, band, flags}
     type in ARRIVE ASSIGN CONTEXT DECIDE HOLD ESCALATE EFFECT UNDO OUTCOME MEETING
     TICK LICENSE KAPPA PATCH. a/b per type as the contract table gives them.
   snapshots.jsonl: one state per day — seats, open obligations, ledger minutes,
     machine {license[cls][band], counters, kappa, counter}, cascade, residual, phase.
   phases.json: [{day, phase, note}] — INSTRUMENT COMPILE REPLAY LICENSE CANARY
     CLIMB COMPILE_OUT CASCADE. The director cuts on these and nothing else.
   commands (the ONLY inbound channel): seek, speed, cam, select, layer, cutover.

2. FRAMES, STATED ONCE
   The building is the cone. floor = 5 - depth: ICs on the ground, C-suite on top.
   Six wings at 60 degree intervals around a core, one per wire; fn=alpha seats in
   an annex beside the tower on their own floors by depth; fn=warrant seats on the
   top floors. Within a wing, teams cluster by boss, seats in a row by id.
   place(seat) -> {x, y, z} in meters: floor pitch 4.5, desk pitch 2.0, tower
   radius 45. One function, frozen test vectors: seat 0 (the C) at the core on
   floor 5; the first IC of wire 0 at wing 0, floor 0, slot 0. three.js is Y-up:
   one conversion, never hand-derived elsewhere.
   The dock (the boundary) is outside the ground floor. The basement is the
   resident: the tape spool, the systems rack, the field grid, the counter on the
   lobby wall. Obligations live at the seat that holds them; OPEN ones wait at the dock.

3. EVERYTHING VISUAL IS DRIVEN
   ARRIVE spawns a token at the dock, colored by wire, sized by value.
   ASSIGN carries it to its seat. CONTEXT draws a beam from the rack to the desk
   and lights the fetch lamp. DECIDE flashes the desk; the token becomes a disk
   tilted by the signed margin. HOLD dims the token into the tray, reason on hover;
   the trays are most of what you will see. ESCALATE climbs the token up the stair
   to seat a, and the receiving desk shows a cold re-fetch as its own CONTEXT rows
   arrive. EFFECT sends the token out the chute at the dock and leaves a ghost;
   UNDO brings the ghost back; OUTCOME lands a mark at the dock, good / late / bad,
   rim colored by by_machine, and clears the ghost. MEETING walks the owner and
   attendees to the glass room on that floor for value minutes; cross-wire draws
   a beam between two wings. LICENSE recolors the class-band tile in the basement
   by rung 0-5, with a history-admitted glyph when flags=1. KAPPA moves the gauge.
   PATCH morphs the building (a span change re-lays the floors; a lane bought
   moves a system into the rack). The eight ledger bars on the plaza read the
   snapshot's minutes; the counter on the lobby wall reads machine.counter.
   The cascade is floors going dark: wings first, annex after, in snapshot order.
   Cutovers cut the director: INSTRUMENT (fiber from the rack into the spool),
   COMPILE (the join graph draws itself over the rack), REPLAY (the tower runs as
   a ghost over its own history at 1000x), LICENSE (tiles light), CANARY (sparks
   on live desks), CLIMB (floors dim), COMPILE_OUT (a class collapses into a
   routine block), CASCADE (the annex empties).

4. IT MUST RUN WITH NO BACKEND
   Ship the golden (firm.json, tape.jsonl, snapshots.jsonl, phases.json for 60 days
   at 200 seats) and an in-page stub that scripts a 30-day tape so the page runs
   cold. Scrub and speed from 1x to 1000x. Dev hooks: __state(), __fire(row),
   __cam(preset), __seek(day), __shot(), __fold_check() which folds the tape to
   day N and diffs against snapshots[N]. Headless mode ?raf=1&day=N&cam=X so an
   agent can drive and screenshot it.

5. LOOK
   Sims + SimCity for the enterprise. Cutaway tower, isometric-capable orbit,
   warm desk lamps against a cold dark plate, tiny figures, glass rooms, a green
   basement. AgX tone mapping and bloom. The estate's register: mono captions,
   purple for measured numbers, green for the tape and the machine, no logos, a
   typographic title card. Legible at 1080p; the ledger bars and the counter are
   readable from the default camera.

Acceptance: the page runs from the golden with every row type visibly driving
something; place() passes its frozen vectors; the golden decodes unchanged;
__fold_check() is zero at every day; eight headless shots, one per phase;
60 fps at 1000x on the golden.
```

## What the backend must add to honor the contract

The binary has no export. A `--dump <dir>` verb, about eighty lines in `main.cpp`, writes the four files from a run: `firm.json` from `Firm` and `schema()` with planted fields omitted; `tape.jsonl` from `Tape::rec` with the type enum spelled out; `snapshots.jsonl` by folding after `world_settle` each day; `phases.json` from the phase boundaries `run_machine` already passes through (warm end, replay done, first live day, first rung-2 promotion, first routine, cascade computed). The golden is `--dump` on `--automate --n 200 --days 60 --warm 20`, hashed with BLAKE2b like everything else. Planted truth goes to a separate `truth.json` that the view never loads; a twin overlay may read it later, labeled as planted, off by default.

## In Claude Design

Blank template, Opus 5. Paste the brief block whole. Attach `firm.json`, a 500-row excerpt of `tape.jsonl`, `phases.json`, and `DWG-004_THE-ONE-DESIGN_REV1.png` and `DWG-007_THE-SPECIES_REV1.png` for the register. Do not attach the README or the sources: the designer builds to the contract, not to the code, which is what lets the code change underneath it.
