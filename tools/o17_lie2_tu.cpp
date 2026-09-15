// O17's second lie: the same translation unit with one include added, the
// governor. Under -DACME_NO_GOVERNOR this file must FAIL to compile on
// governor.h's guard. If it compiles, the kernel can reach the salt.
#define ACME_NO_PLANT 1
#define ACME_NO_GOVERNOR 1
#include "acme/core.h"
#include "acme/firm.h"
#include "acme/ledger.h"
#include "acme/port.h"
#include "acme/license.h"
#include "acme/solver.h"
#include "acme/machine.h"
#include "acme/governor.h"   // THE LIE: the kernel reaching the salt
int acme_o17_lie2() { return 0; }
