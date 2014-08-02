(define_predicate "accumulator_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REG_P (op)
	 && (REGNO (op) == ACC_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "x_reg_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REG_P (op)
	 && (REGNO (op) == X_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "y_reg_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REG_P (op)
	 && (REGNO (op) == Y_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "index_reg_operand"
  (ior (match_operand 0 "x_reg_operand")
       (match_operand 0 "y_reg_operand")))

(define_predicate "hard_reg_operand"
  (match_code "reg,subreg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REG_P (op)
	 && (REGNO (op) == ACC_REGNUM || REGNO (op) == X_REGNUM
	     || REGNO (op) == Y_REGNUM || REGNO (op) >= FIRST_PSEUDO_REGISTER);
})

(define_predicate "mov_dst_operand"
  (match_code "reg,subreg,mem")
{
  if (memory_operand (op, mode))
    return true;

  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  if (!REG_P (op))
    return false;
  
  return true;
})

(define_predicate "mov_src_operand"
  (match_code "reg,subreg,const_int,mem,label_ref,symbol_ref,const")
{
  if (memory_operand (op, mode))
    return true;

  if (CONSTANT_P (op))
    return true;

  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);

  if (!REG_P (op))
    return false;

  return true;
})

(define_predicate "const_mem_operand"
  (and (match_code "mem")
       (match_test "CONSTANT_ADDRESS_P (XEXP (op, 0))")))

(define_predicate "indirect_mem_operand"
  (and (match_code "mem")
       (match_test "REG_P (XEXP (op, 0))")))

(define_predicate "indirect_offset_mem_operand"
  (and (match_code "mem")
       (match_test "m65x_indirect_offset_addr_p (mode, XEXP (op, 0), false)")))

(define_predicate "ind_y_mem_operand"
  (and (match_code "mem")
       (match_test "GET_CODE (XEXP (op, 0)) == PLUS
		    && GET_CODE (XEXP (XEXP (op, 0), 0)) == ZERO_EXTEND
		    && REG_P (XEXP (XEXP (XEXP (op, 0), 0), 0))
		    && REG_P (XEXP (XEXP (op, 0), 1))")))

(define_predicate "hard_sp_operand"
  (and (match_code "mem")
       (match_test "(GET_CODE (XEXP (op, 0)) == POST_DEC
		     || GET_CODE (XEXP (op, 0)) == PRE_INC)
		    && REG_P (XEXP (XEXP (op, 0), 0))
		    && REGNO (XEXP (XEXP (op, 0), 0)) == HARDSP_REGNUM")))

(define_predicate "strict_zp_reg_operand"
  (match_code "subreg,reg")
{
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  return REG_P (op) && IS_ZP_REGNUM (REGNO (op));
})

(define_predicate "zp_reg_operand"
  (match_code "reg,subreg")
{
  int regno;
  
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  if (!REG_P (op))
    return false;
  
  regno = REGNO (op);
  
  return IS_ZP_REGNUM (regno) || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "ptr_reg_operand"
  (match_code "reg,subreg")
{
  int regno;
  
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  if (!REG_P (op))
    return false;
  
  regno = REGNO (op);
  
  return IS_ZP_REGNUM (regno) || regno == ARG_POINTER_REGNUM
	 || regno == FRAME_POINTER_REGNUM || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "strict_ptr_reg_operand"
  (match_code "reg,subreg")
{
  int regno;
  
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  if (!REG_P (op))
    return false;
  
  regno = REGNO (op);
  
  return IS_ZP_REGNUM (regno) || regno == ARG_POINTER_REGNUM
	 || regno == FRAME_POINTER_REGNUM;
})

(define_predicate "reload_zpreg_operand"
  (match_code "reg,subreg")
{
  int regno;
  
  if (reload_in_progress)
    {
      regno = true_regnum (op);

      if (regno == -1)
	return false;
    }
  else
    {
      if (GET_CODE (op) == SUBREG)
	op = SUBREG_REG (op);

      if (!REG_P (op))
        return false;

      regno = REGNO (op);
    }
  
  return IS_ZP_REGNUM (regno);
})

(define_predicate "zp_reg_or_acc_operand"
  (ior (match_operand 0 "accumulator_operand")
       (match_operand 0 "zp_reg_operand")))

(define_predicate "zp_or_hard_reg_operand"
  (ior (match_operand 0 "hard_reg_operand")
       (match_operand 0 "zp_reg_operand")))

(define_predicate "zp_or_index_reg_operand"
  (ior (match_operand 0 "index_reg_operand")
       (match_operand 0 "zp_reg_operand")))

(define_predicate "zp_reg_or_const_mem_operand"
  (ior (match_operand 0 "zp_reg_operand")
       (match_operand 0 "const_mem_operand")))

(define_predicate "qimode_ior_dst_operand"
  (ior (match_operand 0 "accumulator_operand")
       (and (match_test "TARGET_TRB_TSB")
	    (match_operand 0 "zp_reg_or_const_mem_operand"))))

(define_predicate "zp_reg_or_ind_y_mem_operand"
  (ior (match_operand 0 "zp_reg_operand")
       (match_operand 0 "ind_y_mem_operand")))

(define_predicate "zp_reg_or_ind_y_mem_imm_operand"
  (ior (match_operand 0 "zp_reg_operand")
       (match_operand 0 "ind_y_mem_operand")
       (match_operand 0 "immediate_operand")))

(define_predicate "reg_or_const_mem_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "const_mem_operand")))

(define_predicate "zp_memory_operand"
  (and (match_operand 0 "memory_operand")
       (match_test "MEM_ADDR_SPACE (op) == ADDR_SPACE_ZP")))

(define_predicate "symlab_ref_operand"
  (match_code "label_ref,symbol_ref"))

(define_predicate "sym_const_operand"
  (match_code "label_ref,symbol_ref,plus,const")
{
  if (GET_CODE (op) == PLUS)
    return sym_const_operand (XEXP (op, 0), mode);
  
  return true;
})

(define_predicate "zp_reg_or_imm_operand"
  (ior (match_operand 0 "immediate_operand")
       (match_operand 0 "zp_reg_operand")))

(define_predicate "ptr_reg_or_int_operand"
  (ior (match_operand 0 "const_int_operand")
       (match_operand 0 "ptr_reg_operand")))

(define_predicate "zp_acc_imm_operand"
  (ior (match_operand 0 "accumulator_operand")
       (match_operand 0 "zp_reg_operand")
       (match_operand 0 "immediate_operand")))

(define_predicate "addhi_op2"
  (ior (match_operand 0 "zp_acc_imm_operand")
       (and (match_code "zero_extend")
	    (match_test "REG_P (XEXP (op, 0))
			 && REGNO (XEXP (op, 0)) == Y_REGNUM"))))

(define_predicate "const_byte_amount"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 0 && INTVAL (op) < 256")))

(define_predicate "shifthi_amount"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 1 && INTVAL (op) <= 16")))

(define_predicate "shifthi_rt_byteswap"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == 5 || INTVAL (op) == 6")))

(define_predicate "shiftqi_amount"
  (and (match_code "const_int")
       (match_test "INTVAL (op) >= 1 && INTVAL (op) <= 8")))

(define_predicate "const_one_amount"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == 1")))

(define_special_predicate "m65x_comparison"
  (match_code "eq,ne,gtu,ltu,geu,leu,gt,lt,ge,le"))

(define_special_predicate "qimode_ui_comparison"
  (match_code "ltu,geu"))

(define_special_predicate "qimode_v_comparison"
  (match_code "ltu,geu"))

(define_special_predicate "qimode_nz_comparison"
  (match_code "eq,ne,lt,ge"))

(define_special_predicate "qimode_c_comparison"
  (match_code "eq,ne"))

(define_predicate "compareqi_src_operand"
  (match_code "reg,subreg,const_int,mem,label_ref,symbol_ref,const")
{
  int regno;
  
  if (MEM_P (op))
    return true;
  
  if (CONSTANT_P (op))
    return true;

  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);
  
  if (GET_CODE (op) == LABEL_REF || GET_CODE (op) == SYMBOL_REF
      || GET_CODE (op) == CONST)
    return true;
  
  if (!REG_P (op))
    return false;
  
  regno = REGNO (op);
  
  return IS_ZP_REGNUM (regno)
	 || regno == FRAME_POINTER_REGNUM
	 || regno == ARG_POINTER_REGNUM
	 || regno >= FIRST_PSEUDO_REGISTER;
})

(define_predicate "qimode_src_operand"
  (match_operand 0 "compareqi_src_operand"))

(define_predicate "qimode_ior_src_operand"
  (ior (match_operand 0 "qimode_src_operand")
       (and (match_test "TARGET_TRB_TSB")
	    (match_operand 0 "accumulator_operand"))))
