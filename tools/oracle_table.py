#!/usr/bin/env python3
"""Regenerate the README's oracle table from a selftest run, never by hand.

  python tools/oracle_table.py receipts/<step>/selftest.txt [receipts/<step>/o28_o25.txt]

Prints a Markdown table: one row per oracle in battery order, with the reading the
run printed. Tool oracles (O28, O25a) are appended from the second file if given.
"""
import re, sys

LINE = re.compile(r"^\s*\[(PASS|FAIL)\]\s+(O\S+)\s+(.*?)(\s+\(([^()]*)\))?\s*$")


def rows(path):
    out = []
    seen = set()
    for line in open(path, encoding="utf-8", errors="replace"):
        m = LINE.match(line)
        if not m:
            continue
        status, oid, desc, _, detail = m.groups()
        if "lied to" in (detail or "") or oid in seen:
            continue
        seen.add(oid)
        out.append((oid, desc.strip(), status, (detail or "").strip()))
    return out


def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    rs = rows(sys.argv[1])
    if len(sys.argv) > 2:
        rs += [r for r in rows(sys.argv[2]) if r[0] not in {x[0] for x in rs}]
    print("| oracle | asserts | reading |")
    print("|---|---|---|")
    for oid, desc, status, detail in rs:
        reading = status + (", " + detail if detail else "")
        print(f"| {oid} | {desc} | {reading} |")
    return 0


if __name__ == "__main__":
    sys.exit(main())
