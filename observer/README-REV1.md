# ACME-500 Observer · REV 1 · frame v1

*2026-09-13 · Claude Opus 5. The Claude Design output rendered the cone as six wedges around a cylinder, stacked by depth, which read as platters on a spindle. This revision changes two files and nothing else: `acme/contract.js` (the frame, `place()`) and `acme/world.js` (the static world). The tape, the fold, the fold check, the golden, the HTML, the commands and the hooks are untouched. That is the point of the pure-observer contract: the view was rebuilt without touching the data.*

## What the building is now

| where | who | how it reads |
|---|---|---|
| floor 0 | the lobby | reception, turnstiles, the machine counter on the lift core, the dock bay on the right with its apron and chute |
| floors 1 to 6 | one department per floor, wire w on floor w+1 | pods of eight desks with the lead at the head; managers in glass offices on the window wall; directors in the corner offices; one glass meeting room per floor |
| floor 7 | the executive floor | the C in the back-left office, VPs and every warrant seat in glass offices, the boardroom at the front |
| the annex | the α-functions | a three-storey podium beside the tower, bridged at floor 1, its own meeting room |
| the basement | the resident | the tape spool, the systems rack, the 24 × 6 license grid lit by rung, the kappa gauge, the routine block, in section under the ground slab |
| outside | the boundary | the ledger skyline on the plaza, outcome marks on the dock apron, the roof sign |

The front face is the cut. Curtain walls with mullions on the other three sides, slabs with thickness, columns on ten-metre bays, ceiling strips per pod, a warm light per floor. The footprint is derived from the firm at `prepare()`: more pods, a longer floor plate; 200 seats gives a 38 m plate, 500 gives about 68 m.

Every row still drives the same thing it did: `ASSIGN` carries a token from the dock to a desk, `CONTEXT` beams from the rack, `DECIDE` flashes the lamp, `HOLD` stacks the tray, `ESCALATE` crosses the floor to the office or rides the lift to the executive floor, `EFFECT` leaves by the chute with a ghost, `OUTCOME` lands a mark on the apron, `MEETING` walks figures to the glass room, `LICENSE` recolors a basement tile, `PATCH` darkens a pod, a floor or an annex storey. The director's cutovers are the same eight.

## What was checked

- `place ✓`: four frozen vectors, written as closed forms against the derived dims: the C, the first lead of wire 0, the first IC of wire 0, an α IC in the annex.
- `fold Δ 0` at days 0, 14, 30, 40, 52, 58, 59 against `snapshots.jsonl`.
- No console errors in any headless run.
- 137 to 146 fps at 1412 × 747 in headless Chrome, one draw call reported by the HUD counter for the instanced desks.
- Eight phase shots in `shots/01..08` at the ORBIT camera, plus one per close camera.

## Run it

Serve the folder over HTTP (the golden is fetched by relative path) and open the page:

```
python -m http.server 8766
```

`?raf=1&day=N&cam=X` drives it headless. Cameras: ORBIT, ISO, DOCK, BASEMENT, ANNEX, TOP, LEDGER, CORE, WING.

## Not done

`golden/gen.js` still generates a synthetic firm with its own wire names; it is not the acme binary's tape. The `--dump` verb specified in the brief is the way to replace it. The two-file change here is compatible with that: the contract's tape and snapshot formats did not move.
