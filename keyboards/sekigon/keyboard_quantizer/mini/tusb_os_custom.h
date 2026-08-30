// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Custom OSAL for TinyUSB 0.21+ running on RP2040 core1.
// Core1 runs TinyUSB exclusively in a bare-metal loop, so:
// - mutex is no-op (single thread, no contention)
// - spinlock/queue use interrupt disable (ISR vs main loop on same core)
//
// Functions are non-inline to avoid including ch.h here,
// which conflicts with Pico SDK headers in the same TU.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "tusb_config.h"

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------+
// TASK API
//--------------------------------------------------------------------+
typedef void      *osal_task_handle_t;
osal_task_handle_t osal_task_get_current_handle(void);
void               osal_task_delay(uint32_t msec);

//--------------------------------------------------------------------+
// SPINLOCK API
//--------------------------------------------------------------------+
typedef struct {
    uint32_t nested_count;
} osal_spinlock_t;

#define OSAL_SPINLOCK_DEF(_name, _int_set) osal_spinlock_t _name = {.nested_count = 0}

void osal_spin_init(osal_spinlock_t *ctx);
void osal_spin_deinit(osal_spinlock_t *ctx);
void osal_spin_lock(osal_spinlock_t *ctx, bool in_isr);
void osal_spin_unlock(osal_spinlock_t *ctx, bool in_isr);

//--------------------------------------------------------------------+
// MUTEX API (no-op: single thread on core1)
//--------------------------------------------------------------------+
typedef uint8_t osal_mutex_def_t, *osal_mutex_t;

osal_mutex_t osal_mutex_create(osal_mutex_def_t *mdef);
bool         osal_mutex_delete(osal_mutex_t mutex_hdl);
bool         osal_mutex_lock(osal_mutex_t mutex_hdl, uint32_t msec);
bool         osal_mutex_unlock(osal_mutex_t mutex_hdl);

//--------------------------------------------------------------------+
// QUEUE API
//--------------------------------------------------------------------+
#include "common/tusb_fifo.h"

typedef struct {
    tu_fifo_t ff;
    uint16_t  item_size;
} osal_queue_def_t;

typedef osal_queue_def_t *osal_queue_t;

#define OSAL_QUEUE_DEF(_int_set, _name, _depth, _type)    \
    uint8_t          _name##_buf[_depth * sizeof(_type)]; \
    osal_queue_def_t _name = {.ff = TU_FIFO_INIT(_name##_buf, _depth, false), .item_size = sizeof(_type)}

osal_queue_t osal_queue_create(osal_queue_def_t *qdef);
bool         osal_queue_delete(osal_queue_t qhdl);
bool         osal_queue_receive(osal_queue_t qhdl, void *data, uint32_t msec);
bool         osal_queue_send(osal_queue_t qhdl, void const *data, bool in_isr);
bool         osal_queue_empty(osal_queue_t qhdl);

//--------------------------------------------------------------------+
// TIME API
//--------------------------------------------------------------------+
uint32_t osal_time_millis(void);

#ifdef __cplusplus
}
#endif
