#!/usr/bin/env python3
"""O17 · the machine links without the plant.

  python tools/o17.py <repo root> [<c++ compiler>]

Compiles tools/o17_machine_tu.cpp with -DACME_NO_PLANT (must succeed) and
tools/o17_lie_tu.cpp, which adds one include of the plant (must FAIL on the
guard in world.h). Prints one battery-style line per arm. Exit 0 on PASS.
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
    guard_fired = (not ok_lie) and ("world.h is the PLANT" in err_l)
    if ok_machine:
        print("  [PASS] O17  the machine links without the plant: ledger.h, port.h, solver.h, machine.h, report.h compile under -DACME_NO_PLANT")
    else:
        print("  [FAIL] O17  the machine links without the plant: the kernel TU did not compile: " + err_m.strip().splitlines()[0] if err_m.strip() else "  [FAIL] O17  the kernel TU did not compile")
    if guard_fired:
        print("  [PASS] O17  the machine links without the plant: the lie TU (one include of world.h) was refused by the guard   (with oracle 17 lied to: PASS on its line means the lie was caught)")
    else:
        print("  [FAIL] O17  the machine links without the plant: the lie TU compiled or failed for another reason: " + (err_l.strip().splitlines()[0] if err_l.strip() else "no error"))
    return 0 if (ok_machine and guard_fired) else 1


if __name__ == "__main__":
    sys.exit(main())
