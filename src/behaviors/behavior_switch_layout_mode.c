/*
 * Copyright (c) 2026 Marcos Chow Castro
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_switch_layout_behavior_switch_layout_mode

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <dt-bindings/zmk-switch-layout/switch-layout.h>
#include <zmk/behavior.h>
#include <zmk_switch_layout/state.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int behavior_switch_layout_mode_init(const struct device *dev) { return 0; }

static int on_switch_layout_mode_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
    case ZMK_SWITCH_LAYOUT_NEXT:
        return zmk_switch_layout_next();
    case ZMK_SWITCH_LAYOUT_PREV:
        return zmk_switch_layout_prev();
    case ZMK_SWITCH_LAYOUT_DEFAULT:
        return zmk_switch_layout_default();
    default:
        return zmk_switch_layout_set(binding->param1);
    }
}

static int on_switch_layout_mode_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_switch_layout_mode_driver_api = {
    .binding_pressed = on_switch_layout_mode_pressed,
    .binding_released = on_switch_layout_mode_released,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_switch_layout_mode_init, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_switch_layout_mode_driver_api);
