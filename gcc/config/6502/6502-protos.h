#ifndef GCC_65816_PROTOS_H
#define GCC_65816_PROTOS_H

extern void wdc65816_print_operand (FILE *, rtx, int);
extern void wdc65816_print_operand_address (FILE *, rtx);
extern int wdc65816_legitimate_address_p (enum machine_mode, rtx);
extern int wdc65816_mode_dependent_address_p (rtx);

#endif
