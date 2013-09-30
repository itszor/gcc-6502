(define_register_constraint "A" "HARD_ACCUM_REG")

(define_register_constraint "x" "HARD_X_REG")

(define_register_constraint "y" "HARD_Y_REG")

(define_register_constraint "j" "HARD_INDEX_REGS")

(define_register_constraint "h" "HARD_REGS")

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

(define_constraint "M"
  "An integer -255-0."
  (and (match_code "const_int")
       (match_test "ival >= -255 && ival <= 0")))

(define_constraint "N"
  "An integer 1-16."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 16")))

(define_constraint "O"
  "An integer which is hard to right-shift 16-bit values by."
  (and (match_code "const_int")
       (match_test "ival == 5 || ival == 6")))

(define_memory_constraint "U"
  "A constant mem."
  (and (match_code "mem")
       (match_test "CONSTANT_P (XEXP (op, 0))")))
