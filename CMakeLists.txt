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
foreach(N RANGE 0 14)
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
endif()
