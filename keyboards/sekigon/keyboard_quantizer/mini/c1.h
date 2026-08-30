// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>

// Set by core0 once the 120 MHz system clock is up. pio_usb derives its bit
// timing from the clock in effect at init, so core1 must not start the USB
// host before this flag is set.
extern volatile bool c1_clock_ready;

void c1_main_task(void);
void c1_usbh(void);
void c1_start_timer(void);
void c1_before_flash_operation(void);
void c1_after_flash_operation(void);
