#!/usr/bin/env python3
"""O17 · the machine links without the plant and without the governor.

  python tools/o17.py <repo root> [<c++ compiler>]

Compiles tools/o17_machine_tu.cpp with -DACME_NO_PLANT -DACME_NO_GOVERNOR (must
succeed), tools/o17_lie_tu.cpp, which adds one include of the plant (must FAIL
on the guard in world.h), and tools/o17_lie2_tu.cpp, which adds one include of
the governor (must FAIL on the guard in governor.h). Prints one battery-style
line per arm. Exit 0 on PASS.
"""
import os, subprocess, sys


def compile_ok(cxx, root, tu):
    r = subprocess.run([cxx, "-std=c++17", "-fsyntax-only", "-Wall", "-Wextra", "-I", os.path.join(root, "include"),
                        os.path.join(root, "tools", tu)], capture_output=True, text=True)
    return r.returncode == 0, (r.stderr or "")[:400]


def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    root = sys.argv[1]; cxx = sys.argv[2] if len(sys.argv) > 2 else "g++"
    ok_machine, err_m = compile_ok(cxx, root, "o17_machine_tu.cpp")
    ok_lie, err_l = compile_ok(cxx, root, "o17_lie_tu.cpp")
    ok_lie2, err_l2 = compile_ok(cxx, root, "o17_lie2_tu.cpp")
    guard_fired = (not ok_lie) and ("world.h is the PLANT" in err_l)
    guard2_fired = (not ok_lie2) and ("governor.h is the GOVERNOR" in err_l2)
    if ok_machine:
        print("  [PASS] O17  the machine links without the plant or the governor: ledger.h, port.h, license.h, solver.h, machine.h, report.h compile under -DACME_NO_PLANT -DACME_NO_GOVERNOR")
    else:
        first = err_m.strip().splitlines()[0] if err_m.strip() else "no error text"
        print("  [FAIL] O17  the machine links without the plant or the governor: the kernel TU did not compile: " + first)
    if guard_fired and guard2_fired:
        print("  [PASS] O17  the machine links without the plant or the governor: the lie TUs (one include of world.h; one of governor.h) were refused by the guards   (with oracle 17 lied to: PASS on its line means the lie was caught)")
    else:
        which = [] if guard_fired else ["world.h"]
        if not guard2_fired: which.append("governor.h")
        print("  [FAIL] O17  the machine links without the plant or the governor: a lie TU compiled or failed for another reason: " + ", ".join(which))
    return 0 if (ok_machine and guard_fired and guard2_fired) else 1


if __name__ == "__main__":
    sys.exit(main())
