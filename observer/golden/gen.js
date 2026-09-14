/* ACME-500 v0 golden generator — this stands in for the backend.
   It simulates the firm ONCE, offline, and emits the four contract files:
     firm.json  tape.jsonl  snapshots.jsonl  phases.json
   The page never runs this. The page only folds the tape and must match
   snapshots.jsonl exactly (see __fold_check).
   Classic script: defines generate() on the global. No imports, no binaries. */

function mulberry(seed) {
  let a = seed >>> 0;
  return function () {
    a = (a + 0x6d2b79f5) | 0;
    let t = Math.imul(a ^ (a >>> 15), 1 | a);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

var PHASE_PLAN = [
  [0, 'INSTRUMENT', 'the tape is spooled; nothing is claimed'],
  [6, 'COMPILE', 'the join graph draws itself over the rack'],
  [14, 'REPLAY', 'the tower is run against its own history'],
  [20, 'LICENSE', 'rungs are issued per class and band'],
  [30, 'CANARY', 'machine verdicts run beside live desks'],
  [38, 'CLIMB', 'licensed bands climb; floors go quiet'],
  [46, 'COMPILE_OUT', 'classes collapse into routine blocks'],
  [54, 'CASCADE', 'the annex empties; residual is what is left']
];

function generate(opts) {
  opts = opts || {};
  var DAYS = opts.days || 60, SEED = opts.seed || 500, SPAN = 6;
  var R = mulberry(SEED);
  var ri = function (a, b) { return a + Math.floor(R() * (b - a + 1)); };
  var pick = function (arr) { return arr[Math.floor(R() * arr.length)]; };

  /* ---------- firm.json ---------- */
  var WIRE_NAMES = ['INTAKE', 'CREDIT', 'SUPPLY', 'CARE', 'PEOPLE', 'MARKET'];
  var wires = WIRE_NAMES.map(function (name, id) { return { id: id, name: name }; });

  var CLASS_NAMES = [
    'CLAIM_TRIAGE', 'DOC_INTAKE', 'ID_PROOF', 'DUPE_MERGE',
    'LIMIT_REVIEW', 'DISPUTE_ADJ', 'WRITEOFF_APPR', 'FRAUD_HOLD',
    'VENDOR_ONBOARD', 'PO_EXCEPTION', 'LANE_REBOOK', 'SHORTSHIP_ADJ',
    'REFUND_ADJ', 'ESCALATION_CB', 'GOODWILL_CR', 'SLA_BREACH',
    'REQ_APPROVAL', 'COMP_EXCEPT', 'LEAVE_ADJ', 'ACCESS_GRANT',
    'PRICE_EXCEPT', 'PROMO_CLEAR', 'CONTRACT_REDLINE', 'RENEWAL_TERMS'
  ];
  var classes = CLASS_NAMES.map(function (name, id) {
    return {
      id: id, name: name, wire: id >> 2,
      n_systems: ri(1, 5),
      verdict_latency: ri(1, 6),
      base_rate: ri(420, 940),
      arrival_per_day: ri(2, 6),
      value: ri(2, 90) * 1000,
      reversible: R() < 0.55 ? 1 : 0,
      warrant: R() < 0.25 ? 1 : 0
    };
  });

  var KIND_BY_DEPTH = ['C', 'VP', 'director', 'manager', 'lead', 'IC'];
  var SAL = { IC: 110, lead: 150, manager: 205, director: 290, VP: 390, C: 920 };
  var seats = [];
  function mkSeat(kind, fn, wire, boss, depth) {
    var id = seats.length;
    seats.push({
      id: id, kind: kind, fn: fn, wire: wire, boss: boss, depth: depth,
      salary: SAL[kind] * 1000 + ri(0, 40) * 1000,
      attention: ri(180, 460), spec: []
    });
    return id;
  }
  mkSeat('C', 'E', 6, -1, 0);                                      // 0
  var vps = [], dirs = [], mgrs = [], leads = [], ics = [];
  for (var w = 0; w < 6; w++) vps.push(mkSeat('VP', 'E', w, 0, 1));                       // 1..6
  for (w = 0; w < 6; w++) for (var k = 0; k < 2; k++) dirs.push(mkSeat('director', 'E', w, vps[w], 2));   // 7..18
  for (w = 0; w < 6; w++) for (k = 0; k < 3; k++) mgrs.push(mkSeat('manager', 'E', w, dirs[w * 2 + (k % 2)], 3)); // 19..36
  for (w = 0; w < 6; w++) for (k = 0; k < 5; k++) leads.push(mkSeat('lead', 'E', w, mgrs[w * 3 + (k % 3)], 4));   // 37..66
  for (var i = 0; i < 106; i++) {                                                          // 67..172
    var ww = i % 6, kk = Math.floor(i / 6) % 5;
    ics.push(mkSeat('IC', 'E', ww, leads[ww * 5 + kk], 5));
  }
  var alphas = [];
  for (i = 0; i < 18; i++) {                                                               // 173..190
    var d = 1 + (i % 5);
    alphas.push(mkSeat(KIND_BY_DEPTH[d], 'alpha', 6, vps[i % 6], d));
  }
  var warrants = [];
  for (i = 0; i < 9; i++) {                                                                // 191..199
    var dw = 1 + (i % 2);
    warrants.push(mkSeat(KIND_BY_DEPTH[dw], 'warrant', 6, dw === 1 ? 0 : vps[i % 6], dw));
  }

  var byWire = [[], [], [], [], [], []];
  classes.forEach(function (c) { byWire[c.wire].push(c.id); });
  seats.forEach(function (s) {
    var pool = s.wire < 6 ? byWire[s.wire] : classes.map(function (c) { return c.id; });
    if (s.fn === 'warrant') {
      pool = classes.filter(function (c) { return c.warrant === 1; }).map(function (c) { return c.id; });
      if (!pool.length) pool = [0, 1, 2];
    }
    var want = s.kind === 'IC' ? ri(2, 4) : ri(3, 6);
    var set = {};
    for (var j = 0; j < want; j++) set[pick(pool)] = 1;
    s.spec = Object.keys(set).map(Number).sort(function (a, b) { return a - b; });
  });

  var writ = { thin_margin: 40, eps_floor: 15, kappa_max: 1800, audit_min: 12, canary_delta: 25 };
  var firm = { seed: SEED, span: SPAN, n: seats.length, wires: wires, classes: classes, seats: seats, writ: writ };

  /* ---------- phases.json ---------- */
  var phases = PHASE_PLAN.map(function (p) { return { day: p[0], phase: p[1], note: p[2] }; });
  function phaseOf(day) {
    var cur = phases[0].phase;
    for (var q = 0; q < phases.length; q++) if (day >= phases[q].day) cur = phases[q].phase;
    return cur;
  }

  /* ---------- assignment pools ---------- */
  var handlers = classes.map(function () { return []; });
  seats.forEach(function (s) {
    if (s.kind !== 'IC' && s.kind !== 'lead') return;
    s.spec.forEach(function (c) { handlers[c].push(s.id); });
  });
  handlers.forEach(function (h, c) {
    if (!h.length) ics.forEach(function (id) { if (seats[id].wire === classes[c].wire) h.push(id); });
    if (!h.length) h.push(ics[0]);
    h.sort(function (a, b) { return a - b; });
  });

  /* ---------- accounting (the backend's own fold) ---------- */
  var N = seats.length;
  var held = new Array(N).fill(0), minutes = new Array(N).fill(0);
  var trayOf = new Map();
  var ledger = [0, 0, 0, 0, 0, 0, 0, 0]; // FETCH DECIDE HOLD ESCALATE MEETING UNDO AUDIT PATCH
  var CK = ['arrive', 'assign', 'context', 'decide', 'hold', 'escalate', 'effect', 'undo', 'outcome', 'meeting', 'tick', 'license', 'kappa', 'patch'];
  var counters = {}; CK.forEach(function (k) { counters[k] = 0; });
  var license = [], admitted = [];
  for (i = 0; i < 24; i++) { license.push([0, 0, 0, 0, 0, 0]); admitted.push([0, 0, 0, 0, 0, 0]); }
  var kappa = 0, counter = 0, cascade = [], openSet = new Set(), arriveDay = [];
  var rows = [];

  function emit(type, cls, day, oid, seat, a, b, margin, value, arm, band, flags) {
    rows.push([type, cls, day, oid, seat, a, b, margin, value, arm, band, flags]);
    counters[type.toLowerCase()]++;
    if (oid >= 0 && type !== 'HOLD' && trayOf.has(oid)) { held[trayOf.get(oid)]--; trayOf.delete(oid); }
    switch (type) {
      case 'ARRIVE': openSet.add(oid); arriveDay[oid] = day; break;
      case 'CONTEXT': minutes[seat] += b; ledger[0] += b; break;
      case 'DECIDE': minutes[seat] += b; ledger[1] += b; break;
      case 'HOLD':
        if (trayOf.has(oid)) held[trayOf.get(oid)]--;
        trayOf.set(oid, seat); held[seat]++;
        minutes[seat] += b; ledger[2] += b; break;
      case 'ESCALATE': minutes[seat] += b; ledger[3] += b; break;
      case 'MEETING': minutes[seat] += value; ledger[4] += value; break;
      case 'UNDO': minutes[seat] += b; ledger[5] += b; break;
      case 'OUTCOME': ledger[6] += b; openSet.delete(oid); if (flags === 1) counter++; break;
      case 'LICENSE': license[cls][band] = a; if (flags === 1) admitted[cls][band] = 1; break;
      case 'KAPPA': kappa = a; break;
      case 'PATCH':
        ledger[7] += a <= 1 ? 60 : 0;
        if (a === 2) cascade.push('w' + Math.floor(b / 10) + 'f' + (b % 10));
        else if (a === 3) cascade.push('af' + b);
        break;
    }
  }

  /* ---------- the day loop ---------- */
  var open = new Map(), oidNext = 0, snapshots = [];
  var systemsBought = [];
  var collapsed = [];

  function pickSeat(cls) {
    var h = handlers[cls];
    return h[Math.floor(R() * h.length)];
  }
  function bandOf(v) { return Math.max(0, Math.min(5, Math.floor(v / 16000))); }
  function machineFlag(cls, band) {
    var rung = license[cls][band];
    return rung >= 2 && R() < rung / 4 ? 1 : 0;
  }
  function outcome(o, day) {
    var c = classes[o.cls];
    var age = day - o.day;
    var grade = age > c.verdict_latency + 4 ? 1 : (R() > c.base_rate / 1000 ? 2 : 0);
    emit('OUTCOME', o.cls, day, o.oid, o.seat, grade, ri(writ.audit_min, writ.audit_min + 14),
      o.margin, o.value, o.arm, o.band, machineFlag(o.cls, o.band));
    open.delete(o.oid);
  }
  function decide(o, day) {
    var c = classes[o.cls];
    var margin = ri(-140, 300);
    o.margin = margin;
    var verdict = margin > 0 ? 1 : 0;
    emit('DECIDE', o.cls, day, o.oid, o.seat, verdict, ri(5, 40), margin, o.value, o.arm, o.band,
      Math.abs(margin) < writ.thin_margin ? 1 : 0);
    if (verdict === 1) {
      emit('EFFECT', o.cls, day, o.oid, o.seat, ri(0, 3), ri(2, 12), margin, o.value, o.arm, o.band, 0);
      o.st = 'EFFECTED'; o.since = day;
    } else outcome(o, day);
  }
  function escalate(o, day) {
    var to = seats[o.seat].boss;
    if (to < 0) { decide(o, day); return; }
    emit('ESCALATE', o.cls, day, o.oid, o.seat, to, ri(6, 30), o.margin | 0, o.value, o.arm, o.band, 0);
    o.seat = to; o.cold = 1; o.st = 'ASSIGNED'; o.since = day;
  }

  for (var day = 0; day < DAYS; day++) {
    var rows0 = rows.length;
    var phase = phaseOf(day);

    classes.forEach(function (c) {
      if (collapsed.indexOf(c.id) >= 0 && R() < 0.5) return;
      var k = c.arrival_per_day + (R() < 0.4 ? (R() < 0.5 ? 1 : -1) : 0);
      for (var j = 0; j < Math.max(0, k); j++) {
        var oid = oidNext++;
        var value = Math.round(c.value * (0.5 + R()) / 100) * 100;
        var band = bandOf(value), arm = ri(0, 3);
        emit('ARRIVE', c.id, day, oid, -1, 0, 0, 0, value, arm, band, 0);
        open.set(oid, { oid: oid, cls: c.id, seat: -1, st: 'NEW', day: day, since: day, value: value, band: band, arm: arm, margin: 0, cold: 0 });
      }
    });

    var live = Array.from(open.values());
    for (i = 0; i < live.length; i++) {
      var o = live[i], c = classes[o.cls];
      if (o.st === 'NEW') {
        if (R() < 0.78) {
          var s = pickSeat(o.cls);
          emit('ASSIGN', o.cls, day, o.oid, s, -1, held[s], 0, o.value, o.arm, o.band, 0);
          o.seat = s; o.st = 'ASSIGNED'; o.since = day;
        }
      } else if (o.st === 'ASSIGNED') {
        if (R() < 0.62) {
          var ns = classes[o.cls].n_systems;
          for (var q = 0; q < ns; q++)
            emit('CONTEXT', o.cls, day, o.oid, o.seat, (o.cls * 7 + q) % 96, ri(3, 18), 0, o.value, o.arm, o.band, o.cold);
          o.cold = 0;
          var r = R();
          if (r < 0.40) emit('HOLD', o.cls, day, o.oid, o.seat, ri(0, 5), ri(4, 25), o.margin | 0, o.value, o.arm, o.band, 0), o.st = 'HELD', o.since = day;
          else if (r < 0.52 && seats[o.seat].depth > 1) escalate(o, day);
          else decide(o, day);
        }
      } else if (o.st === 'HELD') {
        if (R() < 0.30) {
          if (R() < 0.32 && seats[o.seat].depth > 1) escalate(o, day);
          else decide(o, day);
        }
      } else if (o.st === 'EFFECTED') {
        if (day - o.since >= c.verdict_latency) {
          if (c.reversible === 1 && R() < 0.10) {
            emit('UNDO', o.cls, day, o.oid, o.seat, ri(0, 3), ri(4, 22), o.margin, o.value, o.arm, o.band, 0);
            o.st = 'ASSIGNED'; o.since = day; o.cold = 1;
          } else outcome(o, day);
        }
      }
    }

    var nMeet = ri(3, 9);
    for (i = 0; i < nMeet; i++) {
      var owner = pick(leads.concat(mgrs, dirs));
      var cross = R() < 0.35;
      var other = -1;
      if (cross) { do { other = pick(leads.concat(mgrs)); } while (seats[other].wire === seats[owner].wire); }
      emit('MEETING', -1, day, -1, owner, other, ri(2, 9), 0, ri(15, 90), 0, 0, cross ? 1 : 0);
    }

    if (day >= 20) {
      var nLic = day < 30 ? ri(4, 8) : ri(2, 5);
      for (i = 0; i < nLic; i++) {
        var lc = (day * 5 + i * 7) % 24, lb = (day + i * 2) % 6;
        if (license[lc][lb] >= 5) continue;
        emit('LICENSE', lc, day, -1, -1, license[lc][lb] + 1, 0, 0, 0, 0, lb, R() < 0.3 ? 1 : 0);
      }
    }

    var drift = phase === 'CLIMB' || phase === 'CASCADE' ? ri(10, 110) : ri(-40, 80);
    var nk = Math.max(0, Math.min(writ.kappa_max, kappa + drift));
    emit('KAPPA', -1, day, -1, -1, nk, nk - kappa, 0, 0, 0, 0, 0);

    if (day === 12) { systemsBought.push(11); emit('PATCH', 9, day, -1, -1, 1, 11, 0, 0, 0, 0, 0); }
    if (day === 26) emit('PATCH', -1, day, -1, -1, 0, 7, 0, 0, 0, 0, 0);
    if (day === 33) { systemsBought.push(42); emit('PATCH', 17, day, -1, -1, 1, 42, 0, 0, 0, 0, 0); }
    if (phase === 'CLIMB' && day % 2 === 0) {
      var step = (day - 38) / 2, wg = step % 6, fl = Math.min(5, Math.floor(step / 6) + 1);
      emit('PATCH', -1, day, -1, -1, 2, wg * 10 + fl, 0, 0, 0, 0, 0);
    }
    if (phase === 'COMPILE_OUT' && day % 3 === 1) {
      var cc = ((day - 47) / 3 | 0) * 5 % 24;
      collapsed.push(cc);
      emit('PATCH', cc, day, -1, -1, 4, 0, 0, 0, 0, 0, 0);
    }
    if (phase === 'CASCADE') emit('PATCH', -1, day, -1, -1, 3, Math.min(5, day - 54), 0, 0, 0, 0, 0);

    emit('TICK', -1, day, -1, -1, day, rows.length - rows0 + 1, 0, 0, 0, 0, 0);

    var residual = 0;
    trayOf.forEach(function (st, oid) { if (arriveDay[oid] <= day - 10) residual++; });
    snapshots.push({
      day: day, phase: phase,
      seats: { held: held.slice(), minutes: minutes.slice() },
      open: Array.from(openSet).sort(function (a, b) { return a - b; }),
      ledger: ledger.slice(),
      machine: {
        license: license.map(function (r) { return r.slice(); }),
        admitted: admitted.map(function (r) { return r.slice(); }),
        counters: CK.reduce(function (acc, k) { acc[k] = counters[k]; return acc; }, {}),
        kappa: kappa, counter: counter
      },
      cascade: cascade.slice(), residual: residual
    });
  }

  var tape = rows.map(function (r) {
    return '{"type":"' + r[0] + '","cls":' + r[1] + ',"day":' + r[2] + ',"oid":' + r[3] +
      ',"seat":' + r[4] + ',"a":' + r[5] + ',"b":' + r[6] + ',"margin":' + r[7] +
      ',"value":' + r[8] + ',"arm":' + r[9] + ',"band":' + r[10] + ',"flags":' + r[11] + '}';
  }).join('\n');

  return { firm: firm, tape: tape, snapshots: snapshots, phases: phases, rowCount: rows.length };
}

if (typeof module !== 'undefined') module.exports = { generate: generate };
if (typeof window !== 'undefined') window.generate = generate;
