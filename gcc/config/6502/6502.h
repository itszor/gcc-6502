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
   14 : fp/lo (zp)
   15 : fp/hi (zp)
   16 : a0 (argument regs)
   17 : a1
   18 : a2
   19 : a3
   20 : a4
   21 : a5
   22 : a6
   23 : a7
   24 : s0 (callee-saved regs)
   25 : s1
   26 : s2
   27 : s3
   28 : s4
   29 : s5
   30 : s6
   31 : s7
   32 : virtual FP/lo
   33 : virtual FP/hi
   34 : virtual AP/lo
   35 : virtual AP/hi
*/

#define ACC_REGNUM 0
#define X_REGNUM 4
#define Y_REGNUM 8
#define SP_REGNUM 12
#define FP_REGNUM 14
#define FIRST_ARG_REGISTER 16
#define LAST_ARG_REGISTER (FIRST_ARG_REGISTER + 7)
#define FIRST_CALLER_SAVED 24
#define LAST_CALLER_SAVED (FIRST_CALLER_SAVED + 7)
#define FIRST_ZP_REGISTER SP_REGNUM
#define LAST_ZP_REGISTER LAST_CALLER_SAVED

#define FIXED_REGISTERS		\
  { 0, 0, 0, 0, 0, 0, 0, 0,	\
    0, 0, 0, 0, 1, 1, 1, 1,	\
    /* arg regs.  */		\
    0, 0, 0, 0, 0, 0, 0, 0,	\
    /* callee-saved regs.  */	\
    0, 0, 0, 0, 0, 0, 0, 0,	\
    /* fp, ap regs.  */		\
    1, 1, 1, 1 }

#define CALL_USED_REGISTERS	\
  { 1, 1, 1, 1, 1, 1, 1, 1,	\
    1, 1, 1, 1, 1, 1, 1, 1,	\
    /* arg regs.  */		\
    1, 1, 1, 1, 1, 1, 1, 1,	\
    /* callee-saved regs.  */	\
    0, 0, 0, 0, 0, 0, 0, 0,	\
    /* fp, ap regs.  */		\
    1, 1, 1, 1 }

#define FIRST_PSEUDO_REGISTER 36

#define REG_ALLOC_ORDER \
  { 0, 1, 2, 3, 4, 5, 6, 7, \
    8, 9, 10, 11, 12, 13, 14, 15, \
    16, 17, 18, 19, 20, 21, 22, 23, \
    24, 25, 26, 27, 28, 29, 30, 31, \
    32, 33, 34, 35 }

#define HARD_REGNO_NREGS(REGNO, MODE)		\
  (GET_MODE_SIZE (MODE))

#define HARD_REGNO_MODE_OK(REGNO, MODE) \
  m65x_hard_regno_mode_ok ((REGNO), (MODE))

#define MODES_TIEABLE_P(MODE1, MODE2) 1

/*****************************************************************************
 * Register classes.
 *****************************************************************************/

enum reg_class
{
  NO_REGS,
  HARD_ACCUM_REG,
  HARD_X_REG,
  HARD_Y_REG,
  HARD_INDEX_REGS,
  HARD_REGS,
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
  "HARD_X_REG",			\
  "HARD_Y_REG",			\
  "HARD_INDEX_REGS",		\
  "HARD_REGS",			\
  "STACK_REG",			\
  "ARG_REGS",			\
  "CALLEE_SAVED_REGS",		\
  "GENERAL_REGS",		\
  "ALL_REGS"			\
}

#define REG_CLASS_CONTENTS	\
{				\
  { 0x00000000,	0x0 },		\
  { 0x0000000f,	0x0 },		\
  { 0x000000f0,	0x0 },		\
  { 0x00000f00,	0x0 },		\
  { 0x00000ff0,	0x0 },		\
  { 0x00000fff,	0x0 },		\
  { 0x0000f000,	0x0 },		\
  { 0x00ff0000,	0x0 },		\
  { 0xff000000,	0x0 },		\
  { 0xfffff000,	0x0 },		\
  { 0xffffffff, 0xf },		\
}

#define REGNO_REG_CLASS(REGNO)						\
  ((REGNO) >= ACC_REGNUM && (REGNO) < (ACC_REGNUM + 4)			\
    ? HARD_ACCUM_REG :							\
   (REGNO) >= X_REGNUM && (REGNO) < (X_REGNUM + 4) ? HARD_X_REG :	\
   (REGNO) >= Y_REGNUM && (REGNO) < (Y_REGNUM + 4) ? HARD_Y_REG : 	\
   (REGNO) >= SP_REGNUM && (REGNO) <= (SP_REGNUM + 4) ? STACK_REG :	\
   (REGNO) >= FIRST_ARG_REGISTER && (REGNO) <= LAST_ARG_REGISTER	\
     ? ARG_REGS :							\
   (REGNO) >= FIRST_CALLER_SAVED && (REGNO) <= LAST_CALLER_SAVED	\
     ? CALLEE_SAVED_REGS :						\
   (REGNO) >= FIRST_ZP_REGISTER && (REGNO) <= LAST_ZP_REGISTER		\
     ? GENERAL_REGS : NO_REGS)

#define BASE_REG_CLASS	GENERAL_REGS
#define INDEX_REG_CLASS	HARD_INDEX_REGS

#define REGNO_OK_FOR_BASE_P(NUM) \
  ((NUM) >= FIRST_ARG_REGISTER && (NUM) <= LAST_ZP_REGISTER)

#define REGNO_OK_FOR_INDEX_P(NUM) ((NUM) == X_REGNUM || (NUM) == Y_REGNUM)

#define PREFERRED_RELOAD_CLASS(X, CLASS) CLASS

/*#define SMALL_REGISTER_CLASSES		1*/

#define CLASS_MAX_NREGS(CLASS, MODE)		\
  (GET_MODE_SIZE (MODE))

#define SECONDARY_MEMORY_NEEDED(CLASS1, CLASS2, M) \
  (((CLASS1) == HARD_X_REG && (CLASS2) == HARD_Y_REG) \
   || (CLASS1) == HARD_Y_REG && (CLASS2) == HARD_X_REG)

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

#define STACK_POINTER_REGNUM		SP_REGNUM
#define FRAME_POINTER_REGNUM		32
#define ARG_POINTER_REGNUM		34

/* Eliminating frame pointer/arg pointer.  */

#define ELIMINABLE_REGS					\
  { { ARG_POINTER_REGNUM, STACK_POINTER_REGNUM },	\
    { ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM },	\
    { ARG_POINTER_REGNUM, FP_REGNUM },			\
    { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM },	\
    { FRAME_POINTER_REGNUM, FP_REGNUM } }

/* FIXME: This needs fixing.  */

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)	\
  do { (OFFSET) = 0; } while (0)

/* Passing function arguments on the stack.  */

#define PUSH_ARGS 0

#define FUNCTION_ARG_REGNO_P(REGNO) \
  ((REGNO) >= FIRST_ARG_REGISTER && (REGNO) <= LAST_ARG_REGISTER)

#define FUNCTION_VALUE_REGNO_P(REGNO) \
  ((REGNO) >= ACC_REGNUM && (REGNO) < (ACC_REGNUM + 4))

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

#define SLOW_BYTE_ACCESS		0

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
    "sp", "?sp", "fp", "?fp",				\
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",	\
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",	\
    "?vfpl", "?vfph", "?vapl", "?vaph"			\
  }

#define PRINT_OPERAND(STREAM, X, CODE) \
  m65x_print_operand ((STREAM), (X), (CODE))

#define PRINT_OPERAND_ADDRESS(STREAM, X) \
  m65x_print_operand_address ((STREAM), (X))

#define ASM_OUTPUT_ALIGN(STREAM, POWER) \
  fprintf ((STREAM), ".align %d", (POWER))

#define ASM_FPRINTF_EXTENSIONS(FILE, ARGS, P)		\
  case 'r':						\
    fputs (reg_names [va_arg (ARGS, int)], FILE);	\
    break;

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

/* Prevent emission of call to __main.  */

#define HAS_INIT_SECTION

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
