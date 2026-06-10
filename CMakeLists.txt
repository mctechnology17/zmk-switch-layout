zephyr_library()

# ### zmk-switch-layout START ###
zephyr_include_directories(include)
zephyr_include_directories(${CMAKE_SOURCE_DIR}/include)
zephyr_library_sources(src/empty.c)

zephyr_library_sources_ifdef(CONFIG_ZMK_SWITCH_LAYOUT_STATE src/switch_layout_state.c)
zephyr_library_sources_ifdef(CONFIG_ZMK_SWITCH_LAYOUT_MODE_BEHAVIOR src/behaviors/behavior_switch_layout_mode.c)
zephyr_library_sources_ifdef(CONFIG_ZMK_SWITCH_LAYOUT_KEY_BEHAVIOR src/behaviors/behavior_switch_layout_key.c)
# ### zmk-switch-layout END ###
