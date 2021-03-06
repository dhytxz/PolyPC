#ifndef _HAPARA_TRACE_BASE_H_
#define _HAPARA_TRACE_BASE_H_

#define TIMER_TCSR_OFF              0x00000000
#define TIMER_TLR_OFF               0x00000004
#define TIMER_TCR_OFF               0x00000008

#define XTC_CSR_LOAD_MASK           0x00000020
#define XTC_CSR_ENABLE_TMR_MASK     0x00000080
#define XTC_CSR_INT_OCCURED_MASK    0x00000100


struct hapara_timer_struct {
    volatile unsigned int tcsr;
    volatile unsigned int tlr;
    volatile unsigned int tcr;
};

struct hapara_trace_struct {
    volatile unsigned int group_index;
    volatile unsigned int before_loading;
    volatile unsigned int before_triggle;
    volatile unsigned int after_finish;
};

#endif