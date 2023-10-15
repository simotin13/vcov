#ifndef _CPU_H_
#define _CPU_H_

// ============================================================================
// extern functions
// ============================================================================
extern int _vmxon(void);
extern int _vmxoff(void);

extern unsigned long _read_cr0(void);
extern unsigned long _read_cr4(void);
extern void _write_cr0(unsigned int);
extern void _write_cr4(unsigned int);

extern unsigned long _read_msr_low(unsigned int);
extern unsigned long _read_msr_high(unsigned int);
extern unsigned long long _read_msr(unsigned int);
extern unsigned long _write_msr(unsigned int, unsigned long long);

#endif	// _CPU_H_