# Receipt · ACME-500 v2 step C1 · the judge on its own key, the read budget, planted truth off the path, the governor · 2026-09-14

*Claude Opus 5. Implements step C1 as sequenced in `docs/ARCHITECTURE_v2.md` §14 and §14a: the first step allowed to move a number, with every delta named against C0 (`receipts/step-c0-2026-09-14/addendum-prints/`). Branch `v2` of `C:\55555\acme500` (`github.com/bochen2029-pixel/ACME-500`). Built and run under WSL from a fresh copy; raw outputs in `receipts/step-c1-2026-09-14/` (29 files: the battery, twenty lie arms, the tool oracles, the three modes at the default read budget, and the automate and twin at a budget of 4,000).*

## 0 · What C1 changed

**The judge owns its competence and its noise (F16).** `Judge::read(const Frame&)` takes nothing but the frame. `PlantJudge` (`world.h`) draws its judgement noise and the rented mind's act coin from **its own seed**, never the world's; it holds a competence per class that starts at v1's 0.55 and learns from the outcomes the kernel tells it about through `Judge::observe(cls, right)` — a stand-in for a model that improves with feedback [P], the same rule v1 kept inside the field, moved to the side of the seam it belongs on. Two judges stand behind the port: the resident (`'PLJ2'`, learning) and the frontier (`'PLF1'`, a fixed 0.96 [P]). `Field` no longer holds a machine skill. `--judge plant|null|rules` stands a stub behind a full run; that is the gym's competence sweep at its floor.

**The kernel's completeness is its own.** `Frame::coverage_hat` is stamped by the kernel from the compile step's coverage (1.0 until the replay has measured it), never the plant's determinant mass. The store answers content and a content hash only.

**Planted truth off the path.** The class table is split: `ClassSpec` (`core.h`, authored: name, wire, verdict latency, base rate, nominal arrivals, value, reversible, warrant, and the new `counterparty` bit) and `PlantedSpec` (`world.h`, `planted(c)`: systems per class, judgement intensity), aligned by name at start-up. `build_acme` moves to the plant with the planted α and F returned beside the firm, never on it; `Firm` loses them. The kernel reads measurements instead: the head's judgement feature is the measured decide fraction; the would-cost that is kappa's denominator is **metered minutes per person-decided cell of the class**, a fold over ACT and fetch rows; review and signing minutes use the measured decide fraction; the residual's mass is the measured arrival rate and its counterparty column is the schema's flag; the cascade fits a panel of `(E, N)` points it is handed. O17 now compiles the kernel under `-DACME_NO_PLANT -DACME_NO_GOVERNOR`; there is nothing planted it can name.

**The read budget and the memo.** Every period the field prices every open cell on the compile step's coverage before any judge is read. A cell whose frame hash (the store's content hash, the template pin folded in, never the clock) is unchanged since its last `PROPOSAL` carries that proposal forward from the ledger's memo — a fold of `PROPOSAL` rows; a cell with no valid proposal, or one drawn by the floor sample (rate `eps_floor`, on the kernel's own key), is read, ripest first, up to `Writ::read_budget` (50,000 by default: one card at ~0.6 reads a second over a day-long period [BUDGET]); a cell the budget never reached holds with the new reason `unread`. The forgone dual — the field's own prices on the cells left unread — prints beside the reads.

**The governor in its own translation unit (`governor.h`).** It holds the salt and the ladder. Before the machine's period it draws canary / audit for every open cell and writes `STRATUM` rows (kind, canary rate, audit rate); the gate reads the strata from the ledger's fold of those rows and never touches the salt. After settlement it folds the day's `OUTCOME` rows into the ladder, steps it on the machine's own supervision meter, and writes `LICENSE` and `KAPPA` rows. The licence table (`license.h`) is keyed to the judge it was earned under; a judge with another hash reads rung 0 everywhere. History admission gains a clause: the judge's agreement with the firm on the class must be beyond chance (Cohen's κ ≥ 0.2) — F22, below.

**Two oracles.** O16: a null judge and a coin judge license nothing, from history or the wager; its lie is the plant's reader under the null's name. O21: the canary draw is independent of the machine's band — the governor drew before the judge read, one trial per cell, χ² against the rates on the rows; its lie is a draw that skips the narrow bands. O17 gains a second lie, one include of the governor. Lie arms 18 and 19; CMake range 0–19.

**A fold gap C0 could not see.** A day that ends between a person's decision and its commit leaves the cell `INPROG` with a band the fold did not carry; C1's different trajectory left 19 such cells at the end of the battery world and O14 went red on them. The mid-case `HOLD` row now carries the band. This is exactly what O14 is for.

## 1 · Acceptance

Build 0 warnings. `--selftest` **22 of 22** (20.0 s: five short automate runs now stand inside it). Twenty lie arms, 0–19, each exit 0 with the lied-to oracle's own line PASS and no FAIL line; the two new ones read

```
  [PASS] O16  ... (null judge: top rung 5, 30 LICENSE rows, 2264 wager outcomes; rules judge: top rung 0, 0 rows, 0 outcomes)   lie 18: the plant's reader under the null's name, caught
  [PASS] O21  ... (chi-square 10352.6 on 72 class-band cells against a 99.9% point of 114.9)                                       lie 19: a band-dependent draw, caught
```

O17 green on both lies (`world.h` and `governor.h` refused under their guards), O28 green (18 functions, nothing dead: `completeness_from` lost its last caller to `gather()` and was deleted), O25a green over `port.h` and `license.h` too. `--sim`: every reading identical to C0 (82,479 settled, 42.7 / 48.9 / 8.4, cycle 51.9, 123,927 open); the chain head moves from `b01905e496dba40e` to `3f9a4a26fab68e5c` because the alphabet pin moved (the schema lost two planted fields and gained one authored one; `unread` joined the reasons) and the HEADER row carries it. Tree 4,551 lines.

## 2 · The readings, C0 → C1, each delta named

**The twin** (500 seats, demand 1.5, 260 days, the default budget):

| | C0 | C1 | delta |
|---|---|---|---|
| resident settled | 186,956 | 187,068 | +112 |
| good outcomes % | 38.34 | 38.06 | −0.28 |
| wrong outcomes % | 11.89 | 11.94 | +0.05 |
| late outcomes % | 49.77 | 50.01 | +0.24 |
| mean cycle, days | 57.99 | 58.14 | +0.15 |
| total cost, $M (incumbent 1,837.7) | 1,247.95 | 1,254.27 | +6.3; the machine's advantage −589.8 → −583.4 |
| human minutes, thousands (incumbent 24,518) | 23,325 | 23,355 | +30 |
| true paired effect, points of good rate | −4.36 | −4.64 | |
| within-firm canary estimate | +27.37 | +29.78 | |
| **bias of the estimator** | **+31.73** | **+34.42** | +2.7 |

**The automate:**

| reading | C0 | C1 | named cause |
|---|---|---|---|
| bands admitted from history | 34 | 32 | (b) the kernel's completeness is the compile step's coverage, so replay directions and bands shift; the κ clause changed nothing for the plant's judge in the battery world (§3) |
| class-bands past rung 0 at day 259 | 32 | 29 | (b), (c) |
| unlicensable bands | 26 of 48 | 26 of 48 | measured arrivals per term in place of nominal × demand: same verdict |
| acted unattended | 16,188 | 15,672 | (c) |
| canary acts | 537 | 1,484 | (c) the audit is drawn at the class's own wager count; v1 drew it at the band's, which at a fresh band is n = 0 and a 58 percent audit rate, so most rung-1 cells were drafted before the canary could act |
| audit sample | 37,409 | 35,418 | (c) |
| frontier rentals per day | 1,079 | 986 | (a) the coin on the judge's key; F12 unchanged in kind |
| **kappa** | **0.242** | **0.110** | (d) the denominator is metered: 21.27M minutes of person-work removed against the formula's 9.73M, the review minutes caused 2.34M against 2.36M; the plant's formula priced a round trip at about 90 minutes and the firm's own rows price the decided cell at 2.2 times that |
| licensed decision mass | 63.7% | 63.5% | (b), (c) |
| N, honest fit / origin fit | 193 / 181 | 193 / 182 | the cascade fits a handed panel; α 0.145, F 17.6 unchanged |
| residual: warrant / counterparty / frame (wait + coverage) / thin | 4.7 / 0.7 / 14.3 (10.8 + 3.5) / 16.7 | 4.6 / 0.7 / 10.0 (6.8 + 3.2) / 21.1 | (f) mass is the measured arrival rate; counterparty is the schema's flag on `claim.fraud.refer`, the one class the old rule selected; the classes that lost their rung-2 band moved from frame to thin |
| reads per period | 3,328 (every open cell, every period) | 1,729 (1,684 fresh, 45 floor) | (e) the memo carried 1,462 cells a period; PROPOSAL rows 266,202 → 138,345 |
| the budget bound | — | 1 of 80 periods | (e) the warm backlog at day 180 was 83,778 open cells; 33,778 held unread that first day, none after |
| F21: bands at rung 5 under n₀ | 8 of 10 | 6 of 10 | recorded, not fixed |
| tape rows | 4,357,488 | 4,543,740 | 289,062 STRATUM rows in, 127,857 PROPOSAL rows out, the rest in holds and decisions |

**(e) The price of a small read budget, measured** (`out_*_budget4000.txt`): at 4,000 reads a period the budget binds in 22 of 80 periods while the backlog is read through; 896,248 unread holds; forgone dual 82.1 percent of the field's prices; acts 15,672 → 12,271; licensed mass 63.5 → 59.2; N 193 → 214. In the twin: good rate 38.06 → 31.45, late 50.01 → 56.76, cycle 58.1 → 62.5 days, the cost advantage −583 → −522M, and the resident arm's human minutes go from 1,163k below the incumbent to 309k above it — people pick up what the machine has not read. **The draft loop's queue win is bought with reads; a machine that reads 4,000 cells a day against an 84,000-cell backlog wins the queue in a fortnight and loses seven points of good rate on the way.** The default is set where one card's day does not bind on this backlog, and the dial is `--budget`.

**(a) The judge's key.** The distribution of the read noise is unchanged; its realisation is the judge's own, so every machine decision's noise draw differs from C0's while nothing about its level does. That is the whole of the twin's motion above beyond (b)–(d): −0.28 points of good rate, +0.05 wrong.

**(g)** The governor steps the ladder after the day's outcomes are folded (v1 stepped it inside the period, before them): a promotion lands one day earlier. **(i)** The head's judgement feature is the measured decide fraction centred at 0.3 (its level is biased upward, O13; the planted one was centred at 0.2).

## 3 · F22 · history admitted a coin

Without the beyond-chance clause, a judge that flips a coin keyed on the cell's id agrees with the firm on about half of history at the firm's own good rate, and `p_agree ≥ firm_base − 0.01` holds on about half of its bands by luck. Measured in the battery world (200 seats, warm 40, 70 days, seed 7; `--judge rules`, `-DACME_KAPPA_ADMIT=-1.0`): **8 class-bands admitted to rung 1 by history, 7 still past rung 0 at the end**; with the clause (κ ≥ 0.2): **0**. The null judge (direction 0, the thin band, never admissible): 0 either way. The plant's judge (κ ≈ 0.75 where it agrees on 89 percent): **16 bands either way** — the clause costs a real judge nothing. Fixed in C1 (`Governor::license_from_history`), and O16 stands over it with its lie. The spec's §4.5 carries the clause.

## 4 · What C1 did not do, precisely

- The frontier is still re-rented for the same cell every period (986 rentals a day; F12); the counsel memo keyed on `(oid, frame_hash)` and O23 are step E, and the frame hash now exists for it.
- The retained-control stratum (kind 3) is declared and never drawn (step E, O20).
- The read budget's ranking is ripeness (lateness) only; the residual against the class centroid of §4.2 is not stored yet.
- The licence key `(schema pin, template pin, class, band, judge hash)` lives on the table and refuses another judge; the `LICENSE` row has no field to carry the template hash, and `fold_ladder` (O15) keys by class and band and does not yet assert the judge from the `PROPOSAL` rows.
- The frame hash the memo was read under is held by the resident (`memo_hash`) and recomputed by the store at fold time rather than carried on the `PROPOSAL` row, which has no free 32-bit field. In the synthetic world the record is a pure function of the cell so the two are equal by construction; a durable real lane must checkpoint it or extend the row (step D).
- `STRATUM` rows are written per open cell per period (289,062 here; 1,140,602 at the 4,000 budget, where the backlog stays open longer). Step D's rule — a row when the draw changes, not per period — applies to them as it does to `HOLD`.
- Kappa's numerator (the review and signing minutes a draft or a warrant costs) is still a formula over the measured decide fraction; no row meters a person keying a draft, because no person keys one in this world.
- F14 (26 of 48 unlicensable), F21 (6 bands at rung 5 under n₀) and F11 (novelty per-class constant) stand as recorded.
- The battery's wall time is 20 s (v1: 5 s): O1b, O16 and O21 each run the short automate.

## 5 · The tree after C1

4,551 lines. Hashes (sha256, first 16): core.h `47e4e52a6c22d307` · firm.h `3003a951a70d362e` · world.h `36ef6302f3168b0f` · human.h `49d5b068c5650c9c` · ledger.h `58749479951c2722` · port.h `8449e80adef7cd38` · license.h `6f7fe24c4265e2d3` · governor.h `3751ad0b47465c7e` · solver.h `391d3c4a5defa815` · machine.h `d8baddef2e6c1326` · report.h `e638c26f304b459e` · main.cpp `750d67de0ef5d731` · CMakeLists.txt `9f3092036873019d`.
