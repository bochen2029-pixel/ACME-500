#!/usr/bin/env python3
"""O28 · no dead physics.

Lists every free function defined in include/acme/firm.h and include/acme/world.h
that has no call site anywhere in the sources outside its own definition. The
list must be empty: a documented mechanism with no caller is a defect of the
same kind as the seven in the first-run receipt (F15 was one).

  python tools/dead_symbols.py <repo root>          the check
  python tools/dead_symbols.py <repo root> --lie    plant an uncalled function in a
                                                    scratch copy; PASS only if caught

Prints one battery-style line. Exit 0 on PASS, 1 on FAIL.
"""
import os, re, shutil, sys, tempfile

PHYSICS = ["include/acme/firm.h", "include/acme/world.h"]
SOURCES = ["include/acme/core.h", "include/acme/firm.h", "include/acme/world.h", "include/acme/human.h",
           "include/acme/solver.h", "include/acme/machine.h", "include/acme/report.h", "src/main.cpp"]
# a free function definition at column 0: `inline T name(` or `static T name(` (house style)
DEF = re.compile(r"^(?:inline|static)\s+[A-Za-z_][\w:<>,\s\*&]*?\b([A-Za-z_]\w*)\s*\(")


def read(root, rel):
    with open(os.path.join(root, rel), encoding="utf-8") as f:
        return f.read()


def definitions(root):
    out = []
    for rel in PHYSICS:
        for ln, line in enumerate(read(root, rel).splitlines(), 1):
            m = DEF.match(line)
            if m and m.group(1) not in ("main",):
                out.append((m.group(1), rel, ln))
    return out


def dead(root):
    srcs = {rel: read(root, rel).splitlines() for rel in SOURCES}
    result = []
    for name, rel, ln in definitions(root):
        pat = re.compile(r"\b" + re.escape(name) + r"\s*\(")
        calls = 0
        for srel, lines in srcs.items():
            for sln, line in enumerate(lines, 1):
                if srel == rel and sln == ln:
                    continue
                code = line.split("//", 1)[0]
                calls += len(pat.findall(code))
        if calls == 0:
            result.append((name, rel, ln))
    return result


KERNEL = ["include/acme/machine.h", "include/acme/solver.h", "include/acme/ledger.h", "include/acme/report.h"]
CLOCK = re.compile(r"\bchrono\b|\btime\s*\(|\bclock\s*\(|\bgetenv\s*\(|\bclock_gettime\b|\bgettimeofday\b")


def clock_reads(root):
    """O25a· the kernel reads time only from TICK rows: no clock, no environment, in any kernel file."""
    hits = []
    for rel in KERNEL:
        p = os.path.join(root, rel)
        if not os.path.exists(p):
            continue
        for ln, line in enumerate(open(p, encoding="utf-8").read().splitlines(), 1):
            code = line.split("//", 1)[0]
            if CLOCK.search(code):
                hits.append(f"{rel}:{ln}")
    return hits


def o25(root, lie):
    if lie:
        tmp = tempfile.mkdtemp(prefix="acme_o25_")
        for rel in KERNEL:
            src = os.path.join(root, rel)
            if not os.path.exists(src):
                continue
            dst = os.path.join(tmp, rel); os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copyfile(src, dst)
        with open(os.path.join(tmp, "include/acme/machine.h"), "a", encoding="utf-8") as f:
            f.write("\ninline long acme_planted_clock() { return (long)std::chrono::steady_clock::now().time_since_epoch().count(); }   // THE LIE\n")
        hits = clock_reads(tmp)
        shutil.rmtree(tmp, ignore_errors=True)
        ok = len(hits) == 1
        print(("  [PASS] " if ok else "  [FAIL] ") + f"O25a the kernel reads no clock: the planted clock read was {'caught' if hits else 'MISSED'}"
              + (f" ({len(hits)} hits)" if len(hits) != 1 else "") + "   (with oracle 25 lied to: PASS on its line means the lie was caught)")
        return 0 if ok else 1
    hits = clock_reads(root)
    if not hits:
        print(f"  [PASS] O25a the kernel reads no clock: machine.h, solver.h, ledger.h, report.h read time only from TICK rows")
        return 0
    print(f"  [FAIL] O25a the kernel reads no clock: {len(hits)} clock or environment reads: " + ", ".join(hits))
    return 1


def main():
    if len(sys.argv) < 2:
        print("usage: dead_symbols.py <repo root> [--lie] [--o25 [--lie]]"); return 2
    root = sys.argv[1]; lie = "--lie" in sys.argv
    if "--o25" in sys.argv:
        return o25(root, lie)
    if lie:
        tmp = tempfile.mkdtemp(prefix="acme_o28_")
        for rel in SOURCES:
            dst = os.path.join(tmp, rel); os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copyfile(os.path.join(root, rel), dst)
        with open(os.path.join(tmp, "include/acme/firm.h"), "a", encoding="utf-8") as f:
            f.write("\ninline int acme_planted_orphan(int x) { return x + 1; }   // THE LIE\n")
        found = dead(tmp)
        caught = any(n == "acme_planted_orphan" for n, _, _ in found)
        shutil.rmtree(tmp, ignore_errors=True)
        others = [n for n, _, _ in found if n != "acme_planted_orphan"]
        ok = caught and not others
        print(("  [PASS] " if ok else "  [FAIL] ") + "O28  no dead physics: the planted orphan was "
              + ("caught" if caught else "MISSED") + (", and nothing else is dead" if not others else ", but also dead: " + ", ".join(others))
              + "   (with oracle 28 lied to: PASS on its line means the lie was caught)")
        return 0 if ok else 1
    found = dead(root)
    n = len(definitions(root))
    if not found:
        print(f"  [PASS] O28  no dead physics: every free function in firm.h/world.h has a caller ({n} functions)")
        return 0
    print(f"  [FAIL] O28  no dead physics: {len(found)} of {n} functions have no caller: "
          + ", ".join(f"{nm} ({rel}:{ln})" for nm, rel, ln in found))
    return 1


if __name__ == "__main__":
    sys.exit(main())
