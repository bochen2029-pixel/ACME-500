# ACME-500 — read this first

ACME-500 is a virtual enterprise of 500 knowledge workers and its automation in one C++17 binary: a firm with planted truth, a resident with a field, a gate and a ladder, two graders with disjoint support, and an oracle battery where every oracle carries a lie. It tests the instrument, never a business. `docs/DESIGN.md` is the design; `receipts/` holds every run with its numbers; `docs/KICKOFF_STEP1_HONEST-INSTRUMENT.md` is the brief the last step was built to, with the acceptance for each fix.

Rules that bind here:
- Build with `g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme` or with CMake. No fast-math, no reassociation: determinism is a property the battery tests for.
- Reproduce the battery before editing: 16 of 16, and every `--lie N` run exiting 0 with oracle N's own line reading PASS.
- A lie arm counts only when the lied-to oracle is seen to catch its lie. A nonzero exit for an unrelated reason is not a caught lie.
- Never report a process as running without its PID and CPU time against wall time. Never report green without the lie.
- The physics in `firm.h` and `world.h` is not under review. Do not tune a constant to pass an oracle.
- Every step ends in a dated receipt under `receipts/` with commands, outputs, exit codes, denominators, and what was not done.
- Author files with an editor or a Write tool; do not pipe content through shell literals.
