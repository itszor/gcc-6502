/* Storage layout.  */

#define BITS_BIG_ENDIAN			0
#define BYTES_BIG_ENDIAN		0
#define WORDS_BIG_ENDIAN		0
#define LIBGCC2_WORDS_BIG_ENDIAN 	WORDS_BIG_ENDIAN

#define BITS_PER_UNIT			8
#define UNITS_PER_WORD			2
#define POINTER_SIZE			24
#define POINTERS_EXTEND_UNSIGNED	1

#define STACK_BOUNDARY			8
#define BIGGEST_ALIGNMENT		8

#define STRICT_ALIGNMENT		0

#define FUNCTION_BOUNDARY		8
#define PARM_BOUNDARY			8

/* Layout of source language data types.  */

#define INT_TYPE_SIZE			16
#define LONG_TYPE_SIZE			32
#define LONG_LONG_TYPE_SIZE		64
#define CHAR_TYPE_SIZE			8
#define FLOAT_TYPE_SIZE			32
#define DOUBLE_TYPE_SIZE		64
#define LONG_DOUBLE_TYPE_SIZE		64

#define DEFAULT_SIGNED_CHAR		0

/* Registers.  */

/* 0 : C  [A : B]
   1 : X
   2 : Y
   3 : D (direct register)
   4 : Stack pointer
*/

#define FIXED_REGISTERS		{ 0, 0, 0, 1, 1 }
#define CALL_USED_REGISTERS	{ 1, 1, 1, 1, 1 }

#define FIRST_PSEUDO_REGISTER 5

#define REG_ALLOC_ORDER		{ 0, 1, 2, 3, 4 }

#define HARD_REGNO_NREGS(REGNO, MODE)		\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)	\
   / UNITS_PER_WORD)

#define HARD_REGNO_MODE_OK(REGNO, MODE) 1

#define MODES_TIEABLE_P(MODE1, MODE2) 1

/* Register classes.  */

enum reg_class
{
  NO_REGS,
  ACCUMULATOR_REG,
  X_REG,
  Y_REG,
  INDEX_REGS,
  GENERAL_REGS,
  DIRECT_REG,
  STACK_REG,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define N_REG_CLASSES	(int) LIM_REG_CLASSES

#define REG_CLASS_NAMES		\
{				\
  "NO_REGS",			\
  "ACCUMULATOR_REG",		\
  "X_REG",			\
  "Y_REG",			\
  "INDEX_REGS",			\
  "GENERAL_REGS",		\
  "DIRECT_REG",			\
  "STACK_REG",			\
  "ALL_REGS"			\
}

#define REG_CLASS_CONTENTS	\
{				\
  0x00000000,			\
  0x00000001,			\
  0x00000002,			\
  0x00000004,			\
  0x00000006,			\
  0x00000007,			\
  0x00000008,			\
  0x00000010,			\
  0x0000001f			\
}

#define REGNO_REG_CLASS(REGNO)		\
  ((REGNO) == 0 ? ACCUMULATOR_REG :	\
   (REGNO) == 1 ? X_REG :		\
   (REGNO) == 2 ? Y_REG : 		\
   (REGNO) == 3 ? DIRECT_REG :		\
   (REGNO) == 4 ? STACK_REG :		\
   NO_REGS)

#define BASE_REG_CLASS	NO_REGS
#define INDEX_REG_CLASS	INDEX_REGS

#define REGNO_OK_FOR_BASE_P(NUM) ((NUM) == 3 || (NUM) == 4)

#define REGNO_OK_FOR_INDEX_P(NUM) ((NUM) == 1 || (NUM) == 2)

#define PREFERRED_RELOAD_CLASS(X, CLASS) CLASS

#define SMALL_REGISTER_CLASSES		1

#define CLASS_MAX_NREGS(CLASS, MODE)		\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)	\
   / UNITS_PER_WORD)

/* Stack layout/calling conventions.  */

#define STACK_GROWS_DOWNWARDS		1

/* Hardware stack has "empty" discipline.  */

#define STACK_PUSH_CODE			POST_DEC
#define FRAME_GROWS_DOWNWARDS		0
#define STARTING_FRAME_OFFSET		0

/* Registers accessing stack frame.  */

#define STACK_POINTER_REGNUM		4
#define FRAME_POINTER_REGNUM		3
/* FIXME: Trouble?  */
#define ARG_POINTER_REGNUM		3

/* Eliminating frame pointer/arg pointer.  */

#define FRAME_POINTER_REQUIRED		1

/* Passing function arguments on the stack.  */

#define PUSH_ARGS 1
#define PUSH_ROUNDING(BYTES) (BYTES)
#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, STACKSIZE) 0

#define FUNCTION_ARG_REGNO_P(REGNO)	0

#define FUNCTION_VALUE_REGNO_P(REGNO)	0

/* Dummy definition.  */

typedef int CUMULATIVE_ARGS;

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
  do { } while (0)

#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) 0

#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) \
  do { } while (0)

/*****************************************************************************
 * Trampolines.
 *****************************************************************************/

#define TRAMPOLINE_SIZE			0

/* Section: Addressing modes.  */

#define CONSTANT_ADDRESS_P(X) CONSTANT_P (X)

#define MAX_REGS_PER_ADDRESS		2

#define LEGITIMATE_CONSTANT_P(X)	1

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL)	\
  if (wdc65816_legitimate_address_p ((MODE), (X)))	\
    goto LABEL;

#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR, LABEL)	\
  if (wdc65816_mode_dependent_address_p (ADDR))		\
    goto LABEL;

/* Costs.  */

/* Note: depends on whether we can optimise mode switches.  */
#define SLOW_BYTE_ACCESS		1

/* Assembler format.  */

#define ASM_APP_OFF			""
#define ASM_APP_ON			""

#define ASM_GENERATE_INTERNAL_LABEL(LABEL, PREFIX, NUM) \
  sprintf (LABEL, "*.%s%u", PREFIX, (unsigned) (NUM))


/* Assembler format : Instruction Output.  */

#define REGISTER_NAMES			\
  { "a", "x", "y", "d", "s" }

#define PRINT_OPERAND(STREAM, X, CODE) \
  wdc65816_print_operand ((STREAM), (X), (CODE))

#define PRINT_OPERAND_ADDRESS(STREAM, X) \
  wdc65816_print_operand_address ((STREAM), (X))

/* Misc.  */

#define HAS_LONG_COND_BRANCH		0
#define HAS_LONG_UNCOND_BRANCH		1

#undef WORD_REGISTER_OPERATIONS
#define MOVE_MAX			2

#define Pmode				PSImode
#define FUNCTION_MODE			QImode
#define CASE_VECTOR_MODE		PSImode

#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1
