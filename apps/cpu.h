#ifndef _CPU_H_
#define _CPU_H_

// ============================================================================
// function prototype
// ============================================================================
#ifdef __cplusplus
extern "C" {
#endif
extern void _cpuid_id(char *id);
extern int _is_support_vmx(void);

#ifdef __cplusplus
}
#endif
#endif	// _CPU_H_
