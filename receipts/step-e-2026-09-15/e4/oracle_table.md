| oracle | asserts | reading |
|---|---|---|
| O0a | BLAKE2b-256("abc") matches RFC 7693 | PASS |
| O0b | tape chain verifies over 500 rows | PASS |
| O0c | a single flipped field localises to its row | PASS, localised at row -1 |
| O1 | same seed produces a byte-identical tape | PASS, 168479 rows, memcmp |
| O1b | same seed produces a byte-identical MACHINE arm: tape, chain, licence table | PASS, 413070 rows; rec same, chain same, licence table same |
| O14 | the ledger folds from the tape: cold fold == the live ledger every consumer reads | PASS, 58367 cells folded from 413070 rows; 0 field diffs; 0 minute diffs; alphabet pin matches |
| O29 | every row carries what the v2 table says it carries | PASS, 413070 rows scanned, 0 shape faults |
| O15 | the ladder is a fold of OUTCOME rows: counts rebuilt == live | PASS, 72 class-bands; 0 count diffs against the live ladder |
| O16 | a null judge and a rules judge license nothing, from history or the wager | PASS, null judge: top rung 0, 0 LICENSE rows, 0 wager outcomes; rules judge: top rung 0, 0 rows, 0 outcomes |
| O21 | the canary draw is independent of the machine's band | PASS, chi-square 70.3 on 72 class-band cells against a 99.9% point of 114.9 |
| O18 | a kill survives: the durable tape reopens, the checkpoint restores, the run continues bit for bit | PASS, cut at row 363200 + 17 bytes with checkpoint 59 in flight; restored at day 49, 60646 rows dropped; continued run: 413071 rows, chain verified, 0 fold diffs, rows equal the unkilled run's |
| O30 | every EFFECT re-derives from rows: proposal, stratum, folded rung, schema, switch, the gate's own inputs | PASS, 47628 effects re-derived through the gate: 47628 agree, 0 mismatched, 0 band mismatches, 0 without a proposal |
| O19 | off is invisible: no effect, and the incumbent's rows equal a run with no machine | PASS, 0 effects under off; 288051 incumbent rows against 288051 with no machine, byte-identical |
| O25 | the same rows at different wall spacing give identical verdicts: the machine reads the day, never the clock | PASS, 413070 rows against 413070; 70 TICK rows carry a different wall value; every other byte identical |
| O37 | the shared fact: a proposal is carried across a change only on the judge's certificate, re-derivable from rows, and read again where a margin crosses | PASS, 33 changes of a shared fact; 595 frames moved by it: 267 carried by the certificate, all 267 re-derived from rows, 267 checked against the judge with 0 disagreements; 328 read again, of which 117 changed choice and 291 changed band |
| O38 | outstanding exposure never exceeds the writ's cap, an unanswered cell is closed only by a write-off row, and a write-off is evidence for nothing | PASS, cap $300k; the fold's outstanding peaked at $300k, 0 rows over the cap; 14754 acts held for exposure; 489 write-offs, each a row; the ladder counted 35 wager outcomes against 35 answered; the live ledger's outstanding equals the fold's |
| O20 | the retained stratum never reaches zero: drawn at every rung, decided by a person, never acted on while retained | PASS, 1496 cells drawn into the control arm at eps_floor 0.03; 1304 decided by a person; 0 acted on by the machine while retained; 4809 incumbent outcomes landed in the live period |
| O31 | the ladder v2 leaves a judge inside the null with unearned authority no more of the time than its level (at the exact boundary in a Bernoulli world 183 of 20000 band-terms crossed the first earned rung's threshold, rate 0.0092 against alpha/4 = 0.0125, bound 0.0140; the plant: 3 seeded firms, 86 bands run, the judge on each term's frozen g0 less three points: over the 8688 settled wager cells the process graded, expected good rate 0.3029 from its coins, observed 0.3061, the g0 of their acts' terms 0.4306, so INSIDE the null; 0 ever promoted; 0 of 432 band-terms standing above the test rung, share 0.0000 against alpha 0.05, bound 0.0706; against each band's lowest g0 across terms (mean spread 0.260): 43 bands below it with 0 promoted, 34 above it with 0 promoted) | PASS |
| O31b | the standing share holds under a shared fact that correlates outcomes within its epochs (the shared fact moved; 3 seeded firms, 86 bands run, the judge on each term's frozen g0 less three points: over the 8514 settled wager cells the process graded, expected good rate 0.3033 from its coins, observed 0.3077, the g0 of their acts' terms 0.4193, so INSIDE the null; 0 ever promoted; 0 of 432 band-terms standing above the test rung, share 0.0000 against alpha 0.05, bound 0.0706; against each band's lowest g0 across terms (mean spread 0.263): 43 bands below it with 0 promoted, 35 above it with 0 promoted) | PASS |
| O45 | the ladder v2 is a fold: the bars, both processes and every LICENSE row re-derive from rows | PASS, 72 class-bands; 0 diffs against the live ladder; 64 LICENSE rows folded, 0 unjustified |
| O59 | the regime detector fires on a planted shift of arrivals and stays quiet without one | PASS, a 1.6x shift from day 80: 32 REGIME rows, 20 bands dropped to watching; the world without a shift: 0 REGIME rows |
| O47 | the calibration curve per key is monotone, frozen per term as rows, and an unmeasured or non-monotone key licenses nothing | PASS, 384 CALIB rows over 2 terms; 13 of 24 classes measured, 13 of those monotone, 0 ever flagged non-monotone; 4694 unattended acts, 0 on a key unmeasured or not monotone |
| O48 | the account folds from the tape: every panel number from a cold fold equals the live panel's (the live panel against a cold fold: minutes 2852.4k vs 2852.4k (meetings 459.9k vs 459.9k), correctly discharged 13987 vs 13987, CPI 203.9 vs 203.9, waits 8.6/13.6 vs 8.6/13.6 days; 3427 rentals on 2068 distinct cells) | PASS |
| O2 | the gate never authorises outside its licence | PASS, 8640 lattice points, 390 reached ACT; pressure never widened; off held every point; an exhausted exposure allowance held every act and moved nothing else; retained held every point |
| O3 | out of supervision degrades to HOLD, never to acting | PASS |
| O4 | transport conserves: rows ship, columns never exceed | PASS, max column overshoot 0.00e+00, placed+stock err 1.30e-08 |
| O5 | a binding capacity has a nonzero price, stocks included | PASS, binding seat price 29.6598, unplaced-stock price 28.0206 |
| O6 | the join graph is read off the application, not asked for | PASS, 24 of 24 classes, exactly |
| O7 | coverage read off the world's verdicts tracks the tacit mass | PASS, mean |err| 0.054, corr 0.946 over 19 classes |
| O8 | the disagree/firm-failed cell licenses nothing | PASS, 2174 cells available, 21611 credited, 21611 agreements |
| O9 | the field recovers competence no human in ACME can see | PASS, mean within-class corr 0.242 over 9 classes, 209 seat-class pairs |
| O10 | kappa demotes a class that costs more than it saves | PASS, kappa 2.25 -> rung 1 |
| O11 | effects carry inverses; the window closes at settlement | PASS, 60 effects committed and unwound; post-settlement reversal refused |
| O12 | the origin-fit cascade promises an impossible company | PASS, alpha 0.145 vs 0.197 planted; F 17.6 vs 9.0; gap at E=200 is 9 seats |
| O13 | the measured decide fraction ranks the planted intensity | PASS, partial Spearman 0.963 over 24 classes, n_systems controlled; the LEVEL is never recovered |
| O28 | no dead physics: every free function in firm.h/world.h has a caller | PASS, 21 functions |
| O25a | the kernel reads no clock: machine.h, solver.h, ledger.h, report.h, port.h, license.h, tapefile.h, gate.h read time only from TICK rows | PASS |
| O17 | the machine links without the plant or the governor: ledger.h, port.h, license.h, solver.h, machine.h, report.h compile under -DACME_NO_PLANT -DACME_NO_GOVERNOR | PASS |
| O30a | the gate is pinned: include/acme/gate.h digests to its pin b596fe9f9af4aa25 | PASS |
