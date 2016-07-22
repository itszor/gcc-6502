#ifndef GCC_6502_PROTOS_H
#define GCC_6502_PROTOS_H

#ifdef RTX_CODE
extern void m65x_print_operand (FILE *, rtx, int);
extern void m65x_print_branch (enum machine_mode, rtx, rtx, bool);
extern void m65x_print_operand_address (FILE *, rtx);
extern void m65x_output_ascii (FILE *, const char *, int);
extern int m65x_mode_dependent_address_p (rtx);
extern HOST_WIDE_INT m65x_hard_regno_nregs (int, enum machine_mode);
extern bool m65x_hard_regno_mode_ok (int, enum machine_mode);
extern bool m65x_valid_mov_operands (enum machine_mode mode, rtx *operands);
extern bool m65x_valid_zp_mov_operands (enum machine_mode mode, rtx *operands);
extern bool m65x_indirect_indexed_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_indirect_offset_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_absolute_indexed_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_absolute_x_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_absolute_y_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_zeropage_indexed_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_zeropage_x_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_zeropage_y_addr_p (enum machine_mode, rtx, bool);
extern bool m65x_legitimate_address_p (enum machine_mode mode, rtx, bool);
extern rtx m65x_adjust_address (rtx, enum machine_mode, HOST_WIDE_INT);
extern rtx m65x_legitimize_reload_address (rtx *, enum machine_mode, int, int,
					   int);
extern void m65x_emit_qimode_comparison (enum rtx_code cond, rtx op0, rtx op1,
					 rtx dest);
extern void m65x_emit_himode_comparison (enum rtx_code cond, rtx op0, rtx op1,
					 rtx dest, rtx scratch);
extern HOST_WIDE_INT m65x_elimination_offset (int from, int to);

extern rtx m65x_push (enum machine_mode mode, rtx src);
extern rtx m65x_pop (enum machine_mode mode, rtx dest);
extern void m65x_expand_addsub (enum machine_mode, bool, rtx[]);

extern bool m65x_regno_mode_code_ok_for_base_p (int regno,
    enum machine_mode mode, addr_space_t as, enum rtx_code outer,
    enum rtx_code index);
#endif

extern void m65x_expand_prologue (void);
extern void m65x_expand_epilogue (void);

extern bool m65x_peephole_find_temp_regs (int, int, ...);

#endif
