/* ACME-500 world — the visual layer, frame v1: a headquarters, not a spindle.
   THREE is injected, never imported. Nothing here decides anything: every mesh
   moves because a row said so. Instanced geometry throughout; a day's deltas
   are animated, never a row at a time.

   What changed from v0, and only this: the static world and the map from a seat
   to a place. The tape, the fold, the rows-to-pictures rules, the director and
   the hooks are the same. A department is a floor; a team is a pod of eight
   desks with its lead at the head; a manager has a glass office on the window
   wall; a director has the corner. The executive floor is on top, the alpha
   functions are in a podium annex with a bridge, the lobby holds the counter
   and the dock, and the resident is in a section-cut basement. The front face
   is open, like a dollhouse, so every floor reads at once. */
(function () {
  var ACME = (window.ACME = window.ACME || {});

  ACME.WIRE_HEX = ['#6fa8ff', '#b79cff', '#5fe0b0', '#ffb35c', '#ff8fb0', '#d9e06a', '#8b97a8'];
  var GRADE_HEX = ['#5fe0b0', '#ffc061', '#ff5f6d'];

  ACME.createWorld = function (opt) {
    var THREE = opt.THREE, add = opt.addons, container = opt.container;
    var firm = opt.firm, K = opt.contract, C = K.C, D = C.dims;
    var W = {};
    var V3 = function (x, y, z) { return new THREE.Vector3(x, y, z); };
    var col = function (h) { return new THREE.Color(h); };
    var WIRE = ACME.WIRE_HEX.map(col);
    var GRADE = GRADE_HEX.map(col);
    var PITCH = C.FLOOR_PITCH, SLAB = C.SLAB, NF = C.FLOORS, H = D.H;
    var LX = D.LX, LZ = D.LZ;

    /* ---------- renderer ---------- */
    var renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: 'high-performance', preserveDrawingBuffer: true });
    renderer.setPixelRatio(Math.min(devicePixelRatio || 1, 1.75));
    renderer.setSize(container.clientWidth || 1280, container.clientHeight || 720, false);
    renderer.toneMapping = THREE.AgXToneMapping || THREE.ACESFilmicToneMapping;
    renderer.toneMappingExposure = 1.2;
    renderer.outputColorSpace = THREE.SRGBColorSpace;
    while (container.firstChild) container.removeChild(container.firstChild);
    container.appendChild(renderer.domElement);
    renderer.domElement.style.display = 'block';
    renderer.domElement.style.width = '100%';
    renderer.domElement.style.height = '100%';

    var scene = new THREE.Scene();
    scene.background = col('#070a10');
    scene.fog = new THREE.Fog(0x070a10, 420, 1100);

    var aspect = (container.clientWidth || 1280) / (container.clientHeight || 720);
    var camP = new THREE.PerspectiveCamera(42, aspect, 0.5, 2600);
    var ORTH = 62;
    var camO = new THREE.OrthographicCamera(-ORTH * aspect, ORTH * aspect, ORTH, -ORTH, -600, 900);
    var cam = camP;
    var controls = new add.OrbitControls(camP, renderer.domElement);
    controls.enableDamping = true; controls.dampingFactor = 0.07;
    controls.maxPolarAngle = Math.PI * 0.9; controls.minDistance = 12; controls.maxDistance = 520;

    var composer = new add.EffectComposer(renderer);
    var renderPass = new add.RenderPass(scene, cam);
    composer.addPass(renderPass);
    var bloom = new add.UnrealBloomPass(new THREE.Vector2(container.clientWidth || 1280, container.clientHeight || 720), 0.45, 0.7, 0.92);
    composer.addPass(bloom);
    composer.addPass(new add.OutputPass());

    /* ---------- light: cool outside, warm on every floor ---------- */
    scene.add(new THREE.HemisphereLight(0x33435e, 0x06080c, 0.9));
    var key = new THREE.DirectionalLight(0xa9c2ff, 1.05); key.position.set(-160, 220, -160); scene.add(key);
    var fill = new THREE.DirectionalLight(0x4b5f8f, 0.45); fill.position.set(180, 80, 120); scene.add(fill);
    for (var fl = 0; fl < NF; fl++) {
      var wl = new THREE.PointLight(0xffc98a, fl === 0 ? 26 : 34, 70, 2.0);
      wl.position.set(0, fl * PITCH + 3.2, -LZ / 4); wl.name = 'floor_light_' + fl; scene.add(wl);
    }
    var greenPit = new THREE.PointLight(0x4fe0a0, 120, 140, 2.0); greenPit.position.set(0, -3.5, -6); scene.add(greenPit);
    var greenPit2 = new THREE.PointLight(0x4fe0a0, 60, 90, 2.0); greenPit2.position.set(LX * 0.25, -3.5, 6); scene.add(greenPit2);
    var annexLight = new THREE.PointLight(0xffc98a, 30, 60, 2.0); annexLight.position.set(D.ANNEX_X, 5, -2); scene.add(annexLight);

    /* ---------- materials ---------- */
    function std(o) { return new THREE.MeshStandardMaterial(o); }
    var M = {
      plaza: std({ color: 0x0c1017, roughness: 1 }),
      slab: std({ color: 0x27303f, roughness: 0.85, metalness: 0.05 }),
      carpet: std({ color: 0x1b2230, roughness: 1 }),
      column: std({ color: 0x1a2129, roughness: 0.7, metalness: 0.15 }),
      core: std({ color: 0x141a23, roughness: 0.65, metalness: 0.2 }),
      curtain: std({ color: 0x8fc2ff, transparent: true, opacity: 0.08, roughness: 0.05, metalness: 0.1, side: THREE.DoubleSide, depthWrite: false }),
      office: std({ color: 0xa8dcff, transparent: true, opacity: 0.11, roughness: 0.05, side: THREE.DoubleSide, depthWrite: false }),
      strip: new THREE.MeshBasicMaterial({ color: 0xffe2bf }),
      desk: std({ color: 0xffffff, roughness: 0.6 }),
      lamp: new THREE.MeshBasicMaterial({ color: 0xffffff }),
      fig: std({ color: 0x9aa6b8, roughness: 0.7 }),
      tray: std({ color: 0x222a35, roughness: 0.8 }),
      token: new THREE.MeshBasicMaterial({ color: 0xffffff }),
      ghost: new THREE.MeshBasicMaterial({ color: 0x7fa8c8, transparent: true, opacity: 0.22 }),
      mark: new THREE.MeshBasicMaterial({ color: 0xffffff }),
      rim: new THREE.MeshBasicMaterial({ color: 0xffffff }),
      basefloor: std({ color: 0x061410, roughness: 0.9, emissive: 0x04170f, emissiveIntensity: 0.8 }),
      basewall: std({ color: 0x0a1512, roughness: 0.95 }),
      tile: new THREE.MeshBasicMaterial({ color: 0xffffff }),
      rack: std({ color: 0x1d2732, roughness: 0.6 }),
      spool: std({ color: 0x14241d, roughness: 0.5, emissive: 0x2f9c6a, emissiveIntensity: 0.9 }),
      dock: std({ color: 0x151b24, roughness: 0.9 }),
      roof: std({ color: 0x1c2330, roughness: 0.9 }),
      lift: new THREE.MeshBasicMaterial({ color: 0x9fd0ff })
    };
    var lineMat = function (hex, op) { return new THREE.LineBasicMaterial({ color: hex, transparent: true, opacity: op === undefined ? 0.6 : op }); };
    function edges(geo, hex, op) { return new THREE.LineSegments(new THREE.EdgesGeometry(geo, 30), lineMat(hex, op)); }

    /* ---------- groups (the layer switches) ---------- */
    var groups = {};
    function G(name) { var g = new THREE.Group(); g.name = name; scene.add(g); groups[name] = g; return g; }
    var gFloors = G('floors'), gCore = G('core'), gDesks = G('desks'), gFig = G('figures'),
      gTokens = G('tokens'), gTrays = G('trays'), gGlass = G('glass'), gDock = G('dock'),
      gBase = G('basement'), gLedger = G('ledger'), gBeams = G('beams'), gAnnex = G('annex'), gDirector = G('director');

    /* ---------- the plaza, with the pit left open under the tower ---------- */
    var PX = LX / 2 + 5, PZ = LZ / 2 + 5, R = 260;
    function strip(x, z, sx, sz, name) {
      var m = new THREE.Mesh(new THREE.BoxGeometry(sx, 0.6, sz), M.plaza); m.position.set(x, -0.3, z); m.name = name; scene.add(m); return m;
    }
    strip(0, -(PZ + (R - PZ) / 2), 2 * R, R - PZ, 'plaza_front');
    strip(0, (PZ + (R - PZ) / 2), 2 * R, R - PZ, 'plaza_back');
    strip(-(PX + (R - PX) / 2), 0, R - PX, 2 * PZ, 'plaza_left');
    strip((PX + (R - PX) / 2), 0, R - PX, 2 * PZ, 'plaza_right');
    var gridPts = [];
    for (var gi = -R; gi <= R; gi += 8) {
      gridPts.push(V3(gi, 0.02, -R), V3(gi, 0.02, R));
      gridPts.push(V3(-R, 0.02, gi), V3(R, 0.02, gi));
    }
    var plazaGrid = new THREE.LineSegments(new THREE.BufferGeometry().setFromPoints(gridPts), lineMat(0x2a3446, 0.1));
    plazaGrid.name = 'plaza_grid'; scene.add(plazaGrid);

    /* ---------- the tower ---------- */
    var slabs = [], slabEdges = [];
    for (var f = 0; f <= NF; f++) {
      var isRoof = f === NF;
      var sg = new THREE.BoxGeometry(LX, SLAB, LZ);
      var sm = new THREE.Mesh(sg, isRoof ? M.roof : M.slab.clone());
      sm.name = isRoof ? 'roof' : 'slab_f' + f; sm.position.set(0, f * PITCH - SLAB / 2, 0);
      gFloors.add(sm); slabs[f] = sm;
      var se = edges(sg, 0x8fb0d9, isRoof ? 0.35 : 0.28); se.position.copy(sm.position); se.name = 'slab_edge_f' + f; gFloors.add(se); slabEdges[f] = se;
    }
    /* parapet and penthouse */
    [[0, -LZ / 2 + 0.2, LX, 0.4], [0, LZ / 2 - 0.2, LX, 0.4], [-LX / 2 + 0.2, 0, 0.4, LZ], [LX / 2 - 0.2, 0, 0.4, LZ]].forEach(function (pp, k) {
      var parapet = new THREE.Mesh(new THREE.BoxGeometry(pp[2], 1.1, pp[3]), M.roof); parapet.position.set(pp[0], H + 0.55, pp[1]); parapet.name = 'parapet_' + k; gFloors.add(parapet);
    });
    var pent = new THREE.Mesh(new THREE.BoxGeometry(14, 3.2, 8), M.core); pent.position.set(LX / 4, H + 1.6, LZ / 4); pent.name = 'penthouse'; gFloors.add(pent);
    gFloors.add((function () { var e = edges(pent.geometry, 0x6f86a8, 0.3); e.position.copy(pent.position); return e; })());
    for (var hv = 0; hv < 4; hv++) {
      var unit = new THREE.Mesh(new THREE.BoxGeometry(3.2, 1.6, 2.4), M.column); unit.position.set(-LX / 4 - 6 + hv * 4.4, H + 0.8, LZ / 4 + 2); unit.name = 'roof_unit_' + hv; gFloors.add(unit);
    }

    /* columns: three rows, ten-metre bays, one instanced mesh */
    var colXs = []; for (var cx = -LX / 2 + 2; cx <= LX / 2 - 1.9; cx += 10) colXs.push(cx);
    var colZs = [-LZ / 2 + 1.2, 1.0, LZ / 2 - 1.2];
    var NCOL = colXs.length * colZs.length * NF;
    var columns = new THREE.InstancedMesh(new THREE.BoxGeometry(0.6, PITCH - SLAB, 0.6), M.column, NCOL);
    columns.name = 'columns'; columns.frustumCulled = false;
    (function () {
      var m4 = new THREE.Matrix4(), i = 0;
      for (var ff = 0; ff < NF; ff++) for (var a = 0; a < colXs.length; a++) for (var b = 0; b < colZs.length; b++) {
        m4.makeTranslation(colXs[a], ff * PITCH + (PITCH - SLAB) / 2, colZs[b]); columns.setMatrixAt(i++, m4);
      }
      columns.instanceMatrix.needsUpdate = true;
    })();
    gFloors.add(columns);

    /* curtain walls: back and both sides. the front is the cut. */
    function wall(w, h, x, y, z, ry, name) {
      var m = new THREE.Mesh(new THREE.PlaneGeometry(w, h), M.curtain); m.position.set(x, y, z); m.rotation.y = ry; m.name = name; gFloors.add(m);
      var pts = [];
      for (var u = -w / 2; u <= w / 2 + 0.01; u += 4) pts.push(V3(u, -h / 2, 0), V3(u, h / 2, 0));
      for (var v = 0; v <= h + 0.01; v += PITCH) pts.push(V3(-w / 2, v - h / 2, 0), V3(w / 2, v - h / 2, 0));
      var mul = new THREE.LineSegments(new THREE.BufferGeometry().setFromPoints(pts), lineMat(0x6f86a8, 0.16));
      mul.position.copy(m.position); mul.rotation.y = ry; mul.name = name + '_mullions'; gFloors.add(mul);
    }
    wall(LX, H, 0, H / 2, LZ / 2, 0, 'wall_back');
    wall(LZ, H, -LX / 2, H / 2, 0, Math.PI / 2, 'wall_left');
    wall(LZ, H, LX / 2, H / 2, 0, -Math.PI / 2, 'wall_right');

    /* the core: lifts and stairs, one shaft through every floor */
    var core = new THREE.Mesh(new THREE.BoxGeometry(C.CORE_W, H + 1.2, C.CORE_D), M.core);
    core.name = 'core'; core.position.set(0, (H + 1.2) / 2 - 0.4, D.CORE_Z); gCore.add(core);
    var coreEdge = edges(core.geometry, 0x6f86a8, 0.3); coreEdge.position.copy(core.position); gCore.add(coreEdge);
    var lifts = new THREE.InstancedMesh(new THREE.BoxGeometry(0.9, 2.2, 0.08), M.lift, NF * 2);
    lifts.name = 'lift_doors'; lifts.frustumCulled = false;
    (function () { var m4 = new THREE.Matrix4(), i = 0; for (var ff = 0; ff < NF; ff++) for (var k = -1; k <= 1; k += 2) { m4.makeTranslation(k * 1.4, ff * PITCH + 1.2, D.CORE_Z - C.CORE_D / 2 - 0.05); lifts.setMatrixAt(i++, m4); } lifts.instanceMatrix.needsUpdate = true; })();
    gCore.add(lifts);
    var liftLobby = V3(0, 0, D.CORE_Z - C.CORE_D / 2 - 2.6);   // where an escalation waits for the lift

    /* ---------- seats: pods, offices, the executive floor, the annex ---------- */
    var N = firm.n;
    var seatPos = new Array(N), seatYaw = new Float32Array(N), seatFrame = new Array(N), seatFloor = new Int8Array(N), seatWing = new Int8Array(N), seatPod = new Int16Array(N);
    firm.seats.forEach(function (s) {
      var p = K.place(s);
      seatPos[s.id] = V3(p.x, p.y, p.z); seatYaw[s.id] = p.yaw;
      seatFrame[s.id] = p.frame; seatFloor[s.id] = p.floor; seatWing[s.id] = p.wing === undefined ? -1 : p.wing; seatPod[s.id] = p.pod === undefined ? -1 : p.pod;
    });
    function inst(geo, mat, n, name, parent) {
      var m = new THREE.InstancedMesh(geo, mat, n);
      m.name = name; m.instanceMatrix.setUsage(THREE.DynamicDrawUsage);
      m.instanceColor = new THREE.InstancedBufferAttribute(new Float32Array(n * 3).fill(1), 3);
      m.instanceColor.setUsage(THREE.DynamicDrawUsage);
      m.count = n; m.frustumCulled = false;
      (parent || scene).add(m);
      return m;
    }
    var _m4 = new THREE.Matrix4(), _q = new THREE.Quaternion(), _e = new THREE.Euler(), _s = new THREE.Vector3(), _p = new THREE.Vector3();
    function setI(mesh, i, x, y, z, sx, sy, sz, yaw, tiltX) {
      _e.set(tiltX || 0, yaw || 0, 0); _q.setFromEuler(_e);
      _p.set(x, y, z); _s.set(sx, sy === undefined ? sx : sy, sz === undefined ? sx : sz);
      _m4.compose(_p, _q, _s); mesh.setMatrixAt(i, _m4); mesh.instanceMatrix.needsUpdate = true;
    }
    function setC(mesh, i, c, mul) {
      var r = c.r, g = c.g, b = c.b, k = mul === undefined ? 1 : mul;
      mesh.instanceColor.array[i * 3] = r * k;
      mesh.instanceColor.array[i * 3 + 1] = g * k;
      mesh.instanceColor.array[i * 3 + 2] = b * k;
      mesh.instanceColor.needsUpdate = true;
    }

    /* pod plates (carpet) and ceiling strips, per wire per pod */
    var podsOf = K.pods();
    var podPlate = [[], [], [], [], [], []], stripIdx = [[], [], [], [], [], []];
    var totalPods = 0; for (var w0 = 0; w0 < 6; w0++) totalPods += podsOf[w0].length;
    var strips = inst(new THREE.BoxGeometry(C.POD_X - 3, 0.07, 0.3), M.strip, Math.max(1, totalPods * 2), 'ceiling_strips', gFloors);
    var si = 0;
    for (var w1 = 0; w1 < 6; w1++) {
      var fy = (w1 + 1) * PITCH;
      podsOf[w1].forEach(function (p, pi) {
        var colI = pi % D.PODS_X, rowI = Math.floor(pi / D.PODS_X) % D.PODS_Z;
        var px = -LX / 2 + C.MARGIN + colI * C.POD_X + C.POD_X / 2, pz = -LZ / 2 + C.MARGIN + rowI * C.POD_Z + C.POD_Z / 2;
        var plate = new THREE.Mesh(new THREE.BoxGeometry(C.POD_X - 1.2, 0.06, C.POD_Z - 1.0), M.carpet.clone());
        plate.name = 'pod_w' + w1 + '_p' + pi; plate.position.set(px, fy + 0.03, pz); gFloors.add(plate);
        podPlate[w1][pi] = plate;
        stripIdx[w1][pi] = [si, si + 1];
        setI(strips, si++, px, fy + PITCH - SLAB - 0.35, pz - 1.3, 1); setI(strips, si++, px, fy + PITCH - SLAB - 0.35, pz + 1.3, 1);
        setC(strips, si - 1, col('#ffe2bf'), 1.6); setC(strips, si - 2, col('#ffe2bf'), 1.6);
      });
    }
    strips.count = si;

    /* offices: a glass box around every office seat; corner and chief offices larger */
    var officeOf = {};
    firm.seats.forEach(function (s) {
      var fr = seatFrame[s.id];
      if (fr !== 'office' && fr !== 'corner' && fr !== 'chief' && fr !== 'execoffice') return;
      var p = seatPos[s.id];
      var big = fr === 'corner' || fr === 'chief';
      var gw = big ? C.OFFICE_W * 1.6 : C.OFFICE_W - 0.3, gd = big ? C.OFFICE_D + 1.2 : C.OFFICE_D - 0.2;
      var side = Math.abs(Math.abs(seatYaw[s.id]) - Math.PI / 2) < 0.01;
      var box = new THREE.Mesh(new THREE.BoxGeometry(side ? gd : gw, 3.1, side ? gw : gd), M.office.clone());
      box.name = 'office_s' + s.id; box.position.set(p.x, p.y + 1.6, p.z); gGlass.add(box);
      var oe = edges(box.geometry, fr === 'chief' ? 0xd7c8ff : 0xa9d8ff, fr === 'chief' ? 0.5 : 0.3); oe.position.copy(box.position); oe.name = 'office_edge_s' + s.id; gGlass.add(oe);
      officeOf[s.id] = box;
    });

    /* meeting rooms: one glass room per department floor, the boardroom on top */
    var rooms = {};
    var ROOM_X = LX / 2 - C.ROOM_ZONE / 2 - 0.4;
    for (f = 1; f <= 6; f++) {
      var room = new THREE.Mesh(new THREE.BoxGeometry(C.ROOM_ZONE - 1.2, 3.1, 7), M.office.clone());
      room.name = 'glass_room_f' + f; room.position.set(ROOM_X, f * PITCH + 1.6, -LZ / 2 + 7.5); gGlass.add(room);
      var re = edges(room.geometry, 0xa9d8ff, 0.32); re.position.copy(room.position); re.name = 'glass_edge_f' + f; gGlass.add(re);
      room.edge = re; rooms[f] = room;
    }
    var board = new THREE.Mesh(new THREE.BoxGeometry(14, 3.1, 7), M.office.clone());
    board.name = 'glass_room_f7'; board.position.set(0, C.EXEC * PITCH + 1.6, -LZ / 2 + 7.5); gGlass.add(board);
    var be = edges(board.geometry, 0xd7c8ff, 0.4); be.position.copy(board.position); gGlass.add(be); rooms[C.EXEC] = board;
    var table = new THREE.Mesh(new THREE.BoxGeometry(9, 0.12, 2.2), M.desk); table.position.set(0, C.EXEC * PITCH + 0.8, -LZ / 2 + 7.5); table.name = 'board_table'; gGlass.add(table);
    function glassRoom(floor) { return rooms[floor] || rooms[1]; }

    /* the annex: three floors, a bridge, its own glass room */
    var annexSlab = [], annexRoom = null;
    for (f = 0; f <= C.ANNEX_FLOORS; f++) {
      var ag = new THREE.BoxGeometry(C.ANNEX_W, SLAB, C.ANNEX_D);
      var aslab = new THREE.Mesh(ag, (f === C.ANNEX_FLOORS ? M.roof : M.slab.clone()));
      aslab.name = f === C.ANNEX_FLOORS ? 'annex_roof' : 'annex_f' + f; aslab.position.set(D.ANNEX_X, f * PITCH - SLAB / 2, 0); gAnnex.add(aslab);
      var aedge = edges(ag, 0x9a8fd0, 0.3); aedge.position.copy(aslab.position); aedge.name = 'annex_edge_f' + f; gAnnex.add(aedge);
      aslab.edge = aedge; annexSlab[f] = aslab;
    }
    (function () {
      var ah = C.ANNEX_FLOORS * PITCH;
      function awall(w, h, x, y, z, ry) { var m = new THREE.Mesh(new THREE.PlaneGeometry(w, h), M.curtain); m.position.set(x, y, z); m.rotation.y = ry; gAnnex.add(m); }
      awall(C.ANNEX_W, ah, D.ANNEX_X, ah / 2, C.ANNEX_D / 2, 0);
      awall(C.ANNEX_D, ah, D.ANNEX_X - C.ANNEX_W / 2, ah / 2, 0, Math.PI / 2);
      var bx0 = D.ANNEX_X + C.ANNEX_W / 2, bx1 = -LX / 2;
      var bridge = new THREE.Mesh(new THREE.BoxGeometry(bx1 - bx0 + 1.2, 3.0, 3.6), M.office.clone());
      bridge.name = 'bridge'; bridge.position.set((bx0 + bx1) / 2, PITCH + 1.5, -3); gAnnex.add(bridge);
      var bedge = edges(bridge.geometry, 0x9a8fd0, 0.4); bedge.position.copy(bridge.position); gAnnex.add(bedge);
      var bfloor = new THREE.Mesh(new THREE.BoxGeometry(bx1 - bx0 + 1.2, 0.25, 3.6), M.slab); bfloor.position.set((bx0 + bx1) / 2, PITCH - 0.12, -3); gAnnex.add(bfloor);
      annexRoom = new THREE.Mesh(new THREE.BoxGeometry(6, 3.1, 5), M.office.clone());
      annexRoom.name = 'annex_room'; annexRoom.position.set(D.ANNEX_X - C.ANNEX_W / 2 + 3.6, PITCH + 1.6, C.ANNEX_D / 2 - 3); gAnnex.add(annexRoom);
      var are = edges(annexRoom.geometry, 0xa9d8ff, 0.3); are.position.copy(annexRoom.position); gAnnex.add(are);
    })();

    /* the lobby: reception, turnstiles, the counter on the core */
    var reception = new THREE.Mesh(new THREE.BoxGeometry(7, 1.1, 1.3), M.desk); reception.position.set(0, 0.55, -LZ / 2 + 6.5); reception.name = 'reception'; gCore.add(reception);
    for (var tI = -2; tI <= 2; tI++) { var post = new THREE.Mesh(new THREE.BoxGeometry(0.25, 1.0, 0.9), M.column); post.position.set(tI * 1.4, 0.5, -LZ / 2 + 10.5); gCore.add(post); }

    /* desks, lamps, figures, trays: one instanced mesh each */
    var desks = inst(new THREE.BoxGeometry(1.5, 0.1, 0.9), M.desk, N, 'desks', gDesks);
    var legs = inst(new THREE.BoxGeometry(1.4, 0.7, 0.08), M.column, N, 'desk_legs', gDesks);
    var lamps = inst(new THREE.SphereGeometry(0.2, 7, 6), M.lamp, N, 'desk_lamps', gDesks);
    var figs = inst(new THREE.CapsuleGeometry(0.21, 0.55, 3, 7), M.fig, N, 'figures', gFig);
    var trays = inst(new THREE.BoxGeometry(0.62, 0.07, 0.46), M.tray, N, 'trays', gTrays);
    var deskPos = new Array(N), trayPos = new Array(N), figHome = new Array(N);
    firm.seats.forEach(function (s) {
      var p = seatPos[s.id], yaw = seatYaw[s.id];
      var fx = Math.cos(yaw), fz = Math.sin(yaw);
      deskPos[s.id] = V3(p.x, p.y + 0.75, p.z);
      trayPos[s.id] = V3(p.x - fz * 0.62, p.y + 0.84, p.z + fx * 0.62);
      figHome[s.id] = V3(p.x - fx * 0.85, p.y + 0.42, p.z - fz * 0.85);
      setI(desks, s.id, p.x, p.y + 0.7, p.z, 1, 1, 1, -yaw);
      setI(legs, s.id, p.x, p.y + 0.35, p.z, 1, 1, 1, -yaw);
      setI(lamps, s.id, p.x + fz * 0.5, p.y + 1.0, p.z - fx * 0.5, 1);
      setI(trays, s.id, trayPos[s.id].x, trayPos[s.id].y, trayPos[s.id].z, 1, 1, 1, -yaw);
      setI(figs, s.id, figHome[s.id].x, figHome[s.id].y, figHome[s.id].z, 1);
      var wc = WIRE[s.wire] || WIRE[6];
      setC(desks, s.id, col(s.fn === 'alpha' ? '#66788a' : s.fn === 'warrant' ? '#7d6fa8' : s.kind === 'lead' ? '#8a97ab' : '#6e7c90'));
      setC(legs, s.id, col('#1f2731'));
      setC(lamps, s.id, col('#ffb35c'), 1.9);
      setC(figs, s.id, wc, 0.9);
      setC(trays, s.id, col('#222a35'));
    });

    /* ---------- the dock: a bay on the right of the lobby, the apron outside ---------- */
    var DX = D.DOCK_X;
    var bay = new THREE.Mesh(new THREE.BoxGeometry(18, 0.5, 14), M.dock); bay.name = 'dock'; bay.position.set(DX, 0.25, 2); gDock.add(bay);
    var bayEdge = edges(bay.geometry, 0x9fb4d0, 0.3); bayEdge.position.copy(bay.position); gDock.add(bayEdge);
    var canopy = new THREE.Mesh(new THREE.BoxGeometry(18, 0.3, 14), M.roof); canopy.position.set(DX, 5.2, 2); canopy.name = 'dock_canopy'; gDock.add(canopy);
    for (var pI = 0; pI < 4; pI++) { var cp = new THREE.Mesh(new THREE.BoxGeometry(0.4, 5, 0.4), M.column); cp.position.set(DX - 8.5 + (pI % 2) * 17, 2.6, -4.5 + Math.floor(pI / 2) * 13); gDock.add(cp); }
    var apron = new THREE.Mesh(new THREE.BoxGeometry(18, 0.3, 11), std({ color: 0x0e131a, roughness: 1 })); apron.name = 'dock_apron'; apron.position.set(DX, 0.15, -10.5); gDock.add(apron);
    var chute = new THREE.Mesh(new THREE.BoxGeometry(14, 0.4, 3.2), std({ color: 0x18202b, roughness: 0.7 })); chute.name = 'chute'; chute.position.set(DX + 15, 1.1, 4); chute.rotation.z = 0.18; gDock.add(chute);
    var marks = inst(new THREE.CylinderGeometry(0.34, 0.34, 0.1, 8), M.mark, 512, 'outcome_marks', gDock);
    var rims = inst(new THREE.TorusGeometry(0.46, 0.05, 4, 14), M.rim, 512, 'outcome_rims', gDock);
    var ghosts = inst(new THREE.CylinderGeometry(0.34, 0.34, 0.16, 10), M.ghost, 256, 'ghosts', gDock);
    for (var i = 0; i < 512; i++) { setI(marks, i, 0, -999, 0, 0.001); setI(rims, i, 0, -999, 0, 0.001); }
    for (i = 0; i < 256; i++) setI(ghosts, i, 0, -999, 0, 0.001);
    function markSlot(i) { var c = i % 32, r = Math.floor(i / 32) % 8; return V3(DX - 8.3 + c * 0.54, 0.36, -15 + r * 1.15); }
    function ghostSlot(i) { var c = i % 16, r = Math.floor(i / 16) % 4; return V3(DX - 7.2 + c * 0.96, 0.6, 10 + r * 1.0); }

    /* tokens */
    var TOKN = 4096;
    var tokens = inst(new THREE.CylinderGeometry(0.34, 0.34, 0.17, 10), M.token, TOKN, 'tokens', gTokens);
    for (i = 0; i < TOKN; i++) setI(tokens, i, 0, -999, 0, 0.001);

    /* beams */
    var BEAMN = 128;
    var beamGeo = new THREE.BufferGeometry();
    beamGeo.setAttribute('position', new THREE.BufferAttribute(new Float32Array(BEAMN * 6), 3));
    beamGeo.setAttribute('color', new THREE.BufferAttribute(new Float32Array(BEAMN * 6).fill(1), 3));
    var beams = new THREE.LineSegments(beamGeo, new THREE.LineBasicMaterial({ vertexColors: true, transparent: true, opacity: 0.75, blending: THREE.AdditiveBlending }));
    beams.name = 'beams'; beams.frustumCulled = false; gBeams.add(beams);

    /* ---------- basement: the resident, in section ---------- */
    var BY = K.BASE_Y;
    var bfloor = new THREE.Mesh(new THREE.BoxGeometry(LX + 10, 0.4, LZ + 10), M.basefloor);
    bfloor.name = 'basement_floor'; bfloor.position.y = BY - 0.2; gBase.add(bfloor);
    var bwallB = new THREE.Mesh(new THREE.BoxGeometry(LX + 10, -BY, 0.4), M.basewall); bwallB.position.set(0, BY / 2, LZ / 2 + 4.8); bwallB.name = 'basement_wall_back'; gBase.add(bwallB);
    var bwallL = new THREE.Mesh(new THREE.BoxGeometry(0.4, -BY, LZ + 10), M.basewall); bwallL.position.set(-LX / 2 - 4.8, BY / 2, 0); gBase.add(bwallL);
    var bwallR = new THREE.Mesh(new THREE.BoxGeometry(0.4, -BY, LZ + 10), M.basewall); bwallR.position.set(LX / 2 + 4.8, BY / 2, 0); gBase.add(bwallR);
    var bgPts = [];
    for (gi = -Math.floor(LX / 2 / 4) * 4; gi <= LX / 2; gi += 4) bgPts.push(V3(gi, BY + 0.02, -LZ / 2 - 4), V3(gi, BY + 0.02, LZ / 2 + 4));
    for (gi = -Math.floor(LZ / 2 / 4) * 4; gi <= LZ / 2; gi += 4) bgPts.push(V3(-LX / 2 - 4, BY + 0.02, gi), V3(LX / 2 + 4, BY + 0.02, gi));
    var fieldGrid = new THREE.LineSegments(new THREE.BufferGeometry().setFromPoints(bgPts), lineMat(0x3fbb8a, 0.22));
    fieldGrid.name = 'field_grid'; gBase.add(fieldGrid);

    var spool = new THREE.Mesh(new THREE.TorusGeometry(3.2, 0.9, 10, 30), M.spool);
    spool.name = 'tape_spool'; spool.position.set(K.SPOOL.x, K.SPOOL.y, K.SPOOL.z); spool.rotation.x = Math.PI / 2; gBase.add(spool);
    var spoolHub = new THREE.Mesh(new THREE.CylinderGeometry(0.5, 0.5, 2.6, 10), std({ color: 0x0f1a16, emissive: 0x1f7a56, emissiveIntensity: 0.7 }));
    spoolHub.name = 'spool_hub'; spoolHub.position.copy(spool.position); gBase.add(spoolHub);

    var SYS = 96, sysPos = new Array(SYS);
    var systems = inst(new THREE.BoxGeometry(1.0, 2.0, 0.7), M.rack, SYS, 'systems', gBase);
    for (i = 0; i < SYS; i++) {
      var sc = i % 12, sr = Math.floor(i / 12);
      sysPos[i] = V3(K.RACK.x + (sc - 5.5) * 1.3, BY + 1.1, K.RACK.z + (sr - 3.5) * 1.4);
      setI(systems, i, sysPos[i].x, sysPos[i].y, sysPos[i].z, 1);
      setC(systems, i, col('#27323f'));
    }
    var rackFrame = edges(new THREE.BoxGeometry(16.5, 3.4, 12), 0x4fd2a0, 0.3);
    rackFrame.name = 'rack_frame'; rackFrame.position.set(K.RACK.x, BY + 1.4, K.RACK.z); gBase.add(rackFrame);

    var NCLS = firm.classes.length, TILES = NCLS * 6;
    var tiles = inst(new THREE.BoxGeometry(1.1, 0.16, 1.1), M.tile, TILES, 'license_tiles', gBase);
    var glyphs = inst(new THREE.BoxGeometry(0.28, 0.28, 0.28), std({ color: 0x111111, emissive: 0xb79cff, emissiveIntensity: 2.0 }), TILES, 'admitted_glyphs', gBase);
    var tilePos = new Array(TILES);
    for (var c2 = 0; c2 < NCLS; c2++) for (var b2 = 0; b2 < 6; b2++) {
      var ti = c2 * 6 + b2;
      tilePos[ti] = V3(-(NCLS - 1) * 0.65 + c2 * 1.3, BY + 0.08, -LZ / 2 + 2 + b2 * 1.3);
      setI(tiles, ti, tilePos[ti].x, tilePos[ti].y, tilePos[ti].z, 1);
      setC(tiles, ti, col('#1b2430'));
      setI(glyphs, ti, tilePos[ti].x, BY - 99, tilePos[ti].z, 0.001);
    }

    /* kappa gauge */
    var gaugeG = new THREE.Group(); gaugeG.name = 'kappa_gauge';
    gaugeG.position.set(-LX / 2 + 6, BY + 4.2, LZ / 2 + 3.8); gBase.add(gaugeG);
    var arc = new THREE.Mesh(new THREE.TorusGeometry(3.6, 0.14, 6, 48, Math.PI * 1.2), std({ color: 0x0e1a16, emissive: 0x2f9c6a, emissiveIntensity: 0.8 }));
    arc.name = 'kappa_arc'; arc.rotation.z = -Math.PI * 0.1; gaugeG.add(arc);
    var needle = new THREE.Mesh(new THREE.BoxGeometry(3.4, 0.2, 0.2), std({ color: 0x150e1e, emissive: 0xb79cff, emissiveIntensity: 2.4 }));
    needle.name = 'kappa_needle'; needle.geometry.translate(1.7, 0, 0); gaugeG.add(needle);

    /* canvas textures: the counter on the lobby wall, the ledger labels, the roof sign */
    function canvasTex(w, h) {
      var cv = document.createElement('canvas'); cv.width = w; cv.height = h;
      var tex = new THREE.CanvasTexture(cv); tex.colorSpace = THREE.SRGBColorSpace;
      tex.anisotropy = 4;
      return { cv: cv, ctx: cv.getContext('2d'), tex: tex };
    }
    var MONO = '"IBM Plex Mono", ui-monospace, monospace';
    var counterTex = canvasTex(1024, 256);
    var counterWall = new THREE.Mesh(new THREE.PlaneGeometry(12, 3), new THREE.MeshBasicMaterial({ map: counterTex.tex, transparent: true }));
    counterWall.name = 'counter_wall'; counterWall.position.set(0, 2.6, D.CORE_Z - C.CORE_D / 2 - 0.12); counterWall.rotation.y = Math.PI; gCore.add(counterWall);
    function drawCounter(n, kappa, phase) {
      var x = counterTex.ctx; x.clearRect(0, 0, 1024, 256);
      x.fillStyle = 'rgba(6,18,13,0.92)'; x.fillRect(0, 0, 1024, 256);
      x.strokeStyle = 'rgba(95,224,176,0.5)'; x.lineWidth = 4; x.strokeRect(6, 6, 1012, 244);
      x.font = '600 46px ' + MONO; x.fillStyle = '#5fe0b0'; x.textBaseline = 'top';
      x.fillText('MACHINE COUNTER', 34, 26);
      x.font = '700 132px ' + MONO; x.fillStyle = '#9dffd6';
      x.fillText(String(n).padStart(7, '0'), 30, 86);
      x.font = '500 38px ' + MONO; x.fillStyle = '#b79cff';
      x.fillText('K ' + kappa, 700, 34);
      x.fillStyle = 'rgba(157,255,214,0.65)'; x.fillText(phase || '', 700, 92);
      counterTex.tex.needsUpdate = true;
    }
    drawCounter(0, 0, 'INSTRUMENT');

    var signTex = canvasTex(1024, 192);
    (function () {
      var x = signTex.ctx; x.clearRect(0, 0, 1024, 192);
      x.font = '700 150px ' + MONO; x.fillStyle = '#e8eaf0'; x.textBaseline = 'top'; x.textAlign = 'center';
      x.fillText('ACME-500', 512, 14);
    })();
    var sign = new THREE.Mesh(new THREE.PlaneGeometry(18, 3.4), new THREE.MeshBasicMaterial({ map: signTex.tex, transparent: true, side: THREE.DoubleSide }));
    sign.name = 'roof_sign'; sign.position.set(0, H + 3.1, -LZ / 2 - 0.1); sign.rotation.y = Math.PI; gFloors.add(sign);

    /* ledger: eight bars on the plaza, front-left */
    var LED = 8, ledgerPos = new Array(LED);
    var ledgerBars = inst(new THREE.BoxGeometry(2.8, 1, 2.8), std({ color: 0x1a2230, roughness: 0.5, emissive: 0x24184a, emissiveIntensity: 0.6 }), LED, 'ledger_bars', gLedger);
    for (i = 0; i < LED; i++) {
      ledgerPos[i] = V3(-LX / 2 - 6 - i * 4.4, 0, -LZ / 2 - 16);
      setI(ledgerBars, i, ledgerPos[i].x, 0.5, ledgerPos[i].z, 1, 1, 1);
      setC(ledgerBars, i, col('#b79cff'), 0.5);
    }
    var ledgerTex = canvasTex(2048, 384);
    var ledgerPlate = new THREE.Mesh(new THREE.PlaneGeometry(40, 7.5), new THREE.MeshBasicMaterial({ map: ledgerTex.tex, transparent: true }));
    ledgerPlate.name = 'ledger_labels';
    ledgerPlate.position.set(-LX / 2 - 6 - 3.5 * 4.4, 0.4, -LZ / 2 - 24); ledgerPlate.rotation.x = -Math.PI / 2 + 0.55; gLedger.add(ledgerPlate);
    function drawLedger(minutes) {
      var x = ledgerTex.ctx; x.clearRect(0, 0, 2048, 384);
      x.fillStyle = 'rgba(7,10,16,0.88)'; x.fillRect(0, 0, 2048, 384);
      x.strokeStyle = 'rgba(183,156,255,0.35)'; x.lineWidth = 3; x.strokeRect(4, 4, 2040, 376);
      x.font = '600 40px ' + MONO; x.fillStyle = 'rgba(232,234,240,0.55)'; x.textBaseline = 'top'; x.textAlign = 'left';
      x.fillText('LEDGER — MINUTES', 22, 16);
      for (var k = 0; k < 8; k++) {
        var cx = 30 + (7 - k) * 251;
        x.font = '600 38px ' + MONO; x.fillStyle = 'rgba(232,234,240,0.72)';
        x.fillText(C.LEDGER[k], cx, 96);
        x.font = '700 62px ' + MONO; x.fillStyle = '#b79cff';
        var v = minutes[k] | 0, s = v >= 1000 ? (v / 1000).toFixed(1) + 'k' : String(v);
        x.fillText(s, cx, 152);
      }
      ledgerTex.tex.needsUpdate = true;
    }
    drawLedger([0, 0, 0, 0, 0, 0, 0, 0]);

    /* ---------- director props ---------- */
    var fiber = new THREE.Line(new THREE.BufferGeometry().setFromPoints([V3(K.RACK.x - 8, BY + 1.2, K.RACK.z), V3(K.SPOOL.x + 3.4, BY + 1.2, K.SPOOL.z)]), lineMat(0x5fe0b0, 0.9));
    fiber.name = 'fiber'; fiber.visible = false; gDirector.add(fiber);
    var joinPts = [];
    for (i = 0; i < NCLS; i++) {
      var ang1 = (i / NCLS) * Math.PI * 2;
      for (var j = 1; j <= 2; j++) {
        var ang2 = ((i + j * 5) / NCLS) * Math.PI * 2;
        joinPts.push(V3(K.RACK.x + Math.cos(ang1) * 8, BY + 5.2 + (i % 3) * 0.6, K.RACK.z + Math.sin(ang1) * 6));
        joinPts.push(V3(K.RACK.x + Math.cos(ang2) * 8, BY + 5.2 + (j % 3) * 0.6, K.RACK.z + Math.sin(ang2) * 6));
      }
    }
    var joinGraph = new THREE.LineSegments(new THREE.BufferGeometry().setFromPoints(joinPts), lineMat(0x6fe8b8, 0.6));
    joinGraph.name = 'join_graph'; joinGraph.visible = false; joinGraph.geometry.setDrawRange(0, 0); gDirector.add(joinGraph);

    var ghostTower = new THREE.Group(); ghostTower.name = 'ghost_tower'; ghostTower.visible = false; gDirector.add(ghostTower);
    for (f = 0; f < NF; f++) {
      var gEdge = new THREE.LineSegments(new THREE.EdgesGeometry(new THREE.BoxGeometry(LX + 1.2, PITCH - 0.2, LZ + 1.2)), new THREE.LineBasicMaterial({ color: 0x8fd8ff, transparent: true, opacity: 0.3, blending: THREE.AdditiveBlending }));
      gEdge.position.y = f * PITCH + PITCH / 2; gEdge.name = 'ghost_floor_f' + f; ghostTower.add(gEdge);
    }
    var scanPlane = new THREE.Mesh(new THREE.PlaneGeometry(LX + 6, LZ + 6), new THREE.MeshBasicMaterial({ color: 0x9fe0ff, transparent: true, opacity: 0.12, blending: THREE.AdditiveBlending, side: THREE.DoubleSide }));
    scanPlane.rotation.x = -Math.PI / 2; scanPlane.name = 'replay_scan'; ghostTower.add(scanPlane);

    var SPK = 400;
    var spkGeo = new THREE.BufferGeometry();
    spkGeo.setAttribute('position', new THREE.BufferAttribute(new Float32Array(SPK * 3), 3));
    var sparks = new THREE.Points(spkGeo, new THREE.PointsMaterial({ color: 0x9dffd6, size: 0.5, transparent: true, opacity: 0.9, blending: THREE.AdditiveBlending, sizeAttenuation: true }));
    sparks.name = 'canary_sparks'; sparks.visible = false; sparks.frustumCulled = false; gDirector.add(sparks);

    var routine = new THREE.Mesh(new THREE.BoxGeometry(4, 2.0, 4), std({ color: 0x101a22, emissive: 0x3fbb8a, emissiveIntensity: 1.2, roughness: 0.4 }));
    routine.name = 'routine_block'; routine.position.set(K.RACK.x, BY + 1.0, K.RACK.z - 10); routine.visible = false; routine.scale.setScalar(0.01); gDirector.add(routine);

    /* ---------- token bookkeeping ---------- */
    var tokOf = new Map(), free = [], trayCount = new Int16Array(N), dockN = 0, ghostN = 0, markN = 0;
    for (i = TOKN - 1; i >= 0; i--) free.push(i);
    var tracks = [], now = 0, fast = false;
    var tokState = new Map();

    function tokScale(value, decided) {
      var s = 0.62 + Math.min(1, value / 110000) * 0.95;
      return decided ? [s * 1.26, s * 0.42, s * 1.26] : [s, s, s];
    }
    function restPos(tk) {
      if (tk.st === 'DOCK') { var d = K.dockSlot(tk.slot); return V3(d.x, d.y, d.z); }
      if (tk.st === 'TRAY') { var tp = trayPos[tk.seat]; return V3(tp.x, tp.y + 0.09 + (tk.stack || 0) * 0.1, tp.z); }
      if (tk.st === 'GHOST') return ghostSlot(tk.ghost);
      var dp = deskPos[tk.seat] || V3(0, 0, 0);
      var yaw = seatYaw[tk.seat] || 0;
      return V3(dp.x + Math.cos(yaw) * 0.32, dp.y + 0.14, dp.z + Math.sin(yaw) * 0.32);
    }
    function drawTok(tk) {
      var p = restPos(tk), s = tokScale(tk.value, tk.decided);
      var tilt = tk.decided ? Math.max(-1, Math.min(1, tk.margin / 260)) * 0.85 : 0;
      setI(tokens, tk.i, p.x, p.y, p.z, s[0], s[1], s[2], 0, tilt);
      var wire = firm.classes[tk.cls] ? firm.classes[tk.cls].wire : 6;
      setC(tokens, tk.i, WIRE[wire], tk.st === 'TRAY' ? 0.6 : 1.7);
    }
    function alloc(oid, cls, value) {
      var i = free.pop();
      if (i === undefined) return null;
      var tk = { oid: oid, i: i, cls: cls, value: value, st: 'DOCK', seat: -1, margin: 0, decided: false, slot: dockN++ % 56, stack: 0, ghost: 0 };
      tokState.set(oid, tk); return tk;
    }
    function release(oid) {
      var tk = tokState.get(oid); if (!tk) return;
      setI(tokens, tk.i, 0, -999, 0, 0.001);
      free.push(tk.i); tokState.delete(oid);
    }

    /* ---------- tracks ---------- */
    function flight(tk, to, dur, arc, after) {
      var from = restPos(tk);
      tracks.push({ kind: 'fly', tk: tk, from: from, to: to.clone(), t0: now, t1: now + dur, arc: arc || 3, after: after });
    }
    function addBeam(from, to, dur, c) {
      tracks.push({ kind: 'beam', from: from, to: to, t0: now, t1: now + dur, c: c || col('#5fe0b0') });
    }
    function flash(seat, dur, c) {
      tracks.push({ kind: 'flash', seat: seat, t0: now, t1: now + (dur || 0.5), c: c || col('#ffe9c2') });
    }
    function walk(seat, to, dur, hold) {
      tracks.push({ kind: 'walk', seat: seat, from: figHome[seat].clone(), to: to, t0: now, t1: now + dur, hold: hold || 1.2 });
    }
    /* an escalation crosses the floor to the office; it only leaves the floor by the lift */
    function escalatePath(fromSeat, toSeat) {
      var a = restPos({ st: 'SEAT', seat: fromSeat }), b = restPos({ st: 'SEAT', seat: toSeat });
      if (seatFloor[fromSeat] === seatFloor[toSeat]) return { from: a, mid: V3((a.x + b.x) / 2, a.y + 1.8, (a.z + b.z) / 2), to: b };
      return { from: a, mid: V3(liftLobby.x, (a.y + b.y) / 2 + 0.8, liftLobby.z), to: b };
    }

    /* ---------- rows -> pictures ---------- */
    var T = K.T;
    function onRow(r, tOff) {
      switch (r.type) {
        case T.ARRIVE: {
          var tk = alloc(r.oid, r.cls, r.value);
          if (tk) { drawTok(tk); }
          break;
        }
        case T.ASSIGN: {
          var tk2 = tokState.get(r.oid); if (!tk2) break;
          tk2.st = 'SEAT'; tk2.seat = r.seat;
          if (fast) drawTok(tk2);
          else { var to = restPos(tk2); tk2.st = 'DOCK'; flight(tk2, to, 1.1 + Math.random() * 0.5, 6, function () { tk2.st = 'SEAT'; }); tk2.st = 'SEAT'; }
          break;
        }
        case T.CONTEXT: {
          var src = sysPos[((r.a % SYS) + SYS) % SYS];
          if (!fast) {
            addBeam(src, deskPos[r.seat], 0.55, r.flags === 1 ? col('#9fd0ff') : col('#5fe0b0'));
            flash(r.seat, 0.4, r.flags === 1 ? col('#9fd0ff') : col('#ffd9a0'));
          }
          break;
        }
        case T.DECIDE: {
          var tk3 = tokState.get(r.oid);
          if (tk3) { tk3.decided = true; tk3.margin = r.margin; tk3.seat = r.seat; tk3.st = 'SEAT'; drawTok(tk3); }
          if (!fast) flash(r.seat, 0.55, col('#fff0d0'));
          break;
        }
        case T.HOLD: {
          var tk4 = tokState.get(r.oid); if (!tk4) break;
          if (tk4.st === 'TRAY' && tk4.seat >= 0) trayCount[tk4.seat] = Math.max(0, trayCount[tk4.seat] - 1);
          tk4.seat = r.seat; tk4.stack = trayCount[r.seat]++ % 9;
          var to4 = (function () { var tp = trayPos[r.seat]; return V3(tp.x, tp.y + 0.09 + tk4.stack * 0.1, tp.z); })();
          if (fast) { tk4.st = 'TRAY'; drawTok(tk4); }
          else flight(tk4, to4, 0.5, 1.2, function () { tk4.st = 'TRAY'; drawTok(tk4); });
          break;
        }
        case T.ESCALATE: {
          var tk5 = tokState.get(r.oid); if (!tk5) break;
          if (tk5.st === 'TRAY' && tk5.seat >= 0) trayCount[tk5.seat] = Math.max(0, trayCount[tk5.seat] - 1);
          var toSeat = r.a;
          var path = escalatePath(r.seat, toSeat);
          tk5.seat = toSeat; tk5.st = 'SEAT';
          if (fast) drawTok(tk5);
          else {
            tracks.push({ kind: 'fly3', tk: tk5, from: path.from, mid: path.mid, to: path.to, t0: now, t1: now + 1.5 });
            flash(toSeat, 0.8, col('#9fd0ff'));
          }
          break;
        }
        case T.EFFECT: {
          var tk6 = tokState.get(r.oid); if (!tk6) break;
          var gi2 = ghostN++ % 256;
          tk6.ghostIdx = gi2;
          var gp = ghostSlot(gi2);
          setI(ghosts, gi2, gp.x, gp.y, gp.z, 1);
          var out = V3(DX + 22 + (Math.random() - 0.5), 2.0, 4);
          if (fast) { release(r.oid); tokState.set(r.oid, { oid: r.oid, i: -1, st: 'GHOST', ghostIdx: gi2, cls: r.cls, value: r.value }); }
          else {
            tracks.push({
              kind: 'fly', tk: tk6, from: restPos(tk6), to: out, t0: now, t1: now + 1.4, arc: 9,
              after: function () { var keep = { oid: r.oid, i: -1, st: 'GHOST', ghostIdx: gi2, cls: r.cls, value: r.value }; release(r.oid); tk6.i = -1; tokState.set(r.oid, keep); }
            });
          }
          break;
        }
        case T.UNDO: {
          var tk7 = tokState.get(r.oid);
          var gidx = tk7 ? tk7.ghostIdx : undefined;
          if (gidx !== undefined) setI(ghosts, gidx, 0, -999, 0, 0.001);
          var nt = alloc(r.oid, r.cls, r.value);
          if (nt) { nt.st = 'SEAT'; nt.seat = r.seat; nt.decided = false; drawTok(nt);
            if (!fast) { var to7 = restPos(nt); nt.st = 'GHOST'; nt.ghost = gidx || 0; flight(nt, to7, 1.2, 8, function () { nt.st = 'SEAT'; }); nt.st = 'SEAT'; } }
          break;
        }
        case T.OUTCOME: {
          var tk8 = tokState.get(r.oid);
          if (tk8) {
            if (tk8.st === 'TRAY' && tk8.seat >= 0) trayCount[tk8.seat] = Math.max(0, trayCount[tk8.seat] - 1);
            if (tk8.ghostIdx !== undefined) setI(ghosts, tk8.ghostIdx, 0, -999, 0, 0.001);
            if (tk8.i >= 0) release(r.oid); else tokState.delete(r.oid);
          }
          var mi = markN++ % 512, mp = markSlot(mi);
          setI(marks, mi, mp.x, mp.y, mp.z, 1);
          setC(marks, mi, GRADE[r.a] || GRADE[0], 1);
          setI(rims, mi, mp.x, mp.y + 0.02, mp.z, 1, 1, 1, 0, Math.PI / 2);
          setC(rims, mi, r.flags === 1 ? col('#5fe0b0') : col('#55606e'), r.flags === 1 ? 1 : 0.6);
          break;
        }
        case T.MEETING: {
          var room = glassRoom(seatFloor[r.seat] || 0);
          if (seatFrame[r.seat] === 'annex' && annexRoom) room = annexRoom;
          if (room && !fast) {
            walk(r.seat, V3(room.position.x - 1.2, room.position.y - 1.1, room.position.z), 1.0, 1.6);
            if (r.a >= 0) {
              var room2 = (seatFrame[r.a] === 'annex' && annexRoom) ? annexRoom : glassRoom(seatFloor[r.a] || 0);
              walk(r.a, V3(room2.position.x + 1.2, room2.position.y - 1.1, room2.position.z), 1.0, 1.6);
              if (r.flags === 1) addBeam(seatPos[r.seat], seatPos[r.a], 1.8, col('#b79cff'));
            }
          }
          break;
        }
        case T.LICENSE: {
          var ti2 = r.cls * 6 + r.band;
          var rung = Math.max(0, Math.min(5, r.a));
          var cc = col().lerpColors(col('#1b2430'), col('#5fe0b0'), rung / 5);
          setC(tiles, ti2, cc, 1);
          setI(tiles, ti2, tilePos[ti2].x, BY + 0.08 + rung * 0.06, tilePos[ti2].z, 1, 1 + rung * 0.5, 1);
          if (r.flags === 1) setI(glyphs, ti2, tilePos[ti2].x, BY + 0.9 + rung * 0.1, tilePos[ti2].z, 1);
          break;
        }
        case T.KAPPA: {
          var frac = Math.max(0, Math.min(1, r.a / firm.writ.kappa_max));
          needle.rotation.z = Math.PI * 1.1 - frac * Math.PI * 1.2;
          break;
        }
        case T.PATCH: {
          if (r.a === 0) setSpan(r.b);
          else if (r.a === 1) { var si2 = ((r.b % SYS) + SYS) % SYS; setC(systems, si2, col('#5fe0b0'), 1); setI(systems, si2, sysPos[si2].x, sysPos[si2].y + 0.3, sysPos[si2].z, 1, 1.25, 1); }
          else if (r.a === 2) darkFloor(Math.floor(r.b / 10), r.b % 10);
          else if (r.a === 3) darkAnnex(r.b);
          else if (r.a === 4) collapseClass(r.cls);
          break;
        }
        case T.TICK: {
          spool.rotation.z += 0.6;
          break;
        }
      }
    }

    /* ---------- morphs ---------- */
    var spanNow = firm.span;
    function setSpan(n) { spanNow = Math.max(1, Math.min(12, n)); }
    /* a floor going dark: pod f of wire w; past the last pod, the whole department */
    function darkFloor(w, f) {
      var list = podPlate[w] || [];
      var targets = f < list.length ? [f] : list.map(function (_, k) { return k; });
      targets.forEach(function (pi) {
        var plate = list[pi]; if (!plate) return;
        plate.material.color.set(0x0a0d12);
        (stripIdx[w][pi] || []).forEach(function (sI) { setC(strips, sI, col('#2a2119'), 1); });
      });
      if (f >= list.length) {
        var fl2 = w + 1;
        if (slabs[fl2]) { slabs[fl2].material.color.set(0x141a24); slabEdges[fl2].material.opacity = 0.12; }
        if (rooms[fl2]) rooms[fl2].material.opacity = 0.04;
        scene.getObjectByName('floor_light_' + fl2).intensity = 4;
      }
      firm.seats.forEach(function (s) {
        if (seatWing[s.id] !== w) return;
        if (targets.indexOf(seatPod[s.id]) >= 0 || (f >= list.length)) { setC(lamps, s.id, col('#2a1f12'), 1); setC(figs, s.id, col('#2b3340'), 0.5); }
      });
    }
    function darkAnnex(f) {
      var m = annexSlab[f];
      if (m && m.edge) { m.material.color.set(0x0b0e14); m.edge.material.opacity = 0.08; }
      if (f === 1) annexLight.intensity = 5;
      firm.seats.forEach(function (s) {
        if (s.fn === 'alpha' && seatFloor[s.id] === f) {
          setC(lamps, s.id, col('#241a10'), 1); setC(figs, s.id, col('#242c38'), 0.4);
          setI(figs, s.id, figHome[s.id].x, -999, figHome[s.id].z, 0.001);
        }
      });
    }
    var collapsedCls = [];
    function collapseClass(cls) {
      if (collapsedCls.indexOf(cls) < 0) collapsedCls.push(cls);
      routine.visible = true;
      var k = Math.min(1, 0.35 + collapsedCls.length * 0.22);
      routine.scale.setScalar(k);
      tokState.forEach(function (tk) { if (tk.cls === cls && tk.i >= 0) setC(tokens, tk.i, col('#4a5566'), 1); });
    }
    function resetMorphs() {
      setSpan(firm.span);
      for (var ff = 0; ff < NF; ff++) { slabs[ff].material.color.set(0x27303f); slabEdges[ff].material.opacity = 0.28; scene.getObjectByName('floor_light_' + ff).intensity = ff === 0 ? 26 : 34; }
      for (var w2 = 0; w2 < 6; w2++) (podPlate[w2] || []).forEach(function (pl, pi) { pl.material.color.set(0x1b2230); (stripIdx[w2][pi] || []).forEach(function (sI) { setC(strips, sI, col('#ffe2bf'), 1.6); }); });
      Object.keys(rooms).forEach(function (k) { rooms[k].material.opacity = 0.11; });
      annexSlab.forEach(function (m, k) { if (m && m.edge) { m.material.color.set(k === C.ANNEX_FLOORS ? 0x1c2330 : 0x27303f); m.edge.material.opacity = 0.3; } });
      annexLight.intensity = 30;
      collapsedCls = []; routine.visible = false; routine.scale.setScalar(0.01);
      for (var i2 = 0; i2 < SYS; i2++) { setC(systems, i2, col('#27323f')); setI(systems, i2, sysPos[i2].x, sysPos[i2].y, sysPos[i2].z, 1); }
      firm.seats.forEach(function (s) {
        setC(lamps, s.id, col('#ffb35c'), 1.9);
        setC(figs, s.id, WIRE[s.wire] || WIRE[6], 0.75);
        setI(figs, s.id, figHome[s.id].x, figHome[s.id].y, figHome[s.id].z, 1);
      });
      for (var ti = 0; ti < TILES; ti++) { setC(tiles, ti, col('#1b2430')); setI(tiles, ti, tilePos[ti].x, BY + 0.08, tilePos[ti].z, 1); setI(glyphs, ti, tilePos[ti].x, -999, tilePos[ti].z, 0.001); }
    }

    /* ---------- snap: the fold, drawn ---------- */
    function snap(st) {
      tracks.length = 0;
      tokState.forEach(function (tk) { if (tk.i >= 0) setI(tokens, tk.i, 0, -999, 0, 0.001); });
      tokState.clear(); free.length = 0;
      for (var i = TOKN - 1; i >= 0; i--) free.push(i);
      for (i = 0; i < 256; i++) setI(ghosts, i, 0, -999, 0, 0.001);
      for (i = 0; i < 512; i++) { setI(marks, i, 0, -999, 0, 0.001); setI(rims, i, 0, -999, 0, 0.001); }
      trayCount.fill(0); dockN = 0; ghostN = 0; markN = 0;
      resetMorphs();

      st.systems.forEach(function (b) { var si3 = ((b % SYS) + SYS) % SYS; setC(systems, si3, col('#5fe0b0'), 1); setI(systems, si3, sysPos[si3].x, sysPos[si3].y + 0.3, sysPos[si3].z, 1, 1.25, 1); });
      if (st.span !== firm.span) setSpan(st.span);
      st.collapsed.forEach(collapseClass);
      st.cascade.forEach(function (k) {
        if (k[0] === 'w') darkFloor(+k[1], +k[3]);
        else if (k.indexOf('af') === 0) darkAnnex(+k.slice(2));
      });

      for (var c3 = 0; c3 < NCLS; c3++) for (var b3 = 0; b3 < 6; b3++) {
        var rung = st.machine.license[c3][b3], ti2 = c3 * 6 + b3;
        if (rung > 0) {
          setC(tiles, ti2, col().lerpColors(col('#1b2430'), col('#5fe0b0'), rung / 5), 1);
          setI(tiles, ti2, tilePos[ti2].x, BY + 0.08 + rung * 0.06, tilePos[ti2].z, 1, 1 + rung * 0.5, 1);
        }
        if (st.machine.admitted[c3][b3] === 1) setI(glyphs, ti2, tilePos[ti2].x, BY + 0.9 + rung * 0.1, tilePos[ti2].z, 1);
      }

      var oids = Array.from(st.tokens.keys()).sort(function (a, b) { return a - b; });
      var stacks = new Int16Array(N);
      oids.forEach(function (oid) {
        var s = st.tokens.get(oid);
        var tk = alloc(oid, s.cls, s.value);
        if (!tk) return;
        tk.st = s.st; tk.seat = s.seat; tk.margin = s.margin; tk.decided = s.margin !== 0;
        if (s.st === 'TRAY') { tk.stack = stacks[s.seat]++ % 9; trayCount[s.seat] = stacks[s.seat]; }
        if (s.st === 'GHOST') { tk.ghostIdx = ghostN++ % 256; var gp = ghostSlot(tk.ghostIdx); setI(ghosts, tk.ghostIdx, gp.x, gp.y, gp.z, 1); setI(tokens, tk.i, 0, -999, 0, 0.001); free.push(tk.i); tk.i = -1; return; }
        drawTok(tk);
      });
      var recent = st.marks.slice(-512);
      recent.forEach(function (mk) {
        var mi = markN++ % 512, mp = markSlot(mi);
        setI(marks, mi, mp.x, mp.y, mp.z, 1); setC(marks, mi, GRADE[mk.grade] || GRADE[0], 1);
        setI(rims, mi, mp.x, mp.y + 0.02, mp.z, 1, 1, 1, 0, Math.PI / 2);
        setC(rims, mi, mk.by_machine === 1 ? col('#5fe0b0') : col('#55606e'), mk.by_machine === 1 ? 1 : 0.6);
      });
      firm.seats.forEach(function (s) {
        var h = st.seats.held[s.id] || 0;
        var lit = 1.5 + Math.min(4, h * 0.6);
        setC(lamps, s.id, col('#ffb35c'), lit);
      });
      readouts(st);
    }

    function readouts(st) {
      var max = 1;
      for (var i = 0; i < 8; i++) max = Math.max(max, st.ledger[i]);
      for (i = 0; i < 8; i++) {
        var h = 0.4 + 15 * (st.ledger[i] / max);
        setI(ledgerBars, i, ledgerPos[i].x, h / 2, ledgerPos[i].z, 1, h, 1);
        setC(ledgerBars, i, col('#b79cff'), 0.35 + 0.65 * (st.ledger[i] / max));
      }
      drawLedger(st.ledger);
      drawCounter(st.machine.counter, st.machine.kappa, st.phase);
      var frac = Math.max(0, Math.min(1, st.machine.kappa / firm.writ.kappa_max));
      needle.rotation.z = Math.PI * 1.1 - frac * Math.PI * 1.2;
      spool.rotation.z = st.day * 0.35;
    }

    /* ---------- director ---------- */
    var phaseNow = null, phaseT0 = 0;
    function cutover(phase) {
      if (phase === phaseNow) return;
      phaseNow = phase; phaseT0 = now;
      fiber.visible = false; joinGraph.visible = false; ghostTower.visible = false; sparks.visible = false;
      if (phase === 'INSTRUMENT') fiber.visible = true;
      if (phase === 'COMPILE') { joinGraph.visible = true; joinGraph.geometry.setDrawRange(0, 0); }
      if (phase === 'REPLAY') ghostTower.visible = true;
      if (phase === 'CANARY') { sparks.visible = true; seedSparks(); }
      if (phase === 'COMPILE_OUT') routine.visible = collapsedCls.length > 0;
    }
    function seedSparks() {
      var arr = sparks.geometry.attributes.position.array;
      var live = firm.seats.filter(function (s) { return s.kind === 'IC'; });
      for (var i = 0; i < SPK; i++) {
        var s = live[i % live.length], d = deskPos[s.id];
        arr[i * 3] = d.x + (Math.random() - 0.5) * 1.2;
        arr[i * 3 + 1] = d.y + Math.random() * 1.1;
        arr[i * 3 + 2] = d.z + (Math.random() - 0.5) * 1.2;
      }
      sparks.geometry.attributes.position.needsUpdate = true;
    }

    /* ---------- selection ---------- */
    var ray = new THREE.Raycaster(), ndc = new THREE.Vector2();
    var selRing = new THREE.Mesh(new THREE.TorusGeometry(1.3, 0.06, 5, 28), new THREE.MeshBasicMaterial({ color: 0xb79cff }));
    selRing.rotation.x = Math.PI / 2; selRing.name = 'select_ring'; selRing.visible = false; scene.add(selRing);
    function pickAt(nx, ny) {
      ndc.set(nx, ny); ray.setFromCamera(ndc, cam);
      var hits = ray.intersectObject(desks, false);
      if (hits.length && hits[0].instanceId !== undefined) return hits[0].instanceId;
      return null;
    }
    function select(id) {
      if (id === null || id === undefined) { selRing.visible = false; return; }
      var p = seatPos[id];
      selRing.position.set(p.x, p.y + 0.1, p.z); selRing.visible = true;
    }

    /* ---------- cameras, framed from the building's own dims ---------- */
    var PRESETS = {
      ORBIT: [V3(LX * 0.9 + 30, H * 0.9 + 12, -(LZ + 78)), V3(0, H * 0.42, -2)],
      ISO: [V3(190, 170, -190), V3(0, H * 0.4, 0)],
      DOCK: [V3(DX + 26, 16, -46), V3(DX, 2.5, 0)],
      BASEMENT: [V3(LX * 0.3, -2.4, -(LZ / 2 + 26)), V3(0, -4.2, 2)],
      ANNEX: [V3(D.ANNEX_X - 26, 22, -48), V3(D.ANNEX_X + 4, 6, -2)],
      TOP: [V3(LX * 0.5, H + 22, -(LZ + 34)), V3(0, H - 2, 0)],
      LEDGER: [V3(-LX / 2 - 20, 22, -(LZ / 2 + 62)), V3(-LX / 2 - 20, 6, -(LZ / 2 + 16))],
      CORE: [V3(10, 18, -(LZ / 2 + 30)), V3(0, 12, LZ / 2 - 5)],
      WING: [V3(-LX / 2 + 14, PITCH + 9, -(LZ / 2 + 26)), V3(-LX / 2 + 16, PITCH + 1, -4)]
    };
    var camName = 'ORBIT';
    function setCam(name) {
      var p = PRESETS[name]; if (!p) return;
      camName = name;
      var useOrtho = name === 'ISO';
      cam = useOrtho ? camO : camP;
      controls.object = cam;
      cam.position.copy(p[0]);
      controls.target.copy(p[1]);
      if (useOrtho) { cam.zoom = 1; cam.updateProjectionMatrix(); }
      cam.lookAt(p[1]);
      controls.update();
      renderPass.camera = cam;
    }
    setCam('ORBIT');

    /* ---------- frame ---------- */
    var beamPos = beams.geometry.attributes.position.array, beamCol = beams.geometry.attributes.color.array;
    function frame(dt) {
      now += dt;
      controls.update();
      var nb = 0;
      for (var i = tracks.length - 1; i >= 0; i--) {
        var tr = tracks[i];
        var u = (now - tr.t0) / (tr.t1 - tr.t0);
        if (u < 0) continue;
        if (tr.kind === 'fly' || tr.kind === 'fly3') {
          var tk = tr.tk;
          if (!tokState.has(tk.oid) || tk.i < 0) { tracks.splice(i, 1); continue; }
          var uu = Math.min(1, u), e = uu * uu * (3 - 2 * uu);
          var x, y, z;
          if (tr.kind === 'fly3') {
            var m = tr.mid, a = tr.from, b = tr.to;
            if (e < 0.5) { var k = e * 2; x = a.x + (m.x - a.x) * k; y = a.y + (m.y - a.y) * k; z = a.z + (m.z - a.z) * k; }
            else { var k2 = (e - 0.5) * 2; x = m.x + (b.x - m.x) * k2; y = m.y + (b.y - m.y) * k2; z = m.z + (b.z - m.z) * k2; }
          } else {
            x = tr.from.x + (tr.to.x - tr.from.x) * e;
            y = tr.from.y + (tr.to.y - tr.from.y) * e + Math.sin(Math.PI * e) * tr.arc;
            z = tr.from.z + (tr.to.z - tr.from.z) * e;
          }
          var sc = tokScale(tk.value, tk.decided);
          setI(tokens, tk.i, x, y, z, sc[0], sc[1], sc[2], 0, tk.decided ? Math.max(-1, Math.min(1, tk.margin / 260)) * 0.85 : 0);
          var wire = firm.classes[tk.cls] ? firm.classes[tk.cls].wire : 6;
          setC(tokens, tk.i, WIRE[wire], 1.9);
          if (u >= 1) { if (tr.after) tr.after(); if (tokState.has(tk.oid) && tk.i >= 0) drawTok(tk); tracks.splice(i, 1); }
        } else if (tr.kind === 'beam') {
          if (u >= 1) { tracks.splice(i, 1); continue; }
          if (nb < BEAMN) {
            var fade = 1 - u;
            beamPos[nb * 6] = tr.from.x; beamPos[nb * 6 + 1] = tr.from.y; beamPos[nb * 6 + 2] = tr.from.z;
            beamPos[nb * 6 + 3] = tr.to.x; beamPos[nb * 6 + 4] = tr.to.y; beamPos[nb * 6 + 5] = tr.to.z;
            beamCol[nb * 6] = tr.c.r * 0.2; beamCol[nb * 6 + 1] = tr.c.g * 0.2; beamCol[nb * 6 + 2] = tr.c.b * 0.2;
            beamCol[nb * 6 + 3] = tr.c.r * fade; beamCol[nb * 6 + 4] = tr.c.g * fade; beamCol[nb * 6 + 5] = tr.c.b * fade;
            nb++;
          }
        } else if (tr.kind === 'flash') {
          if (u >= 1) { setC(lamps, tr.seat, col('#ffb35c'), 1.9); tracks.splice(i, 1); continue; }
          setC(lamps, tr.seat, tr.c, 1.9 + (1 - u) * 5.0);
        } else if (tr.kind === 'walk') {
          var total = tr.hold + 2 * (tr.t1 - tr.t0);
          var el = now - tr.t0, dur = tr.t1 - tr.t0, ww;
          if (el < dur) ww = el / dur;
          else if (el < dur + tr.hold) ww = 1;
          else ww = Math.max(0, 1 - (el - dur - tr.hold) / dur);
          setI(figs, tr.seat, tr.from.x + (tr.to.x - tr.from.x) * ww, tr.from.y + (tr.to.y - tr.from.y) * ww, tr.from.z + (tr.to.z - tr.from.z) * ww, 1);
          if (el > total) { setI(figs, tr.seat, tr.from.x, tr.from.y, tr.from.z, 1); tracks.splice(i, 1); }
        }
      }
      beams.geometry.setDrawRange(0, nb * 2);
      beams.geometry.attributes.position.needsUpdate = true;
      beams.geometry.attributes.color.needsUpdate = true;

      /* director motion */
      if (phaseNow === 'COMPILE') {
        var el2 = now - phaseT0, n2 = Math.min(joinPts.length, Math.floor(el2 * 26) * 2);
        joinGraph.geometry.setDrawRange(0, n2);
      }
      if (phaseNow === 'REPLAY') {
        scanPlane.position.y = (now * 9) % H;
        ghostTower.children.forEach(function (c, k) { if (c.material && c !== scanPlane) c.material.opacity = 0.12 + 0.22 * Math.abs(Math.sin(now * 2 + k)); });
      }
      if (phaseNow === 'CANARY' && sparks.visible) {
        sparks.material.opacity = 0.55 + 0.45 * Math.abs(Math.sin(now * 6));
        if (Math.random() < 0.06) seedSparks();
      }
      if (selRing.visible) selRing.rotation.z = now * 0.7;
      spool.rotation.z += dt * 0.15;

      composer.render();
    }

    function resize() {
      var w = container.clientWidth || 1280, h = container.clientHeight || 720;
      renderer.setSize(w, h, false); composer.setSize(w, h);
      bloom.setSize(w, h);
      camP.aspect = w / h; camP.updateProjectionMatrix();
      camO.left = -ORTH * (w / h); camO.right = ORTH * (w / h); camO.top = ORTH; camO.bottom = -ORTH; camO.updateProjectionMatrix();
    }

    W.scene = scene; W.renderer = renderer; W.composer = composer;
    W.get = function (n) { return scene.getObjectByName(n); };
    W.camera = function () { return cam; };
    W.controls = controls;
    W.frame = frame; W.resize = resize; W.snap = snap; W.onRow = onRow; W.readouts = readouts;
    W.setCam = setCam; W.camName = function () { return camName; };
    W.cutover = cutover; W.select = select; W.pickAt = pickAt;
    W.setFast = function (v) { fast = !!v; if (v) tracks.length = 0; };
    var layerMap = { desks: gDesks, figures: gFig, tokens: gTokens, trays: gTrays, glass: gGlass, dock: gDock, basement: gBase, ledger: gLedger, beams: gBeams, annex: gAnnex, floors: gFloors, director: gDirector };
    W.setLayer = function (name, on) { if (layerMap[name]) layerMap[name].visible = !!on; };
    W.layerOn = function (name) { return layerMap[name] ? layerMap[name].visible : false; };
    W.seatPos = seatPos; W.deskPos = deskPos;
    W.project = function (v) {
      var p = v.clone().project(cam);
      var w = container.clientWidth, h = container.clientHeight;
      return { x: (p.x * 0.5 + 0.5) * w, y: (-p.y * 0.5 + 0.5) * h, z: p.z };
    };
    W.renderOnce = function () { controls.update(); composer.render(); };
    W.setBloom = function (v) { bloom.strength = +v; };
    W.setExposure = function (v) { renderer.toneMappingExposure = +v; };
    W.shot = function () { composer.render(); return renderer.domElement.toDataURL('image/png'); };
    W.counts = function () { return { tokens: tokState.size, tracks: tracks.length, calls: renderer.info.render.calls, tris: renderer.info.render.triangles }; };
    W.dims = D;
    return W;
  };
})();
