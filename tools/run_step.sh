#!/bin/bash
# The whole acceptance run for one step or sub-step, under WSL, staged into the
# repo's receipts. This is the script every receipt since C1 was produced by
# (generalised in D2). Run it from Windows as
#
#   wsl -d Ubuntu-24.04 -u root --exec bash -c 'bash /mnt/c/55555/acme500/tools/run_step.sh <STAGE> [<PREV>]'
#
#   STAGE  the stage folder under receipts/, e.g. step-e-2026-09-16/e0
#   PREV   the previous stage folder to diff the three modes against, e.g.
#          receipts/step-d-2026-09-14/d2 (optional; default: none)
#
# What it does: syncs include/src/tools/CMakeLists into the WSL build copy,
# builds with the pinned flags, runs the battery, every lie arm the CMake
# lists, the tool oracles (O17, O28, O25a, O30a), the three modes, the diffs
# against PREV, stages every output, prints the tree's line count and hashes.
# It takes ~45 minutes at 26 oracles and 24 lie arms; run it in the
# background and poll the output file. It never edits the tree.
set -u
STAGE=${1:?stage folder under receipts/, e.g. step-e-2026-09-16/e0}
PREV=${2:-}
SRC=/mnt/c/55555/acme500
W=/root/acme_v2
OUT=/root/acme_v2/out_$(echo "$STAGE" | tr '/' '_')
DEST=$SRC/receipts/$STAGE
rm -rf "$OUT" "$DEST"; mkdir -p "$OUT" "$DEST" "$W"
rsync -a --delete "$SRC/include/" "$W/include/"
rsync -a --delete "$SRC/src/" "$W/src/"
rsync -a --delete "$SRC/tools/" "$W/tools/"
cp "$SRC/CMakeLists.txt" "$W/CMakeLists.txt"
cd "$W"
COMMIT=$(cd "$SRC" && git rev-parse --short HEAD 2>/dev/null || echo unknown)
g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off -DACME_COMMIT="\"$COMMIT\"" src/main.cpp -o acme > "$OUT/build.txt" 2>&1
echo "build exit $?  warnings: $(grep -c warning "$OUT/build.txt")  commit $COMMIT"
/usr/bin/time -f "selftest wall %e s" ./acme --selftest > "$OUT/selftest.txt" 2> "$OUT/selftest.time"
echo "selftest exit $?  $(tail -1 "$OUT/selftest.txt")  $(cat "$OUT/selftest.time")"
grep "FAIL" "$OUT/selftest.txt"
MAXLIE=$(sed -n 's/.*foreach(N RANGE 0 \([0-9][0-9]*\)).*/\1/p' CMakeLists.txt | head -1)
if [ -z "$MAXLIE" ]; then echo "could not read the lie range from CMakeLists.txt"; exit 2; fi
# E3: the lie arms run LIE_JOBS at a time (default 6 of the box's 16 threads); each
# arm is its own process with its own temp files (O18 keys its directory on the
# lie), so the arms are independent, and each writes its exit code beside its
# output so the summary below reads it back in order.
LIE_JOBS=${LIE_JOBS:-6}
echo "lie arms 0..$MAXLIE, $LIE_JOBS at a time"
seq 0 "$MAXLIE" | xargs -P "$LIE_JOBS" -I{} sh -c './acme --selftest --lie {} > "$0/lie-{}.txt" 2>&1; echo $? > "$0/lie-{}.exit"' "$OUT"
for n in $(seq 0 "$MAXLIE"); do
  L=$(grep -o "set(LIE_LINE_$n \"[A-Za-z0-9]*\")" CMakeLists.txt | sed "s/.*\"\(.*\)\".*/\1/")
  echo "lie $n exit $(cat "$OUT/lie-$n.exit")  lied-to $L PASS lines: $(grep -c "\[PASS\] $L " "$OUT/lie-$n.txt")  FAIL lines: $(grep -c '\[FAIL\]' "$OUT/lie-$n.txt")"
done
rm -f "$OUT"/lie-*.exit
python3 tools/o17.py . g++ > "$OUT/o17.txt" 2>&1; echo "o17 exit $?"
{ python3 tools/dead_symbols.py . ; python3 tools/dead_symbols.py . --lie ; python3 tools/dead_symbols.py . --o25 ; python3 tools/dead_symbols.py . --o25 --lie ; } > "$OUT/o28_o25a.txt" 2>&1; echo "o28/o25a exit $?"
{ python3 tools/gate_hash.py . ; python3 tools/gate_hash.py . --lie ; } > "$OUT/o30a.txt" 2>&1; echo "o30a exit $?"
cat "$OUT/o17.txt" "$OUT/o28_o25a.txt" "$OUT/o30a.txt"
./acme --sim > "$OUT/out_sim.txt" 2>&1; echo "sim exit $?"
./acme --automate > "$OUT/out_automate.txt" 2>&1; echo "automate exit $?"
./acme --twin > "$OUT/out_twin.txt" 2>&1; echo "twin exit $?"
if [ -n "$PREV" ]; then
  echo "== diffs against $PREV (lines removed/changed <, count of added >)"
  for f in out_sim out_automate out_twin; do
    echo "--- $f"; diff "$SRC/$PREV/$f.txt" "$OUT/$f.txt" | grep "^<" | cut -c1-150
    echo "    added: $(diff "$SRC/$PREV/$f.txt" "$OUT/$f.txt" | grep -c "^>")"
  done
fi
rm -f "$OUT/build.txt" "$OUT/selftest.time"
python3 tools/oracle_table.py "$OUT/selftest.txt" "$OUT/o28_o25a.txt" "$OUT/o17.txt" "$OUT/o30a.txt" > "$OUT/oracle_table.md" 2>/dev/null
cp "$OUT"/* "$DEST"/
echo "staged $(ls "$DEST" | wc -l) files under receipts/$STAGE"
echo "== tree lines"; cat include/acme/*.h src/main.cpp | wc -l
echo "== hashes"
for f in include/acme/*.h src/main.cpp CMakeLists.txt; do printf "%s %s\n" "$(sha256sum "$f" | cut -c1-16)" "$f"; done
