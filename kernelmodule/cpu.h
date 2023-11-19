#ifndef _CPU_H_
#define _CPU_H_

// ============================================================================
// extern functions
// ============================================================================
extern int _vmxon(uint64_t phy_addr);
extern int _vmxoff(void);
extern void _enable_vmxe(void);
extern void _disable_vmxe(void);
extern int _vmwrite(uint64_t selector, uint64_t value);
extern uint64_t _vmread(uint64_t selector);

extern uint64_t _get_reg_rax(void);
extern uint64_t _get_reg_rbx(void);
extern uint64_t _get_reg_cs(void);
extern uint64_t _get_reg_ds(void);
extern uint64_t _get_reg_es(void);
extern uint64_t _get_reg_ss(void);
extern uint64_t _get_reg_fs(void);
extern uint64_t _get_reg_gs(void);

extern uint32_t _read_cr0(void);
extern uint32_t _read_cr3(void);
extern uint32_t _read_cr4(void);
extern uint32_t _read_dr7(void);
extern uint64_t _read_rflag(void);

extern void _write_cr0(unsigned int);
extern void _write_cr4(unsigned int);

extern uint32_t _read_msr_low(unsigned int);
extern uint32_t _read_msr_high(unsigned int);
extern uint64_t _read_msr(unsigned int);
extern uint32_t _write_msr(unsigned int, uint64_t val);

#endif	// _CPU_H_