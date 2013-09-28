(define_predicate "accumulator_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REGNO (op) == ACC_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER;
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
  (match_code "reg,subreg,const_int,mem")
{
  int regno;

  if (MEM_P (op))
    return true;

  if (GET_CODE (op) == CONST_INT)
    return true;
  
  regno = REGNO (op);
  
  return regno == ACC_REGNUM
	 || regno == X_REGNUM
	 || regno == Y_REGNUM
  	 || (regno >= FIRST_ZP_REGISTER && regno <= LAST_ZP_REGISTER)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})
