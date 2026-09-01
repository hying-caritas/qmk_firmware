// Copyright 2020 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* Allow VIA to read the switch matrix state (required for VIA's Test Matrix
 * feature; without it the matrix state always reports zero). */
#define VIA_INSECURE

#define MATRIX_COLS_DEFAULT 8
#define MATRIX_MSG_ROWS 31

#define DYNAMIC_KEYMAP_LAYER_COUNT 8
#define WEAR_LEVELING_BACKING_SIZE (8192 * 2)
#define WEAR_LEVELING_LOGICAL_SIZE 8192
