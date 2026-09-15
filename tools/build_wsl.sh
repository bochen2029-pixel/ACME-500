#!/bin/bash
# Sync the tree into the WSL build copy and build with the pinned flags; print
# the first errors. Run from Windows as
#   wsl -d Ubuntu-24.04 -u root --exec bash -c 'bash /mnt/c/55555/acme500/tools/build_wsl.sh'
# Then, for a quick check:  wsl ... -c 'cd /root/acme_v2 && ./acme --selftest'
set -u
SRC=/mnt/c/55555/acme500
W=/root/acme_v2
mkdir -p "$W"
rsync -a --delete "$SRC/include/" "$W/include/"
rsync -a --delete "$SRC/src/" "$W/src/"
rsync -a --delete "$SRC/tools/" "$W/tools/"
cp "$SRC/CMakeLists.txt" "$W/CMakeLists.txt"
cd "$W"
g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme > build.txt 2>&1
echo "build exit $?  errors: $(grep -c 'error' build.txt)  warnings: $(grep -c 'warning' build.txt)"
grep -n "error\|warning" build.txt | head -40
