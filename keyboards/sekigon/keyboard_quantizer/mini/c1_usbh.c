// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "c1.h"
#include "tusb.h"
#include "pio_usb.h"
#include "pio_usb_ll.h"
#include "hardware/sync.h"

// dummy implementation
void alarm_pool_add_repeating_timer_us(void) {}
void alarm_pool_create(void) {}

// Initialize USB host stack on core1
void c1_usbh(void) {
    while (!c1_clock_ready) {
        continue;
    }

    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.skip_alarm_pool         = true;
    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

    tusb_rhport_init_t rh_init = {
        .role  = TUSB_ROLE_HOST,
        .speed = TUSB_SPEED_FULL,
    };
    tusb_init(1, &rh_init);
    c1_start_timer();
}

// USB host stack main task
void c1_main_task(void) {
    tuh_task();
}
