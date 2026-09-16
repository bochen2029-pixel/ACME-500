cmake_minimum_required(VERSION 3.16)
project(acme500 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(acme src/main.cpp)
target_include_directories(acme PRIVATE include)

if(MSVC)
  target_compile_options(acme PRIVATE /W4 /O2)
else()
  target_compile_options(acme PRIVATE -Wall -Wextra -O2)
  # Determinism is a property this program tests for, so no fast-math and no
  # reassociation. The fixed-point accumulators in core.h assume it.
  target_compile_options(acme PRIVATE -ffp-contract=off)
endif()

enable_testing()
add_test(NAME oracles COMMAND acme --selftest)
# The lie arms. Under --lie N the mechanism oracle N claims to test is corrupted
# on purpose, and oracle N prints PASS only if it CAUGHT the corruption. So a
# healthy lie run exits 0 with the lied-to oracle's own line reading PASS and
# no oracle reading FAIL. That is asserted per oracle, on its own line: a
# nonzero exit for an unrelated reason must never satisfy a lie arm, and a
# lied-to oracle that prints FAIL is a broken oracle, not a caught lie.
set(LIE_LINE_0 "O0c")
set(LIE_LINE_1 "O1")  set(LIE_LINE_2 "O2")   set(LIE_LINE_3 "O3")   set(LIE_LINE_4 "O4")
set(LIE_LINE_5 "O5")  set(LIE_LINE_6 "O6")   set(LIE_LINE_7 "O7")   set(LIE_LINE_8 "O8")
set(LIE_LINE_9 "O9")  set(LIE_LINE_10 "O10") set(LIE_LINE_11 "O11") set(LIE_LINE_12 "O12")
set(LIE_LINE_13 "O13")
set(LIE_LINE_14 "O1b")   # lie 14 = O1b, determinism over the machine arm (REV 2 spec S0)
set(LIE_LINE_15 "O14")   # lie 15 = O14, fold-to-identity (a field set without a row)
set(LIE_LINE_16 "O15")   # lie 16 = O15, the ladder is a fold (an outcome counted twice)
set(LIE_LINE_17 "O29")   # lie 17 = O29, row-shape conformance (an ARRIVE with no due day)
set(LIE_LINE_18 "O16")   # lie 18 = O16, a null judge licenses nothing (a truth-reading judge under the null's name)
set(LIE_LINE_19 "O21")   # lie 19 = O21, the canary draw is independent of the band (a band-dependent draw)
set(LIE_LINE_20 "O18")   # lie 20 = O18, a kill survives (a restore that keeps the period in flight)
set(LIE_LINE_21 "O30")   # lie 21 = O30, every EFFECT re-derives from rows (an effect whose proposal was never written)
set(LIE_LINE_22 "O19")   # lie 22 = O19, off is invisible (a resident that lets one cell through under off)
set(LIE_LINE_23 "O25")   # lie 23 = O25, the same rows at different wall spacing (a resident that reads the clock)
set(LIE_LINE_24 "O37")   # lie 24 = O37, the shared fact (the old proposal carried as if the fact had not moved)
set(LIE_LINE_25 "O38")   # lie 25 = O38, outstanding exposure (one act in a hundred past the cap)
set(LIE_LINE_26 "O20")   # lie 26 = O20, the retained stratum never reaches zero (the wide band's control arm switched off)
set(LIE_LINE_27 "O31")   # lie 27 = O31, the false-promotion rate under a boundary judge (a judge above the boundary under its name)
set(LIE_LINE_28 "O31b")  # lie 28 = O31b, the same under the shared fact (the flip schedule removed under the same name)
set(LIE_LINE_29 "O45")   # lie 29 = O45, the ladder v2 is a fold (a fold that skips one band's term boundary)
set(LIE_LINE_30 "O59")   # lie 30 = O59, the regime detector (a detector that never hears)
set(LIE_LINE_31 "O47")   # lie 31 = O47, the calibration harness (a confidently-wrong judge admitted on a class)
set(LIE_LINE_32 "O48")   # lie 32 = O48, the account folds from the tape (a fold that drops the meetings)
set(LIE_LINE_33 "O42")   # lie 33 = O42, the tape of tapes (a row of a sealed period folded into the period after it)
foreach(N RANGE 0 33)
  add_test(NAME lie_${N} COMMAND acme --selftest --lie ${N})
  set_tests_properties(lie_${N} PROPERTIES
    PASS_REGULAR_EXPRESSION "\\[PASS\\] ${LIE_LINE_${N}} "
    FAIL_REGULAR_EXPRESSION "\\[FAIL\\]")
endforeach()

# O28 · no dead physics. A function defined in firm.h or world.h with no caller
# anywhere is a documented mechanism that does not run (F15). The tool lists
# them; the list must be empty. Its lie plants an uncalled function in a scratch
# copy and the tool must be seen to catch it.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  add_test(NAME dead_symbols     COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/dead_symbols.py ${CMAKE_SOURCE_DIR})
  add_test(NAME dead_symbols_lie COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/dead_symbols.py ${CMAKE_SOURCE_DIR} --lie)
  set_tests_properties(dead_symbols     PROPERTIES PASS_REGULAR_EXPRESSION "\\[PASS\\] O28 " FAIL_REGULAR_EXPRESSION "\\[FAIL\\]")
  set_tests_properties(dead_symbols_lie PROPERTIES PASS_REGULAR_EXPRESSION "\\[PASS\\] O28 " FAIL_REGULAR_EXPRESSION "\\[FAIL\\]")
  # O25 · the kernel reads no clock: the machine's files read time only from TICK rows.
  add_test(NAME no_clock     COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/dead_symbols.py ${CMAKE_SOURCE_DIR} --o25)
  add_test(NAME no_clock_lie COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/dead_symbols.py ${CMAKE_SOURCE_DIR} --o25 --lie)
  set_tests_properties(no_clock     PROPERTIES PASS_REGULAR_EXPRESSION "\\[PASS\\] O25a" FAIL_REGULAR_EXPRESSION "\\[FAIL\\]")
  set_tests_properties(no_clock_lie PROPERTIES PASS_REGULAR_EXPRESSION "\\[PASS\\] O25a" FAIL_REGULAR_EXPRESSION "\\[FAIL\\]")
  # O17 · the machine links without the plant or the governor. A build fact: the
  # kernel's headers compile under -DACME_NO_PLANT -DACME_NO_GOVERNOR, and one
  # include of world.h or of governor.h is refused (the lies).
  add_test(NAME links_without_plant COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/o17.py ${CMAKE_SOURCE_DIR} ${CMAKE_CXX_COMPILER})
  set_tests_properties(links_without_plant PROPERTIES PASS_REGULAR_EXPRESSION "\\[PASS\\] O17 " FAIL_REGULAR_EXPRESSION "\\[FAIL\\]")
endif()
