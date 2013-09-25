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
#include "dbxout.h"
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

static bool
m65x_legitimate_address_p (enum machine_mode mode, rtx x, bool strict)
{
  if (CONSTANT_P (x))
    return 1;
  
  if (!strict && REG_P (x))
    return 1;

  if (strict)
    {
      if (REG_P (x) && REGNO (x) < FIRST_PSEUDO_REGISTER)
        return 1;
    }

  return 0;
}

int
m65x_mode_dependent_address_p (rtx x)
{
  /* Another big lie!  */
  return 0;
}

static rtx
m65x_function_arg (cumulative_args_t ca, enum machine_mode mode,
		   const_tree type, bool named)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);

  if (*pcum < 8)
    return gen_rtx_REG (mode, (*pcum) + FIRST_ARG_REGISTER);
  else
    return NULL_RTX;
}

static void
m65x_function_arg_advance (cumulative_args_t ca, enum machine_mode mode,
			   const_tree type, bool named)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  
  (*pcum) += GET_MODE_SIZE (mode);
}

static rtx
m65x_function_value (const_tree ret_type, const_tree fn_decl_or_type,
		     bool outgoing)
{
  enum machine_mode mode;
  
  mode = TYPE_MODE (ret_type);
  
  return gen_rtx_REG (mode, 0);
}

static rtx
m65x_libcall_value (enum machine_mode mode, const_rtx fun)
{
  return gen_rtx_REG (mode, 0);
}

static void
m65x_asm_globalize_label (FILE *stream, const char *name)
{
  fprintf (stream, "; .globl %s", name);
}

#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG m65x_function_arg

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE m65x_function_arg_advance

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE m65x_function_value

#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE m65x_libcall_value

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P m65x_legitimate_address_p

#undef TARGET_ASM_GLOBALIZE_LABEL
#define TARGET_ASM_GLOBALIZE_LABEL m65x_asm_globalize_label

struct gcc_target targetm = TARGET_INITIALIZER;
