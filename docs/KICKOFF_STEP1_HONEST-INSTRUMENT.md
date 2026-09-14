# KICKOFF · ACME-500 step 1 · make the instrument honest

*2026-09-13 · Claude Opus 5 · the brief for the session that implements step 1 of `NEXT_2026-09-13_OPUS5.md`. You start blind. Read this whole, then `C:\55555\acme500\receipts\RECEIPT_ACME500_FIRST-RUN_2026-09-13.md`, then `C:\55555\acme500\README.md`, then the source in the order `core.h`, `firm.h`, `world.h`, `human.h`, `solver.h`, `machine.h`, `report.h`, `main.cpp`. Nothing else until a specific question sends you there.*

## Where things are

| thing | path |
|---|---|
| the source, 3,180 lines, C++17, no dependencies | `C:\55555\acme500\` (`include\acme\*.h`, `src\main.cpp`, `CMakeLists.txt`, `build.sh`) |
| the first-run receipt with the seven defects, each with its line | `C:\55555\acme500\receipts\RECEIPT_ACME500_FIRST-RUN_2026-09-13.md` |
| raw outputs of every mode and every lie arm from that run | `C:\55555\acme500\receipts\first-run-2026-09-13\` |
| the build that ran, under WSL | `/root/acme500/` in `Ubuntu-24.04` (a copy; the tree above is the source of record) |
| the observer, frame v1, its golden and its README | `C:\55555\ACME500-OBSERVER-REV1\` |
| the observer's contract brief, including the `--dump` spec | `C:\55555\BRIEF_ACME500-WORLD_PURE-OBSERVER_2026-09-13.md` |
| the standing order this step comes from | `C:\55555\NEXT_2026-09-13_OPUS5.md` |
| the registered kills for step 2 | `C:\55555\QUESTION-BACKLOG_ABDUCTION-ROWS_2026-09-12.md` |

## Build and run

There is no g++ on Windows here. Build under WSL, forward slashes everywhere, no heredocs, content through the Write and Edit tools only:

```
wsl -d Ubuntu-24.04 -u root --exec bash -c 'rm -rf /root/acme500 && cp -r /mnt/c/55555/acme500 /root/acme500 && cd /root/acme500 && g++ -O2 -std=c++17 -Iinclude -Wall -Wextra -ffp-contract=off src/main.cpp -o acme && ./acme --selftest'
```

Timings from the first run on this box, single core: build 5 s; `--selftest` 5 s; each lie arm 5 s; `--sim` 4 s; `--automate` and `--twin` 37 s each; `--multiverse` 44 minutes. Run the multiverse only as a background tool call, and verify it by its PID and CPU time, never by `pgrep -f` on a pattern your own shell contains.

Edit the Windows tree, copy to WSL, build, run. Never edit the WSL copy.

## The baseline you must reproduce before touching anything

`./acme --selftest` prints 15 passed, 1 failed, O7 red at mean abs err 0.071 and corr −0.052 over 19 classes. The fourteen lie arms exit 1 for lies 0 to 6 and 8 to 13, and 0 for lie 7; under lie 6 the O6 line prints FAIL. Reproduce that table first. If it differs, stop and report.

## The seven fixes, in order, each with its acceptance

**1. The lie-arm convention** (`main.cpp`, every `ck(LIE == N ? !ok : ok, …)` and the final print; `CMakeLists.txt` lines 24 to 28). Decide it once: under `--lie N`, oracle N prints PASS when it caught its lie, so a healthy lie run exits 0. Change the final print to say that. In CMake, drop `WILL_FAIL` and instead assert, per lie test, that the run exits 0 and that the lied-to oracle's line reads PASS, with a `PASS_REGULAR_EXPRESSION` on that oracle's id. Acceptance: with O7 still red, every lie run still exits 1 and you can see it is O7 and only O7; after fix 7, lie runs 0 to 6 and 8 to 13 exit 0.

**2. O6's lie** (`main.cpp:560` to `563`). The lie sets `C.join_graph[3] = 1u`; the oracle compares `C.n_systems[c]`, computed inside `compile_from_tape` before the lie. Recompute `n_systems` from `join_graph` inside the oracle, or corrupt `n_systems[3]` directly. Acceptance: under `--lie 6` the O6 line prints PASS (lie caught), and in the baseline it still prints PASS with 24 of 24.

**3. The incumbent's band** (`human.h` `human_work`, where `o.margin` is set at line 192; `machine.h:551` reads `o.band`). Set `o.band = band_of(o.margin, f.writ)` for human decisions at decide time. `band_of` lives in `machine.h`; move it to `solver.h` or `firm.h` so `human.h` can call it without a circular include. Acceptance: in `--automate` the ladder table's `good_h` column is nonzero for band 2, and `p_incumbent(c, 2)` is a measured rate, not 0.72.

**4. The backlog in the twin's score** (`report.h:130` `score_arm`). Add the open obligations at `w.writ.w_unplaced × value`, and print the backlog cost as its own line in `cmd_twin` beside the writ cost. Acceptance: the twin prints both arms' backlog counts and costs; the sign of the cost delta is whatever it is, printed with its denominator.

**5. Drafts and the wager** (`machine.h` `V_DRAFT` case, lines 500 to 512; `Resident::grade`). Decide what a draft is and make the code say it. The cheaper honest choice: keep `by_machine = 1` for drafts but count them separately in the ladder (`n_draft` beside `n_machine`), let only `V_ACT` rows feed `n_machine` and the e-process, and print `canary`, `draft` and `act` counts against n₀ on the counter. Acceptance: the counter on the wall reads machine-chosen outcomes from acts only; the twin's within-firm estimate is recomputed from the same set and the estimator bias is printed again.

**6. Lateness out of novelty** (`machine.h:447`). The conformal score is `|completeness − 0.7| + 0.3·|lateness|`. Drop the lateness term or give it its own reason code (`RS_LATE`) at the gate, so a late case is held for lateness and a strange case for novelty. Acceptance: the refusal-reason histogram in `--automate` shows `novel-case` falling by an order of magnitude and a new reason carrying the late cases.

**7. `eps_floor`** (`firm.h:92`, `main.cpp:370`). Read it as a uniform component of the canary draw, `rate = max(rate, eps_floor)`, or delete it and its multiverse patch. Acceptance: the multiverse's "explore more" row no longer reproduces the baseline to the digit.

Then **O7**: the coverage estimator. The confident-stratum estimator is in `machine.h:262` to `292`; the receipt's own diagnosis is a selection effect. Reweight the stratum by its selection probability, or revert to the full-sample estimator and raise the oracle's tolerance honestly and print why. Acceptance: O7 green with its lie arm (`--lie 7`) seen to fail.

## The receipt this step ends in

`C:\55555\acme500\receipts\RECEIPT_ACME500_HONEST-INSTRUMENT_<date>.md`: the battery 16 of 16; fourteen lie arms with the lied oracle caught in each; `--twin` with the backlog priced on both arms and the estimator bias recomputed; `--automate` with the new ladder table and the residual; the cascade number restated or retracted; every number with its command and denominator; a numbered list of what was not done.

## What you must not do

Do not tune a number to make an oracle pass. Do not touch `decision_quality`, `p_good`, `realize_outcome` or `truth_decision` in `firm.h` and `world.h`; the physics is not under review. Do not edit `README.md` except to append a dated section. Do not quote 500 → 57 or −4.41 anywhere until the honest twin has printed its replacement.

## After this step

Step 2 of NEXT is the three cheap kills in the backlog file; step 3 is `--dump` per the observer brief, replacing `golden/gen.js`. Neither starts until the receipt above exists.
