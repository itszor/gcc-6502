#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "stor-layout.h"
#include "calls.h"
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

#undef DEBUG_LEGIT_RELOAD
#undef DEBUG_SECONDARY_RELOAD
#undef DEBUG_ADDRESS

static void
m65x_file_start (void)
{
  int i;
  int zploc = 0x70;
  
  fprintf (asm_out_file, "\t.feature at_in_identifiers\n");
  fprintf (asm_out_file, "\t.feature dollar_in_identifiers\n");
  fprintf (asm_out_file, "\t.autoimport +\n");
  fprintf (asm_out_file, "\t.psc02\n");
  
  fprintf (asm_out_file, "\t.define _ah $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _ah2 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _ah3 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _xh $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _xh2 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _xh3 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _yh $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _yh2 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _yh3 $%x\n", zploc++);
  
  fprintf (asm_out_file, "\t.define _sp0 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _sp1 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _fp0 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _fp1 $%x\n", zploc++);
  
  for (i = 0; i < 8; i++)
    fprintf (asm_out_file, "\t.define _r%d $%x\n", i, zploc++);

  for (i = 0; i < 8; i++)
    fprintf (asm_out_file, "\t.define _s%d $%x\n", i, zploc++);
  
  fprintf (asm_out_file, "\t.define _tmp0 $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _tmp1 $%x\n", zploc++);
}

static void m65x_asm_globalize_label (FILE *, const char *);

void
m65x_print_operand (FILE *stream, rtx x, int code)
{
  switch (code)
    {
    case 'h':
      gcc_assert (REG_P (x));
      asm_fprintf (stream, "%r", REGNO (x) + 1);
      break;

    case 'R':
      gcc_assert (REG_P (x));
      switch (REGNO (x))
	{
	case ACC_REGNUM: asm_fprintf (stream, "a"); break;
	case X_REGNUM: asm_fprintf (stream, "x"); break;
	case Y_REGNUM: asm_fprintf (stream, "y"); break;
	default: gcc_unreachable ();
	}
      break;

    case 'C':
      gcc_assert (REG_P (x));
      switch (REGNO (x))
        {
	case ACC_REGNUM: asm_fprintf (stream, "cmp"); break;
	case X_REGNUM: asm_fprintf (stream, "cpx"); break;
	case Y_REGNUM: asm_fprintf (stream, "cpy"); break;
	default: gcc_unreachable ();
	}
      break;

    default:
      switch (GET_CODE (x))
        {
	case REG:
	  asm_fprintf (stream, "%r", REGNO (x));
	  break;
	
	case MEM:
	  output_address (XEXP (x, 0));
	  break;
	
	case CONST_INT:
	  asm_fprintf (stream, "$%.2x", (int) INTVAL (x) & 0xff);
	  break;
	
	default:
	  m65x_print_operand_address (stream, x);
	}
    }
}

void
m65x_print_operand_address (FILE *stream, rtx x)
{
  switch (GET_CODE (x))
    {
#if 0
    case PLUS:
      if (GET_MODE (x) == HImode
	  && REG_P (XEXP (x, 0)) && IS_ZP_REGNUM (REGNO (XEXP (x, 0)))
	  && GET_CODE (XEXP (x, 1)) == ZERO_EXTEND
	  && GET_MODE (XEXP (XEXP (x, 1), 0)) == QImode
	  && REG_P (XEXP (XEXP (x, 1), 0))
	  && REGNO (XEXP (XEXP (x, 1), 0)) == Y_REGNUM)
	asm_fprintf (stream, "(%r),y", REGNO (XEXP (x, 0)));
      else
        output_operand_lossage ("invalid PLUS operand");
      break;
#endif

#if 1
    case PLUS:
      if (GET_MODE (x) == HImode
          && REG_P (XEXP (x, 0)) && IS_ZP_REGNUM (REGNO (XEXP (x, 0)))
	  && REG_P (XEXP (x, 1)) && REGNO (XEXP (x, 1)) == Y_REGNUM)
	asm_fprintf (stream, "(%r),y", REGNO (XEXP (x, 0)));
      else if (REG_P (XEXP (x, 0)) && CONST_INT_P (XEXP (x, 1)))
        asm_fprintf (stream, "(%r),##%d##", REGNO (XEXP (x, 0)),
		     (int) INTVAL (XEXP (x, 1)));
      else
        output_operand_lossage ("invalid LO_SUM operand");
      break;
#endif

    case REG:
      asm_fprintf (stream, "(%r)", REGNO (x));
      break;

    case CONST_INT:
      asm_fprintf (stream, "$%.4x", (int) INTVAL (x) & 0xffff);
      break;

    case SUBREG:
      /* If we have this, we're probably dealing with the address of a label
         or similar -- that is, we're outputting an immediate.  */
      if (SUBREG_BYTE (x) == 0)
	asm_fprintf (stream, "<(");
      else if (SUBREG_BYTE (x) == 1)
        asm_fprintf (stream, ">(");
      else
        output_operand_lossage ("invalid subreg operand");
      output_addr_const (stream, x);
      asm_fprintf (stream, ")");
      break;

    default:
      output_addr_const (stream, x);
    }
}

void
m65x_print_branch (enum machine_mode mode, rtx cond, rtx dest, bool synth)
{
  if (synth)
    {
      const char *br;

      switch (mode)
        {
	case CC_NZmode:
	  switch (GET_CODE (cond))
	    {
	    case EQ: br = "bne :+"; break;
	    case NE: br = "beq :+"; break;
	    case LT: br = "bpl :+"; break;
	    case GE: br = "bmi :+"; break;
	    default: gcc_unreachable ();
	    }
	  break;
	
	case CC_Vmode:
	  switch (GET_CODE (cond))
	    {
	    case EQ: br = "bvs :+"; break;
	    case NE: br = "bvc :+"; break;
	    default: gcc_unreachable ();
	    }
	  break;

	case CC_Cmode:
	  switch (GET_CODE (cond))
	    {
	    case EQ: br = "bcs :+"; break;
	    case NE: br = "bcc :+"; break;
	    default: gcc_unreachable ();
	    }
	  break;

	default:
	  gcc_unreachable ();
	}

      output_asm_insn (br, &dest);
      output_asm_insn ("jmp %0", &dest);
      output_asm_insn (":", &dest);
    }
  else
    {
      const char *fmt;

      switch (mode)
	{
	case CC_NZmode:
	  switch (GET_CODE (cond))
            {
	    case EQ: fmt = "beq %0"; break;
	    case NE: fmt = "bne %0"; break;
	    case LT: fmt = "bmi %0"; break;
	    case GE: fmt = "bpl %0"; break;
	    default: gcc_unreachable ();
	    }
	  break;

	case CC_Vmode:
	  switch (GET_CODE (cond))
	    {
	    case EQ: fmt = "bvc %0"; break;
	    case NE: fmt = "bvs %0"; break;
	    default: gcc_unreachable ();
	    }
	  break;

	case CC_Cmode:
	  switch (GET_CODE (cond))
	    {
	    case EQ: fmt = "bcc %0"; break;
	    case NE: fmt = "bcs %0"; break;
	    default: gcc_unreachable ();
	    }
	  break;

	default:
	  gcc_unreachable ();
	}

      output_asm_insn (fmt, &dest);
    }
}


void
m65x_output_ascii (FILE *f, const char *str, int len)
{
  enum {
    START,
    PRINT,
    NONPRINT
  } state = START;
  int i;
  
  for (i = 0; i < len; i++)
    {
      /* This doesn't pay much attention to character encoding issues...  */
      if (str[i] < 32 || str[i] >= 127)
        {
	  switch (state)
	    {
	    case START:
	      fprintf (f, "\t.byte $%x", str[i]);
	      break;
	    
	    case PRINT:
	      fprintf (f, "\", $%x", str[i]);
	      break;

	    case NONPRINT:
	      fprintf (f, ", $%x", str[i]);
	    }
	  state = NONPRINT;
	}
      else
        {
	  switch (state)
	    {
	    case START:
	      fprintf (f, "\t.byte \"%c", str[i]);
	      break;
	    
	    case PRINT:
	      fputc (str[i], f);
	      break;
	    
	    case NONPRINT:
	      fprintf (f, ", \"%c", str[i]);
	    }
	  state = PRINT;
	}
    }
  
  if (state == PRINT)
    fputc ('"', f);

  fputc ('\n', f);
}

static bool
m65x_reg_ok_for_base_p (const_rtx x, bool strict_p)
{
  if (strict_p && IS_ZP_REGNUM (REGNO (x)))
    return true;
  else if (!strict_p
	   && (REGNO (x) >= FIRST_PSEUDO_REGISTER
	       || IS_ZP_REGNUM (REGNO (x))
	       || REGNO (x) == FRAME_POINTER_REGNUM
	       || REGNO (x) == ARG_POINTER_REGNUM))
    return true;
  else
    return false;
}

static bool
m65x_address_register_p (const_rtx x, int strict_p)
{
  if (REG_P (x) && m65x_reg_ok_for_base_p (x, strict_p))
    return true;
  
  return false;
}

bool
m65x_indirect_indexed_addr_p (enum machine_mode mode, rtx x, bool strict)
{
  HOST_WIDE_INT modesize = GET_MODE_SIZE (mode);

  if (GET_CODE (x) == PLUS && m65x_address_register_p (XEXP (x, 0), strict)
      && CONST_INT_P (XEXP (x, 1)) && INTVAL (XEXP (x, 1)) >= 0
      && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
    return true;

  return false;
}

bool
m65x_legitimate_address_p (enum machine_mode mode, rtx x, bool strict)
{
#ifdef DEBUG_ADDRESS
  fprintf (stderr, "checking legitimacy of address (strict: %s, %s):\n",
	   strict ? "yes" : "no", reload_in_progress ? "reload in progress"
	   : reload_completed ? "reload completed" : "before reload");
  debug_rtx (x);
#endif

  if (CONSTANT_ADDRESS_P (x))
    return true;

  /* Allow pre-increment/post-decrement only for the hardware stack pointer,
     i.e. ph* and pl* instructions.  */
  if ((GET_CODE (x) == PRE_INC || GET_CODE (x) == POST_DEC)
      && mode == QImode
      && REG_P (XEXP (x, 0)) && REGNO (XEXP (x, 0)) == HARDSP_REGNUM)
    return true;

  /* Plain (mem (reg)) can't be disallowed, else the middle end gets very
     upset.  */
  if (m65x_address_register_p (x, strict))
    return true;

  if (0 && m65x_indirect_indexed_addr_p (mode, x, strict))
    return true;

  if (0 && !strict && GET_CODE (x) == PLUS && REG_P (XEXP (x, 0))
      && CONST_INT_P (XEXP (x, 1)))
    return true;

  if (0 && !strict)
    {
      if (m65x_address_register_p (x, strict))
	return true;

      if (GET_CODE (x) == PLUS)
	{
	  HOST_WIDE_INT modesize = GET_MODE_SIZE (mode);

	  if (m65x_address_register_p (XEXP (x, 0), strict)
	      && CONST_INT_P (XEXP (x, 1))
	      && INTVAL (XEXP (x, 1)) >= 0
	      && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
	    return true;
	}
    }

  return false;
}

static bool
m65x_mode_dependent_address (const_rtx x,
			     addr_space_t addrspace ATTRIBUTE_UNUSED)
{
  if (GET_CODE (x) == PLUS)
    return true;
  
  return false;
}

static rtx
m65x_legitimize_address (rtx x, rtx oldx, enum machine_mode mode)
{
  int modesize = GET_MODE_SIZE (mode);

  if (CONSTANT_ADDRESS_P (x))
    return x;

  return x; // force_reg (Pmode, x);
      
  if (REG_P (x))
    x = gen_rtx_PLUS (Pmode, x, GEN_INT (0));
  else if (GET_CODE (x) == PLUS && CONST_INT_P (XEXP (x, 1))
	   && INTVAL (XEXP (x, 1)) >= 0
	   && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
    x = gen_rtx_PLUS (Pmode, force_reg (Pmode, XEXP (x, 0)), XEXP (x, 1));
  else if (GET_CODE (x) == PLUS && REG_P (XEXP (x, 0)) && REG_P (XEXP (x, 1)))
    ;
  else
    x = gen_rtx_PLUS (Pmode, force_reg (Pmode, x), GEN_INT (0));

  return x;
}

/* This might not be useful.  */

static rtx
m65x_delegitimize_address (rtx orig_x)
{
  rtx x = delegitimize_mem_from_attrs (orig_x);
  
  /*if (GET_CODE (x) == LO_SUM && REG_P (XEXP (x, 0))
      && CONST_INT_P (XEXP (x, 1)))
    x = gen_rtx_PLUS (GET_MODE (x), XEXP (x, 0), XEXP (x, 1));*/

  return x;
}

rtx
m65x_adjust_address (rtx mem, enum machine_mode mode, HOST_WIDE_INT adj)
{
  rtx x;
  
  gcc_assert (MEM_P (mem));
  
  x = XEXP (mem, 0);
  
  /*if (GET_CODE (x) == LO_SUM && REG_P (XEXP (x, 0))
      && CONST_INT_P (XEXP (x, 1)))
    {
      HOST_WIDE_INT offset = INTVAL (XEXP (x, 1));
      offset += adj;
      if (offset >= 0 && offset < 256)
	return change_address (mem, mode, gen_rtx_LO_SUM (Pmode, XEXP (x, 0),
			       GEN_INT (offset)));
    }*/
  
  return adjust_address (x, mode, adj);
}

static int
m65x_address_cost (rtx address, enum machine_mode mode, addr_space_t as,
		   bool speed)
{
  if (GET_CODE (address) == PLUS
      && REG_P (XEXP (address, 0))
      && CONST_INT_P (XEXP (address, 1))
      && INTVAL (XEXP (address, 1)) >= 0
      && INTVAL (XEXP (address, 1)) < 256)
    return 6;
  else if (GET_CODE (address) == PLUS
	   && REG_P (XEXP (address, 0)) && REG_P (XEXP (address, 1)))
    return 4;
  else if (CONSTANT_ADDRESS_P (address))
    return 2;
  else
    return 8;
}

static int
m65x_register_move_cost (enum machine_mode mode, reg_class_t from,
			 reg_class_t to)
{
  if (mode == QImode && (HARD_REG_CLASS_P (to) || HARD_REG_CLASS_P (from)))
    return 2;
  /*else if (mode == HImode && HARDISH_REG_CLASS_P (to))
    return 2;*/
  else if (ZP_REG_CLASS_P (from) && ZP_REG_CLASS_P (to))
    return 6;

  return 4;
}

static HOST_WIDE_INT
adjust_offset_for_reload_type (enum machine_mode mode, reload_type reltype,
			       HOST_WIDE_INT offset)
{
  if (mode != HImode)
    return offset;

  switch (reltype)
    {
    case RELOAD_FOR_OUTPUT_ADDRESS:
    case RELOAD_FOR_OUTADDR_ADDRESS:
      return offset;
    
    case RELOAD_FOR_INPUT_ADDRESS:
    case RELOAD_FOR_INPADDR_ADDRESS:
      return offset + 1;
    
    default:
      debug_reload_to_stream (stderr);
      abort ();
    }
}

rtx
m65x_legitimize_reload_address (rtx *px, enum machine_mode mode, int opnum,
				int type, int ind_levels ATTRIBUTE_UNUSED)
{
  rtx x = *px;

#ifdef DEBUG_LEGIT_RELOAD
  fprintf (stderr, "m65x_legitimize_reload_address\n");
  debug_rtx (x);
#endif

  return NULL_RTX;

  if (GET_MODE (x) != HImode)
    return NULL_RTX;

  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && CONST_INT_P (XEXP (x, 1))
      && INTVAL (XEXP (x, 1)) >= 0
      && INTVAL (XEXP (x, 1)) < 256)
    {
      HOST_WIDE_INT offs = adjust_offset_for_reload_type (mode,
			     (reload_type) type, INTVAL (XEXP (x, 1)));
      rtx zext = gen_rtx_ZERO_EXTEND (HImode, GEN_INT (offs));
      rtx sum = gen_rtx_LO_SUM (HImode, XEXP (x, 0), zext);
      push_reload (XEXP (zext, 0), NULL_RTX, &XEXP (zext, 0), NULL, HARD_Y_REG,
		   QImode, VOIDmode, 0, 0, opnum, (reload_type) type);
      if (!REGNO_OK_FOR_BASE_P (REGNO (XEXP (x, 0))))
	push_reload (XEXP (sum, 0), NULL_RTX, &XEXP (sum, 0), NULL,
		     GENERAL_REGS, HImode, VOIDmode, 0, 0, opnum,
		     (reload_type) type);
      return sum;
    }
  else if (REG_P (x))
    {
      HOST_WIDE_INT offs = adjust_offset_for_reload_type (mode,
			     (reload_type) type, 0);
      rtx zext = gen_rtx_ZERO_EXTEND (HImode, GEN_INT (offs));
      rtx sum = gen_rtx_LO_SUM (HImode, x, zext);
      push_reload (XEXP (zext, 0), NULL_RTX, &XEXP (zext, 0), NULL, HARD_Y_REG,
		   QImode, VOIDmode, 0, 0, opnum, (reload_type) type);
      if (!REGNO_OK_FOR_BASE_P (REGNO (x)))
	push_reload (XEXP (sum, 0), NULL_RTX, &XEXP (sum, 0), NULL,
		     GENERAL_REGS, HImode, VOIDmode, 0, 0, opnum,
		     (reload_type) type);
      return sum;
    }
  else if (GET_CODE (x) == LO_SUM
	   && REG_P (XEXP (x, 0))
	   && GET_CODE (XEXP (x, 1)) == ZERO_EXTEND
	   && CONST_INT_P (XEXP (XEXP (x, 1), 0))
	   && INTVAL (XEXP (XEXP (x, 1), 0)) >= 0
	   && INTVAL (XEXP (XEXP (x, 1), 0)) < 256)
    {
      push_reload (XEXP (XEXP (x, 1), 0), NULL_RTX, &XEXP (XEXP (x, 1), 0),
		   NULL, HARD_Y_REG, QImode, VOIDmode, 0, 0, opnum,
		   (reload_type) type);
      if (!REGNO_OK_FOR_BASE_P (REGNO (XEXP (x, 0))))
	push_reload (XEXP (x, 0), NULL_RTX, &XEXP (x, 0), NULL, GENERAL_REGS,
		     HImode, VOIDmode, 0, 0, opnum, (reload_type) type);
      return x;
    }
  
  return NULL_RTX;
}

static rtx
m65x_function_arg (cumulative_args_t ca, enum machine_mode mode,
		   const_tree type,
		   bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  int modesize;

  if (mode == BLKmode)
    modesize = int_size_in_bytes (type);
  else
    modesize = GET_MODE_SIZE (mode);

  if ((*pcum) + modesize <= 8)
    return gen_rtx_REG (mode, (*pcum) + FIRST_ARG_REGISTER);
  else
    return NULL_RTX;
}

static void
m65x_function_arg_advance (cumulative_args_t ca, enum machine_mode mode,
			   const_tree type, bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  int modesize;
  
  if (mode == BLKmode)
    modesize = int_size_in_bytes (type);
  else
    modesize = GET_MODE_SIZE (mode);
  
  (*pcum) += modesize;
}

static rtx
m65x_function_value (const_tree ret_type,
		     const_tree fn_decl_or_type ATTRIBUTE_UNUSED,
		     bool outgoing ATTRIBUTE_UNUSED)
{
  enum machine_mode mode;
  
  mode = TYPE_MODE (ret_type);
  
  return gen_rtx_REG (mode, FIRST_ARG_REGISTER);
}

static void
m65x_setup_incoming_varargs (cumulative_args_t ca, enum machine_mode mode,
			     tree type, int *pretend_size, int second_time)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  *pretend_size = 8 - *pcum;
}

static rtx
m65x_libcall_value (enum machine_mode mode, const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, FIRST_ARG_REGISTER);
}

static void
m65x_asm_globalize_label (FILE *stream, const char *name)
{
  fprintf (stream, "\t.export %s%s\n", name, strlen (name) == 1 ? "$" : "");
}

static void
m65x_asm_named_section (const char *name, unsigned int flags ATTRIBUTE_UNUSED,
			tree decl ATTRIBUTE_UNUSED)
{
  fprintf (asm_out_file, "\t.segment \"%s\"\n", name);
}

static section *
m65x_asm_function_section (tree decl ATTRIBUTE_UNUSED,
			   enum node_frequency freq ATTRIBUTE_UNUSED,
			   bool startup ATTRIBUTE_UNUSED,
			   bool exit ATTRIBUTE_UNUSED)
{
  /* We could actually put startup code in a different segment, but not yet.  */
  return get_named_text_section (decl, "CODE", NULL);
}

static section *
m65x_asm_select_section (tree exp ATTRIBUTE_UNUSED,
			 int reloc ATTRIBUTE_UNUSED,
			 unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  const char *sname;
  
  switch (categorize_decl_for_section (exp, reloc))
    {
    case SECCAT_BSS:
      sname = "BSS";
      break;
    
    case SECCAT_RODATA:
    case SECCAT_RODATA_MERGE_STR:
    case SECCAT_RODATA_MERGE_STR_INIT:
    case SECCAT_RODATA_MERGE_CONST:
      sname = "RODATA";
      break;
    
    case SECCAT_DATA:
      sname = "DATA";
      break;
    
    case SECCAT_TEXT:
      sname = "CODE";
      break;
    
    default:
      sname = "UNKNOWN";
    }
  
  return get_named_section (exp, sname, reloc);
}

bool
m65x_hard_regno_mode_ok (int regno, enum machine_mode mode)
{
  HOST_WIDE_INT modesize = GET_MODE_SIZE (mode);

  if (modesize == 1)
    return true;

  if (modesize <= 0)
    return regno >= 12;

  /* For the "hard" registers, force values to have the actual LSB in the hard
     register for greater-than-byte-size modes.  */
  if (regno < 12)
    return false; // (regno % modesize) == 0;
  else
    return true;
}

/* Model the accumulator, X and Y registers as able to hold any value up to 32
   bits in size.  */

HOST_WIDE_INT
m65x_hard_regno_nregs (int regno ATTRIBUTE_UNUSED, enum machine_mode mode)
{
  HOST_WIDE_INT modesize = GET_MODE_SIZE (mode);
  
  return modesize;
}

static reg_class_t
m65x_spill_class (reg_class_t klass, enum machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (klass)
    {
    case HARD_ACCUM_REG:
    case HARD_X_REG:
    case HARD_Y_REG:
      return GENERAL_REGS;
    
    default:
      return NO_REGS;
    }
}

static bool
base_plus_const_byte_offset_mem (enum machine_mode mode, rtx x)
{
  int modesize = GET_MODE_SIZE (mode);
  
  if (!MEM_P (x))
    return false;
  
  x = XEXP (x, 0);

  if (REG_P (x))
    return true;

  if (0 && GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && CONST_INT_P (XEXP (x, 1))
      && INTVAL (XEXP (x, 1)) >= 0
      && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
    return true;
  
  if (0 && GET_CODE (x) == LO_SUM && REG_P (XEXP (x, 0)))
    {
      if (CONST_INT_P (XEXP (x, 1))
          && INTVAL (XEXP (x, 1)) >= 0
	  && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
	return true;

      if (REG_P (XEXP (x, 1)))
        return true;
    }

  return false;
}

static reg_class_t
m65x_secondary_reload (bool in_p, rtx x, reg_class_t reload_class,
		       enum machine_mode reload_mode,
		       secondary_reload_info *sri)
{
#ifdef DEBUG_SECONDARY_RELOAD
  fprintf (stderr, "reload-%s x, class=%s\n", in_p ? "in" : "out",
	   m65x_reg_class_name (reload_class));
  debug_rtx (x);
#endif

  /* If IN_P, X needs to be copied to a register of class RELOAD_CLASS,
     else a register of class RELOAD_CLASS needs to be copied to X.  */

  if (reload_mode == QImode)
    {
      if (base_plus_const_byte_offset_mem (QImode, x))
	{
	  if (reload_class == HARD_ACCUM_REG)
	    return NO_REGS;
	  else
	    return HARD_ACCUM_REG;
	}
      else if (ZP_REG_CLASS_P (reload_class) && REG_P (x)
	       && IS_ZP_REGNUM (REGNO (x)))
	return ACTUALLY_HARD_REGS;
      /*else if (REG_P (x) && IS_ZP_REGNUM (REGNO (x)))
        {
	  if (reg_classes_intersect_p (reload_class, GENERAL_REGS))
	    {
	      sri->icode = CODE_FOR_reload_inoutqi_zp;
	      return NO_REGS;
	    }
	}*/
#ifdef DEBUG_SECONDARY_RELOAD
      else
        {
	  fprintf (stderr, "non base-plus-mem (qimode):\n");
	  debug_rtx (x);
	}
#endif
    }
#if 0
  else if (reload_mode == HImode)
    {
      if (base_plus_const_byte_offset_mem (HImode, x))
        {
	  if (reload_class == WORD_ACCUM_REGS)
	    return NO_REGS;
	  else
	    return WORD_ACCUM_REGS;
	}
      /*else if (in_p && REG_P (x) && IS_ZP_REGNUM (REGNO (x)))
	sri->icode = CODE_FOR_reload_inhi_indexreg;*/
    }
  else if (reload_mode == SImode)
    {
      if (base_plus_const_byte_offset_mem (SImode, x))
	{
	  if (reload_class == ACCUM_REGS)
	    return NO_REGS;
	  else
	    return ACCUM_REGS;
	}
    }
#endif

  return NO_REGS;
}

static bool
m65x_lra_p (void)
{
  /* Reload sux!  */
  return m65x_lra_flag;
}

bool
m65x_valid_mov_operands (enum machine_mode mode, rtx *operands)
{
  bool retval = false;
#define return retval =

  if (0 && reload_in_progress)
    {
      if ((REG_P (operands[0]) || GET_CODE (operands[0]) == SUBREG)
	  && true_regnum (operands[0]) == -1)
	return false;

      if ((REG_P (operands[1]) || GET_CODE (operands[1]) == SUBREG)
	  && true_regnum (operands[1]) == -1)
	return false;
    }

#if 0
  fprintf (stderr, "valid mov operands? op0=\n");
  debug_rtx (operands[0]);
  fprintf (stderr, "op1=\n");
  debug_rtx (operands[1]);
#endif
  
  if (MEM_P (operands[0]))
    {
      if (GET_CODE (XEXP (operands[0], 0)) == POST_DEC
	  || GET_CODE (XEXP (operands[0], 0)) == PRE_INC)
        return mode == QImode && hard_reg_operand (operands[1], mode);
      else
	return (register_operand (operands[1], mode)
		|| immediate_operand (operands[1], mode));
    }
  else if (MEM_P (operands[1]))
    {
      if (GET_CODE (XEXP (operands[1], 0)) == POST_DEC
	  || GET_CODE (XEXP (operands[1], 0)) == PRE_INC)
	return mode == QImode && hard_reg_operand (operands[0], mode);
      else
	return register_operand (operands[0], mode);
    }
  else
    return (accumulator_operand (operands[0], mode)
            && (index_reg_operand (operands[1], mode)
	        || ptr_reg_operand (operands[1], mode)))
	   || (index_reg_operand (operands[0], mode)
	       && accumulator_operand (operands[1], mode))
           || (hard_reg_operand (operands[0], mode)
	       && (ptr_reg_operand (operands[1], mode)
		   || immediate_operand (operands[1], mode)))
	   || (ptr_reg_operand (operands[0], mode)
	       && (hard_reg_operand (operands[1], mode)
		   /*|| ptr_reg_operand (operands[1], mode)*/
		   || immediate_operand (operands[1], mode)))
	   || (y_reg_operand (operands[0], mode)
	       && x_reg_operand (operands[1], mode))
	   || (x_reg_operand (operands[0], mode)
	       && y_reg_operand (operands[1], mode));
#undef return
#if 0
  fprintf (stderr, "returning %s\n", retval ? "true" : "false");
#endif
  return retval;
}

static void
m65x_canonicalize_comparison (int *code, rtx *op0, rtx *op1,
			      bool op0_preserve_value)
{
  rtx tmp;
  bool swap = false;

  if (!op0_preserve_value
      && (*code == GTU || *code == LEU || *code == GT || *code == LE))
    {
      *code = (int) swap_condition ((enum rtx_code) *code);
      tmp = *op0;
      *op0 = *op1;
      *op1 = tmp;
    }
}

static void
m65x_emit_cbranchqi (enum rtx_code cond, rtx cc_reg, int prob, rtx dest)
{
  rtx cmp = gen_rtx_fmt_ee (cond, VOIDmode, cc_reg, const0_rtx);
  rtx branch = gen_rtx_SET (VOIDmode, pc_rtx,
		 gen_rtx_IF_THEN_ELSE (VOIDmode, cmp,
		   gen_rtx_LABEL_REF (Pmode, dest), pc_rtx));
  rtx jmp_insn = emit_jump_insn (branch);
  if (prob != -1)
    add_int_reg_note (jmp_insn, REG_BR_PROB, prob);
}

void
m65x_emit_qimode_comparison (enum rtx_code cond, rtx op0, rtx op1, rtx dest)
{
  rtx cmp;
  rtx scratch;
  rtx new_label;
  rtx nzflags = gen_rtx_REG (CC_NZmode, NZ_REGNUM);
  rtx vflags = gen_rtx_REG (CC_Vmode, OVERFLOW_REGNUM);
  rtx cmpreg;

  if (CONSTANT_P (op0))
    op0 = force_reg (QImode, op0);

  switch (cond)
    {
    case EQ:
    case NE:
      cmpreg = gen_rtx_REG (CC_NZmode, NZ_REGNUM);
      goto emit_cmp;
    case LTU:
    case GEU:
      cmpreg = gen_rtx_REG (CC_Cmode, CARRY_REGNUM);
      cond = (cond == LTU) ? EQ : NE;
    emit_cmp:
      emit_insn (gen_compareqi (op0, op1));
      cmp = gen_rtx_fmt_ee (cond, VOIDmode, cmpreg, const0_rtx);
      emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx,
				   gen_rtx_IF_THEN_ELSE (VOIDmode, cmp,
				     gen_rtx_LABEL_REF (Pmode, dest), pc_rtx)));
      break;

    case LT:
    case GE:
      scratch = gen_reg_rtx (QImode);
      new_label = gen_label_rtx ();
      emit_move_insn (scratch, op0);
      emit_insn (gen_sec ());
      emit_insn (gen_sbcqi3_nzv (scratch, scratch, op1));
      m65x_emit_cbranchqi (EQ, vflags,
			   split_branch_probability == -1 ? -1 :
			   split_branch_probability / 2, new_label);
      emit_insn (gen_negate_highbit (scratch, scratch));
      emit_label (new_label);
      m65x_emit_cbranchqi (cond, nzflags, split_branch_probability, dest);
      break;
    
    default:
      gcc_unreachable ();
    }
}

void
m65x_emit_himode_comparison (enum rtx_code cond, rtx op0, rtx op1, rtx dest,
			     rtx scratch)
{
  rtx op0_lo = simplify_gen_subreg (QImode, op0, HImode, 0);
  rtx op1_lo = simplify_gen_subreg (QImode, op1, HImode, 0);
  rtx op0_hi = simplify_gen_subreg (QImode, op0, HImode, 1);
  rtx op1_hi = simplify_gen_subreg (QImode, op1, HImode, 1);
  rtx nzflags = gen_rtx_REG (CC_NZmode, NZ_REGNUM);
  rtx vflag = gen_rtx_REG (CC_Vmode, OVERFLOW_REGNUM);
  rtx cflag = gen_rtx_REG (CC_Cmode, CARRY_REGNUM);
  rtx new_label = NULL_RTX;
  int rev_prob = REG_BR_PROB_BASE - split_branch_probability;

  if ((cond == EQ || cond == NE || cond == LT || cond == GE)
      && REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
    {
      emit_move_insn (scratch, op0_lo);
      op0_lo = scratch;
    }
  
  if (cond != NE)
    new_label = gen_label_rtx ();

  switch (cond)
    {
    case EQ:
      /* Low part.  */
      emit_insn (gen_compareqi (op0_lo, op1_lo));
      m65x_emit_cbranchqi (NE, nzflags, rev_prob, new_label);

      /* High part.  */
      emit_move_insn (scratch, op0_hi);
      emit_insn (gen_compareqi (scratch, op1_hi));
      m65x_emit_cbranchqi (EQ, nzflags, split_branch_probability, dest);
      emit_label (new_label);
      break;

    case NE:
      /* Low part.  */
      emit_insn (gen_compareqi (op0_lo, op1_lo));
      m65x_emit_cbranchqi (NE, nzflags, split_branch_probability, dest);

      /* High part.  */
      emit_move_insn (scratch, op0_hi);
      emit_insn (gen_compareqi (scratch, op1_hi));
      m65x_emit_cbranchqi (NE, nzflags, split_branch_probability, dest);
      break;
    
    case LTU:
      /* High part.  */
      emit_move_insn (scratch, op0_hi);
      emit_insn (gen_compareqi (scratch, op1_hi));
      m65x_emit_cbranchqi (EQ, cflag, split_branch_probability, dest);
      m65x_emit_cbranchqi (NE, nzflags, split_branch_probability, new_label);

      /* Low part.  */
      if (REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
	{
	  emit_move_insn (scratch, op0_lo);
	  op0_lo = scratch;
	}

      emit_insn (gen_compareqi (op0_lo, op1_lo));
      m65x_emit_cbranchqi (EQ, cflag, split_branch_probability, dest);
      emit_label (new_label);
      break;
    
    case GEU:
      /* High part.  */
      emit_move_insn (scratch, op1_hi);
      emit_insn (gen_compareqi (scratch, op0_hi));
      m65x_emit_cbranchqi (EQ, cflag, split_branch_probability, dest);
      m65x_emit_cbranchqi (NE, nzflags, split_branch_probability, new_label);

      /* Low part.  */
      if (REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
	{
	  emit_move_insn (scratch, op0_lo);
	  op0_lo = scratch;
	}

      emit_insn (gen_compareqi (op0_lo, op1_lo));
      m65x_emit_cbranchqi (NE, cflag, split_branch_probability, dest);
      emit_label (new_label);
      break;
    
    case LT:
    case GE:
      emit_insn (gen_compareqi (op0_lo, op1_lo));
      emit_move_insn (scratch, op0_hi);
      emit_insn (gen_sbcqi3_nzv (scratch, scratch, op1_hi));
      m65x_emit_cbranchqi (EQ, vflag, split_branch_probability / 2,
			   new_label);
      emit_insn (gen_negate_highbit (scratch, scratch));
      emit_label (new_label);
      m65x_emit_cbranchqi (cond, nzflags, split_branch_probability, dest);
      break;
    
    default:
      gcc_unreachable ();
    }
}

/* We have:

    __________________________
    |                        |
    |     incoming args      |
    |                        |
    |________________________|  <-- old stack pointer
    |      pretend args      |
    |________________________|  <-- soft arg pointer
    |                        |
    |   locals (frame size)  |
    |                        |
    |________________________|  <-- soft/hard frame pointer
    |                        |
    |      outgoing args     |
    |                        |
    |________________________|  <-- current/outgoing stack pointer
    
*/

HOST_WIDE_INT
m65x_elimination_offset (int from, int to)
{
  HOST_WIDE_INT frame_size = get_frame_size ();
  
  if (from == ARG_POINTER_REGNUM)
    switch (to)
      {
      case STACK_POINTER_REGNUM:
        return -(crtl->outgoing_args_size + frame_size);
      case FRAME_POINTER_REGNUM:
        return -frame_size;
      case HARD_FRAME_POINTER_REGNUM:
        return -frame_size;
      default:
        gcc_unreachable ();
      }
  else if (from == FRAME_POINTER_REGNUM)
    switch (to)
      {
      case STACK_POINTER_REGNUM:
        return -crtl->outgoing_args_size;
      case HARD_FRAME_POINTER_REGNUM:
        return 0;
      default:
        gcc_unreachable ();
      }
  else
    gcc_unreachable ();
}

static bool
m65x_can_eliminate (const int from, const int to)
{
  if ((from == ARG_POINTER_REGNUM && to == FRAME_POINTER_REGNUM)
      || (frame_pointer_needed && to == STACK_POINTER_REGNUM))
    return false;
  
  return true;
}

/* See ASCII diagram for m65x_elimination_offset.  */

void
m65x_expand_prologue (void)
{
  int regno;
  rtx accum = gen_rtx_REG (QImode, ACC_REGNUM), insn;
  HOST_WIDE_INT frame_size = get_frame_size ();
  /*HOST_WIDE_INT stack_offset = frame_size + crtl->outgoing_args_size;*/
  rtx yreg = gen_rtx_REG (QImode, Y_REGNUM);
  int args_pushed = crtl->args.pretend_args_size;
  rtx push_rtx = gen_rtx_MEM (QImode,
		   gen_rtx_POST_DEC (HImode,
				     gen_rtx_REG (HImode, HARDSP_REGNUM)));
  
  /* Push SP if we modify it.  */
  if (crtl->args.pretend_args_size + frame_size + crtl->outgoing_args_size != 0)
    for (regno = 1; regno >= 0; regno--)
      {
	emit_insn (gen_movqi (accum, gen_rtx_REG (QImode, SP_REGNUM + regno)));
	insn = emit_insn (gen_pushqi1 (push_rtx, accum));
	RTX_FRAME_RELATED_P (insn) = 1;
      }
  
  /* Likewise, push FP if we are going to modify/use it.  */
  if (frame_pointer_needed)
    for (regno = 1; regno >= 0; regno--)
      {
	emit_insn (gen_movqi (accum, gen_rtx_REG (QImode, FP_REGNUM + regno)));
	insn = emit_insn (gen_pushqi1 (push_rtx, accum));
	RTX_FRAME_RELATED_P (insn) = 1;
      }
  
  if (crtl->args.pretend_args_size > 0)
    {
      /* We don't even pretend (no pun intended!) to make varargs functions
         efficient...  */
      insn = emit_insn (gen_addhi3 (stack_pointer_rtx, stack_pointer_rtx,
				    GEN_INT (-args_pushed)));
      RTX_FRAME_RELATED_P (insn) = 1;
      insn = emit_move_insn (yreg, const0_rtx);
      RTX_FRAME_RELATED_P (insn) = 1;

      for (regno = 8 - crtl->args.pretend_args_size; regno < 8; regno++)
        {
	  insn = emit_move_insn (accum,
		   gen_rtx_REG (QImode, FIRST_ARG_REGISTER + regno));
	  RTX_FRAME_RELATED_P (insn) = 1;
	  insn = emit_insn (gen_storeqi_indy (yreg, stack_pointer_rtx, accum));
	  RTX_FRAME_RELATED_P (insn) = 1;
	  if (regno < 7)
	    {
	      insn = emit_insn (gen_addqi3 (yreg, yreg, const1_rtx));
	      RTX_FRAME_RELATED_P (insn) = 1;
	    }
	}
    }

  /* SP now points at "soft arg pointer".  */

  if (frame_pointer_needed)
    {
      insn = emit_insn (gen_addhi3 (hard_frame_pointer_rtx, stack_pointer_rtx,
				    GEN_INT (-frame_size)));
      RTX_FRAME_RELATED_P (insn) = 1;
    }
  
  if (frame_size + crtl->outgoing_args_size != 0)
    {
      insn = emit_insn (gen_addhi3 (stack_pointer_rtx, stack_pointer_rtx,
			  GEN_INT (-(frame_size + crtl->outgoing_args_size))));
      RTX_FRAME_RELATED_P (insn) = 1;
    }

  for (regno = LAST_CALLER_SAVED; regno >= FIRST_CALLER_SAVED; regno--)
    if (df_regs_ever_live_p (regno))
      {
        emit_insn (gen_movqi (accum, gen_rtx_REG (QImode, regno)));
	insn = emit_insn (gen_pushqi1 (push_rtx, accum));
	RTX_FRAME_RELATED_P (insn) = 1;
      }
}

void
m65x_expand_epilogue (void)
{
  int regno;
  rtx accum = gen_rtx_REG (QImode, ACC_REGNUM), insn;
  HOST_WIDE_INT frame_size = get_frame_size ();
  HOST_WIDE_INT stack_offset = frame_size + crtl->outgoing_args_size
			       + crtl->args.pretend_args_size;
  rtx pop_rtx = gen_rtx_MEM (QImode,
		   gen_rtx_PRE_INC (HImode,
				    gen_rtx_REG (HImode, HARDSP_REGNUM)));

  emit_insn (gen_blockage ());

  for (regno = FIRST_CALLER_SAVED; regno <= LAST_CALLER_SAVED; regno++)
    if (df_regs_ever_live_p (regno))
      {
        emit_insn (gen_popqi1 (accum, pop_rtx));
	emit_insn (gen_movqi (gen_rtx_REG (QImode, regno), accum));
      }
  
  if (frame_pointer_needed)
    for (regno = 0; regno <= 1; regno++)
      {
        emit_insn (gen_popqi1 (accum, pop_rtx));
	emit_insn (gen_movqi (gen_rtx_REG (QImode, FP_REGNUM + regno), accum));
      }
  
  if (stack_offset != 0)
    {
      for (regno = 0; regno <= 1; regno++)
	{
          emit_insn (gen_popqi1 (accum, pop_rtx));
	  emit_insn (gen_movqi (gen_rtx_REG (QImode, SP_REGNUM + regno),
				accum));
	}
  
      emit_use (gen_rtx_REG (HImode, SP_REGNUM));
    }
  
  if (frame_pointer_needed)
    emit_use (gen_rtx_REG (HImode, FP_REGNUM));
  
  emit_jump_insn (gen_m65x_return ());
}

static bool
m65x_scalar_mode_supported_p (enum machine_mode mode)
{
  if (mode == SImode)
    return true;
  
  return default_scalar_mode_supported_p (mode);
}

#undef TARGET_ASM_FILE_START
#define TARGET_ASM_FILE_START m65x_file_start

#undef TARGET_ASM_BYTE_OP
#define TARGET_ASM_BYTE_OP "\t.byte\t"

#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP "\t.word\t"

#undef TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP "\t.dword\t"

#undef TARGET_ASM_ALIGNED_DI_OP
#define TARGET_ASM_ALIGNED_DI_OP NULL

#undef TARGET_ASM_ALIGNED_TI_OP
#define TARGET_ASM_ALIGNED_TI_OP NULL

#undef TARGET_ASM_UNALIGNED_HI_OP
#define TARGET_ASM_UNALIGNED_HI_OP "\t.word\t"

#undef TARGET_ASM_UNALIGNED_SI_OP
#define TARGET_ASM_UNALIGNED_SI_OP "\t.dword\t"

#undef TARGET_ASM_UNALIGNED_DI_OP
#define TARGET_ASM_UNALIGNED_DI_OP NULL

#undef TARGET_ASM_UNALIGNED_TI_OP
#define TARGET_ASM_UNALIGNED_TI_OP NULL

#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG m65x_function_arg

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE m65x_function_arg_advance

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE m65x_function_value

#undef TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS m65x_setup_incoming_varargs

#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE m65x_libcall_value

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P m65x_legitimate_address_p

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS m65x_legitimize_address

#undef TARGET_DELEGITIMIZE_ADDRESS
#define TARGET_DELEGITIMIZE_ADDRESS m65x_delegitimize_address

#undef TARGET_MODE_DEPENDENT_ADDRESS
#define TARGET_MODE_DEPENDENT_ADDRESS m65x_mode_dependent_address

#undef TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P
#define TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P hook_bool_mode_true

#undef TARGET_SPILL_CLASS
#define TARGET_SPILL_CLASS m65x_spill_class

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD m65x_secondary_reload

#undef TARGET_LRA_P
#define TARGET_LRA_P m65x_lra_p

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST m65x_address_cost

#undef TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST m65x_register_move_cost

#undef TARGET_CANONICALIZE_COMPARISON
#define TARGET_CANONICALIZE_COMPARISON m65x_canonicalize_comparison

#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION m65x_asm_named_section

#undef TARGET_ASM_FUNCTION_SECTION
#define TARGET_ASM_FUNCTION_SECTION m65x_asm_function_section

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION m65x_asm_select_section

#undef TARGET_ASM_GLOBALIZE_LABEL
#define TARGET_ASM_GLOBALIZE_LABEL m65x_asm_globalize_label

#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE m65x_can_eliminate

struct gcc_target targetm = TARGET_INITIALIZER;
