/* ACME-500 v0 CONTRACT — frames (v1) + fold. Injected as a global, never imported.
   One place() for the whole build. One fold, written against the record names
   exactly as the tape gives them. The page is a pure observer: nothing here
   invents a row, and nothing here is simulated.

   FRAME v1 — THE BUILDING, STATED ONCE.
   The tape did not change. Only the map from a seat to a place did, and it lives
   in this one function. Frame v0 put depth on the vertical axis and drew the
   cone as six wedges around a cylinder, which read as platters on a spindle.
   Frame v1 reads the way a headquarters reads:

     floor 0        the lobby: reception, the counter on the wall, the dock
     floors 1..6    one department per floor, wire w on floor w+1
     floor 7        the executive floor: the C, the VPs, every warrant seat
     the annex      the alpha-functions, a lower podium beside the tower, with a bridge
     the basement   the resident: the tape spool, the systems rack, the field, the gauge

   Inside a department floor, the org chart is the furniture. ICs sit in pods of
   eight with their lead at the head of the pod; managers have glass offices on
   the window wall; directors have the corner offices. An escalation crosses the
   floor to the office, and only leaves the floor when it goes to a VP.

   Units: metres, three.js Y-up (y = floor * FLOOR_PITCH). The tower's footprint
   is derived from the firm at prepare(): more people, a longer floor plate. */
(function () {
  var C = {
    FLOOR_PITCH: 4.5, SLAB: 0.4, DESK_PITCH: 2.0, ROW_PITCH: 2.4,
    POD_X: 10, POD_Z: 7.2, POD_ICS: 8, MARGIN: 4, ROOM_ZONE: 8,
    OFFICE_W: 4.6, OFFICE_D: 4.2, CORE_W: 9, CORE_D: 6,
    LZ: 30, FLOORS: 8, LOBBY: 0, EXEC: 7,
    ANNEX_W: 18, ANNEX_D: 14, ANNEX_GAP: 7, ANNEX_FLOORS: 3, ANNEX_PER_ROW: 6,
    LEDGER: ['FETCH', 'DECIDE', 'HOLD', 'ESCALATE', 'MEETING', 'UNDO', 'AUDIT', 'PATCH'],
    TYPES: ['ARRIVE', 'ASSIGN', 'CONTEXT', 'DECIDE', 'HOLD', 'ESCALATE', 'EFFECT', 'UNDO',
      'OUTCOME', 'MEETING', 'TICK', 'LICENSE', 'KAPPA', 'PATCH'],
    COUNTER_KEYS: ['arrive', 'assign', 'context', 'decide', 'hold', 'escalate', 'effect', 'undo',
      'outcome', 'meeting', 'tick', 'license', 'kappa', 'patch'],
    HOLD_REASON: ['thin margin', 'no license', 'missing field', 'awaiting boss', 'queue depth', 'policy unclear'],
    GRADE: ['good', 'late', 'bad'],
    PATCH_KIND: ['span change', 'lane bought', 'floor dark', 'annex dark', 'class collapsed'],
    PHASES: ['INSTRUMENT', 'COMPILE', 'REPLAY', 'LICENSE', 'CANARY', 'CLIMB', 'COMPILE_OUT', 'CASCADE'],
    dims: null
  };
  var T = {}; C.TYPES.forEach(function (t, i) { T[t] = i; });

  /* ---------------- FRAMES ---------------- */
  var slots = null, firmRef = null, pods = null;

  function floorOfWire(w) { return w + 1; }

  function prepare(firm) {
    firmRef = firm;
    slots = new Array(firm.n);
    pods = [[], [], [], [], [], []];            // per wire: list of pods {lead, ics[]}
    var byId = {};
    firm.seats.forEach(function (s) { byId[s.id] = s; });

    var wireIC = [[], [], [], [], [], []], wireMgr = [[], [], [], [], [], []], wireDir = [[], [], [], [], [], []];
    var exec = [], annex = [];
    firm.seats.forEach(function (s) {
      if (s.kind === 'C' || s.kind === 'VP' || s.fn === 'warrant' || (s.wire >= 6 && s.fn === 'E')) { exec.push(s); return; }
      if (s.fn === 'alpha') { annex.push(s); return; }
      if (s.kind === 'IC' || s.kind === 'lead') { wireIC[s.wire].push(s); return; }
      if (s.kind === 'manager') { wireMgr[s.wire].push(s); return; }
      wireDir[s.wire].push(s);
    });

    /* pods: one team per lead, eight ICs a pod, overflow to a headless pod */
    var maxPods = 1;
    for (var w = 0; w < 6; w++) {
      var leads = wireIC[w].filter(function (s) { return s.kind === 'lead'; }).sort(function (a, b) { return a.id - b.id; });
      var ics = wireIC[w].filter(function (s) { return s.kind === 'IC'; }).sort(function (a, b) { return a.boss - b.boss || a.id - b.id; });
      var teams = {}, order = [];
      leads.forEach(function (l) { teams[l.id] = { lead: l, ics: [] }; order.push(l.id); });
      ics.forEach(function (s) {
        if (!teams[s.boss]) { teams[s.boss] = { lead: null, ics: [] }; order.push(s.boss); }
        teams[s.boss].ics.push(s);
      });
      order.forEach(function (key) {
        var t = teams[key];
        for (var k = 0; k < Math.max(1, Math.ceil(t.ics.length / C.POD_ICS)); k++)
          pods[w].push({ lead: k === 0 ? t.lead : null, ics: t.ics.slice(k * C.POD_ICS, (k + 1) * C.POD_ICS), team: key });
      });
      if (pods[w].length > maxPods) maxPods = pods[w].length;
    }
    var PODS_X = Math.max(3, Math.ceil(maxPods / 2));
    var LX = PODS_X * C.POD_X + 2 * C.MARGIN + C.ROOM_ZONE;
    var LZ = C.LZ;
    C.dims = {
      LX: LX, LZ: LZ, PODS_X: PODS_X, PODS_Z: 2,
      OFFICE_Z: LZ / 2 - 0.6 - C.OFFICE_D / 2,
      CORE_Z: LZ / 2 - C.CORE_D / 2 - 0.6,
      ANNEX_X: -(LX / 2 + C.ANNEX_GAP + C.ANNEX_W / 2),
      DOCK_X: LX / 2 + 9,
      H: C.FLOORS * C.FLOOR_PITCH
    };

    /* department floors */
    for (w = 0; w < 6; w++) {
      var floor = floorOfWire(w);
      pods[w].forEach(function (p, pi) {
        if (p.lead) slots[p.lead.id] = { frame: 'lead', floor: floor, wing: w, pod: pi, slot: 0 };
        p.ics.forEach(function (s, k) { slots[s.id] = { frame: 'pod', floor: floor, wing: w, pod: pi, slot: k + 1 }; });
      });
      var mg = wireMgr[w].sort(function (a, b) { return a.id - b.id; });
      var dr = wireDir[w].sort(function (a, b) { return a.id - b.id; });
      var officeSlot = 0;
      dr.forEach(function (s, i) {
        if (i < 2) slots[s.id] = { frame: 'corner', floor: floor, wing: w, pod: -1, slot: i };
        else slots[s.id] = { frame: 'office', floor: floor, wing: w, pod: -1, slot: officeSlot++ };
      });
      mg.forEach(function (s) { slots[s.id] = { frame: 'office', floor: floor, wing: w, pod: -1, slot: officeSlot++ }; });
    }

    /* executive floor: the C in the back-left office, then offices by depth then id */
    exec.sort(function (a, b) { return (a.kind === 'C' ? -1 : b.kind === 'C' ? 1 : 0) || a.depth - b.depth || a.id - b.id; });
    var ex = 0;
    exec.forEach(function (s) {
      if (s.kind === 'C') { slots[s.id] = { frame: 'chief', floor: C.EXEC, wing: -1, pod: -1, slot: 0 }; return; }
      slots[s.id] = { frame: 'execoffice', floor: C.EXEC, wing: -1, pod: -1, slot: ex++ };
    });

    /* annex: the alpha-functions, by depth, in rows */
    var af = [[], [], []];
    annex.sort(function (a, b) { return a.id - b.id; }).forEach(function (s) {
      af[s.depth >= 4 ? 0 : s.depth >= 2 ? 1 : 2].push(s);
    });
    af.forEach(function (list, f) { list.forEach(function (s, i) { slots[s.id] = { frame: 'annex', floor: f, wing: -1, pod: -1, slot: i }; }); });

    /* anything unplaced (should not happen) goes to the lobby */
    var lobbySlot = 0;
    firm.seats.forEach(function (s) { if (!slots[s.id]) slots[s.id] = { frame: 'lobby', floor: 0, wing: -1, pod: -1, slot: lobbySlot++ }; });
    return slots;
  }

  function podCenter(pi) {
    var D = C.dims;
    var col = pi % D.PODS_X, row = Math.floor(pi / D.PODS_X) % D.PODS_Z;
    return {
      x: -D.LX / 2 + C.MARGIN + col * C.POD_X + C.POD_X / 2,
      z: -D.LZ / 2 + C.MARGIN + row * C.POD_Z + C.POD_Z / 2
    };
  }
  /* offices along the window wall, skipping the core's span; n-th office from the left */
  function officeX(n) {
    var D = C.dims;
    var left = -D.LX / 2 + C.OFFICE_W + 0.6 + C.OFFICE_W / 2;    // corner office occupies the first bay
    var x = left + n * (C.OFFICE_W + 0.4);
    var coreL = -C.CORE_W / 2 - 0.6, coreR = C.CORE_W / 2 + 0.6;
    if (x + C.OFFICE_W / 2 > coreL && x - C.OFFICE_W / 2 < coreR) x += (coreR - coreL) + C.OFFICE_W * 0.5;
    var maxX = D.LX / 2 - C.OFFICE_W - 0.6 - C.OFFICE_W / 2;
    if (x > maxX) { x = -D.LX / 2 + 0.6 + C.OFFICE_W / 2; return { x: x, side: -1, k: n }; }   // overflow to the left side wall
    return { x: x, side: 0, k: n };
  }

  function place(seat) {
    var sl = slots[seat.id], D = C.dims;
    var y = sl.floor * C.FLOOR_PITCH;
    var base = { frame: sl.frame, floor: sl.floor, slot: sl.slot, wing: sl.wing, pod: sl.pod, yaw: 0, y: y };
    var pc, o;
    switch (sl.frame) {
      case 'pod': {
        pc = podCenter(sl.pod);
        var k = sl.slot - 1, r = k % 2, c = Math.floor(k / 2);
        base.x = pc.x - 2 + c * C.DESK_PITCH; base.z = pc.z + (r - 0.5) * C.ROW_PITCH;
        base.yaw = r === 0 ? Math.PI / 2 : -Math.PI / 2;          // the two rows face each other
        return base;
      }
      case 'lead': {
        pc = podCenter(sl.pod);
        base.x = pc.x - 4.4; base.z = pc.z; base.yaw = 0;          // head of the pod, facing the team
        return base;
      }
      case 'office': {
        o = officeX(sl.slot);
        if (o.side === 0) { base.x = o.x; base.z = D.OFFICE_Z; base.yaw = Math.PI; }
        else { base.x = -D.LX / 2 + 0.6 + C.OFFICE_D / 2; base.z = D.LZ / 2 - C.OFFICE_W * 1.5 - 2 - (o.k % 3) * (C.OFFICE_W + 0.4); base.yaw = -Math.PI / 2; }
        return base;
      }
      case 'corner': {
        var sx = sl.slot === 0 ? -1 : 1;
        base.x = sx * (D.LX / 2 - 0.6 - C.OFFICE_W / 2); base.z = D.OFFICE_Z; base.yaw = Math.PI;
        return base;
      }
      case 'chief': {
        base.x = -(D.LX / 2 - 0.6 - C.OFFICE_W); base.z = D.OFFICE_Z; base.yaw = Math.PI;
        return base;
      }
      case 'execoffice': {
        o = officeX(sl.slot + 1);
        if (o.side === 0) { base.x = o.x; base.z = D.OFFICE_Z; base.yaw = Math.PI; }
        else { base.x = D.LX / 2 - 0.6 - C.OFFICE_D / 2; base.z = D.LZ / 2 - C.OFFICE_W * 1.5 - 2 - (o.k % 3) * (C.OFFICE_W + 0.4); base.yaw = Math.PI / 2; }
        return base;
      }
      case 'annex': {
        var col2 = sl.slot % C.ANNEX_PER_ROW, row2 = Math.floor(sl.slot / C.ANNEX_PER_ROW);
        base.x = D.ANNEX_X + (col2 - (C.ANNEX_PER_ROW - 1) / 2) * C.DESK_PITCH;
        base.z = -C.ANNEX_D / 2 + 2.2 + row2 * C.ROW_PITCH;
        base.yaw = Math.PI / 2;
        return base;
      }
      default: {   // lobby
        base.x = -6 + (sl.slot % 6) * 2; base.z = -D.LZ / 2 + 6 + Math.floor(sl.slot / 6) * 2.4; base.yaw = 0;
        return base;
      }
    }
  }

  /* frozen test vectors, written as closed forms against the dims so they hold
     for any firm size and still catch a wrong place() */
  function checkPlace(firm) {
    var out = [], ok = true, D = C.dims;
    function eq(got, want, label) {
      var d = Math.abs(got.x - want[0]) + Math.abs(got.y - want[1]) + Math.abs(got.z - want[2]);
      var pass = d < 1e-9; ok = ok && pass;
      out.push({ label: label, got: [+got.x.toFixed(4), +got.y.toFixed(4), +got.z.toFixed(4)], want: want.map(function (v) { return +v.toFixed(4); }), pass: pass });
    }
    var chief = null; firm.seats.forEach(function (s) { if (s.kind === 'C') chief = s; });
    if (chief) eq(place(chief), [-(D.LX / 2 - 0.6 - C.OFFICE_W), C.EXEC * C.FLOOR_PITCH, D.OFFICE_Z], 'the C — executive floor, back-left office');
    var p0 = pods[0][0];
    var pc = { x: -D.LX / 2 + C.MARGIN + C.POD_X / 2, z: -D.LZ / 2 + C.MARGIN + C.POD_Z / 2 };
    if (p0 && p0.lead) eq(place(p0.lead), [pc.x - 4.4, C.FLOOR_PITCH, pc.z], 'first lead of wire 0 — floor 1, head of pod 0');
    if (p0 && p0.ics.length) eq(place(p0.ics[0]), [pc.x - 2, C.FLOOR_PITCH, pc.z - C.ROW_PITCH / 2], 'first IC of wire 0 — floor 1, pod 0, slot 1');
    var an = null; firm.seats.forEach(function (s) { if (!an && s.fn === 'alpha' && s.depth >= 4) an = s; });
    if (an) { var sl = slots[an.id]; eq(place(an), [D.ANNEX_X + (sl.slot % C.ANNEX_PER_ROW - 2.5) * C.DESK_PITCH, 0, -C.ANNEX_D / 2 + 2.2 + Math.floor(sl.slot / C.ANNEX_PER_ROW) * C.ROW_PITCH], 'an alpha IC — annex floor 0'); }
    return { ok: ok, vectors: out };
  }

  /* the dock is a bay on the right of the lobby; marks land on its apron */
  function dockSlot(i) { var col = i % 14, row = Math.floor(i / 14) % 4; return { x: C.dims.DOCK_X - 6.5 + col * 1.0, y: 0.55, z: -4 + row * 2.2 }; }
  var DOCK = { get x() { return C.dims ? C.dims.DOCK_X : 40; }, y: 0, z: 2 };
  var RACK = { get x() { return C.dims ? C.dims.LX * 0.22 : 12; }, y: -4.4, z: 3 };
  var SPOOL = { get x() { return C.dims ? -C.dims.LX * 0.26 : -14; }, y: -3.6, z: 2 };
  var BASE_Y = -7;

  /* ---------------- TAPE ---------------- */
  /* fixed key order, so the scan is a walk, not a parse. 1.2e6 rows decode in
     columns; the day index makes any fold a slice. */
  function columns(text) {
    var lines = text.split('\n');
    var n = 0, i;
    for (i = 0; i < lines.length; i++) if (lines[i].length > 2) n++;
    var col = {
      n: n,
      type: new Uint8Array(n), cls: new Int16Array(n), day: new Int16Array(n),
      oid: new Int32Array(n), seat: new Int16Array(n), a: new Int32Array(n),
      b: new Int32Array(n), margin: new Int32Array(n), value: new Int32Array(n),
      arm: new Int8Array(n), band: new Int8Array(n), flags: new Int8Array(n)
    };
    var k = 0, NUM = ['cls', 'day', 'oid', 'seat', 'a', 'b', 'margin', 'value', 'arm', 'band', 'flags'];
    for (i = 0; i < lines.length; i++) {
      var L = lines[i];
      if (L.length < 3) continue;
      var p = L.indexOf('"type":"');
      if (p < 0) { var o = JSON.parse(L); col.type[k] = T[o.type]; NUM.forEach(function (f) { col[f][k] = o[f]; }); k++; continue; }
      p += 8;
      var q = L.indexOf('"', p);
      col.type[k] = T[L.slice(p, q)];
      for (var f = 0; f < 11; f++) {
        p = L.indexOf(':', q) + 1;
        q = f === 10 ? L.indexOf('}', p) : L.indexOf(',', p);
        col[NUM[f]][k] = +L.slice(p, q);
      }
      k++;
    }
    var maxDay = 0;
    for (i = 0; i < n; i++) if (col.day[i] > maxDay) maxDay = col.day[i];
    var start = new Int32Array(maxDay + 2);
    for (i = n - 1; i >= 0; i--) start[col.day[i]] = i;
    for (var d = 1; d <= maxDay; d++) if (start[d] === 0) start[d] = start[d - 1];
    start[maxDay + 1] = n;
    var end = new Int32Array(maxDay + 1);
    for (d = 0; d <= maxDay; d++) end[d] = d === maxDay ? n : start[d + 1];
    col.dayStart = start; col.dayEnd = end; col.days = maxDay + 1;
    return col;
  }

  function blankState(firm) {
    var lic = [], adm = [], i;
    for (i = 0; i < firm.classes.length; i++) { lic.push([0, 0, 0, 0, 0, 0]); adm.push([0, 0, 0, 0, 0, 0]); }
    var counters = {}; C.COUNTER_KEYS.forEach(function (k) { counters[k] = 0; });
    return {
      day: -1, phase: C.PHASES[0],
      seats: { held: new Array(firm.n).fill(0), minutes: new Array(firm.n).fill(0) },
      open: [], ledger: [0, 0, 0, 0, 0, 0, 0, 0],
      machine: { license: lic, admitted: adm, counters: counters, kappa: 0, counter: 0 },
      cascade: [], residual: 0,
      tokens: new Map(), trayOf: new Map(), openSet: new Set(), arriveDay: [],
      marks: [], collapsed: [], systems: [], span: firm.span, rowsToday: 0, meetings: []
    };
  }

  /* one row, one state change. the visual layer listens to the same rows. */
  function applyRow(st, r) {
    var ty = r.type;
    st.machine.counters[C.TYPES[ty].toLowerCase()]++;
    if (r.oid >= 0 && ty !== T.HOLD && st.trayOf.has(r.oid)) {
      var was = st.trayOf.get(r.oid);
      st.seats.held[was]--; st.trayOf.delete(r.oid);
      var tk0 = st.tokens.get(r.oid); if (tk0) tk0.st = 'SEAT';
    }
    switch (ty) {
      case T.ARRIVE:
        st.openSet.add(r.oid); st.arriveDay[r.oid] = r.day;
        st.tokens.set(r.oid, { oid: r.oid, cls: r.cls, seat: -1, st: 'DOCK', value: r.value, band: r.band, arm: r.arm, margin: 0, day: r.day });
        break;
      case T.ASSIGN: {
        var tk = st.tokens.get(r.oid); if (tk) { tk.seat = r.seat; tk.st = 'SEAT'; }
        break;
      }
      case T.CONTEXT:
        st.seats.minutes[r.seat] += r.b; st.ledger[0] += r.b; break;
      case T.DECIDE: {
        st.seats.minutes[r.seat] += r.b; st.ledger[1] += r.b;
        var tk2 = st.tokens.get(r.oid); if (tk2) { tk2.margin = r.margin; tk2.seat = r.seat; tk2.st = 'SEAT'; }
        break;
      }
      case T.HOLD: {
        if (st.trayOf.has(r.oid)) st.seats.held[st.trayOf.get(r.oid)]--;
        st.trayOf.set(r.oid, r.seat); st.seats.held[r.seat]++;
        st.seats.minutes[r.seat] += r.b; st.ledger[2] += r.b;
        var tk3 = st.tokens.get(r.oid); if (tk3) { tk3.seat = r.seat; tk3.st = 'TRAY'; tk3.reason = r.a; }
        break;
      }
      case T.ESCALATE: {
        st.seats.minutes[r.seat] += r.b; st.ledger[3] += r.b;
        var tk4 = st.tokens.get(r.oid); if (tk4) { tk4.seat = r.a; tk4.st = 'SEAT'; tk4.cold = 1; }
        break;
      }
      case T.EFFECT: {
        var tk5 = st.tokens.get(r.oid); if (tk5) { tk5.st = 'GHOST'; }
        break;
      }
      case T.UNDO: {
        st.seats.minutes[r.seat] += r.b; st.ledger[5] += r.b;
        var tk6 = st.tokens.get(r.oid); if (tk6) { tk6.st = 'SEAT'; tk6.seat = r.seat; }
        break;
      }
      case T.MEETING:
        st.seats.minutes[r.seat] += r.value; st.ledger[4] += r.value;
        st.meetings.push({ seat: r.seat, other: r.a, n: r.b, minutes: r.value, cross: r.flags, day: r.day });
        if (st.meetings.length > 64) st.meetings.shift();
        break;
      case T.OUTCOME:
        st.ledger[6] += r.b; st.openSet.delete(r.oid);
        if (r.flags === 1) st.machine.counter++;
        st.tokens.delete(r.oid);
        st.marks.push({ oid: r.oid, grade: r.a, by_machine: r.flags, day: r.day, cls: r.cls });
        if (st.marks.length > 420) st.marks.shift();
        break;
      case T.LICENSE:
        st.machine.license[r.cls][r.band] = r.a;
        if (r.flags === 1) st.machine.admitted[r.cls][r.band] = 1;
        break;
      case T.KAPPA: st.machine.kappa = r.a; break;
      case T.PATCH:
        st.ledger[7] += r.a <= 1 ? 60 : 0;
        if (r.a === 0) st.span = r.b;
        else if (r.a === 1) st.systems.push(r.b);
        else if (r.a === 2) st.cascade.push('w' + Math.floor(r.b / 10) + 'f' + (r.b % 10));
        else if (r.a === 3) st.cascade.push('af' + r.b);
        else if (r.a === 4) st.collapsed.push(r.cls);
        break;
      case T.TICK: st.rowsToday = r.b; break;
    }
  }

  function rowAt(col, i) {
    return {
      type: col.type[i], cls: col.cls[i], day: col.day[i], oid: col.oid[i], seat: col.seat[i],
      a: col.a[i], b: col.b[i], margin: col.margin[i], value: col.value[i],
      arm: col.arm[i], band: col.band[i], flags: col.flags[i]
    };
  }

  function foldTo(firm, col, day, phases) {
    var st = blankState(firm);
    var end = col.dayEnd[Math.min(day, col.days - 1)];
    for (var i = 0; i < end; i++) applyRow(st, rowAt(col, i));
    finishDay(st, day, phases);
    return st;
  }

  function finishDay(st, day, phases) {
    st.day = day;
    st.open = Array.from(st.openSet).sort(function (a, b) { return a - b; });
    var res = 0;
    st.trayOf.forEach(function (seat, oid) { if (st.arriveDay[oid] <= day - 10) res++; });
    st.residual = res;
    if (phases) { var cur = phases[0].phase; for (var q = 0; q < phases.length; q++) if (day >= phases[q].day) cur = phases[q].phase; st.phase = cur; }
    return st;
  }

  /* ---------------- FOLD CHECK ---------------- */
  function diff(st, snap) {
    var bad = [], i, j;
    function cmp(label, a, b) { if (a !== b) bad.push(label + ' ' + a + ' != ' + b); }
    cmp('day', st.day, snap.day);
    cmp('phase', st.phase, snap.phase);
    cmp('residual', st.residual, snap.residual);
    cmp('kappa', st.machine.kappa, snap.machine.kappa);
    cmp('counter', st.machine.counter, snap.machine.counter);
    for (i = 0; i < 8; i++) cmp('ledger[' + C.LEDGER[i] + ']', st.ledger[i], snap.ledger[i]);
    C.COUNTER_KEYS.forEach(function (k) { cmp('counters.' + k, st.machine.counters[k], snap.machine.counters[k]); });
    cmp('open.length', st.open.length, snap.open.length);
    for (i = 0; i < Math.min(st.open.length, snap.open.length); i++) cmp('open[' + i + ']', st.open[i], snap.open[i]);
    for (i = 0; i < snap.seats.held.length; i++) {
      cmp('held[' + i + ']', st.seats.held[i], snap.seats.held[i]);
      cmp('minutes[' + i + ']', st.seats.minutes[i], snap.seats.minutes[i]);
    }
    for (i = 0; i < snap.machine.license.length; i++) for (j = 0; j < 6; j++) {
      cmp('license[' + i + '][' + j + ']', st.machine.license[i][j], snap.machine.license[i][j]);
      cmp('admitted[' + i + '][' + j + ']', st.machine.admitted[i][j], snap.machine.admitted[i][j]);
    }
    cmp('cascade.length', st.cascade.length, snap.cascade.length);
    for (i = 0; i < Math.min(st.cascade.length, snap.cascade.length); i++) cmp('cascade[' + i + ']', st.cascade[i], snap.cascade[i]);
    return bad;
  }

  window.ACME = window.ACME || {};
  window.ACME.contract = {
    C: C, T: T, prepare: prepare, place: place, checkPlace: checkPlace,
    columns: columns, rowAt: rowAt, blankState: blankState, applyRow: applyRow,
    foldTo: foldTo, finishDay: finishDay, diff: diff,
    DOCK: DOCK, RACK: RACK, SPOOL: SPOOL, BASE_Y: BASE_Y, dockSlot: dockSlot,
    slots: function () { return slots; }, pods: function () { return pods; }, frame: 'v1'
  };
})();
