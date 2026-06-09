/*
 * Copyright (c) 2026 Marcos Chow Castro
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

uint8_t zmk_switch_layout_get(void);
uint8_t zmk_switch_layout_count(void);
int zmk_switch_layout_set(uint8_t layout);
int zmk_switch_layout_default(void);
int zmk_switch_layout_next(void);
int zmk_switch_layout_prev(void);
