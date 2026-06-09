/*
 * Copyright (c) 2026 Marcos Chow Castro
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <stdint.h>

#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <zmk_switch_layout/state.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define SWITCH_LAYOUT_MODE_COMPAT zmk_switch_layout_behavior_switch_layout_mode
#define SWITCH_LAYOUT_MODE_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(SWITCH_LAYOUT_MODE_COMPAT)

static uint8_t switch_layout;
static const uint8_t switch_layout_count = DT_PROP(SWITCH_LAYOUT_MODE_NODE, layout_count);
static const uint8_t switch_layout_default = DT_PROP(SWITCH_LAYOUT_MODE_NODE, default_layout);

uint8_t zmk_switch_layout_get(void) { return switch_layout; }

uint8_t zmk_switch_layout_count(void) { return switch_layout_count; }

static int switch_layout_save(void) {
    int ret = settings_save_one("zmk_switch_layout/layout", &switch_layout, sizeof(switch_layout));

    if (ret < 0) {
        LOG_WRN("Could not save switch layout (%d).", ret);
    }

    return ret;
}

int zmk_switch_layout_set(uint8_t layout) {
    if (switch_layout_count == 0 || layout >= switch_layout_count) {
        return -EINVAL;
    }

    switch_layout = layout;
    return switch_layout_save();
}

int zmk_switch_layout_default(void) {
    return zmk_switch_layout_set(switch_layout_default < switch_layout_count ? switch_layout_default
                                                                             : 0);
}

int zmk_switch_layout_next(void) {
    if (switch_layout_count == 0) {
        return -EINVAL;
    }

    return zmk_switch_layout_set((switch_layout + 1) % switch_layout_count);
}

int zmk_switch_layout_prev(void) {
    if (switch_layout_count == 0) {
        return -EINVAL;
    }

    return zmk_switch_layout_set((switch_layout + switch_layout_count - 1) % switch_layout_count);
}

static int switch_layout_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;

    if (settings_name_steq(name, "layout", &next) && !next) {
        if (len != sizeof(switch_layout)) {
            return -EINVAL;
        }

        int ret = read_cb(cb_arg, &switch_layout, sizeof(switch_layout));
        if (ret >= 0 && switch_layout >= switch_layout_count) {
            switch_layout = switch_layout_default < switch_layout_count ? switch_layout_default : 0;
        }

        return ret;
    }

    return -ENOENT;
}

static struct settings_handler switch_layout_conf = {
    .name = "zmk_switch_layout",
    .h_set = switch_layout_set,
};

static int switch_layout_init(void) {
    if (switch_layout_count == 0) {
        LOG_ERR("switch-layout layout-count must be greater than zero.");
        return -EINVAL;
    }

    switch_layout = switch_layout_default < switch_layout_count ? switch_layout_default : 0;

    settings_subsys_init();

    int ret = settings_register(&switch_layout_conf);
    if (ret < 0) {
        LOG_ERR("Could not register switch layout settings (%d).", ret);
        return ret;
    }

    settings_load_subtree("zmk_switch_layout");

    return 0;
}
SYS_INIT(switch_layout_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
