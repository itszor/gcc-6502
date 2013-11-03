(define_register_constraint "Ah" "HARD_ACCUM_REG")
(define_register_constraint "Ac" "WORD_ACCUM_REGS")

(define_register_constraint "xh" "HARD_X_REG")
(define_register_constraint "xc" "WORD_X_REGS")

(define_register_constraint "yh" "HARD_Y_REG")
(define_register_constraint "yc" "WORD_Y_REGS")

(define_register_constraint "jh" "HARD_INDEX_REGS")
(define_register_constraint "jc" "INDEX_REGS")

(define_register_constraint "hh" "HARD_REGS")
(define_register_constraint "hc" "HARDISH_REGS")

(define_register_constraint "S" "STACK_REG")

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

(define_memory_constraint "U"
  "A constant mem."
  (and (match_code "mem")
       (match_test "CONSTANT_P (XEXP (op, 0))")))
