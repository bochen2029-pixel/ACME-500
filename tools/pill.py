#!/usr/bin/env python3
"""The pill · read the machine's heartbeat and say whether it is alive.

  python tools/pill.py <intellect.heartbeat.json> [--period SECONDS] [--beats N]

The world port writes the heartbeat by atomic rename at the close of every
period: the day, the rows, the chain head, the switch, the judge, the wall
clock. This reads it and prints ALIVE if its age is under N periods (default 3)
of the stated period (default 60 s), STALLED otherwise. Exit 0 alive, 1 stalled,
2 unreadable. The machine cannot write this file; only the port that runs it can.
"""
import json, os, sys, time


def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    path = sys.argv[1]
    period = 60.0; beats = 3
    if "--period" in sys.argv: period = float(sys.argv[sys.argv.index("--period") + 1])
    if "--beats" in sys.argv: beats = int(sys.argv[sys.argv.index("--beats") + 1])
    try:
        hb = json.load(open(path, encoding="utf-8"))
    except Exception as e:
        print("UNREADABLE %s: %s" % (path, e)); return 2
    age = time.time() - float(hb.get("wall", 0))
    state = "ALIVE" if age <= beats * period else "STALLED"
    print("%s  day %s  rows %s  switch %s  judge 0x%08x  head %s...  age %.0f s (limit %.0f s)"
          % (state, hb.get("day"), hb.get("rows"), hb.get("switch"), int(hb.get("judge", 0)), str(hb.get("chain_head", ""))[:16], age, beats * period))
    return 0 if state == "ALIVE" else 1


if __name__ == "__main__":
    sys.exit(main())
