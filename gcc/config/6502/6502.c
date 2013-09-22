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
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "debug.h"
#include "langhooks.h"
#include "df.h"

static void m65x_asm_globalize_label (FILE *, const char *);

void
m65x_print_operand (FILE *stream, rtx x, int code)
{
  switch (code)
    {
    default:
      ;
    }
}

void
m65x_print_operand_address (FILE *stream, rtx x)
{

}

int
m65x_legitimate_address_p (enum machine_mode mode, rtx x)
{
  /* A big lie!  */
  return 1;
}

int
m65x_mode_dependent_address_p (rtx x)
{
  /* Another big lie!  */
  return 0;
}

static void
m65x_asm_globalize_label (FILE *stream, const char *name)
{
  fprintf (stream, "; .globl %s", name);
}

#undef TARGET_ASM_GLOBALIZE_LABEL
#define TARGET_ASM_GLOBALIZE_LABEL m65x_asm_globalize_label

struct gcc_target targetm = TARGET_INITIALIZER;
