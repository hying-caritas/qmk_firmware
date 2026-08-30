// Copyright 2020 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "quantizer_mouse.h"
#include "identity_keymap.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[0] = IDENTITY_KEYMAP_LAYER};

static uint8_t get_gesture_threshold(void) {
    return 50;
}

void keyboard_post_init_user(void) {
    set_mouse_gesture_threshold(get_gesture_threshold());
}

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    return pre_process_record_mouse(keycode, record);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return process_record_mouse(keycode, record);
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    post_process_record_mouse(keycode, record);
}
