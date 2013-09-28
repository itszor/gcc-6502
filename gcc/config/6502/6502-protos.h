#ifndef GCC_6502_PROTOS_H
#define GCC_6502_PROTOS_H

#ifdef RTX_CODE
extern void m65x_print_operand (FILE *, rtx, int);
extern void m65x_print_operand_address (FILE *, rtx);
extern int m65x_mode_dependent_address_p (rtx);
extern bool m65x_hard_regno_mode_ok (int, enum machine_mode);
#endif

#endif
