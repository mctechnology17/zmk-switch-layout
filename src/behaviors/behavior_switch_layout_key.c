/*
 * Copyright (c) 2026 Marcos Chow Castro
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_switch_layout_behavior_switch_layout_key

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util_macro.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk_switch_layout/state.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct behavior_switch_layout_key_config {
    const struct zmk_behavior_binding *bindings;
    uint8_t binding_count;
};

struct behavior_switch_layout_key_data {
    const struct zmk_behavior_binding *pressed_binding;
};

static int on_switch_layout_key_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_switch_layout_key_config *cfg = dev->config;
    struct behavior_switch_layout_key_data *data = dev->data;

    if (data->pressed_binding != NULL) {
        LOG_ERR("Can't press the same switch-layout key twice.");
        return -ENOTSUP;
    }

    uint8_t layout = zmk_switch_layout_get();
    if (layout >= cfg->binding_count) {
        layout = 0;
    }

    data->pressed_binding = &cfg->bindings[layout];
    return zmk_behavior_invoke_binding(data->pressed_binding, event, true);
}

static int on_switch_layout_key_released(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_switch_layout_key_data *data = dev->data;

    if (data->pressed_binding == NULL) {
        LOG_ERR("Switch-layout key already released.");
        return -ENOTSUP;
    }

    const struct zmk_behavior_binding *pressed_binding = data->pressed_binding;
    data->pressed_binding = NULL;
    return zmk_behavior_invoke_binding(pressed_binding, event, false);
}

static const struct behavior_driver_api behavior_switch_layout_key_driver_api = {
    .binding_pressed = on_switch_layout_key_pressed,
    .binding_released = on_switch_layout_key_released,
};

#define SWITCH_LAYOUT_KEY_BINDING(idx, node) ZMK_KEYMAP_EXTRACT_BINDING(idx, node)

#define SWITCH_LAYOUT_KEY_BINDINGS(n)                                                             \
    {LISTIFY(DT_INST_PROP_LEN(n, bindings), SWITCH_LAYOUT_KEY_BINDING, (, ), DT_DRV_INST(n))}

#define SWITCH_LAYOUT_KEY_INST(n)                                                                  \
    static const struct zmk_behavior_binding switch_layout_key_##n##_bindings[] =                  \
        SWITCH_LAYOUT_KEY_BINDINGS(n);                                                            \
    static const struct behavior_switch_layout_key_config switch_layout_key_##n##_config = {       \
        .bindings = switch_layout_key_##n##_bindings,                                             \
        .binding_count = DT_INST_PROP_LEN(n, bindings),                                           \
    };                                                                                             \
    static struct behavior_switch_layout_key_data switch_layout_key_##n##_data = {};               \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, &switch_layout_key_##n##_data,                          \
                            &switch_layout_key_##n##_config, POST_KERNEL,                          \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_switch_layout_key_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SWITCH_LAYOUT_KEY_INST)
