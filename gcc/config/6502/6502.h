/*****************************************************************************
 * Run-time target.
 *****************************************************************************/

#define TARGET_CPU_CPP_BUILTINS()		\
  do {						\
    builtin_define ("__6502__");		\
  } while (0)

/*****************************************************************************
 * Storage layout.
 *****************************************************************************/

#define BITS_BIG_ENDIAN			0
#define BYTES_BIG_ENDIAN		0
#define WORDS_BIG_ENDIAN		0
/*#define LIBGCC2_WORDS_BIG_ENDIAN 	WORDS_BIG_ENDIAN*/

#define BITS_PER_UNIT			8
#define UNITS_PER_WORD			2
#define POINTER_SIZE			16
#define POINTERS_EXTEND_UNSIGNED	1

#define STACK_BOUNDARY			8
#define BIGGEST_ALIGNMENT		8

#define STRICT_ALIGNMENT		0

#define FUNCTION_BOUNDARY		8
#define PARM_BOUNDARY			8

/*****************************************************************************
 * Layout of source language data types.
 *****************************************************************************/

#define INT_TYPE_SIZE			16
#define LONG_TYPE_SIZE			32
#define LONG_LONG_TYPE_SIZE		32
#define CHAR_TYPE_SIZE			8
#define FLOAT_TYPE_SIZE			32
#define DOUBLE_TYPE_SIZE		32
#define LONG_DOUBLE_TYPE_SIZE		32

#define DEFAULT_SIGNED_CHAR		0

/*****************************************************************************
 * Registers.
 *****************************************************************************/

/* 0 : A (real accumulator)
   1 : ah (zp)
   2 : ah2 (zp)
   3 : ah3 (zp)
   4 : X (real X register)
   5 : xh (zp)
   6 : xh2 (zp)
   7 : xh3 (zp)
   8 : Y (real Y register)
   9 : yh (zp)
   10 : yh2 (zp)
   11 : yh3 (zp)
   12 : sp/lo (zp)
   13 : sp/hi (zp)
   14 : a0 (argument regs)
   15 : a1
   16 : a2
   17 : a3
   18 : a4
   19 : a5
   20 : a6
   21 : a7
   22 : s0 (callee-saved regs)
   23 : s1
   24 : s2
   25 : s3
   26 : s4
   27 : s5
   28 : s6
   29 : s7
*/

#define ACC_REGNUM 0
#define X_REGNUM 4
#define Y_REGNUM 8
#define SP_REGNUM 12
#define FIRST_ARG_REGISTER 14
#define FIRST_CALLER_SAVED 22

#define FIXED_REGISTERS \
  { 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 1, 1, \
    /* arg regs.  */ \
    0, 0, 0, 0, 0, 0, 0, 0, \
    /* callee-saved regs.  */ \
    0, 0, 0, 0, 0, 0, 0, 0 }

#define CALL_USED_REGISTERS \
  { 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, \
    /* arg regs.  */ \
    1, 1, 1, 1, 1, 1, 1, 1, \
    /* callee-saved regs.  */ \
    0, 0, 0, 0, 0, 0, 0, 0 }

#define FIRST_PSEUDO_REGISTER 30

#define REG_ALLOC_ORDER \
  { 0, 1, 2, 3, 4, 5, 6, 7, \
    8, 9, 10, 11, 12, 13, \
    14, 15, 16, 17, 18, 19, 20, 21, \
    22, 23, 24, 25, 26, 27, 28, 29 }

#define HARD_REGNO_NREGS(REGNO, MODE)		\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)	\
   / UNITS_PER_WORD)

#define HARD_REGNO_MODE_OK(REGNO, MODE) 1

#define MODES_TIEABLE_P(MODE1, MODE2) 1

/*****************************************************************************
 * Register classes.
 *****************************************************************************/

enum reg_class
{
  NO_REGS,
  HARD_ACCUM_REG,
  ACCUMULATOR_REGS,
  HARD_X_REG,
  X_REGS,
  HARD_Y_REG,
  Y_REGS,
  INDEX_REGS,
  STACK_REG,
  ARG_REGS,
  CALLEE_SAVED_REGS,
  GENERAL_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define N_REG_CLASSES	(int) LIM_REG_CLASSES

#define REG_CLASS_NAMES		\
{				\
  "NO_REGS",			\
  "HARD_ACCUM_REG",		\
  "ACCUMULATOR_REGS",		\
  "HARD_X_REG",			\
  "X_REGS",			\
  "HARD_Y_REG",			\
  "Y_REGS",			\
  "INDEX_REGS",			\
  "STACK_REG",			\
  "ARG_REGS",			\
  "CALLEE_SAVED_REGS",		\
  "GENERAL_REGS",		\
  "ALL_REGS"			\
}

#define REG_CLASS_CONTENTS	\
{				\
  0x00000000,			\
  0x00000001,			\
  0x0000000f,			\
  0x00000010,			\
  0x000000f0,			\
  0x00000100,			\
  0x00000f00,			\
  0x00000ff0,			\
  0x00003000,			\
  0x003fc000,			\
  0x3fc00000,			\
  0x3fffcfff,			\
  0x3fffffff			\
}

#define REGNO_REG_CLASS(REGNO)				\
  ((REGNO) == 0 ? HARD_ACCUM_REG :			\
   (REGNO) >= 0 && (REGNO) <= 3 ? ACCUMULATOR_REGS :	\
   (REGNO) == 4 ? HARD_X_REG :				\
   (REGNO) >= 4 && (REGNO) <= 7 ? X_REGS :		\
   (REGNO) == 8 ? HARD_Y_REG : 				\
   (REGNO) >= 8 && (REGNO) <= 11 ? Y_REGS :		\
   (REGNO) >= 12 && (REGNO) <= 13 ? STACK_REG :		\
   (REGNO) >= 14 && (REGNO) <= 21 ? ARG_REGS :		\
   (REGNO) >= 22 && (REGNO) <= 29 ? CALLEE_SAVED_REGS :	\
   NO_REGS)

#define BASE_REG_CLASS	NO_REGS
#define INDEX_REG_CLASS	INDEX_REGS

#define REGNO_OK_FOR_BASE_P(NUM) ((NUM) == 3 || (NUM) == 4)

#define REGNO_OK_FOR_INDEX_P(NUM) ((NUM) == 1 || (NUM) == 2)

#define PREFERRED_RELOAD_CLASS(X, CLASS) CLASS

/*#define SMALL_REGISTER_CLASSES		1*/

#define CLASS_MAX_NREGS(CLASS, MODE)		\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)	\
   / UNITS_PER_WORD)

/*****************************************************************************
 * Stack layout/calling conventions.
 *****************************************************************************/

#define STACK_GROWS_DOWNWARDS		1

/* Hardware stack has "empty" discipline.  */

#define STACK_PUSH_CODE			POST_DEC
#define FRAME_GROWS_DOWNWARDS		0
#define STARTING_FRAME_OFFSET		0

#define FIRST_PARM_OFFSET(FNDECL)	0

/* Registers accessing stack frame.  */

#define STACK_POINTER_REGNUM		4
#define FRAME_POINTER_REGNUM		3
/* FIXME: Trouble?  */
#define ARG_POINTER_REGNUM		3

/* Eliminating frame pointer/arg pointer.  */

/*#define FRAME_POINTER_REQUIRED		1*/

/* Utterly bogus!  Maybe define ELIMINABLE_REGS instead.  */

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH_VAR)	4

/* Passing function arguments on the stack.  */

#define PUSH_ARGS 1
#define PUSH_ROUNDING(BYTES) (BYTES)
/*#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, STACKSIZE) 0*/

#define FUNCTION_ARG_REGNO_P(REGNO)	0

#define FUNCTION_VALUE_REGNO_P(REGNO)	0

/* Scalar return.  */

#define LIBCALL_VALUE(MODE)		0

/* Dummy definition.  */

typedef int CUMULATIVE_ARGS;

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
  do { (CUM) = 0; } while (0)

/* Profiling.  */

#define FUNCTION_PROFILER(FILE, LABELNO) \
  do { /* Unimplemented.  */ } while (0)

/*****************************************************************************
 * Trampolines.
 *****************************************************************************/

#define TRAMPOLINE_SIZE			0

#if 0
#define INITIALIZE_TRAMPOLINE(ADDR, FNADDR, STATIC_CHAIN) \
  do { /* Unimplemented.  */ } while (0)
#endif

/*****************************************************************************
 * Addressing modes.
 *****************************************************************************/

#define CONSTANT_ADDRESS_P(X) CONSTANT_P (X)

#define MAX_REGS_PER_ADDRESS		2

/*#define LEGITIMATE_CONSTANT_P(X)	1*/

/*#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR, LABEL)	\
  if (m65x_mode_dependent_address_p (ADDR))		\
    goto LABEL;*/

/*****************************************************************************
 * Costs.
 *****************************************************************************/

/* Note: depends on whether we can optimise mode switches.  */
#define SLOW_BYTE_ACCESS		1

/*****************************************************************************
 * Assembler format.
 *****************************************************************************/

#define ASM_APP_OFF			""
#define ASM_APP_ON			""

#define ASM_GENERATE_INTERNAL_LABEL(LABEL, PREFIX, NUM) \
  sprintf (LABEL, "*.%s%u", PREFIX, (unsigned) (NUM))

/* Instruction Output.  */

#define REGISTER_NAMES					\
  {							\
    "a", "ah", "ah2", "ah3",				\
    "x", "xh", "xh2", "xh3",				\
    "y", "yh", "yh2", "yh3",				\
    "sp", "?sp",					\
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",	\
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"	\
  }

#define PRINT_OPERAND(STREAM, X, CODE) \
  m65x_print_operand ((STREAM), (X), (CODE))

#define PRINT_OPERAND_ADDRESS(STREAM, X) \
  m65x_print_operand_address ((STREAM), (X))

#define ASM_OUTPUT_ALIGN(STREAM, POWER) \
  fprintf ((STREAM), ".align %d", (POWER))

/* Output of uninitialized variables.  */

#define ASM_OUTPUT_COMMON(STREAM, NAME, SIZE, ROUNDED)		\
  do {								\
    fprintf ((STREAM), "; .common");				\
    assemble_name ((STREAM), (NAME));				\
  } while (0)

#define ASM_OUTPUT_LOCAL(STREAM, NAME, SIZE, ROUNDED)		\
  do {								\
    fprintf ((STREAM), "; .local");				\
    assemble_name ((STREAM), (NAME));				\
  } while (0)

#define ASM_OUTPUT_SKIP(STREAM, NBYTES)				\
  do {								\
    fprintf ((STREAM), ".dsb %d,$00", (int) (NBYTES));		\
  } while (0)

/*****************************************************************************
 * Misc.
 *****************************************************************************/

#define HAS_LONG_COND_BRANCH		0
#define HAS_LONG_UNCOND_BRANCH		1

#undef WORD_REGISTER_OPERATIONS
#define MOVE_MAX			2

#define Pmode				HImode
#define FUNCTION_MODE			QImode
#define CASE_VECTOR_MODE		HImode

#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1
