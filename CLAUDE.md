# ACME-500 — read this first

ACME-500 is a virtual enterprise of 500 knowledge workers and its automation in one C++17 binary: a firm with planted truth, a resident with a field, a gate and a ladder, two graders with disjoint support, and an oracle battery where every oracle carries a lie. It tests the instrument, never a business. `docs/DESIGN.md` is the design; `receipts/` holds every run with its numbers; `docs/KICKOFF_STEP1_HONEST-INSTRUMENT.md` is the brief the last step was built to, with the acceptance for each fix.

Rules that bind here:
- Build with `g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme` or with CMake. No fast-math, no reassociation: determinism is a property the battery tests for.
- Reproduce the battery before editing: 20 of 20 in `--selftest`, every `--lie N` run (0 to 17; 14 is O1b, 15 is O14, 16 is O15, 17 is O29) exiting 0 with the lied-to oracle's own line reading PASS, `python tools/dead_symbols.py .` PASS on both arms (O28) and with `--o25` (O25a), and `python tools/o17.py . g++` PASS on both arms (O17, the machine links without the plant). The standing spec is `docs/ARCHITECTURE_v2.md` (§14 is the order of steps, §14a the amendments after review); `receipts/RECEIPT_ACME500_STEP-C0_2026-09-14.md` is the last step landed, and the next, C1, moves numbers and must name every delta against C0.
- `ledger.h`, `port.h`, `solver.h`, `machine.h`, `report.h` never include `world.h` or `human.h`; the plant is reached through `Store`/`Judge` only. A change that breaks that is caught by O17, and the fix is never to weaken the guard.
- A lie arm counts only when the lied-to oracle is seen to catch its lie. A nonzero exit for an unrelated reason is not a caught lie.
- Never report a process as running without its PID and CPU time against wall time. Never report green without the lie.
- The physics in `firm.h` and `world.h` is not under review. Do not tune a constant to pass an oracle.
- Every step ends in a dated receipt under `receipts/` with commands, outputs, exit codes, denominators, and what was not done.
- Author files with an editor or a Write tool; do not pipe content through shell literals.
