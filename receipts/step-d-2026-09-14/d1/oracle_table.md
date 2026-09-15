| oracle | asserts | reading |
|---|---|---|
| O0a | BLAKE2b-256("abc") matches RFC 7693 | PASS |
| O0b | tape chain verifies over 500 rows | PASS |
| O0c | a single flipped field localises to its row | PASS, localised at row -1 |
| O1 | same seed produces a byte-identical tape | PASS, 168479 rows, memcmp |
| O1b | same seed produces a byte-identical MACHINE arm: tape, chain, licence table | PASS, 402660 rows; rec same, chain same, licence table same |
| O14 | the ledger folds from the tape: cold fold == the live ledger every consumer reads | PASS, 58367 cells folded from 402660 rows; 0 field diffs; 0 minute diffs; alphabet pin matches |
| O29 | every row carries what the v2 table says it carries | PASS, 402660 rows scanned, 0 shape faults |
| O15 | the ladder is a fold of OUTCOME rows: counts rebuilt == live | PASS, 72 class-bands; 0 count diffs against the live ladder |
| O16 | a null judge and a rules judge license nothing, from history or the wager | PASS, null judge: top rung 0, 0 LICENSE rows, 0 wager outcomes; rules judge: top rung 0, 0 rows, 0 outcomes |
| O21 | the canary draw is independent of the machine's band | PASS, chi-square 72.2 on 72 class-band cells against a 99.9% point of 114.9 |
| O18 | a kill survives: the durable tape reopens, the checkpoint restores, the run continues bit for bit | PASS, cut at row 352711 + 17 bytes with checkpoint 59 in flight; restored at day 49, 60069 rows dropped; continued run: 402661 rows, chain verified, 0 fold diffs, rows equal the unkilled run's |
| O2 | the gate never authorises outside its licence | PASS, 8640 lattice points, 390 reached ACT |
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
| O28 | no dead physics: every free function in firm.h/world.h has a caller | PASS, 18 functions |
| O25a | the kernel reads no clock: machine.h, solver.h, ledger.h, report.h, port.h, license.h, tapefile.h read time only from TICK rows | PASS |
| O17 | the machine links without the plant or the governor: ledger.h, port.h, license.h, solver.h, machine.h, report.h compile under -DACME_NO_PLANT -DACME_NO_GOVERNOR | PASS |
