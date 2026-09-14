# The observer

A pure observer of the ACME-500 tape: it folds `tape.jsonl` into a state at the scrub position and draws a building that reacts to rows. It never simulates the firm. Frame v1 (`acme/contract.js`) draws the org chart as a headquarters: one department per floor, pods of desks with the lead at the head, managers in glass offices on the window wall, directors in the corners, the executive floor on top, the α-functions in a podium annex with a bridge, the resident in a section-cut basement. `README-REV1.md` has the details and what was checked.

## Running it

The page fetches the golden by relative path, so serve the folder over HTTP:

```
python -m http.server 8766
```

then open `http://127.0.0.1:8766/ACME-500%20Observer.dc.html`. `?raf=1&day=N&cam=X` drives it headless. Cameras: `ORBIT ISO DOCK BASEMENT ANNEX TOP LEDGER CORE WING`.

## One file is deliberately missing

The page was produced with Claude Design and its property panel expects that tool's runtime, `support.js`, beside the HTML. That file is generated code from the design tool and is not part of this repository's licence, so it is not committed. It is present in the original design export; place it beside the HTML to run the page as exported. Everything under `acme/` and `golden/` is this repository's own.

## The golden is synthetic

`golden/` was produced by `golden/gen.js`, a scripted firm with its own wire names, not by the ACME-500 binary. `docs/OBSERVER_BRIEF.md` specifies the `--dump` verb that replaces it with the binary's own tape, snapshots and phases. The contract's tape and snapshot formats did not move in frame v1, so that step is compatible with what is here.
