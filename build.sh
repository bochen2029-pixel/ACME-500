#!/bin/sh
# ACME-500 — one file, one compiler, no dependencies.
set -e
CXX="${CXX:-g++}"
$CXX -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme
echo "built ./acme"
echo
echo "  ./acme --sim         run ACME as it is"
echo "  ./acme --twin        both arms, one seed, one physics"
echo "  ./acme --automate    the whole programme"
echo "  ./acme --multiverse  policy search over structure"
echo "  ./acme --selftest    the oracles (--lie N to corrupt one; it must fail)"
