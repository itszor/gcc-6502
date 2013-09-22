#ifndef GCC_6502_PROTOS_H
#define GCC_6502_PROTOS_H

#ifdef RTX_CODE
extern void m65x_print_operand (FILE *, rtx, int);
extern void m65x_print_operand_address (FILE *, rtx);
extern int m65x_legitimate_address_p (enum machine_mode, rtx);
extern int m65x_mode_dependent_address_p (rtx);
#endif

#endif
