#ifndef GCC_6502_PROTOS_H
#define GCC_6502_PROTOS_H

#ifdef RTX_CODE
extern void m65x_print_operand (FILE *, rtx, int);
extern void m65x_print_branch (enum machine_mode, rtx, rtx);
extern void m65x_print_operand_address (FILE *, rtx);
extern void m65x_output_ascii (FILE *, const char *, int);
extern int m65x_mode_dependent_address_p (rtx);
extern HOST_WIDE_INT m65x_hard_regno_nregs (int, enum machine_mode);
extern bool m65x_hard_regno_mode_ok (int, enum machine_mode);
extern bool m65x_valid_mov_operands (enum machine_mode mode, rtx *operands);
extern bool m65x_indirect_indexed_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_legitimate_address_p (enum machine_mode mode, rtx, bool);
extern rtx m65x_adjust_address (rtx, enum machine_mode, HOST_WIDE_INT);
extern rtx m65x_legitimize_reload_address (rtx *, enum machine_mode, int, int,
					   int);
extern void m65x_emit_qimode_comparison (enum rtx_code cond, rtx op0, rtx op1,
					 rtx dest);
extern void m65x_emit_himode_comparison (enum rtx_code cond, rtx op0, rtx op1,
					 rtx dest, rtx scratch);
#endif

#endif
