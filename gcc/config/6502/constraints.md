(define_register_constraint "Aq" "HARD_ACCUM_REG")
;(define_register_constraint "Ah" "WORD_ACCUM_REGS")
;(define_register_constraint "As" "ACCUM_REGS")

(define_register_constraint "xq" "HARD_X_REG")
;(define_register_constraint "xh" "WORD_X_REGS")
;(define_register_constraint "xs" "X_REGS")

(define_register_constraint "yq" "HARD_Y_REG")
;(define_register_constraint "yh" "WORD_Y_REGS")
;(define_register_constraint "ys" "Y_REGS")

(define_register_constraint "jq" "HARD_INDEX_REGS")
;(define_register_constraint "jh" "WORD_INDEX_REGS")
;(define_register_constraint "js" "INDEX_REGS")

(define_register_constraint "hq" "ACTUALLY_HARD_REGS")
;(define_register_constraint "hh" "WORD_HARD_REGS")
;(define_register_constraint "hs" "HARD_REGS")

(define_register_constraint "Zp" "HARD_ZP_REGS")

(define_constraint "I"
  "An integer 0-255."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 255")))

(define_constraint "J"
  "An integer 0-65535."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 65535")))

(define_constraint "K"
  "The integer 1."
  (and (match_code "const_int")
       (match_test "ival == 1")))

(define_constraint "L"
  "The integer -1."
  (and (match_code "const_int")
       (match_test "ival == -1")))

(define_constraint "z"
  "The integer zero."
  (and (match_code "const_int")
       (match_test "ival == 0")))

(define_constraint "M"
  "An integer -255-0."
  (and (match_code "const_int")
       (match_test "ival >= -255 && ival <= 0")))

(define_constraint "Ns"
  "An integer 1-16."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 16")))

(define_constraint "Nr"
  "A right-shift amount that needs no scratch register (1-4, 7-16)."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 16 && ival != 5 && ival != 6")))

(define_constraint "Nc"
  "An integer which is hard to right-shift 16-bit values by."
  (and (match_code "const_int")
       (match_test "ival == 5 || ival == 6")))

(define_constraint "Nl"
  "A left-shift amount that needs no scratch register (1-5, 7-16)."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 16 && ival != 6")))

(define_constraint "Nx"
  "The integer 6."
  (and (match_code "const_int")
       (match_test "ival == 6")))

(define_constraint "Na"
  "Arithmetic right-shift amounts which need a scratch reg (1-14)."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 14")))

(define_constraint "Nb"
  "The integers 15 or 16."
  (and (match_code "const_int")
       (match_test "ival == 15 || ival == 16")))

(define_constraint "NB"
  "An integer 1-8."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 8")))

(define_constraint "S"
  "A symbol_ref or a label_ref or a subreg of either."
  (and (match_code "symbol_ref,label_ref,subreg")
       (match_test "GET_CODE (op) != SUBREG
		    || GET_CODE (XEXP (op, 0)) == SYMBOL_REF
		    || GET_CODE (XEXP (op, 0)) == LABEL_REF")))

(define_constraint "Uc"
  "A constant mem."
  (and (match_code "mem")
       (match_test "CONSTANT_ADDRESS_P (XEXP (op, 0))")))

(define_constraint "Uo"
  "An indirect offset mem."
  (and (match_code "mem")
       (match_test "m65x_indirect_offset_addr_p (mode, XEXP (op, 0), false)")))

(define_constraint "Uy"
  "An indirect indexed mem."
  (and (match_code "mem")
       (match_test "m65x_indirect_indexed_addr_p (mode, XEXP (op, 0), false)")))

(define_constraint "Uj"
  "An absolute indexed mem."
  (and (match_code "mem")
       (match_test "m65x_absolute_indexed_addr_p (mode, XEXP (op, 0), false)")))

(define_constraint "UX"
  "An absolute,X mem."
  (and (match_code "mem")
       (match_test "m65x_absolute_x_addr_p (mode, XEXP (op, 0), false)")))

(define_constraint "UY"
  "An absolute,Y mem."
  (and (match_code "mem")
       (match_test "m65x_absolute_y_addr_p (mode, XEXP (op, 0), false)")))

(define_memory_constraint "Ur"
  "An indirect mem."
  (and (match_code "mem")
       (match_test "REG_P (XEXP (op, 0))")))

(define_constraint "ZX"
  "A zero page,X mem."
  (and (match_code "mem")
       (match_test "MEM_ADDR_SPACE (op) == ADDR_SPACE_ZP
		    && m65x_zeropage_x_addr_p (mode, XEXP (op, 0), false)")))

(define_constraint "ZY"
  "A zero page,Y mem."
  (and (match_code "mem")
       (match_test "MEM_ADDR_SPACE (op) == ADDR_SPACE_ZP
		    && m65x_zeropage_y_addr_p (mode, XEXP (op, 0), false)")))

(define_constraint "Zj"
  "A zero page,[xy] mem."
  (and (match_code "mem")
       (match_test "MEM_ADDR_SPACE (op) == ADDR_SPACE_ZP
		    && m65x_zeropage_indexed_addr_p (mode, XEXP (op, 0),
						     false)")))
