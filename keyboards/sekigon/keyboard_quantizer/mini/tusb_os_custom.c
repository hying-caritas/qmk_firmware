// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ch.h"
#include "tusb_os_custom.h"

//--------------------------------------------------------------------+
// TASK
//--------------------------------------------------------------------+
osal_task_handle_t osal_task_get_current_handle(void) {
    return (osal_task_handle_t)1;
}

void osal_task_delay(uint32_t msec) {
    chThdSleepMilliseconds(msec);
}

//--------------------------------------------------------------------+
// SPINLOCK (interrupt disable for ISR vs main loop on same core)
//--------------------------------------------------------------------+
void osal_spin_init(osal_spinlock_t *ctx) {
    (void)ctx;
}

void osal_spin_deinit(osal_spinlock_t *ctx) {
    (void)ctx;
}

void osal_spin_lock(osal_spinlock_t *ctx, bool in_isr) {
    if (!in_isr && ctx->nested_count == 0) {
        chSysLock();
    }
    ctx->nested_count++;
}

void osal_spin_unlock(osal_spinlock_t *ctx, bool in_isr) {
    if (ctx->nested_count == 0) return;
    ctx->nested_count--;
    if (!in_isr && ctx->nested_count == 0) {
        chSysUnlock();
    }
}

//--------------------------------------------------------------------+
// MUTEX (interrupt disable, same as old 0.15 adaptation)
//--------------------------------------------------------------------+
osal_mutex_t osal_mutex_create(osal_mutex_def_t *mdef) {
    return mdef;
}

bool osal_mutex_delete(osal_mutex_t mutex_hdl) {
    (void)mutex_hdl;
    return true;
}

bool osal_mutex_lock(osal_mutex_t mutex_hdl, uint32_t msec) {
    (void)mutex_hdl;
    (void)msec;
    chSysLock();
    return true;
}

bool osal_mutex_unlock(osal_mutex_t mutex_hdl) {
    (void)mutex_hdl;
    chSysUnlock();
    return true;
}

//--------------------------------------------------------------------+
// QUEUE (interrupt-protected tu_fifo)
//--------------------------------------------------------------------+
osal_queue_t osal_queue_create(osal_queue_def_t *qdef) {
    tu_fifo_clear(&qdef->ff);
    return (osal_queue_t)qdef;
}

bool osal_queue_delete(osal_queue_t qhdl) {
    (void)qhdl;
    return true;
}

bool osal_queue_receive(osal_queue_t qhdl, void *data, uint32_t msec) {
    (void)msec;
    chSysLock();
    bool success = (tu_fifo_read_n(&qhdl->ff, data, qhdl->item_size) > 0);
    chSysUnlock();
    return success;
}

bool osal_queue_send(osal_queue_t qhdl, void const *data, bool in_isr) {
    if (!in_isr) {
        chSysLock();
    }
    bool success = (tu_fifo_write_n(&qhdl->ff, data, qhdl->item_size) > 0);
    if (!in_isr) {
        chSysUnlock();
    }
    return success;
}

bool osal_queue_empty(osal_queue_t qhdl) {
    return tu_fifo_empty(&qhdl->ff);
}

//--------------------------------------------------------------------+
// TIME
//--------------------------------------------------------------------+
uint32_t osal_time_millis(void) {
    return (uint32_t)TIME_I2MS(chVTGetSystemTimeX());
}