(define_predicate "accumulator_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REGNO (op) == ACC_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "x_reg_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return (REGNO (op) == X_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "y_reg_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return (REGNO (op) == Y_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "index_reg_operand"
  (ior (match_operand 0 "x_reg_operand")
       (match_operand 0 "y_reg_operand")))

(define_predicate "hard_reg_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return (REGNO (op) == ACC_REGNUM || REGNO (op) == X_REGNUM
	  || REGNO (op) == Y_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "movhi_dst_operand"
  (match_code "reg,subreg,mem")
{
  int regno;

  if (MEM_P (op))
    return true;

  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  regno = REGNO (op);
  
  return regno == ACC_REGNUM
	 || regno == X_REGNUM
	 || regno == Y_REGNUM
  	 || IS_ZP_REGNUM (regno)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "movhi_src_operand"
  (match_code "reg,subreg,const_int,mem,label_ref,const")
{
  int regno;

  if (MEM_P (op))
    return true;

  if (GET_CODE (op) == LABEL_REF)
    return true;

  if (GET_CODE (op) == CONST_INT)
    return true;
  
  if (GET_CODE (op) == CONST)
    {
      op = XEXP (op, 0);
      return ((GET_CODE (op) == PLUS && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	       && GET_CODE (XEXP (op, 1)) == CONST_INT)
	      || GET_CODE (op) == SYMBOL_REF);
    }
  
  regno = REGNO (op);
  
  return regno == ACC_REGNUM
	 || regno == X_REGNUM
	 || regno == Y_REGNUM
  	 || IS_ZP_REGNUM (regno)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "const_mem_operand"
  (and (match_code "mem")
       (match_test "CONSTANT_P (XEXP (op, 0))")))

(define_predicate "ind_y_mem_operand"
  (and (match_code "mem")
       (match_test "GET_CODE (XEXP (op, 0)) == PLUS
		    && zp_reg_operand (XEXP (XEXP (op, 0), 0),
				       GET_MODE (XEXP (XEXP (op, 0), 0)))
		    && GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
		    && INTVAL (XEXP (XEXP (op, 0), 1)) >= 0
		    && INTVAL (XEXP (XEXP (op, 0), 1)) < 256")))

(define_predicate "zp_reg_operand"
  (match_code "reg,subreg")
{
  int regno;
  
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  regno = REGNO (op);
  
  return IS_ZP_REGNUM (regno);
})

(define_predicate "zp_reg_or_acc_operand"
  (ior (match_operand 0 "accumulator_operand")
       (match_operand 0 "zp_reg_operand")))

(define_predicate "zp_reg_or_const_mem_operand"
  (ior (match_operand 0 "zp_reg_operand")
       (match_operand 0 "const_mem_operand")))

(define_predicate "shifthi_amount"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 1 && INTVAL (op) <= 16")))

(define_predicate "shifthi_rt_byteswap"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == 5 || INTVAL (op) == 6")))

(define_special_predicate "himode_comparison"
  (match_code "eq,ne,gtu,geu,gt,ge"))

(define_special_predicate "qimode_comparison"
  (match_code "eq,ne"))

(define_predicate "qimode_src_operand"
  (match_code "reg,subreg,const_int,mem,label_ref,const")
{
  int regno;
  
  if (MEM_P (op) || GET_CODE (op) == LABEL_REF || GET_CODE (op) == CONST_INT)
    return true;
  
  if (GET_CODE (op) == CONST)
    {
      op = XEXP (op, 0);
      return ((GET_CODE (op) == PLUS && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	       && GET_CODE (XEXP (op, 1)) == CONST_INT)
	      || GET_CODE (op) == SYMBOL_REF);
    }
  
  regno = REGNO (op);
  
  return regno == ACC_REGNUM
	 || regno == X_REGNUM
	 || regno == Y_REGNUM
  	 || IS_ZP_REGNUM (regno)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})
