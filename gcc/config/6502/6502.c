#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "hash-set.h"
#include "machmode.h"
#include "vec.h"
#include "double-int.h"
#include "input.h"
#include "alias.h"
#include "symtab.h"
#include "wide-int.h"
#include "inchash.h"
#include "tree.h"
#include "stor-layout.h"
#include "calls.h"
#include "obstack.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "expmed.h"
#include "dojump.h"
#include "explow.h"
#include "function.h"
#include "emit-rtl.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "reload.h"
#include "expr.h"
#include "insn-codes.h"
#include "optabs.h"
#include "toplev.h"
#include "recog.h"
#include "predict.h"
#include "dominance.h"
#include "cfg.h"
#include "cfgrtl.h"
#include "cfganal.h"
#include "cfgbuild.h"
#include "cfgcleanup.h"
#include "basic-block.h"
#include "ggc.h"
#include "except.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "debug.h"
#include "dbxout.h"
#include "langhooks.h"
#include "df.h"
#include "rtl-iter.h"
#include "varasm.h"
#include "diagnostic.h"
#include "tree-pass.h"
#include "context.h"
#include "builtins.h"

#undef DEBUG_LEGIT_RELOAD

static bool
gate_reconstruct_absidx (void)
{
  return optimize > 0 && flag_symbol_prop;
}

static bool
m65x_subreg_const (rtx *sym, rtx_insn *insn, rtx use, unsigned int subreg_byte)
{
  if (GET_CODE (use) == SUBREG
      && CONSTANT_ADDRESS_P (XEXP (use, 0))
      && SUBREG_BYTE (use) == subreg_byte)
    {
      *sym = XEXP (use, 0);
      return true;
    }

  if (GET_CODE (use) == CONST && GET_CODE (XEXP (use, 0)) == TRUNCATE)
    {
      rtx op = XEXP (XEXP (use, 0), 0);
      if (CONSTANT_ADDRESS_P (op) && subreg_byte == 0)
	{
	  *sym = op;
	  return true;
	}
      else if (GET_CODE (op) == LSHIFTRT && CONST_INT_P (XEXP (op, 1))
	       && INTVAL (XEXP (op, 1)) == 8 && subreg_byte == 1)
	{
	  *sym = XEXP (op, 0);
	  return true;
	}
    }

  df_ref this_use;

  FOR_EACH_INSN_USE (this_use, insn)
    {
      if ((DF_REF_FLAGS (this_use) & (DF_REF_READ_WRITE | DF_REF_SUBREG))
	   == (DF_REF_READ_WRITE | DF_REF_SUBREG))
	continue;
      else if (rtx_equal_p (DF_REF_REG (this_use), use))
	{
	  struct df_link *link = DF_REF_CHAIN (this_use);

	  if (link && !link->next)
	    {
	      df_ref def = link->ref;

	      if (DF_REF_IS_ARTIFICIAL (def))
		break;
	      else
		{
		  rtx def_reg = DF_REF_REG (def), set, src;

		  if ((REG_P (def_reg)
		       || (GET_CODE (def_reg) == SUBREG
			   && SUBREG_BYTE (def_reg) == subreg_byte))
		      && (set = single_set (DF_REF_INSN (def)))
		      && (src = SET_SRC (set))
		      && (REG_P (src) || GET_CODE (src) == SUBREG
			  || GET_CODE (src) == CONST))
		    return m65x_subreg_const (sym, DF_REF_INSN (def), src,
					      subreg_byte);
		  else
		    break;
		}
	    }
	  else
	    break;
	}
      else
	break;
    }

  return false;
}

static unsigned int
rest_of_handle_reconstruct_absidx (void)
{
  unsigned i;

  df_chain_add_problem (DF_UD_CHAIN);
  df_set_flags (DF_DEFER_INSN_RESCAN);

  df_analyze ();
  df_maybe_reorganize_use_refs (DF_REF_ORDER_BY_INSN);

  for (i = 0; i < DF_USES_TABLE_SIZE (); i++)
    {
      df_ref use = DF_USES_GET (i);

      if (use && !DF_REF_IS_ARTIFICIAL (use))
        {
	  switch (DF_REF_TYPE (use))
	    {
	    case DF_REF_REG_DEF:
	    case DF_REF_REG_USE:
	      continue;

	    case DF_REF_REG_MEM_LOAD:
	    case DF_REF_REG_MEM_STORE:
	      rtx_insn *use_insn = DF_REF_INSN (use);

	      struct df_link *link = DF_REF_CHAIN (use);

	      int subreg_mask = 0;
	      rtx sym = NULL_RTX;

	      while (link)
	        {
		  df_ref def = link->ref;

		  if (!DF_REF_IS_ARTIFICIAL (def))
		    {
		      rtx_insn *def_insn = DF_REF_INSN (def);
		      rtx set = single_set (def_insn);

		      if (set && GET_CODE (SET_DEST (set)) == SUBREG)
			{
			  rtx nsym, src;
			  unsigned int subreg_byte
			    = SUBREG_BYTE (SET_DEST (set));

			  src = SET_SRC (set);

			  if ((REG_P (src)
			       || (GET_CODE (src) == SUBREG
				   && SUBREG_BYTE (src) == subreg_byte))
			      && m65x_subreg_const (&nsym, def_insn, src,
						    subreg_byte))
			    {
			      if (!sym)
				sym = nsym;
			      else if (!rtx_equal_p (sym, nsym))
				goto next_use;

			      subreg_mask |= 1 << SUBREG_BYTE (SET_DEST (set));
			    }
			  else
			    goto next_use;
			}
		    }

		  link = link->next;
		}

	      if (subreg_mask == ((1 << (POINTER_SIZE / BITS_PER_UNIT)) - 1)
		  && sym != NULL_RTX)
		{
		  if (dump_file)
		    {
		      fprintf (dump_file, "Trying to substitute symbol in:\n");
		      dump_insn_slim (dump_file, use_insn);
		    }

		  validate_unshare_change (use_insn, DF_REF_LOC (use), sym,
					   true);
		  if (verify_changes (0))
		    {
		      confirm_change_group ();
		      if (dump_file)
		        {
		          fprintf (dump_file, "Succeeded, insn is now:\n");
			  dump_insn_slim (dump_file, use_insn);
			}
		    }
		  else
		    {
		      cancel_changes (0);
		      if (dump_file)
			fprintf (dump_file,
				 "Failed, replacement not recognized.\n");
		    }
		}

	      break;
	    }
	}
      next_use: ;
    }

  return 0;
}

namespace {

const pass_data pass_data_reconstruct_absidx =
{
  RTL_PASS, /* type */
  "absidx", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  ( TODO_df_finish | 0), /* todo_flags_finish */
};

class pass_reconstruct_absidx : public rtl_opt_pass
{
public:
  pass_reconstruct_absidx(gcc::context *ctxt)
    : rtl_opt_pass(pass_data_reconstruct_absidx, ctxt)
  {}

  /* opt_pass methods: */
  virtual bool gate (function *)
  {
    return gate_reconstruct_absidx ();
  }

  virtual unsigned int execute (function *)
  {
    return rest_of_handle_reconstruct_absidx ();
  }

}; // class pass_reconstruct_absidx

} // anon namespace

rtl_opt_pass *
make_pass_reconstruct_absidx (gcc::context *ctxt)
{
  return new pass_reconstruct_absidx (ctxt);
}

static void
m65x_option_override (void)
{
  /* We can slightly speed up floating-point maths by rearranging the fields
     in the IEEE754 single float format to line up with byte boundaries.  */
  REAL_MODE_FORMAT (SFmode) = &m65x_single_format;

  opt_pass *pass_reconstruct_absidx = make_pass_reconstruct_absidx (g);
  static struct register_pass_info reconstruct_absidx_info
    = { pass_reconstruct_absidx, "fwprop1",
	1, PASS_POS_INSERT_AFTER
      };

  register_pass (&reconstruct_absidx_info);
}

static void
m65x_file_start (void)
{
  unsigned int i;

  fprintf (asm_out_file, "\t.feature at_in_identifiers\n");
  fprintf (asm_out_file, "\t.feature dollar_in_identifiers\n");
  fprintf (asm_out_file, "\t.autoimport +\n");
  switch (m65x_cpu_option)
    {
    case m6502:
      fprintf (asm_out_file, "\t.p02\n");
      break;

    case w65c02:
      fprintf (asm_out_file, "\t.pc02\n");
      break;

    case w65sc02:
      fprintf (asm_out_file, "\t.psc02\n");
      break;
    
    case m6502x:
      /* There isn't an assembler directive for this mode!  */
      break;

    default:
      sorry ("CPU unsupported");
      break;
    }
  
  fprintf (asm_out_file, "\t.importzp _sp0, _sp1, _fp0, _fp1\n");
  
  fprintf (asm_out_file, "\t.importzp _r0, _r1, _r2, _r3, _r4, "
			 "_r5, _r6, _r7\n");

  fprintf (asm_out_file, "\t.importzp _s0, _s1, _s2, _s3, _s4, "
			 "_s5, _s6, _s7\n");

  for (i = 0; i < 32; i++)
    {
      if ((i & 7) == 0)
        fprintf (asm_out_file, "\t.importzp ");
      fprintf (asm_out_file, "_e%d%s", i, (i & 7) == 7 ? "\n" : ", ");
    }

  fprintf (asm_out_file, "\t.importzp _tmp0, _tmp1\n");
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

    case 'L':
      if (GET_CODE (x) == CONST_INT)
	asm_fprintf (stream, "<$%.4x", (int) INTVAL (x) & 0xffff);
      else
        {
	  asm_fprintf (stream, "<");
	  m65x_print_operand_address (stream, x);
	}
      break;
    
    case 'H':
      if (GET_CODE (x) == CONST_INT)
	asm_fprintf (stream, ">$%.4x", (int) INTVAL (x) & 0xffff);
      else
        {
	  asm_fprintf (stream, ">");
	  m65x_print_operand_address (stream, x);
	}
      break;

    case 'm':
      gcc_assert (GET_CODE (x) == CONST_INT && INTVAL (x) <= 0
		  && INTVAL (x) > -256);
      asm_fprintf (stream, "$%.2x", (int) -INTVAL (x));
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
restart:
  switch (GET_CODE (x))
    {
#if 1
    case PLUS:
      if (GET_MODE (x) == HImode
	  && REG_P (XEXP (x, 1)) && IS_ZP_REGNUM (REGNO (XEXP (x, 1)))
	  && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
	  && GET_MODE (XEXP (XEXP (x, 0), 0)) == QImode
	  && REG_P (XEXP (XEXP (x, 0), 0))
	  && REGNO (XEXP (XEXP (x, 0), 0)) == Y_REGNUM)
	asm_fprintf (stream, "(%r),y", REGNO (XEXP (x, 1)));
      else if (GET_MODE (x) == QImode
	       && REG_P (XEXP (x, 0))
	       && (REGNO (XEXP (x, 0)) == X_REGNUM
		   || REGNO (XEXP (x, 0)) == Y_REGNUM)
	       && CONSTANT_ADDRESS_P (XEXP (x, 1)))
	{
	  output_addr_const (stream, XEXP (x, 1));
	  asm_fprintf (stream, ",%s",
		       REGNO (XEXP (x, 0)) == X_REGNUM ? "x" : "y");
	}
      else if (GET_MODE (x) == HImode
	       && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
	       && GET_MODE (XEXP (XEXP (x, 0), 0)) == QImode
	       && REG_P (XEXP (XEXP (x, 0), 0))
	       && (REGNO (XEXP (XEXP (x, 0), 0)) == X_REGNUM
		   || REGNO (XEXP (XEXP (x, 0), 0)) == Y_REGNUM)
	       && CONSTANT_ADDRESS_P (XEXP (x, 1)))
	{
	  output_addr_const (stream, XEXP (x, 1));
	  asm_fprintf (stream, ",%s",
		       REGNO (XEXP (x, 0)) == X_REGNUM ? "x" : "y");
	}
      else if (CONSTANT_ADDRESS_P (XEXP (x, 0)))
        output_addr_const (stream, x);
      else
        output_operand_lossage ("invalid PLUS operand");
      break;
#endif

#if 0
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

    case TRUNCATE:
      x = XEXP (x, 0);

      if (GET_CODE (x) == LSHIFTRT
	  && CONST_INT_P (XEXP (x, 1))
	  && INTVAL (XEXP (x, 1)) == 8)
	{
	  asm_fprintf (stream, ">(");
	  x = XEXP (x, 0);
	}
      else
        asm_fprintf (stream, "<(");

      output_addr_const (stream, x);
      asm_fprintf (stream, ")");
      break;

    case REG:
      if (GET_MODE (x) == HImode)
	asm_fprintf (stream, "(%r)", REGNO (x));
      else if (GET_MODE (x) == QImode)
        asm_fprintf (stream, "0,%c", (REGNO (x) == X_REGNUM) ? 'x'
		     : (REGNO (x) == Y_REGNUM) ? 'y' : '?');
      else
        output_operand_lossage ("invalid REG operand");
      break;

    case CONST_INT:
      asm_fprintf (stream, "$%.4x", (int) INTVAL (x) & 0xffff);
      break;

#if 0
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
#endif

    case CONST:
      x = XEXP (x, 0);
      goto restart;

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
	    case LTU: br = "bvs :+"; break;
	    case GEU: br = "bvc :+"; break;
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

	case CC_UImode:
	  switch (GET_CODE (cond))
	    {
	    case LTU: br = "bcs :+"; break;
	    case GEU: br = "bcc :+"; break;
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
	    case LTU: fmt = "bvc %0"; break;
	    case GEU: fmt = "bvs %0"; break;
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

	case CC_UImode:
	  switch (GET_CODE (cond))
	    {
	    case LTU: fmt = "bcc %0"; break;
	    case GEU: fmt = "bcs %0"; break;
	    default: gcc_unreachable ();
	    }
	  break;

	default:
	  gcc_unreachable ();
	}

      output_asm_insn (fmt, &dest);
    }
}

static const char *
m65x_print_movqi_1 (int which_alternative, rtx *operands, bool *clobbers_flags)
{
#define NL "\n\t"
  *clobbers_flags = true;

  switch (which_alternative)
    {
    case 0:
      return "ld%R0 #%1";

    case 1: case 3:
      return "ld%R0 %1";

    case 2: case 4:
      *clobbers_flags = false;
      return "st%R1 %0";

    case 5:
      switch (REGNO (operands[0]))
        {
        case ACC_REGNUM:
	  switch (REGNO (operands[1]))
	    {
	    case ACC_REGNUM: *clobbers_flags = false; return "";
	    case X_REGNUM: return "txa";
	    case Y_REGNUM: return "tya";
	    default: gcc_unreachable ();
	    }
	  break;

	case X_REGNUM:
	  switch (REGNO (operands[1]))
	    {
	    case ACC_REGNUM:
	      return "tax";
	    case X_REGNUM:
	      *clobbers_flags = false;
	      return "";
	    case Y_REGNUM:
	      if (TARGET_PHX)
	        return "phy"		NL
		       "plx";
	      else
		return "sty _tmp0"	NL
		       "ldx _tmp0";
	    default:
	      gcc_unreachable ();
	    }

	case Y_REGNUM:
	  switch (REGNO (operands[1]))
	    {
	    case ACC_REGNUM:
	      return "tay";
	    case X_REGNUM:
	      if (TARGET_PHX)
	        return "phx"		NL
		       "ply";
	      else
	        return "stx _tmp0"	NL
		       "ldy _tmp0";
	    case Y_REGNUM:
	      *clobbers_flags = false;
	      return "";
	    default:
	      gcc_unreachable ();
	    }

	default:
	  gcc_unreachable ();
	}
      break;

    case 6: case 8:
      *clobbers_flags = false;
      return "ph%R1";

    case 7: case 9:
      return "pl%R0";

    case 10:
      if ((REG_P (XEXP (operands[1], 0)) && TARGET_ZPIND)
	  || GET_CODE (XEXP (operands[1], 0)) == PLUS
	  || CONSTANT_ADDRESS_P (XEXP (operands[1], 0)))
        return "lda %1";
      else if (REG_P (XEXP (operands[1], 0)))
        {
	  operands[1] = XEXP (operands[1], 0);
          return "sty _tmp0"	NL
		 "ldy #0"	NL
		 "lda (%1),y"	NL
		 "ldy _tmp0";
	}
      else
        gcc_unreachable ();

    case 11:
      if ((REG_P (XEXP (operands[0], 0)) && TARGET_ZPIND)
	  || GET_CODE (XEXP (operands[0], 0)) == PLUS
	  || CONSTANT_ADDRESS_P (XEXP (operands[0], 0)))
        return "sta %0";
      else if (REG_P (XEXP (operands[0], 0)))
        {
	  operands[0] = XEXP (operands[0], 0);
	  return "sty _tmp0"	NL
		 "ldy #0"	NL
		 "sta (%0),y"	NL
		 "ldy _tmp0";
	}
      else
        gcc_unreachable ();

    case 12: case 13:
      *clobbers_flags = false;
      return "stz %0";

    case 14:
      if ((REG_P (XEXP (operands[1], 0)) && TARGET_ZPIND)
	  || GET_CODE (XEXP (operands[1], 0)) == PLUS)
        {
	  switch (REGNO (operands[0]))
	    {
	    case ACC_REGNUM:
              return "lda %1";
	    case X_REGNUM:
	      if (abs_y_mem_operand (operands[1], QImode))
	        return "ldx %1";
	      else
		return "sta _tmp0"	NL
		       "lda %1"		NL
		       "tax"		NL
		       "lda _tmp0";
	    case Y_REGNUM:
	      if (abs_x_mem_operand (operands[1], QImode))
	        return "ldy %1";
	      else
		return "sta _tmp0"	NL
		       "lda %1"		NL
		       "tay"		NL
		       "lda _tmp0";
	    default:
	      gcc_unreachable ();
	    }
	}
      else if (REG_P (XEXP (operands[1], 0)))
	{
	  operands[1] = XEXP (operands[1], 0);

	  switch (REGNO (operands[0]))
	    {
	    case ACC_REGNUM:
              return "sty _tmp0"	NL
		     "ldy #0"		NL
		     "lda (%1),y"	NL
		     "ldy _tmp0";
	    case X_REGNUM:
	      return "sta _tmp0"	NL
		     "ldx #0"		NL
		     "lda (%1,x)"	NL
		     "tax"		NL
		     "lda _tmp0";
	    case Y_REGNUM:
	      return "sta _tmp0"	NL
		     "ldy #0"		NL
		     "lda (%1),y"	NL
		     "tay"		NL
		     "lda _tmp0";
	    default:
	      gcc_unreachable ();
	    }
	}
      else if (CONSTANT_ADDRESS_P (XEXP (operands[1], 0)))
	return "ld%R0 %1";
      else
        gcc_unreachable ();
      break;

    case 15:
      if ((REG_P (XEXP (operands[0], 0)) && TARGET_ZPIND)
	  || GET_CODE (XEXP (operands[0], 0)) == PLUS)
	{
	  switch (REGNO (operands[1]))
	    {
	    case ACC_REGNUM:
	      *clobbers_flags = false;
	      return "sta %0";
	    case X_REGNUM:
	      return "sta _tmp0"	NL
		     "txa"		NL
		     "sta %0"		NL
		     "lda _tmp0";
	    case Y_REGNUM:
	      /* This is actually storing the Y register, but still using it
	         as an index.  */
	      return "sta _tmp0\n\t"	NL
		     "tya"		NL
		     "sta %0"		NL
		     "lda _tmp0";
	    default:
	      gcc_unreachable ();
	    }
	}
      else if (REG_P (XEXP (operands[0], 0)))
	{
	  operands[0] = XEXP (operands[0], 0);

	  switch (REGNO (operands[1]))
	    {
	    case ACC_REGNUM:
	      return "sty _tmp0"	NL
		     "ldy #0"		NL
		     "sta (%0),y"	NL
		     "ldy _tmp0";
	    case X_REGNUM:
	      return "sta _tmp0"	NL
		     "txa"		NL
		     "ldx #0"		NL
		     "sta (%0,x)"	NL
		     "tax"		NL
		     "lda _tmp0";
	    case Y_REGNUM:
	      return "sta _tmp0"	NL
		     "tya"		NL
		     "ldy #0"		NL
		     "sta (%0),y"	NL
		     "tay"		NL
		     "lda _tmp0";
	    default:
	      gcc_unreachable ();
	    }
	  return "";
	}
      else if (CONSTANT_ADDRESS_P (XEXP (operands[0], 0)))
	{
	  *clobbers_flags = false;
	  return "st%R1 %0";
	}
      else
	gcc_unreachable ();
      break;

    case 16:
      if ((REG_P (XEXP (operands[1], 0)) && TARGET_ZPIND)
	  || GET_CODE (XEXP (operands[1], 0)) == PLUS)
        return "sta _tmp0"	NL
	       "lda %1"		NL
	       "sta %0"		NL
	       "lda _tmp0";
      else if (REG_P (XEXP (operands[1], 0)))
        {
	  operands[1] = XEXP (operands[1], 0);
	  return "sta _tmp0"	NL
		 "sty _tmp1"	NL
		 "ldy #0"	NL
		 "lda (%1),y"	NL
		 "sta %0"	NL
		 "ldy _tmp1"	NL
		 "lda _tmp0";
	}
      else if (CONSTANT_ADDRESS_P (XEXP (operands[1], 0)))
	return "sta _tmp0"	NL
	       "lda %1"	NL
	       "sta %0"	NL
	       "lda _tmp0";
      else
        gcc_unreachable ();

    case 17:
      if ((REG_P (XEXP (operands[0], 0)) && TARGET_ZPIND)
	  || GET_CODE (XEXP (operands[0], 0)) == PLUS)
	return "sta _tmp0"	NL
	       "lda %1"		NL
	       "sta %0"		NL
	       "lda _tmp0";
      else if (REG_P (XEXP (operands[0], 0)))
	{
	  operands[0] = XEXP (operands[0], 0);
	  return "sta _tmp0"	NL
		 "sty _tmp1"	NL
		 "ldy #0"	NL
		 "lda %1"	NL
		 "sta (%0),y"	NL
		 "ldy _tmp1"	NL
		 "lda _tmp0";
	}
      else if (CONSTANT_ADDRESS_P (XEXP (operands[0], 0)))
	return "sta _tmp0"	NL
	       "lda %1"	NL
	       "sta %0"	NL
	       "lda _tmp0";
      else
	gcc_unreachable ();

    case 18:
      gcc_assert (rtx_equal_p (operands[0], operands[1]));
      *clobbers_flags = false;
      return "";

    default:
      gcc_unreachable ();
    }

  gcc_unreachable ();
#undef NL
}

const char *
m65x_print_movqi (int which_alternative, rtx *operands, bool save_flags)
{
  bool clobbers_flags;
  const char *insn = m65x_print_movqi_1 (which_alternative, operands,
					 &clobbers_flags);

  if (strcmp (insn, "#") == 0 || strcmp (insn, "") == 0)
    return insn;
  else if (save_flags && clobbers_flags)
    {
      output_asm_insn ("php", operands);
      output_asm_insn (insn, operands);
      return "plp";
    }
  else
    return insn;
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
      if (str[i] == '\\' || str[i] == '"' || str[i] < 32 || str[i] >= 127)
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
m65x_indirect_indexed_addr_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			      bool strict)
{
  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == ptr_mode
      && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
      && GET_MODE (XEXP (XEXP (x, 0), 0)) == QImode
      && REG_P (XEXP (XEXP (x, 0), 0))
      && ((!strict && REGNO (XEXP (XEXP (x, 0), 0)) >= FIRST_PSEUDO_REGISTER)
	  || REGNO (XEXP (XEXP (x, 0), 0)) == Y_REGNUM)
      && REG_P (XEXP (x, 1))
      && m65x_address_register_p (XEXP (x, 1), strict))
    return true;

  return false;
}

bool
m65x_indirect_offset_addr_p (enum machine_mode mode, rtx x, bool strict)
{
  HOST_WIDE_INT modesize = GET_MODE_SIZE (mode);

  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == ptr_mode
      && REG_P (XEXP (x, 0))
      && m65x_address_register_p (XEXP (x, 0), strict)
      && CONST_INT_P (XEXP (x, 1))
      && INTVAL (XEXP (x, 1)) >= 0
      && (INTVAL (XEXP (x, 1)) + modesize - 1) < 256)
    return true;

  return false;
}

bool
m65x_absolute_indexed_addr_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			      bool strict)
{
  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == ptr_mode
      && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
      && GET_MODE (XEXP (XEXP (x, 0), 0)) == QImode
      && REG_P (XEXP (XEXP (x, 0), 0))
      && ((!strict && REGNO (XEXP (XEXP (x, 0), 0)) >= FIRST_PSEUDO_REGISTER)
	  || REGNO (XEXP (XEXP (x, 0), 0)) == X_REGNUM
	  || REGNO (XEXP (XEXP (x, 0), 0)) == Y_REGNUM)
      && CONSTANT_ADDRESS_P (XEXP (x, 1)))
    return true;

  return false;
}

bool
m65x_absolute_x_addr_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			bool strict)
{
  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == ptr_mode
      && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
      && GET_MODE (XEXP (XEXP (x, 0), 0)) == QImode
      && REG_P (XEXP (XEXP (x, 0), 0))
      && ((!strict && REGNO (XEXP (XEXP (x, 0), 0)) >= FIRST_PSEUDO_REGISTER)
	  || REGNO (XEXP (XEXP (x, 0), 0)) == X_REGNUM)
      && CONSTANT_ADDRESS_P (XEXP (x, 1)))
    return true;
  
  return false;
}

bool
m65x_absolute_y_addr_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			bool strict)
{
  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == ptr_mode
      && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
      && GET_MODE (XEXP (XEXP (x, 0), 0)) == QImode
      && REG_P (XEXP (XEXP (x, 0), 0))
      && ((!strict && REGNO (XEXP (XEXP (x, 0), 0)) >= FIRST_PSEUDO_REGISTER)
	  || REGNO (XEXP (XEXP (x, 0), 0)) == Y_REGNUM)
      && CONSTANT_ADDRESS_P (XEXP (x, 1)))
    return true;
  
  return false;
}

bool
m65x_zeropage_x_addr_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			bool strict)
{
  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == QImode
      && REG_P (XEXP (x, 0))
      && ((!strict && REGNO (XEXP (x, 0)) >= FIRST_PSEUDO_REGISTER)
	  || REGNO (XEXP (x, 0)) == X_REGNUM)
      && CONSTANT_ADDRESS_P (XEXP (x, 1)))
    return true;
  
  return false;
}

bool
m65x_zeropage_y_addr_p (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x,
			bool strict)
{
  if (GET_CODE (x) == PLUS
      && GET_MODE (x) == QImode
      && REG_P (XEXP (x, 0))
      && ((!strict && REGNO (XEXP (x, 0)) >= FIRST_PSEUDO_REGISTER)
	  || REGNO (XEXP (x, 0)) == Y_REGNUM)
      && CONSTANT_ADDRESS_P (XEXP (x, 1)))
    return true;
  
  return false;
}

bool
m65x_zeropage_indexed_addr_p (enum machine_mode mode, rtx x, bool strict)
{
  return m65x_zeropage_x_addr_p (mode, x, strict)
	 || m65x_zeropage_y_addr_p (mode, x, strict);
}

bool
m65x_legitimate_address_p (enum machine_mode mode, rtx x, bool strict,
			   addr_space_t as)
{
  bool legit = false;

  switch (as)
    {
    case ADDR_SPACE_GENERIC:
      {
	if (CONSTANT_ADDRESS_P (x))
	  legit = true;

	/* Allow pre-increment/post-decrement only for the hardware stack
	   pointer, i.e. ph* and pl* instructions.  */
	else if ((GET_CODE (x) == PRE_INC || GET_CODE (x) == POST_DEC)
		 && mode == QImode
		 && REG_P (XEXP (x, 0)) && REGNO (XEXP (x, 0)) == HARDSP_REGNUM)
	  legit = true;

	/* Plain (mem (reg)) can't be disallowed, else the middle end gets very
	   upset.  */
	else if (m65x_address_register_p (x, strict))
	  legit = true;

	else if (mode == QImode
		 && m65x_indirect_indexed_addr_p (mode, x, strict))
	  legit = true;

	else if ((mode == HImode || mode == SImode)
		 && m65x_indirect_offset_addr_p (mode, x, strict))
	  legit = true;

	else if (m65x_absolute_indexed_addr_p (mode, x, strict))
	  legit = true;
#if 0
	/* (zp),y addressing mode -- the inner dereference must be in the ZP
	   named address space: (mem/gen (plus (mem/zp (const)) (yreg))).  */
	else if (mode == QImode
		 && GET_CODE (XEXP (x, 0)) == PLUS
		 && MEM_P (XEXP (XEXP (x, 0), 0))
		 && MEM_ADDR_SPACE (XEXP (XEXP (x, 0), 0)) == ADDR_SPACE_ZP
		 && CONSTANT_P (XEXP (XEXP (XEXP (x, 0), 0), 0))
		 && REG_P (XEXP (XEXP (x, 0), 1))
		 && (!strict || REGNO (XEXP (XEXP (x, 0), 1)) == Y_REGNUM))
	  legit = true;
	
	/* (zp,x) addressing mode: (mem/gen (mem/zp (plus (xreg) (const)))).  */
	else if (mode == QImode
		 && MEM_P (x)
		 && MEM_ADDR_SPACE (x) == ADDR_SPACE_ZP
		 && GET_CODE (XEXP (x, 0)) == PLUS
		 && REG_P (XEXP (XEXP (x, 0), 0))
		 && (!strict || REGNO (XEXP (XEXP (x, 0), 0)) == X_REGNUM)
		 && CONSTANT_P (XEXP (XEXP (x, 0), 1)))
	  legit = true;
#endif
      }
      break;
  
    case ADDR_SPACE_ZP:
      {
        if (CONSTANT_ADDRESS_P (x))
	  legit = true;
	/* zp,x and zp,y addressing modes, starting at zero.  */
	else if (REG_P (x)
		 && ((!strict && REGNO (x) >= FIRST_PSEUDO_REGISTER)
		     || REGNO (x) == X_REGNUM || REGNO (x) == Y_REGNUM))
	  legit = true;
	/* zp,x and zp,y addressing modes.  */
	else if (GET_CODE (x) == PLUS
		 && REG_P (XEXP (x, 0))
		 && ((!strict && REGNO (XEXP (x, 0)) >= FIRST_PSEUDO_REGISTER)
		     || REGNO (XEXP (x, 0)) == X_REGNUM
		     || REGNO (XEXP (x, 0)) == Y_REGNUM)
		 && CONSTANT_P (XEXP (x, 1)))
	  legit = true;
      }
      break;
    
    default:
      gcc_unreachable ();
    }

  if (TARGET_DEBUG_ADDRESS)
    {
      fprintf (stderr, "%s address (strict: %s, %s):\n",
	       legit ? "legitimate" : "illegitimate", strict ? "yes" : "no",
	       reload_in_progress ? "reload in progress"
	       : lra_in_progress ? "lra in progress" : reload_completed
	       ? "reload completed" : "before reload");
      debug_rtx (x);
    }

  return legit;
}

static rtx
m65x_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
			 enum machine_mode mode)
{
  int modesize = GET_MODE_SIZE (mode);

  if (CONSTANT_ADDRESS_P (x) || GET_MODE (x) != ptr_mode)
    return x;

  if (TARGET_DEBUG_LEGITIMIZE_ADDR)
    {
      fprintf (stderr, "Legitimize: ");
      dump_value_slim (stderr, x, 0);
      fputc ('\n', stderr);
    }

  switch (GET_MODE (x))
    {
    case QImode:
      {
        if (mode != QImode)
	  return x;
	
	if (REG_P (x))
	  return x;
	else if (GET_CODE (x) == PLUS)
	  {
	    rtx plus0 = XEXP (x, 0);
	    rtx plus1 = XEXP (x, 1);
	    
	    if (CONSTANT_P (plus1))
	      x = gen_rtx_PLUS (QImode, force_reg (QImode, plus0), plus1);
	    else
	      return x;
	  }
      }
      break;

    case HImode:
      if (mode == QImode && REG_P (x))
	x = gen_rtx_PLUS (Pmode, gen_rtx_ZERO_EXTEND (Pmode, const0_rtx), x);
      else if (GET_CODE (x) == PLUS)
	{
	  rtx plus0 = XEXP (x, 0);
	  rtx plus1 = XEXP (x, 1);

	  if (mode == QImode && !CONSTANT_ADDRESS_P (plus0)
	      && CONST_INT_P (plus1) && INTVAL (plus1) >= 0
	      && (INTVAL (plus1) + modesize - 1) < 256)
	    x = gen_rtx_PLUS (Pmode,
			      gen_rtx_ZERO_EXTEND (Pmode,
				force_reg (QImode,
				  gen_int_mode (INTVAL (plus1), QImode))),
			      force_reg (Pmode, plus0));
	  else if (mode == QImode && GET_CODE (plus0) != ZERO_EXTEND)
            {
	      rtx plus1_lo, plus1_hi, tmp_hi;
	      rtx tmp = gen_reg_rtx (HImode);

	      if (!REG_P (plus1) && !MEM_P (plus1))
	        plus1 = force_reg (Pmode, plus1);

	      plus1_lo = operand_subword (plus1, 0, 1, HImode);
	      plus1_hi = operand_subword (plus1, 1, 1, HImode);
	      tmp_hi = operand_subword (tmp, 1, 1, HImode);

	      if (!REG_P (plus0) && !MEM_P (plus0))
		plus0 = force_reg (Pmode, plus0);

	      emit_move_insn (tmp, plus0);
	      emit_insn (gen_addqi3 (tmp_hi, tmp_hi, plus1_hi));

	      x = gen_rtx_PLUS (Pmode,
		    gen_rtx_ZERO_EXTEND (Pmode, force_reg (QImode, plus1_lo)),
		    tmp);
	    }
	  else if (mode == QImode && GET_CODE (plus0) == ZERO_EXTEND)
	    x = gen_rtx_PLUS (Pmode, plus0, force_reg (Pmode, plus1));
	}
	break;

    default:
      gcc_unreachable ();
    }
  
  if (TARGET_DEBUG_LEGITIMIZE_ADDR)
    {
      fprintf (stderr, "to: ");
      dump_value_slim (stderr, x, 0);
      fputc ('\n', stderr);
    }

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

  return adjust_address (x, mode, adj);
}

static int
m65x_address_cost (rtx address, enum machine_mode mode,
		   addr_space_t as ATTRIBUTE_UNUSED,
		   bool speed ATTRIBUTE_UNUSED)
{
  if (mode != QImode
      && GET_CODE (address) == PLUS
      && REG_P (XEXP (address, 0))
      && CONST_INT_P (XEXP (address, 1))
      && INTVAL (XEXP (address, 1)) >= 0
      && INTVAL (XEXP (address, 1)) < 256)
    return 4;
  else if (mode == QImode
	   && GET_CODE (address) == PLUS
	   && GET_CODE (XEXP (address, 0)) == ZERO_EXTEND
	   && REG_P (XEXP (address, 1)))
    return 4;
  else if (CONSTANT_ADDRESS_P (address)
	   || (GET_CODE (address) == PLUS
	       && GET_CODE (XEXP (address, 0)) == ZERO_EXTEND
	       && CONSTANT_ADDRESS_P (XEXP (address, 1))))
    return 2;
  else if (GET_MODE (address) == QImode
	   && (REG_P (address)
	       || (GET_CODE (address) == PLUS
		   && REG_P (XEXP (address, 0))
		   && CONSTANT_ADDRESS_P (XEXP (address, 1)))))
    return 2;
  else
    return 8;
}

static int
m65x_register_move_cost (enum machine_mode mode, reg_class_t from,
			 reg_class_t to)
{
  /* LRA assumes that register moves of cost 2 never need reloads.  */
  if (mode == QImode &&
      ((HARD_REG_CLASS_P (to) && HARD_REG_CLASS_P (from))
       || (HARD_REG_CLASS_P (to) && ZP_REG_CLASS_P (from))
       || (ZP_REG_CLASS_P (to) && HARD_REG_CLASS_P (from))))
    return 2;
  if (mode != QImode || (ZP_REG_CLASS_P (from) && ZP_REG_CLASS_P (to)))
    return 8;

  return 4;
}

static rtx
m65x_function_arg (cumulative_args_t ca, enum machine_mode mode,
		   const_tree type, bool named)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  int modesize;

  if (mode == BLKmode)
    modesize = int_size_in_bytes (type);
  else
    modesize = GET_MODE_SIZE (mode);

  if (!named)
    return NULL_RTX;

  if ((*pcum) + modesize <= 8)
    return gen_rtx_REG (mode, (*pcum) + FIRST_ARG_REGISTER);
  else
    return NULL_RTX;
}

static void
m65x_function_arg_advance (cumulative_args_t ca, enum machine_mode mode,
			   const_tree type, bool named)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (ca);
  int modesize;
  
  if (mode == BLKmode)
    modesize = int_size_in_bytes (type);
  else
    modesize = GET_MODE_SIZE (mode);
  
  if (!named)
    return;
  
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
m65x_setup_incoming_varargs (cumulative_args_t ca,
			     enum machine_mode mode ATTRIBUTE_UNUSED,
			     tree type ATTRIBUTE_UNUSED, int *pretend_size,
			     int second_time ATTRIBUTE_UNUSED)
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

/* Handle some ca65-specific constant address quirks.  */

static void
m65x_output_addr_const (FILE *file, rtx x, unsigned int size)
{
 restart:
  switch (GET_CODE (x))
    {
    case CONST_INT:
      switch (size)
	{
	case 1:
	  fprintf (asm_out_file, "$%.2x", (int) INTVAL (x) & 0xff);
	  return;
	case 2:
	  fprintf (asm_out_file, "$%.4x", (int) INTVAL (x) & 0xffff);
	  return;
	case 4:
	  fprintf (asm_out_file, "$%.8x", (int) INTVAL (x) & 0xffffffff);
	  return;
	}
      break;

    /* Directives like ".word L1-L2" yielding a negative result confuse ca65.
       Work around that here.  This is derived from the code in final.c.  */
    case MINUS:
      x = simplify_subtraction (x);
      if (GET_CODE (x) != MINUS)
        goto restart;
      fputs (targetm.asm_out.open_paren, asm_out_file);
      output_addr_const (asm_out_file, XEXP (x, 0));
      fprintf (asm_out_file, "-");
      if ((CONST_INT_P (XEXP (x, 1)) && INTVAL (XEXP (x, 1)) >= 0)
	  || GET_CODE (XEXP (x, 1)) == PC
	  || GET_CODE (XEXP (x, 1)) == SYMBOL_REF)
	output_addr_const (asm_out_file, XEXP (x, 1));
      else
        {
	  fputs (targetm.asm_out.open_paren, asm_out_file);
	  output_addr_const (asm_out_file, XEXP (x, 1));
	  fputs (targetm.asm_out.close_paren, asm_out_file);
	}
      fputs (targetm.asm_out.close_paren, asm_out_file);
      switch (size)
        {
	case 1:
	  fputs (" & $ff", asm_out_file);
	  return;
	case 2:
	  fputs (" & $ffff", asm_out_file);
	  return;
	case 4:
	  fputs (" & $ffffffff", asm_out_file);
	  return;
	default:
	  gcc_unreachable ();
	}
      break;
    
    default:
      ;
    }

  output_addr_const (file, x);
}

static bool
m65x_asm_integer (rtx x, unsigned int size, int aligned_p ATTRIBUTE_UNUSED)
{
  const char *op = integer_asm_op (size, aligned_p);

  if (!op)
    return false;

  fputs (op, asm_out_file);
  m65x_output_addr_const (asm_out_file, x, size);
  fputc ('\n', asm_out_file);

  return true;
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
m65x_asm_select_section (tree exp,
			 int reloc ATTRIBUTE_UNUSED,
			 unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  const char *sname;
  addr_space_t as = TYPE_ADDR_SPACE (TREE_TYPE (exp));
  
  switch (categorize_decl_for_section (exp, reloc))
    {
    case SECCAT_BSS:
      sname = (as == ADDR_SPACE_ZP) ? "ZEROPAGE" : "BSS";
      break;
    
    case SECCAT_RODATA:
    case SECCAT_RODATA_MERGE_STR:
    case SECCAT_RODATA_MERGE_STR_INIT:
    case SECCAT_RODATA_MERGE_CONST:
      sname = (as == ADDR_SPACE_ZP) ? "ZEROPAGE" : "RODATA";
      break;
    
    case SECCAT_DATA:
      sname = (as == ADDR_SPACE_ZP) ? "ZEROPAGE" : "DATA";
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
    return regno == 0 || regno == 4 || regno == 8 || IS_ZP_REGNUM (regno);

  return IS_ZP_REGNUM (regno);
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
    case HARD_INDEX_REGS:
    case ACTUALLY_HARD_REGS:
    case GENERAL_REGS:
      return NO_REGS;
    
    default:
      return NO_REGS;
    }
}

static reg_class_t
m65x_preferred_reload_class (rtx x ATTRIBUTE_UNUSED, reg_class_t klass)
{
  switch (klass)
    {
    case HARD_ZP_REGS:
      return GENERAL_REGS;

    default:
      ;
    }

  return klass;
}

static bool
m65x_class_likely_spilled_p (reg_class_t klass)
{
  return default_class_likely_spilled_p (klass);
}

static bool
m65x_small_classes_for_mode (enum machine_mode mode)
{
  return mode == VOIDmode || mode == QImode;
}

static bool
base_plus_const_byte_offset_mem (enum machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  if (!MEM_P (x))
    return false;
  
  x = XEXP (x, 0);

  if (REG_P (x))
    return true;

  if (GET_CODE (x) == PLUS
      && GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
      && REG_P (XEXP (XEXP (x, 0), 0))
      && REG_P (XEXP (x, 1)))
    return true;

  return false;
}

static reg_class_t
m65x_secondary_reload (bool in_p, rtx x, reg_class_t reload_class,
		       enum machine_mode reload_mode,
		       secondary_reload_info *sri ATTRIBUTE_UNUSED)
{
  bool spilled_pseudo = (REG_P (x) || GET_CODE (x) == SUBREG)
			&& true_regnum (x) == -1;
  reg_class_t sclass = NO_REGS;
  enum machine_mode mode = GET_MODE (x);
    
  if (TARGET_DEBUG_SECONDARY_RELOAD)
    {
      fprintf (stderr, "reload-%s ", in_p ? "in" : "out");
      dump_value_slim (stderr, x, 1);
      fprintf (stderr, " class=%s mode=%s reload_mode=%s",
	       reg_class_names[reload_class], GET_MODE_NAME (mode),
	       GET_MODE_NAME (reload_mode));
      if (spilled_pseudo)
	fprintf (stderr, "  (spilled to stack)\n");
      else
        fputc ('\n', stderr);      
    }

  /* If IN_P, X needs to be copied to a register of class RELOAD_CLASS,
     else a register of class RELOAD_CLASS needs to be copied to X.  */

  if (reload_mode == QImode)
    {
      if (base_plus_const_byte_offset_mem (QImode, x))
	{
	  if (reload_class == HARD_ACCUM_REG)
	    sclass = NO_REGS;
	  else
	    sclass = HARD_ACCUM_REG;
	}
      else if (!HARD_REG_CLASS_P (reload_class) && CONSTANT_P (x)
	       && mode == QImode)
        sclass = ACTUALLY_HARD_REGS;
      else if (ZP_REG_CLASS_P (reload_class)
	       && zp_reg_operand (x, QImode)
	       && true_regnum (x) != -1
	       && true_regnum (x) < FIRST_PSEUDO_REGISTER)
	{
	  /*if (TARGET_DEBUG_SECONDARY_RELOAD)
	    fprintf (stderr, "(using reload_inoutqi_zp pattern)\n");*/
	  sclass = ACTUALLY_HARD_REGS;
	  //sri->icode = CODE_FOR_reload_inoutqi_zp;
	}
    }
  /*else if (in_p && reload_mode == HImode && ZP_REG_CLASS_P (reload_class)
	   && REG_P (x))
    sri->icode = CODE_FOR_reload_inhi_mem;*/

  if (TARGET_DEBUG_SECONDARY_RELOAD)
    fprintf (stderr, "  --> %s\n", reg_class_names[sclass]);

  return sclass;
}

static bool
m65x_lra_p (void)
{
  /* Reload sux!  */
  return m65x_lra_flag;
}

static bool
m65x_valid_mov_operands_1 (enum machine_mode mode, rtx *operands, bool relaxed)
{
  bool strict = reload_in_progress || lra_in_progress || reload_completed;
  
  if (MEM_P (operands[0]))
    {
      if (MEM_ADDR_SPACE (operands[0]) != ADDR_SPACE_GENERIC)
        return false;

      if (GET_CODE (XEXP (operands[0], 0)) == POST_DEC
	  || GET_CODE (XEXP (operands[0], 0)) == PRE_INC)
        return mode == QImode
	       && ((TARGET_PHX && hard_reg_operand (operands[1], mode))
		   || (!TARGET_PHX && accumulator_operand (operands[1], mode)));
      else
        {
	  if (m65x_indirect_indexed_addr_p (mode, XEXP (operands[0], 0),
					    strict)
	      || m65x_address_register_p (XEXP (operands[0], 0), strict))
	    return (relaxed && register_operand (operands[1], mode))
		   || (!relaxed && accumulator_operand (operands[1], mode));
	  else
	    return register_operand (operands[1], mode)
		   || immediate_operand (operands[1], mode);
	}
    }
  else if (MEM_P (operands[1]))
    {
      if (MEM_ADDR_SPACE (operands[1]) != ADDR_SPACE_GENERIC)
        return false;

      if (GET_CODE (XEXP (operands[1], 0)) == POST_DEC
	  || GET_CODE (XEXP (operands[1], 0)) == PRE_INC)
	return mode == QImode
	       && ((TARGET_PHX && hard_reg_operand (operands[0], mode))
		   || (!TARGET_PHX && accumulator_operand (operands[0], mode)));
      else
        {
	  if (m65x_indirect_indexed_addr_p (mode, XEXP (operands[1], 0),
					    strict)
	      || m65x_address_register_p (XEXP (operands[1], 0), strict))
	    return (relaxed && register_operand (operands[0], mode))
		   || (!relaxed && accumulator_operand (operands[0], mode));
	  else
	    return register_operand (operands[0], mode);
	}
    }
  else
    return (accumulator_operand (operands[0], mode)
            && (index_reg_operand (operands[1], mode)
	        || ptr_reg_operand (operands[1], mode)))
	   || (index_reg_operand (operands[0], mode)
	       && accumulator_operand (operands[1], mode))
           || (hard_reg_operand (operands[0], mode)
	       && (ptr_reg_operand (operands[1], mode)
		   || immediate_operand (operands[1], mode)
		   || hard_reg_operand (operands[1], mode)))
	   || (ptr_reg_operand (operands[0], mode)
	       && (hard_reg_operand (operands[1], mode)
		   || rtx_equal_p (operands[1], const0_rtx)))
	   || rtx_equal_p (operands[0], operands[1]);

  return false;
}

bool
m65x_valid_mov_operands (enum machine_mode mode, rtx *operands, bool relaxed)
{
  int retval;

#if 0
  fprintf (stderr, "valid mov operands? op0=\n");
  debug_rtx (operands[0]);
  fprintf (stderr, "op1=\n");
  debug_rtx (operands[1]);
#endif
  
  retval = m65x_valid_mov_operands_1 (mode, operands, relaxed);

#if 0
  fprintf (stderr, "returning %s\n", retval ? "true" : "false");
#endif
  
  return retval;
}

/* Valid "ZP" move operations: only moves to/from the named zero-page memory
   space are included.  Other moves are covered by movqi_insn.  */

bool
m65x_valid_zp_mov_operands (enum machine_mode mode ATTRIBUTE_UNUSED,
			    rtx *operands)
{
  if (MEM_P (operands[0]))
    return MEM_ADDR_SPACE (operands[0]);
  else if (MEM_P (operands[1]))
    return MEM_ADDR_SPACE (operands[1]);

  return REG_P (operands[0]) && REG_P (operands[1]);
}

rtx
m65x_gen_subreg (enum machine_mode outmode, rtx x, enum machine_mode inmode,
		 int byte)
{
  if (outmode == QImode && inmode == HImode
      && (GET_CODE (x) == SYMBOL_REF || GET_CODE (x) == LABEL_REF
	  || GET_CODE (x) == CONST))
    {
      if (GET_CODE (x) == CONST)
        x = XEXP (x, 0);

      if (byte > 0)
        x = gen_rtx_LSHIFTRT (HImode, x, GEN_INT (byte * 8));

      return gen_rtx_CONST (QImode, gen_rtx_TRUNCATE (QImode, x));
    }

  return simplify_gen_subreg (outmode, x, inmode, byte);
}

static void
m65x_canonicalize_comparison (int *code, rtx *op0, rtx *op1,
			      bool op0_preserve_value)
{
  rtx tmp;

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
  rtx dest_label = gen_rtx_LABEL_REF (Pmode, dest);
  rtx branch = gen_rtx_SET (pc_rtx,
		 gen_rtx_IF_THEN_ELSE (VOIDmode, cmp, dest_label, pc_rtx));
  rtx jmp_insn = emit_jump_insn (branch);
  JUMP_LABEL (jmp_insn) = dest;
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
  rtx vflag = gen_rtx_REG (CC_Vmode, OVERFLOW_REGNUM);
  rtx cmpreg, label_ref, jump_insn;

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
      cmpreg = gen_rtx_REG (CC_UImode, CARRY_REGNUM);
    emit_cmp:
      emit_insn (gen_compareqi (op0, op1));
      cmp = gen_rtx_fmt_ee (cond, VOIDmode, cmpreg, const0_rtx);
      label_ref = gen_rtx_LABEL_REF (Pmode, dest);
      jump_insn = emit_jump_insn (gen_rtx_SET (pc_rtx,
				    gen_rtx_IF_THEN_ELSE (VOIDmode, cmp,
							  label_ref, pc_rtx)));
      JUMP_LABEL (jump_insn) = dest;
      break;

    case LT:
    case GE:
      scratch = gen_reg_rtx (QImode);
      new_label = gen_label_rtx ();
      emit_move_insn (scratch, op0);
      emit_insn (gen_sec ());
      emit_insn (gen_sbcqi3_nzv (scratch, scratch, op1));
      m65x_emit_cbranchqi (LTU, vflag,
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
  rtx op0_lo = m65x_gen_subreg (QImode, op0, HImode, 0);
  rtx op1_lo = m65x_gen_subreg (QImode, op1, HImode, 0);
  rtx op0_hi = m65x_gen_subreg (QImode, op0, HImode, 1);
  rtx op1_hi = m65x_gen_subreg (QImode, op1, HImode, 1);
  rtx nzflags = gen_rtx_REG (CC_NZmode, NZ_REGNUM);
  rtx vflag = gen_rtx_REG (CC_Vmode, OVERFLOW_REGNUM);
  rtx cflag = gen_rtx_REG (CC_UImode, CARRY_REGNUM);
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
      m65x_emit_cbranchqi (LTU, cflag, split_branch_probability, dest);
      m65x_emit_cbranchqi (NE, nzflags, split_branch_probability, new_label);

      /* Low part.  */
      if (REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
	{
	  emit_move_insn (scratch, op0_lo);
	  op0_lo = scratch;
	}

      emit_insn (gen_compareqi (op0_lo, op1_lo));
      m65x_emit_cbranchqi (LTU, cflag, split_branch_probability, dest);
      emit_label (new_label);
      break;
    
    case GEU:
      /* High part.  */
      emit_move_insn (scratch, op1_hi);
      emit_insn (gen_compareqi (scratch, op0_hi));
      m65x_emit_cbranchqi (LTU, cflag, split_branch_probability, dest);
      m65x_emit_cbranchqi (NE, nzflags, split_branch_probability, new_label);

      /* Low part.  */
      if (REG_P (op0) && IS_ZP_REGNUM (REGNO (op0)))
	{
	  emit_move_insn (scratch, op0_lo);
	  op0_lo = scratch;
	}

      emit_insn (gen_compareqi (op0_lo, op1_lo));
      m65x_emit_cbranchqi (GEU, cflag, split_branch_probability, dest);
      emit_label (new_label);
      break;
    
    case LT:
    case GE:
      emit_insn (gen_compareqi (op0_lo, op1_lo));
      emit_move_insn (scratch, op0_hi);
      emit_insn (gen_sbcqi3_nzv (scratch, scratch, op1_hi));
      m65x_emit_cbranchqi (LTU, vflag, split_branch_probability / 2,
			   new_label);
      emit_insn (gen_negate_highbit (scratch, scratch));
      emit_label (new_label);
      m65x_emit_cbranchqi (cond, nzflags, split_branch_probability, dest);
      break;
    
    default:
      gcc_unreachable ();
    }
}

typedef struct {
  unsigned HOST_WIDE_INT frame_size;
  unsigned HOST_WIDE_INT pretend_size;
  unsigned HOST_WIDE_INT outgoing_args_size;
  unsigned HOST_WIDE_INT stack_adj_total;
} m65x_stack_info;

static m65x_stack_info *
m65x_compute_frame_layout (void)
{
  static m65x_stack_info info;
  
  if (reload_completed)
    return &info;
  
  info.frame_size = get_frame_size ();
  info.pretend_size = crtl->args.pretend_args_size;
  info.outgoing_args_size = crtl->outgoing_args_size;
  info.stack_adj_total = info.frame_size + info.pretend_size
			 + info.outgoing_args_size;

  return &info;
}

/* We have a stack layout as follows:

    __________________________
    |                        |
    |     incoming args      |
    |________________________|  <-- old stack pointer
    |                        |
    |      pretend args      |
    |________________________|  <-- soft arg pointer
    |                        |
    |   locals (frame size)  |
    |________________________|  <-- soft/hard frame pointer
    |                        |
    |      alloca space      |
    |________________________|
    |                        |
    |      outgoing args     |
    |________________________|  <-- current/outgoing stack pointer
    
*/

HOST_WIDE_INT
m65x_elimination_offset (int from, int to)
{
  m65x_stack_info *info = m65x_compute_frame_layout ();
  HOST_WIDE_INT adjust;
  
  if (from == ARG_POINTER_REGNUM)
    switch (to)
      {
      case STACK_POINTER_REGNUM:
        adjust = info->outgoing_args_size + info->frame_size;
	break;
      case FRAME_POINTER_REGNUM:
        adjust = info->frame_size;
	break;
      case HARD_FRAME_POINTER_REGNUM:
        adjust = info->frame_size;
	break;
      default:
        gcc_unreachable ();
      }
  else if (from == FRAME_POINTER_REGNUM)
    switch (to)
      {
      case STACK_POINTER_REGNUM:
        adjust = info->outgoing_args_size;
	break;
      case HARD_FRAME_POINTER_REGNUM:
        adjust = 0;
	break;
      default:
        gcc_unreachable ();
      }
  else
    gcc_unreachable ();
  
  if (TARGET_DEBUG_STACK)
    fprintf (stderr, "eliminate %s to %s, adding %d\n", reg_names[from],
	     reg_names[to], (int) adjust);

  return adjust;
}

static bool
m65x_can_eliminate (const int from, const int to)
{
  if ((from == ARG_POINTER_REGNUM && to == FRAME_POINTER_REGNUM)
      || (frame_pointer_needed && to == STACK_POINTER_REGNUM))
    return false;
  
  return true;
}

void
m65x_asm_function_prologue (FILE *out, HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  if (TARGET_DEBUG_STACK)
    fprintf (stderr, "function: '%s'\n",
	     (const char *) XSTR (XEXP (DECL_RTL (current_function_decl), 0),
				  0));
  fprintf (out, "; frame size %d, pretend size %d, outgoing size %d\n",
	   (int) get_frame_size (), (int) crtl->args.pretend_args_size,
	   (int) crtl->outgoing_args_size);
}

/* See ASCII diagram for m65x_elimination_offset.  */

void
m65x_expand_prologue (void)
{
  int regno;
  rtx accum = gen_rtx_REG (QImode, ACC_REGNUM), insn;
  m65x_stack_info *info = m65x_compute_frame_layout ();
  /*HOST_WIDE_INT stack_offset = frame_size + crtl->outgoing_args_size;*/
  rtx yreg = gen_rtx_REG (QImode, Y_REGNUM);
  rtx push_rtx = gen_rtx_MEM (QImode,
		   gen_rtx_POST_DEC (HImode,
				     gen_rtx_REG (HImode, HARDSP_REGNUM)));

  if (optimize_size && !frame_pointer_needed && info->pretend_size == 0)
    {
      int num_saved = 0;

      for (regno = FIRST_CALLER_SAVED; regno <= LAST_CALLER_SAVED; regno++)
        if (df_regs_ever_live_p (regno))
	  num_saved++;

      if (num_saved == 8)
        {
	  if (info->stack_adj_total > 0)
	    {
	      int neg_adj = -info->stack_adj_total;
	      rtx xreg = gen_rtx_REG (QImode, X_REGNUM);
	      emit_insn (gen_movqi (xreg,
				    gen_int_mode (neg_adj & 0xff, QImode)));
	      emit_insn (gen_movqi (yreg,
				    gen_int_mode ((neg_adj >> 8) & 0xff,
						  QImode)));
	      emit_insn (gen_m65x_savestack_s7s0_sp (xreg, yreg));
	    }
	  else
	    emit_insn (gen_m65x_savestack_s7s0 ());

	  return;
	}
    }

  /* Push SP if we modify it.  */
  if (info->stack_adj_total != 0)
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
  
  if (info->pretend_size > 0)
    {
      /* We don't even pretend (no pun intended!) to make varargs functions
         efficient...  */
      insn = emit_insn (gen_addhi3 (stack_pointer_rtx, stack_pointer_rtx,
				    gen_int_mode (-info->pretend_size, Pmode)));
      RTX_FRAME_RELATED_P (insn) = 1;
      insn = emit_move_insn (yreg, const0_rtx);
      RTX_FRAME_RELATED_P (insn) = 1;

      for (regno = 8 - info->pretend_size; regno < 8; regno++)
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
				    gen_int_mode (-info->frame_size, Pmode)));
      RTX_FRAME_RELATED_P (insn) = 1;
    }
  
  if (info->frame_size + info->outgoing_args_size != 0)
    {
      insn = emit_insn (gen_addhi3 (stack_pointer_rtx, stack_pointer_rtx,
			  gen_int_mode (-(info->frame_size
					+ info->outgoing_args_size), Pmode)));
      RTX_FRAME_RELATED_P (insn) = 1;
    }

  for (regno = LAST_CALLER_SAVED; regno >= FIRST_CALLER_SAVED; regno--)
    if (df_regs_ever_live_p (regno))
      {
        emit_insn (gen_movqi (accum, gen_rtx_REG (QImode, regno)));
	insn = emit_insn (gen_pushqi1 (push_rtx, accum));
	RTX_FRAME_RELATED_P (insn) = 1;
      }
  
  if (frame_pointer_needed)
    emit_insn (gen_blockage ());
}

void
m65x_expand_epilogue (void)
{
  int regno;
  rtx accum = gen_rtx_REG (QImode, ACC_REGNUM);
  m65x_stack_info *info = m65x_compute_frame_layout ();
  rtx pop_rtx = gen_rtx_MEM (QImode,
		   gen_rtx_PRE_INC (HImode,
				    gen_rtx_REG (HImode, HARDSP_REGNUM)));

  emit_insn (gen_blockage ());

  if (optimize_size && !frame_pointer_needed && info->pretend_size == 0)
    {
      int num_restored = 0;

      for (regno = FIRST_CALLER_SAVED; regno <= LAST_CALLER_SAVED; regno++)
        if (df_regs_ever_live_p (regno))
	  num_restored++;

      if (num_restored == 8)
        {
	  emit_use (gen_rtx_REG (HImode, HARDSP_REGNUM));
	  emit_insn (gen_rtx_CLOBBER (VOIDmode,
				      gen_rtx_REG (QImode, TMP0_REGNUM)));
	  emit_insn (gen_rtx_CLOBBER (VOIDmode,
				      gen_rtx_REG (QImode, TMP1_REGNUM)));

	  if (info->stack_adj_total > 0)
	    emit_jump_insn (gen_m65x_restorestack_s7s0_sp_rts ());
	  else
	    emit_jump_insn (gen_m65x_restorestack_s7s0_rts ());

	  return;
	}
    }

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
  
  if (info->stack_adj_total != 0)
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
  
  emit_use (gen_rtx_REG (HImode, HARDSP_REGNUM));
  emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (QImode, TMP0_REGNUM)));
  emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (QImode, TMP1_REGNUM)));
  
  emit_jump_insn (gen_m65x_return ());
}

static bool
m65x_scalar_mode_supported_p (enum machine_mode mode)
{
  if (mode == SImode)
    return true;
  else if (mode == DFmode)
    return false;
  
  return default_scalar_mode_supported_p (mode);
}

bool
m65x_peephole_find_temp_regs (int from, int to, ...)
{
  va_list ap;
  HARD_REG_SET insn_temps;
  bool all_allocated = true;
  const int fixed_tmps = 2;
  int i, next_tmp = 0;
  bool allocated_tmps[fixed_tmps];
  int tmp_num;
  
  for (i = 0; i < fixed_tmps; i++)
    allocated_tmps[i] = false;
  
  CLEAR_HARD_REG_SET (insn_temps);
  
  va_start (ap, to);
  
  for (tmp_num = 0; ; tmp_num++)
    {
      rtx *tmp = va_arg (ap, rtx *);

      if (tmp == NULL)
        break;

      *tmp = NULL_RTX;

      /* Try the registers tmp0/tmp1 first.  The peep2_find_free_register
	 function won't consider these, because they are marked as fixed.
	 FIXME: This doesn't work, because (I think) liveness information
	 isn't calculated for the fixed registers.  Disable for now.  */
      while (0 && next_tmp < fixed_tmps)
        {
	  if (peep2_regno_dead_p (to, TMP0_REGNUM + next_tmp)
	      && !allocated_tmps[next_tmp])
	    {
	      *tmp = gen_rtx_REG (QImode, TMP0_REGNUM + next_tmp);
	      allocated_tmps[next_tmp++] = true;
	      break;
	    }
	  else
	    next_tmp++;
	}

      /* Next, try some other register which might be free.  */
      if (*tmp == NULL_RTX)
	*tmp = peep2_find_free_register (from, to, "r", QImode, &insn_temps);

#if 0
      if (*tmp != NULL_RTX)
        {
	  fprintf (stderr, "allocated tmp (%d): ", tmp_num);
	  dump_value_slim (stderr, *tmp, 0);
	  fputc ('\n', stderr);
	}
      else
        fprintf (stderr, "failed to allocate tmp (%d)\n", tmp_num);
#endif

      if (*tmp == NULL_RTX)
        {
	  all_allocated = false;
	  break;
	}
    }
  
  va_end (ap);
  
  return all_allocated;
}

rtx
m65x_push (enum machine_mode mode, rtx src)
{
  rtx push_rtx = gen_rtx_MEM (mode,
		   gen_rtx_POST_DEC (Pmode,
				     gen_rtx_REG (Pmode, HARDSP_REGNUM)));
  return gen_pushqi1 (push_rtx, src);
}

rtx
m65x_pop (enum machine_mode mode, rtx dest)
{
  rtx pop_rtx = gen_rtx_MEM (mode,
		  gen_rtx_PRE_INC (Pmode,
				   gen_rtx_REG (Pmode, HARDSP_REGNUM)));
  return gen_popqi1 (dest, pop_rtx);
}

void
m65x_expand_addsub (enum machine_mode mode, bool add, rtx operands[])
{
  rtx acc, dstpart, op1part, op2part;
  int i, modesize = GET_MODE_SIZE (mode);
  bool need_clobber = false;
  rtx dest = operands[0], seq;

  /* References to virtual_<foo> can turn into adds/subs themselves.  */
  for (i = 1; i <= 2; i++)
    if (reg_mentioned_p (virtual_incoming_args_rtx, operands[i]))
      operands[i] = copy_to_mode_reg (mode, operands[i]);

  if (MEM_P (operands[1]) && !incdec_operand (operands[1], mode))
    operands[1] = force_reg (mode, operands[1]);

  start_sequence ();

  acc = gen_reg_rtx (QImode);

  if ((!rtx_equal_p (operands[0], operands[1])
       && reg_overlap_mentioned_p (operands[0], operands[1]))
      || (!rtx_equal_p (operands[0], operands[2])
	  && reg_overlap_mentioned_p (operands[0], operands[2]))
      || (MEM_P (operands[0]) && !incdec_operand (operands[0], mode)))
    dest = gen_reg_rtx (mode);

  /* Do subtractions of immediates as subtractions not additions with negated
     values (as they are canonicalised): then we can use DEC for the high part
     when the corresponding part of op2 is zero.  */
  if (add && CONST_INT_P (operands[2]) && INTVAL (operands[2]) < 0)
    {
      operands[2] = GEN_INT (-INTVAL (operands[2]));
      add = false;
    }

  /* Special-case multibyte decrement by 1.  */
  if (!add && CONST_INT_P (operands[2]) && INTVAL (operands[2]) == 1)
    {
      rtx labels[3];

      emit_move_insn (dest, operands[1]);

      for (i = 0; i < modesize - 1; i++)
        {
	  labels[i] = gen_label_rtx ();

	  op1part = operand_subword (operands[1], i, 1, mode);

	  emit_insn (gen_loadqi_nz (gen_reg_rtx (QImode), op1part));
	  m65x_emit_cbranchqi (NE,
	    gen_rtx_REG (CC_NZmode, NZ_REGNUM), PROB_LIKELY, labels[i]);
	}

      for (i = modesize - 1; i >= 0; i--)
        {
	  dstpart = operand_subword (dest, i, 1, mode);

	  if (i < modesize - 1)
	    emit_label (labels[i]);
	  emit_insn (gen_incdecqi3 (dstpart, dstpart, constm1_rtx));
	}
    }
  else
    {
      bool valid_carry = false;
      bool valid_nz = false;
      int ones = 0, trailing_zeros = 0;
      rtx end_label = NULL_RTX;

      if (CONST_INT_P (operands[2]))
	{
	  for (i = 0; i < modesize; i++)
	    {
	      int byte = (INTVAL (operands[2]) >> (i * 8)) & 0xff;

	      if (byte == 1)
		ones++;
	      else if (byte != 0)
		{
	          ones = -1;
		  break;
		}
	    }

	  for (i = modesize - 1; i >= 0; i--)
            if (((INTVAL (operands[2]) >> (i * 8)) & 0xff) == 0)
	      trailing_zeros++;
	    else
	      break;
	}

      /* If we're going to use any INC/DEC instructions, they will be in-place:
	 copying values in the middle of the add/sub sequence may destroy the NZ
	 flags.  So, copy op1 to dest first.  */
      if (trailing_zeros > 0)
	{
	  emit_move_insn (dest, operands[1]);
	  operands[1] = dest;
	}

      /* If we're adding and there's only one 1-byte, then we'll only use
	 increments, so no need to clear the carry flag.  */
      if (!add || ones != 1)
	{
	  if (add)
	    emit_insn (gen_clc ());
	  else
	    emit_insn (gen_sec ());
	}

      for (i = 0; i < modesize; i++)
	{
	  bool last = i + 1 == modesize;

	  dstpart = operand_subword (dest, i, 1, mode);
	  op1part = operand_subword (operands[1], i, 1, mode);
	  op2part = operand_subword (operands[2], i, 1, mode);

	  if (op2part == 0 && CONSTANT_P (operands[2]))
            {
	      operands[2] = use_anchored_address (force_const_mem (mode,
						  operands[2]));
	      op2part = operand_subword (operands[2], i, 1, mode);
	    }
	  else if (op2part == 0)
	    op2part = operand_subword_force (operands[2], i, mode);

	  gcc_assert (dstpart && op1part && op2part);

	  need_clobber |= (GET_CODE (dstpart) == SUBREG);

	  if (CONST_INT_P (op2part))
	    {
	      unsigned HOST_WIDE_INT remaining
		= INTVAL (operands[2]) >> (i * 8);
	      unsigned HOST_WIDE_INT thispart = INTVAL (op2part);

	      if (thispart == 0 && !valid_carry && !valid_nz)
	        {
		  emit_move_insn (dstpart, op1part);
		  continue;
		}
	      else if ((add || last) && !valid_carry && remaining == 1)
		{
		  emit_insn (gen_incdecqi3_nz (dstpart, dstpart,
					       add ? const1_rtx : constm1_rtx));
		  if (!last)
		    {
		      if (!end_label)
		        end_label = gen_label_rtx ();
		      m65x_emit_cbranchqi (NE,
			gen_rtx_REG (CC_NZmode, NZ_REGNUM), PROB_LIKELY,
				     end_label);
		    }
		  valid_nz = add;
		  continue;
		}
	      else if (valid_nz && remaining == 0)
	        {
		  emit_insn (gen_incdecqi3_nz (dstpart, dstpart,
			       add ? const1_rtx : constm1_rtx));
		  if (!last)
		    {
		      if (!end_label)
		        end_label = gen_label_rtx ();
		      m65x_emit_cbranchqi (NE,
			gen_rtx_REG (CC_NZmode, NZ_REGNUM), PROB_LIKELY,
				     end_label);
		    }
		  continue;
		}
	      else if (last && valid_carry && remaining == 0)
		{
		  if (!end_label)
		    end_label = gen_label_rtx ();

		  m65x_emit_cbranchqi (add ? EQ : NE,
		    gen_rtx_REG (CC_Cmode, CARRY_REGNUM), PROB_LIKELY,
				 end_label);

		  emit_insn (gen_incdecqi3 (dstpart, dstpart,
			       add ? const1_rtx : constm1_rtx));

		  continue;
		}
	    }

	  emit_move_insn (acc, op1part);
	  if (add)
	    {
	      if (last)
	        emit_insn (gen_adcqi3 (acc, acc, op2part));
	      else
		emit_insn (gen_adcqi3_c (acc, acc, op2part));
	    }
	  else
	    {
	      if (last)
		emit_insn (gen_sbcqi3 (acc, acc, op2part));
	      else
		emit_insn (gen_sbcqi3_c (acc, acc, op2part));
	    }
	  emit_move_insn (dstpart, acc);

	  valid_carry = true;
	  gcc_assert (!valid_nz);
	}

      if (end_label)
	emit_label (end_label);
    }

  if (dest != operands[0])
    emit_move_insn (operands[0], dest);

  seq = get_insns ();
  end_sequence ();

  if (REG_P (operands[0])
      && operands[0] != operands[1]
      && operands[0] != operands[2]
      && !(lra_in_progress || reload_in_progress || reload_completed)
      && need_clobber)
    emit_clobber (operands[0]);

  emit_insn (seq);
}

void
m65x_emit_movqi (rtx dst, rtx src, bool can_clobber_nz)
{
  if (can_clobber_nz)
    emit_insn (gen_movqi_insn (dst, src));
  else
    emit_insn (gen_movqi_noclob (dst, src));
}

/* DST can be:
    - the accumulator
    - an index register (X/Y)
    - a zero-page register
   BASE is always a zero-page register pair.
   INDEX can be the Y register or NULL_RTX.  */

bool
m65x_peep_load (rtx dst, rtx base, rtx index, bool can_clobber_nz)
{
  rtx acc = gen_rtx_REG (QImode, ACC_REGNUM);
  rtx xreg = gen_rtx_REG (QImode, X_REGNUM);
  rtx yreg = gen_rtx_REG (QImode, Y_REGNUM);
  rtx zpreg, seq;
  bool save_needed;
  bool x_available = peep2_regno_dead_p (1, X_REGNUM);
  bool y_available = peep2_regno_dead_p (1, Y_REGNUM);

  gcc_assert (index == NULL_RTX || rtx_equal_p (index, yreg));

  if (rtx_equal_p (dst, acc))
    {
      gcc_assert (index == NULL_RTX);

      if (y_available)
	{
	  m65x_emit_movqi (yreg, const0_rtx, can_clobber_nz);
	  if (can_clobber_nz)
	    emit_insn (gen_loadqi_indy_nz (acc, yreg, base));
	  else
	    emit_insn (gen_loadqi_indy (acc, yreg, base));
	}
      else if (x_available)
	{
	  m65x_emit_movqi (xreg, const0_rtx, can_clobber_nz);
	  if (can_clobber_nz)
	    emit_insn (gen_loadqi_xind_nz (acc, base, xreg));
	  else
	    emit_insn (gen_loadqi_xind (acc, base, xreg));
	}
      else
	return false;

      return true;
    }

  if ((save_needed = !peep2_regno_dead_p (1, ACC_REGNUM))
      && !m65x_peephole_find_temp_regs (0, 1, &zpreg, NULL))
    return false;

  start_sequence ();

  if (save_needed)
    m65x_emit_movqi (zpreg, acc, can_clobber_nz);

  if (index == NULL_RTX)
    {
      if (TARGET_ZPIND)
	m65x_emit_movqi (acc, gen_rtx_MEM (QImode, base), can_clobber_nz);
      else
	{
	  if (y_available)
	    {
	      m65x_emit_movqi (yreg, const0_rtx, can_clobber_nz);
	      if (can_clobber_nz)
		emit_insn (gen_loadqi_indy_nz (acc, yreg, base));
	      else
		emit_insn (gen_loadqi_indy (acc, yreg, base));
	    }
	  else if (x_available)
	    {
	      m65x_emit_movqi (xreg, const0_rtx, can_clobber_nz);
	      if (can_clobber_nz)
		emit_insn (gen_loadqi_xind_nz (acc, base, xreg));
	      else
		emit_insn (gen_loadqi_xind (acc, base, xreg));
	    }
	  else
	    {
	      end_sequence ();
	      return false;
	    }
	}
    }
  else
    {
      if (can_clobber_nz)
	emit_insn (gen_loadqi_indy_nz (acc, index, base));
      else
	emit_insn (gen_loadqi_indy (acc, index, base));
    }

  m65x_emit_movqi (dst, acc, can_clobber_nz);

  if (save_needed)
    m65x_emit_movqi (acc, zpreg, can_clobber_nz);

  seq = get_insns ();
  end_sequence ();

  emit_insn (seq);

  return true;
}

bool
m65x_peep_store (rtx base, rtx index, rtx src, bool can_clobber_nz)
{
  rtx acc = gen_rtx_REG (QImode, ACC_REGNUM);
  rtx xreg = gen_rtx_REG (QImode, X_REGNUM);
  rtx yreg = gen_rtx_REG (QImode, Y_REGNUM);
  rtx zpreg, seq;
  bool save_needed;
  bool x_available = peep2_regno_dead_p (1, X_REGNUM);
  bool y_available = peep2_regno_dead_p (1, Y_REGNUM);

  gcc_assert (index == NULL_RTX || rtx_equal_p (index, yreg));

  if (rtx_equal_p (src, acc))
    {
      gcc_assert (index == NULL_RTX);

      if (y_available)
	{
	  m65x_emit_movqi (yreg, const0_rtx, can_clobber_nz);
	  emit_insn (gen_storeqi_indy (yreg, base, acc));
	}
      else if (x_available)
	{
	  m65x_emit_movqi (xreg, const0_rtx, can_clobber_nz);
	  emit_insn (gen_storeqi_xind (base, xreg, acc));
	}
      else
	return false;

      return true;
    }

  if ((save_needed = !peep2_regno_dead_p (1, ACC_REGNUM))
      && !m65x_peephole_find_temp_regs (0, 1, &zpreg, NULL))
    return false;

  start_sequence ();

  if (save_needed)
    m65x_emit_movqi (zpreg, acc, can_clobber_nz);

  m65x_emit_movqi (acc, src, can_clobber_nz);

  if (index == NULL_RTX)
    {
      if (TARGET_ZPIND)
	m65x_emit_movqi (gen_rtx_MEM (QImode, base), acc, can_clobber_nz);
      else
	{
	  if (y_available)
	    {
	      m65x_emit_movqi (yreg, const0_rtx, can_clobber_nz);
	      emit_insn (gen_storeqi_indy (yreg, base, acc));
	    }
	  else if (x_available)
	    {
	      m65x_emit_movqi (xreg, const0_rtx, can_clobber_nz);
	      emit_insn (gen_storeqi_xind (base, xreg, acc));
	    }
	  else
	    {
	      end_sequence ();
	      return false;
	    }
	}
    }
  else
    emit_insn (gen_storeqi_indy (yreg, base, acc));

  if (save_needed)
    m65x_emit_movqi (acc, zpreg, can_clobber_nz);

  seq = get_insns ();
  end_sequence ();

  emit_insn (seq);

  return true;
}

static enum machine_mode
m65x_addr_space_pointer_mode (addr_space_t aspace)
{
  switch (aspace)
    {
    case ADDR_SPACE_GENERIC:
      return ptr_mode;
    case ADDR_SPACE_ZP:
      return QImode;
    default:
      gcc_unreachable ();
    }
}

static bool
m65x_addr_space_valid_pointer_mode (enum machine_mode mode, addr_space_t aspace)
{
  switch (aspace)
    {
    case ADDR_SPACE_GENERIC:
      return mode == ptr_mode;
    case ADDR_SPACE_ZP:
      return mode == ptr_mode || mode == QImode;
    default:
      gcc_unreachable ();
    }
}

static bool
m65x_addr_space_subset_p (addr_space_t subset, addr_space_t superset)
{
  if (subset == QImode)
    return superset == QImode || superset == ptr_mode;
  else if (subset == ptr_mode)
    return superset == ptr_mode;
  else
    gcc_unreachable ();
}

static rtx
m65x_addr_space_convert (rtx op, tree from_type, tree to_type)
{
  addr_space_t from_as = TYPE_ADDR_SPACE (TREE_TYPE (from_type));
  addr_space_t to_as = TYPE_ADDR_SPACE (TREE_TYPE (to_type));
  
  if (to_as == ADDR_SPACE_GENERIC && from_as == ADDR_SPACE_ZP)
    return force_reg (ptr_mode, op);
  else if (to_as == ADDR_SPACE_ZP && from_as == ADDR_SPACE_GENERIC)
    return force_reg (QImode, op);
  else
    gcc_unreachable ();
}

bool
m65x_regno_mode_code_ok_for_base_p (int regno, enum machine_mode mode,
				    addr_space_t as, enum rtx_code outer,
				    enum rtx_code index)
{
  switch (as)
    {
    case ADDR_SPACE_GENERIC:
      switch (outer)
        {
	case MEM:
	case ADDRESS:
	  return IS_ZP_REGNUM (regno);
	case PLUS:
	  if (mode == QImode)
	    return index == REG && IS_ZP_REGNUM (regno);
	  else
	    return index != REG && IS_ZP_REGNUM (regno);
	default:
	  gcc_unreachable ();
	}

    case ADDR_SPACE_ZP:
      switch (outer)
        {
	case MEM:
	case ADDRESS:
	  return regno == X_REGNUM || regno == Y_REGNUM;
	case PLUS:
	  return index != REG && index != MEM
		 && (regno == X_REGNUM || regno == Y_REGNUM);
	default:
	  gcc_unreachable ();
	}
      break;

    default:
      gcc_unreachable ();
    }

  return false;
}

static void
m65x_reorg (void)
{
  basic_block bb;
  regset_head live;
  rtx acc = gen_rtx_REG (QImode, ACC_REGNUM);

  INIT_REG_SET (&live);

  compute_bb_for_insn ();

  df_live_add_problem ();
  df_live_set_all_dirty ();
  df_analyze ();

  if (!optimize)
    split_all_insns_noflow ();

  FOR_EACH_BB_FN (bb, cfun)
    {
      rtx_insn *insn, *curr;

      COPY_REG_SET (&live, DF_LIVE_OUT (bb));
      df_simulate_initialize_backwards (bb, &live);

      FOR_BB_INSNS_REVERSE_SAFE (bb, insn, curr)
	{
	  if (NONDEBUG_INSN_P (insn))
	    {
	      rtx set;
	    retry:
	      int icode = recog_memoized (insn);

	      switch (icode)
		{
		case CODE_FOR_movqi_insn:
		  set = single_set (insn);

		  if (set)
		    {
		      rtx dst = SET_DEST (set);
		      rtx src = SET_SRC (set);

		      /* Split insns that store to indirect-indexed or
		         ZP-indirect addresses from somewhere other than the
			 accumulator into moves via the accumulator, if it is
			 dead anyway:

			   (zp),y := <X>  -->

			   lda <X>
			   sta (zp,Y)
		      */

		      if (MEM_P (dst)
			  && (m65x_indirect_indexed_addr_p (QImode,
							    XEXP (dst, 0), true)
			      || m65x_address_register_p (XEXP (dst, 0), true))
			  && !accumulator_operand (src, QImode)
			  && !REGNO_REG_SET_P (&live, ACC_REGNUM))
			{
			  emit_insn_before (gen_movqi_insn (acc, src),
					    insn);
			  SET_SRC (set) = acc;
			}
		    }
		  break;

		case CODE_FOR_movqi_noclob:
		  if (!REGNO_REG_SET_P (&live, NZ_REGNUM)
		      && (set = single_set (insn)))
		    {
		      PATTERN (insn)
			= gen_movqi_insn (SET_DEST (set), SET_SRC (set));
		      INSN_CODE (insn) = -1;
		      goto retry;
		    }
		  break;

		case CODE_FOR_addqi3_noclob:
		  if (!REGNO_REG_SET_P (&live, NZ_REGNUM)
		      && !REGNO_REG_SET_P (&live, CARRY_REGNUM)
		      && !REGNO_REG_SET_P (&live, OVERFLOW_REGNUM)
		      && (set = single_set (insn)))
		    {
		      PATTERN (insn)
			= gen_addqi3 (SET_DEST (set),
				      XEXP (SET_SRC (set), 0),
				      XEXP (SET_SRC (set), 1));
		      INSN_CODE (insn) = -1;
		    }
		  break;

		case CODE_FOR_addhi3_noclobacc:
		  if (!REGNO_REG_SET_P (&live, NZ_REGNUM)
		      && !REGNO_REG_SET_P (&live, CARRY_REGNUM)
		      && !REGNO_REG_SET_P (&live, OVERFLOW_REGNUM)
		      && !REGNO_REG_SET_P (&live, ACC_REGNUM)
		      && (set = single_set (insn)))
		    {
		      PATTERN (insn)
			= gen_addhi3_clobacc (SET_DEST (set),
					      XEXP (SET_SRC (set), 0),
					      XEXP (SET_SRC (set), 1));
		      INSN_CODE (insn) = -1;
		    }
		  break;

		default:
		  ;
		}

	      df_simulate_one_insn_backwards (bb, insn, &live);
	    }
	}
    }
}

rtx
m65x_static_chain (const_tree fndecl, bool incoming_p)
{
  if (!DECL_STATIC_CHAIN (fndecl))
    return NULL;

  return default_static_chain (fndecl, incoming_p);
}

bool
m65x_prefer_constant_equiv_p (rtx cst ATTRIBUTE_UNUSED)
{
  return true;
}

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE m65x_option_override

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

#undef TARGET_SCALAR_MODE_SUPPORTED_P
#define TARGET_SCALAR_MODE_SUPPORTED_P m65x_scalar_mode_supported_p

#undef TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P m65x_legitimate_address_p

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS m65x_legitimize_address

#undef TARGET_DELEGITIMIZE_ADDRESS
#define TARGET_DELEGITIMIZE_ADDRESS m65x_delegitimize_address

#undef TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P
#define TARGET_SMALL_REGISTER_CLASSES_FOR_MODE_P m65x_small_classes_for_mode

#undef TARGET_SPILL_CLASS
#define TARGET_SPILL_CLASS m65x_spill_class

#undef TARGET_CLASS_LIKELY_SPILLED_P
#define TARGET_CLASS_LIKELY_SPILLED_P m65x_class_likely_spilled_p

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD m65x_secondary_reload

#undef TARGET_PREFERRED_RELOAD_CLASS
#define TARGET_PREFERRED_RELOAD_CLASS m65x_preferred_reload_class

#undef TARGET_LRA_P
#define TARGET_LRA_P m65x_lra_p

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST m65x_address_cost

#undef TARGET_DIFFERENT_ADDR_DISPLACEMENT
#define TARGET_DIFFERENT_ADDR_DISPLACEMENT hook_void_true

#undef TARGET_REGISTER_MOVE_COST
#define TARGET_REGISTER_MOVE_COST m65x_register_move_cost

#undef TARGET_CANONICALIZE_COMPARISON
#define TARGET_CANONICALIZE_COMPARISON m65x_canonicalize_comparison

#undef TARGET_ASM_INTEGER
#define TARGET_ASM_INTEGER m65x_asm_integer

#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION m65x_asm_named_section

#undef TARGET_ASM_FUNCTION_SECTION
#define TARGET_ASM_FUNCTION_SECTION m65x_asm_function_section

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION m65x_asm_select_section

#undef TARGET_ASM_GLOBALIZE_LABEL
#define TARGET_ASM_GLOBALIZE_LABEL m65x_asm_globalize_label

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE m65x_asm_function_prologue

#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE m65x_can_eliminate

#undef TARGET_ADDR_SPACE_POINTER_MODE
#define TARGET_ADDR_SPACE_POINTER_MODE m65x_addr_space_pointer_mode

#undef TARGET_ADDR_SPACE_ADDRESS_MODE
#define TARGET_ADDR_SPACE_ADDRESS_MODE m65x_addr_space_pointer_mode

#undef TARGET_ADDR_SPACE_VALID_POINTER_MODE
#define TARGET_ADDR_SPACE_VALID_POINTER_MODE m65x_addr_space_valid_pointer_mode

#undef TARGET_ADDR_SPACE_SUBSET_P
#define TARGET_ADDR_SPACE_SUBSET_P m65x_addr_space_subset_p

#undef TARGET_ADDR_SPACE_CONVERT
#define TARGET_ADDR_SPACE_CONVERT m65x_addr_space_convert

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG m65x_reorg

#undef TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN m65x_static_chain

#undef TARGET_PREFER_CONSTANT_EQUIV_P
#define TARGET_PREFER_CONSTANT_EQUIV_P m65x_prefer_constant_equiv_p

struct gcc_target targetm = TARGET_INITIALIZER;
