(define_register_constraint "A" "ACCUMULATOR_REG")

(define_register_constraint "x" "X_REG")

(define_register_constraint "y" "Y_REG")

(define_register_constraint "d" "INDEX_REGS")

(define_register_constraint "D" "DIRECT_REG")

(define_register_constraint "S" "STACK_REG")

(define_constraint "I"
  "An integer 0-255."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 255")))

(define_constraint "J"
  "An integer 0-65535."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival <= 65535")))
