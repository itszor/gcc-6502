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

static void
m65x_file_start (void)
{
  int i;
  int zploc = 0x70;
  
  fprintf (asm_out_file, "\t.feature at_in_identifiers\n");
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
  
  fprintf (asm_out_file, "\t.define _sp $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _sph $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _fp $%x\n", zploc++);
  fprintf (asm_out_file, "\t.define _fph $%x\n", zploc++);
  
  for (i = 0; i < 8; i++)
    fprintf (asm_out_file, "\t.define _a%d $%x\n", i, zploc++);

  for (i = 0; i < 8; i++)
    fprintf (asm_out_file, "\t.define _s%d $%x\n", i, zploc++);
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
	
	default:
	  output_addr_const (stream, x);
	}
    }
}

void
m65x_print_operand_address (FILE *stream, rtx x)
{
  switch (GET_CODE (x))
    {
    case PLUS:
      asm_fprintf (stream, "<foo>+<bar>");
      break;

    case REG:
      asm_fprintf (stream, "(%r)", REGNO (x));
      break;

    default:
      output_addr_const (stream, x);
    }
}

void
m65x_print_branch (enum machine_mode mode, rtx cond, rtx dest)
{
  const char *fmt;

  switch (mode)
    {
    case CCmode:
      /* These are used for unsigned comparisons.  */
      switch (GET_CODE (cond))
        {
	case EQ: fmt = "beq %0"; break;
	case NE: fmt = "bne %0"; break;
	case LTU: fmt = "bcc %0"; break;
	case GEU: fmt = "bcs %0"; break;
	default: gcc_unreachable ();
	}
      break;

    case CC_NVmode:
      /* These are used to synthesize signed comparisons.  */
      switch (GET_CODE (cond))
	{
	case EQ: fmt = "bvc %0"; break;
	case NE: fmt = "bvs %0"; break;
	case GE: fmt = "bpl %0"; break;
	case LT: fmt = "bmi %0"; break;
	default: gcc_unreachable ();
	}
      break;

    default:
      gcc_unreachable ();
    }
  
  output_asm_insn (fmt, &dest);
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
m65x_address_register_p (rtx x, int strict_p)
{
  int regno;

  if (!REG_P (x))
    return false;
    
  if (strict_p)
    {
      regno = true_regnum (x);
      return regno != -1 && IS_ZP_REGNUM (regno);
    }

  regno = REGNO (x);

  return (IS_ZP_REGNUM (regno)
	  || regno >= FIRST_PSEUDO_REGISTER
	  || regno == FRAME_POINTER_REGNUM
	  || regno == ARG_POINTER_REGNUM);
}

bool
m65x_legitimate_address_p (enum machine_mode mode, rtx x, bool strict)
{
  if (CONSTANT_P (x) || GET_CODE (x) == LABEL_REF || GET_CODE (x) == SYMBOL_REF)
    return true;
  
  if (GET_CODE (x) == CONST)
    x = XEXP (x, 0);
  
  if (m65x_address_register_p (x, strict))
    return true;

  if (GET_CODE (x) == PLUS)
    {
      HOST_WIDE_INT modesize = GET_MODE_SIZE (mode);

      if (m65x_address_register_p (XEXP (x, 0), strict)
	  && GET_CODE (XEXP (x, 1)) == CONST_INT
	  && INTVAL (XEXP (x, 1)) >= 0
	  && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
	return true;

#if 0
      if (m65x_address_register_p (XEXP (x, 0), strict)
	  && REG_P (XEXP (x, 1))
	  && REGNO (XEXP (x, 1)) == HARD_Y_REGNUM)
	return true;

      if (CONSTANT_P (XEXP (x, 0))
	  && REG_P (XEXP (x, 1))
	  && (REGNO (XEXP (x, 1)) == HARD_X_REGNUM
	      || REGNO (XEXP (x, 1)) == HARD_Y_REGNUM))
	return true;
#endif
    }

  return false;
}

static bool
m65x_pseudo_register (rtx op)
{
  if (!REG_P (op) && GET_CODE (op) != SUBREG)
    return false;

  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);

  if (REG_P (op)
      && (REGNO (op) == ARG_POINTER_REGNUM
	  || REGNO (op) == FRAME_POINTER_REGNUM
	  || REGNO (op) >= FIRST_PSEUDO_REGISTER))
    return true;

  return false;
}

/* A simple address -- one which does not require any index registers to
   form.  Mostly this is just constant addresses, but maybe it could be
   zp-indirect on 65x variants supporting that.  */

bool
m65x_simple_address_p (enum machine_mode mode, rtx x)
{
  /* Allow (mem (reg)) addresses with pseudo registers, otherwise the compiler
     gets upset.  */
  if (m65x_pseudo_register (x)
      || (GET_CODE (x) == PLUS && GET_CODE (XEXP (x, 1)) == CONST_INT
	  && m65x_pseudo_register (XEXP (x, 0))))
    return true;

  return (CONSTANT_P (x) || GET_CODE (x) == LABEL_REF
	  || GET_CODE (x) == SYMBOL_REF);
}

int
m65x_mode_dependent_address_p (rtx x ATTRIBUTE_UNUSED)
{
  /* Another big lie!  */
  return 0;
}

static rtx
m65x_function_arg (cumulative_args_t ca, enum machine_mode mode,
		   const_tree type ATTRIBUTE_UNUSED,
		   bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);

  if (*pcum < 8)
    return gen_rtx_REG (mode, (*pcum) + FIRST_ARG_REGISTER);
  else
    return NULL_RTX;
}

static void
m65x_function_arg_advance (cumulative_args_t ca, enum machine_mode mode,
			   const_tree type ATTRIBUTE_UNUSED,
			   bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  
  (*pcum) += GET_MODE_SIZE (mode);
}

static rtx
m65x_function_value (const_tree ret_type,
		     const_tree fn_decl_or_type ATTRIBUTE_UNUSED,
		     bool outgoing ATTRIBUTE_UNUSED)
{
  enum machine_mode mode;
  
  mode = TYPE_MODE (ret_type);
  
  return gen_rtx_REG (mode, ACC_REGNUM);
}

static rtx
m65x_libcall_value (enum machine_mode mode, const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, ACC_REGNUM);
}

static void
m65x_asm_globalize_label (FILE *stream, const char *name)
{
  fprintf (stream, "\t.export %s\n", name);
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
      sname = "RODATA";
      break;
    
    case SECCAT_DATA:
      sname = "DATA";
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
    return (regno % modesize) == 0;
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

static const char * __attribute__ ((used))
m65x_reg_class_name (reg_class_t c)
{
  switch (c)
    {
    case NO_REGS: return "NO_REGS";
    case HARD_ACCUM_REG: return "HARD_ACCUM_REG";
    case ACCUM_REGS: return "ACCUM_REGS";
    case WORD_ACCUM_REGS: return "WORD_ACCUM_REGS";
    case HARD_X_REG: return "HARD_X_REG";
    case WORD_X_REGS: return "WORD_X_REGS";
    case X_REGS: return "X_REGS";
    case HARD_Y_REG: return "HARD_Y_REG";
    case WORD_Y_REGS: return "WORD_Y_REGS";
    case Y_REGS: return "Y_REGS";
    case HARD_INDEX_REGS: return "HARD_INDEX_REGS";
    case INDEX_REGS: return "INDEX_REGS";
    case HARD_REGS: return "HARD_REGS";
    case HARDISH_REGS: return "HARDISH_REGS";
    case STACK_REG: return "STACK_REG";
    case ARG_REGS: return "ARG_REGS";
    case CALLEE_SAVED_REGS: return "CALLEE_SAVED_REGS";
    case CC_REGS: return "CC_REGS";
    case GENERAL_REGS: return "GENERAL_REGS";
    case ALL_REGS: return "ALL_REGS";
    default: gcc_unreachable ();
    }
  
  return NULL;
}

static bool
base_plus_const_byte_offset_mem (enum machine_mode mode, rtx x)
{
  int modesize = GET_MODE_SIZE (mode);
  
  if (!MEM_P (x))
    return false;
  
  x = XEXP (x, 0);

  if (REG_P (x) && REGNO_OK_FOR_BASE_P (true_regnum (x)))
    return true;

  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && REGNO_OK_FOR_BASE_P (true_regnum (XEXP (x, 0)))
      && GET_CODE (XEXP (x, 1)) == CONST_INT
      && INTVAL (XEXP (x, 1)) >= 0
      && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
    return true;

  return false;
}

static reg_class_t
m65x_secondary_reload (bool in_p, rtx x, reg_class_t reload_class,
		       enum machine_mode reload_mode,
		       secondary_reload_info *sri)
{
#if 0
  fprintf (stderr, "reload-%s x, class=%s\n", in_p ? "in" : "out",
	   m65x_reg_class_name (reload_class));
  debug_rtx (x);
#endif

  if (reload_mode == QImode)
    {
      /* X needs to be copied to a register of class RELOAD_CLASS.  */
      if (base_plus_const_byte_offset_mem (QImode, x))
	{
	  if (reload_class == HARD_ACCUM_REG)
	    {
	      /* We can only do (zp),y addressing mode for the accumulator
	         reg.  */
	      sri->icode = in_p ? CODE_FOR_reload_inqi_acc_indy
				: CODE_FOR_reload_outqi_acc_indy;
	      return NO_REGS;
	    }
	  else if (reload_class == HARD_Y_REG)
	    return NO_REGS;
	  else
	    return HARD_ACCUM_REG;
	}
    }
  else if (reload_mode == HImode)
    {
      /* If IN_P, X needs to be copied to a register of class RELOAD_CLASS,
	 else a register of class RELOAD_CLASS needs to be copied to X.  */
      if (base_plus_const_byte_offset_mem (HImode, x))
	{
	  if (reload_class == HARD_ACCUM_REG || reload_class == WORD_ACCUM_REGS)
	    {
	      /* We can only do (zp),y addressing mode for the accumulator
		 reg.  */
	      sri->icode = in_p ? CODE_FOR_reload_inhi_acc_indy
				: CODE_FOR_reload_outhi_acc_indy;
	      return NO_REGS;
	    }
	  else if (reload_class == HARD_Y_REG || reload_class == WORD_Y_REGS)
	    /* We're trying to load/store Y, so we can't use Y as scratch.
	       This will use the movhi_{ldy,sty}_indy pattern.  */
	    return NO_REGS;
	  else
	    return WORD_ACCUM_REGS;
	}
      else
	{
	  if (reg_classes_intersect_p (reload_class, HARD_REGS))
	    {
	      if (!in_p)
	        {
		  /* Storing HImode hard registers (including ZP parts) needs
		     a scratch register.  This arranges to have one
		     available.  */
		  sri->icode = CODE_FOR_reload_outhi_hardreg_abs;
		  return NO_REGS;
		}
	      else
	        return NO_REGS;
	    }
	  else
	    return HARDISH_REGS;
	}
    }

  return NO_REGS;
}

bool
m65x_valid_mov_operands (enum machine_mode mode, rtx *operands)
{
  if (reload_in_progress)
    {
      if ((REG_P (operands[0]) || GET_CODE (operands[0]) == SUBREG)
	  && true_regnum (operands[0]) == -1)
	return false;

      if ((REG_P (operands[1]) || GET_CODE (operands[1]) == SUBREG)
	  && true_regnum (operands[1]) == -1)
	return false;
    }
  
  if (MEM_P (operands[0]))
    return m65x_simple_address_p (mode, XEXP (operands[0], 0))
	   && (register_operand (operands[1], mode)
	       || immediate_operand (operands[1], mode));
  else if (MEM_P (operands[1]))
    return m65x_simple_address_p (mode, XEXP (operands[1], 0))
	   && register_operand (operands[0], mode);
  else
    return (accumulator_operand (operands[0], mode)
            && (index_reg_operand (operands[1], mode)
	        || zp_reg_operand (operands[1], mode)))
	   || (index_reg_operand (operands[0], mode)
	       && accumulator_operand (operands[1], mode))
           || (hard_reg_operand (operands[0], mode)
	       && zp_reg_or_imm_operand (operands[1], mode))
	   || (zp_reg_operand (operands[0], mode)
	       && (hard_reg_operand (operands[1], mode)
		   || zp_reg_or_imm_operand (operands[1], mode)))
	   || (y_reg_operand (operands[0], mode)
	       && x_reg_operand (operands[1], mode))
	   || (x_reg_operand (operands[0], mode)
	       && y_reg_operand (operands[1], mode));
}

static void
m65x_canonicalize_comparison (int *code, rtx *op0, rtx *op1,
			      bool op0_preserve_value)
{
  rtx tmp;
  bool swap = false;

  switch (*code)
    {
    case GTU:
      swap = true;
      *code = LTU;
      break;

    case LEU:
      swap = true;
      *code = GEU;
      break;

    case GT:
      swap = true;
      *code = LT;
      break;

    case LE:
      swap = true;
      *code = GE;
      break;

    default:
      ;
    }
  
  if (swap)
    {
      gcc_assert (!op0_preserve_value);
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
  add_reg_note (jmp_insn, REG_BR_PROB, GEN_INT (prob));
}

void
m65x_emit_himode_comparison (enum rtx_code cond, rtx op0, rtx op1, rtx dest,
			     rtx scratch)
{
  rtx op0_lo = gen_lowpart (QImode, op0);
  rtx op1_lo = gen_lowpart (QImode, op1);
  rtx op0_hi = gen_highpart_mode (QImode, HImode, op0);
  rtx op1_hi = gen_highpart_mode (QImode, HImode, op1);
  rtx cc_reg = gen_rtx_REG (CCmode, CC_REGNUM);
  rtx nvflags = gen_rtx_REG (CC_NVmode, CC_REGNUM);
  rtx new_label = NULL_RTX;
  int rev_prob = REG_BR_PROB_BASE - split_branch_probability;

  if ((cond == EQ || cond == NE || cond == LT || cond == GE)
      && REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
    {
      emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_lo));
      op0_lo = scratch;
    }
  
  if (cond != NE)
    new_label = gen_label_rtx ();

  switch (cond)
    {
    case EQ:
      /* Low part.  */
      emit_insn (gen_compareqi_cc (op0_lo, op1_lo));
      m65x_emit_cbranchqi (NE, cc_reg, rev_prob, new_label);

      /* High part.  */
      emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_hi));
      emit_insn (gen_compareqi_cc (scratch, op1_hi));
      m65x_emit_cbranchqi (EQ, cc_reg, split_branch_probability, dest);
      emit_label (new_label);
      break;

    case NE:
      /* Low part.  */
      emit_insn (gen_compareqi_cc (op0_lo, op1_lo));
      m65x_emit_cbranchqi (NE, cc_reg, split_branch_probability, dest);

      /* High part.  */
      emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_hi));
      emit_insn (gen_compareqi_cc (scratch, op1_hi));
      m65x_emit_cbranchqi (NE, cc_reg, split_branch_probability, dest);
      break;
    
    case LTU:
      /* High part.  */
      emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_hi));
      emit_insn (gen_compareqi_cc (scratch, op1_hi));
      m65x_emit_cbranchqi (LTU, cc_reg, split_branch_probability, dest);
      m65x_emit_cbranchqi (NE, cc_reg, split_branch_probability, new_label);

      /* Low part.  */
      if (REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
	{
	  emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_lo));
	  op0_lo = scratch;
	}

      emit_insn (gen_compareqi_cc (op0_lo, op1_lo));
      m65x_emit_cbranchqi (LTU, cc_reg, split_branch_probability, dest);
      emit_label (new_label);
      break;
    
    case GEU:
      /* High part.  */
      emit_insn (gen_rtx_SET (VOIDmode, scratch, op1_hi));
      emit_insn (gen_compareqi_cc (scratch, op0_hi));
      m65x_emit_cbranchqi (LTU, cc_reg, split_branch_probability, dest);
      m65x_emit_cbranchqi (NE, cc_reg, split_branch_probability, new_label);

      /* Low part.  */
      if (REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
	{
	  emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_lo));
	  op0_lo = scratch;
	}

      emit_insn (gen_compareqi_cc (op0_lo, op1_lo));
      m65x_emit_cbranchqi (GEU, cc_reg, split_branch_probability, dest);
      emit_label (new_label);
      break;
    
    case LT:
    case GE:
      emit_insn (gen_compareqi_cc_c (op0_lo, op1_lo));
      emit_insn (gen_rtx_SET (VOIDmode, scratch, op0_hi));
      emit_insn (gen_sbcqi3_nv (scratch, scratch, op1_hi));
      m65x_emit_cbranchqi (EQ, nvflags, split_branch_probability / 2,
			   new_label);
      emit_insn (gen_negate_highbit (scratch, scratch));
      emit_label (new_label);
      m65x_emit_cbranchqi (cond, nvflags, split_branch_probability, dest);
      break;
    
    default:
      gcc_unreachable ();
    }
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

#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE m65x_libcall_value

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P m65x_legitimate_address_p

#undef TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P
#define TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P hook_bool_mode_true

#undef TARGET_SPILL_CLASS
#define TARGET_SPILL_CLASS m65x_spill_class

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD m65x_secondary_reload

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

struct gcc_target targetm = TARGET_INITIALIZER;
