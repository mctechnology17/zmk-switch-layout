# zmk-switch-layout

ZMK module for switching a logical layout inside the keyboard. The keyboard can toggle the base layout with any layout while using the keyboard. The change always remains active, after reboot, after flash and after power off.

- It does not translate the whole keymap.
- It only affects keys replaced by a `switch-layout-key`.
- Each `switch-layout-key` can use any ZMK behavior valid as a binding, not only simple keys.
- Bindings can include modifiers such as `&kp RA(MINUS)`, `&kp LS(A)`, or `&kp LC(Z)`.
- If the active layout does not exist for a key, entry `0` is used.
- It can be changed live while the keyboard is running.
- `NEXT`, `PREV`, `DEFAULT`, and direct indexes allow switching and restoring states.
- This module is independent from `zmk-switch-layer`.

The host keyboard language does not change. The keyboard chooses which binding to send for the keys defined by the user. The selected layout is stored with `settings`, so it survives power off and reboot.

## Files to modify

To use the module in a ZMK config:

| File | What changes |
| --- | --- |
| `config/west.yml` | Add the repository as an external module for online builds. |
| `config/<keyboard>.keymap` | Define `sw_layout`, define the layout-aware keys, and replace normal keys with those behaviors. |
| `Makefile` or local build command | Add the module path to `ZMK_EXTRA_MODULES` when building locally. |

Example as an external module in `config/west.yml`:

```yaml
manifest:
  remotes:
    - name: mctechnology17
      url-base: https://github.com/mctechnology17
  projects:
    - name: zmk-switch-layout
      remote: mctechnology17
      revision: main
  self:
    path: config
```

Example local build with `ZMK_EXTRA_MODULES`:

```sh
-DZMK_EXTRA_MODULES="/path/to/zmk-switch-layout"
```

## Keymap configuration

`switch-layout-mode` stores the active layout.

```dts
#include <dt-bindings/zmk-switch-layout/switch-layout.h>

/ {
    behaviors {
        sw_layout: switch_layout {
            compatible = "zmk-switch-layout,behavior-switch-layout-mode";
            #binding-cells = <1>;
            layout-count = <3>;
            default-layout = <0>;
        };
    };
};
```

Usage:

```dts
&sw_layout 0                    // layout 0
&sw_layout 1                    // layout 1
&sw_layout 2                    // layout 2
&sw_layout ZMK_SWITCH_LAYOUT_NEXT
&sw_layout ZMK_SWITCH_LAYOUT_PREV
&sw_layout ZMK_SWITCH_LAYOUT_DEFAULT
```

`switch-layout-key` defines one layout-aware key. Each entry in `bindings` matches the same index configured by `layout-count`.

Example 1: the `Z` key sends `Z` in layout 0, `Y` in layout 1, and `Z` in layout 2.

```dts
/ {
    behaviors {
        sl_z: switch_layout_z {
            compatible = "zmk-switch-layout,behavior-switch-layout-key";
            #binding-cells = <0>;
            bindings = <&kp Z>, <&kp Y>, <&kp Z>;
        };
    };
};
```

Example 2: the `0` key on the number layer sends `N0` in layout 0, `N1` in layout 1, and `N2` in layout 2.

```dts
/ {
    behaviors {
        sl_num0: switch_layout_num0 {
            compatible = "zmk-switch-layout,behavior-switch-layout-key";
            #binding-cells = <0>;
            bindings = <&kp N0>, <&kp N1>, <&kp N2>;
        };
    };
};
```

Use it in the keymap:

```dts
// In the position where &kp Z used to be:
&sl_z

// In the position where &kp N0 used to be:
&sl_num0
```

Result:

| Active layout | `&sl_z` sends | `&sl_num0` sends |
| --- | --- | --- |
| `0` | `&kp Z` | `&kp N0` |
| `1` | `&kp Y` | `&kp N1` |
| `2` | `&kp Z` | `&kp N2` |

## Layout control

Map one or more controls wherever you want to change the layout:

```dts
&sw_layout ZMK_SWITCH_LAYOUT_NEXT     // 0 -> 1 -> 2 -> 0
&sw_layout ZMK_SWITCH_LAYOUT_PREV     // 0 -> 2 -> 1 -> 0
&sw_layout ZMK_SWITCH_LAYOUT_DEFAULT  // return to default-layout

&sw_layout 0                          // go directly to layout 0
&sw_layout 1                          // go directly to layout 1
&sw_layout 2                          // go directly to layout 2
```

## Modifiers

`bindings` entries are not limited to simple key presses. They can use modifiers or any ZMK behavior valid as a binding:

```dts
/ {
    behaviors {
        sl_minus: switch_layout_minus {
            compatible = "zmk-switch-layout,behavior-switch-layout-key";
            #binding-cells = <0>;
            bindings = <&kp MINUS>, <&kp RA(MINUS)>, <&kp LS(MINUS)>;
        };
    };
};
```
