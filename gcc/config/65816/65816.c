#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "obstack.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "reload.h"
#include "function.h"
#include "expr.h"
#include "optabs.h"
#include "toplev.h"
#include "recog.h"
#include "ggc.h"
#include "except.h"
#include "c-pragma.h"
#include "integrate.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "debug.h"
#include "langhooks.h"
#include "df.h"

struct gcc_target targetm = TARGET_INITIALIZER;

void
wdc65816_print_operand (FILE *stream, rtx x, int code)
{
  switch (code)
    {
    default:
      ;
    }
}

void
wdc65816_print_operand_address (FILE *stream, rtx x)
{

}

int
wdc65816_legitimate_address_p (enum machine_mode mode, rtx x)
{
  /* A big lie!  */
  return 1;
}

int
wdc65816_mode_dependent_address_p (rtx x)
{
  /* Another big lie!  */
  return 0;
}
