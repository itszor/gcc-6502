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

(define_predicate "reg_or_acc_operand"
  (ior (match_operand 0 "accumulator_operand")
       (match_operand 0 "register_operand")))

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
  	 || (regno >= FIRST_ZP_REGISTER && regno <= LAST_ZP_REGISTER)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "movhi_src_operand"
  (match_code "reg,subreg,const_int,mem,const")
{
  int regno;

  if (MEM_P (op))
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
  	 || (regno >= FIRST_ZP_REGISTER && regno <= LAST_ZP_REGISTER)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "const_mem_operand"
  (and (match_code "mem")
       (match_test "CONSTANT_P (XEXP (op, 0))")))

(define_predicate "zp_reg_operand"
  (match_code "reg,subreg")
{
  int regno;
  
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  regno = REGNO (op);
  
  return (regno >= FIRST_PSEUDO_REGISTER || REGNO_OK_FOR_BASE_P (regno));
})

(define_predicate "zp_reg_or_const_mem_operand"
  (ior (match_operand 0 "zp_reg_operand")
       (match_operand 0 "const_mem_operand")))
