#ifndef _THREAD_STRUCT_H_
#define _THREAD_STRUCT_H_

#ifdef __USER_PROGRAMS__
#include <stdint.h>
#else
#include <linux/types.h>
#endif

struct hapara_id_pair {
    uint32_t id0;
    uint32_t id1;
};

struct hapara_reg_pair {
    uint8_t off;
    uint8_t target;
};

struct hapara_thread_struct {
    uint8_t valid;                       //1: valid; 0: invalid
    uint8_t priority;
    uint8_t type;
    uint8_t next;
    uint8_t tid;
    struct hapara_id_pair group_id;
}__attribute__((aligned(4)));

#endif