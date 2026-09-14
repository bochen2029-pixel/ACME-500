// O17's lie: the same translation unit with one include added, the plant.
// Under -DACME_NO_PLANT this file must FAIL to compile on world.h's guard. If
// it compiles, the guard is gone and the oracle has no teeth.
#define ACME_NO_PLANT 1
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/ledger.h"
#include "acme/port.h"
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/world.h"      // THE LIE: the kernel reaching the plant
int acme_o17_lie() { return 0; }
