// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "keyboard.h"
#include "pico/stdlib.h"
#include "bootloader.h"
#include "debug.h"
#include "c1.h"
#include "tusb.h"
#include "pio_usb_ll.h"
#include "tusb_os_custom.h"
#include "virtser.h"
#include "util.h"

#include <string.h>

static int8_t virtser_send_wrap(uint8_t c) {
    virtser_send(c);
    return 0;
}

volatile bool c1_clock_ready;

void keyboard_pre_init_kb(void) {
    set_sys_clock_khz(120000, true);
    c1_clock_ready = true;
    print_set_sendchar(virtser_send_wrap);
    keyboard_pre_init_user();
}

static void print_usb_status(void) {
    for (int i = 0; i < PIO_USB_ROOT_PORT_CNT; i++) {
        root_port_t      *root   = PIO_USB_ROOT_PORT(i);
        uint8_t           dp_raw = gpio_get(root->pin_dp);
        uint8_t           dm_raw = gpio_get(root->pin_dm);
        port_pin_status_t ls     = pio_usb_bus_get_line_state(root);
        printf("port%d: init=%d conn=%d susp=%d fs=%d ints=0x%lx dp=%d dm=%d ls=%d\n", i, root->initialized, root->connected, root->suspended, root->is_fullspeed, (unsigned long)root->ints, dp_raw, dm_raw, ls);
    }
    for (uint8_t addr = 1; addr <= CFG_TUH_DEVICE_MAX; addr++) {
        if (tuh_connected(addr)) {
            uint16_t vid = 0, pid = 0;
            tuh_vid_pid_get(addr, &vid, &pid);
            printf("dev%d: vid=0x%04x pid=0x%04x hid_if=%d\n", addr, vid, pid, tuh_hid_instance_count(addr));
        }
    }
}

#define CMD_BUF_LEN 32

typedef void (*cmd_fn_t)(void);

static char    cmd_buf[CMD_BUF_LEN];
static uint8_t cmd_len;

static void cmd_boot(void) {
    bootloader_jump();
}

static void cmd_debug(void) {
    debug_enable = !debug_enable;
    printf("Debug %s\n", debug_enable ? "enabled" : "disabled");
}

static void cmd_version(void) {
    printf("firmware version: %s\n", STR(GIT_DESCRIBE));
}

static void cmd_usbinfo(void) {
    print_usb_status();
}

#if CFG_TUSB_DEBUG
static void cmd_usbdebug(void) {
    tusb_print_debug_buffer();
}
#endif

static void cmd_help(void) {
#if CFG_TUSB_DEBUG
    printf("commands: boot debug version usbinfo usbdebug help (or b d v u h)\n");
#else
    printf("commands: boot debug version usbinfo help (or b d v u h)\n");
#endif
}

static const struct {
    const char *name;
    cmd_fn_t    fn;
} commands[] = {
    {"boot", cmd_boot},         {"b", cmd_boot}, {"debug", cmd_debug}, {"d", cmd_debug}, {"version", cmd_version}, {"v", cmd_version}, {"usbinfo", cmd_usbinfo}, {"u", cmd_usbinfo},
#if CFG_TUSB_DEBUG
    {"usbdebug", cmd_usbdebug},
#endif
    {"help", cmd_help},         {"h", cmd_help}, {"?", cmd_help},
};

static void exec_command(const char *name) {
    for (int i = 0; i < ARRAY_SIZE(commands); i++) {
        if (strcmp(name, commands[i].name) == 0) {
            commands[i].fn();
            return;
        }
    }
    printf("unknown command: %s\n", name);
}

void virtser_recv(uint8_t c) {
    if (c == '\r' || c == '\n') {
        printf("\r\n");
        if (cmd_len > 0) {
            cmd_buf[cmd_len] = '\0';
            exec_command(cmd_buf);
            cmd_len = 0;
        }
        printf("> ");
        return;
    }
    if (c == '\b' || c == 0x7f) {
        if (cmd_len > 0) {
            cmd_len--;
            printf("\b \b");
        }
        return;
    }
    if (cmd_len < sizeof(cmd_buf) - 1) {
        cmd_buf[cmd_len++] = c;
        printf("%c", c);
    }
}

bool backing_store_lock(void) {
    c1_after_flash_operation();
    return true;
}

bool backing_store_unlock(void) {
    c1_before_flash_operation();
    return true;
}
