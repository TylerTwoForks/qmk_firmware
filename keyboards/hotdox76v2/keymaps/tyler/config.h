#pragma once

#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

#define DUMMY_MOD_NEUTRALIZER_KEYCODE KC_RIGHT_CTRL
#define MODS_TO_NEUTRALIZE \
    { MOD_BIT(KC_LEFT_ALT), MOD_BIT(KC_LEFT_GUI), MOD_BIT(KC_RIGHT_GUI), MOD_BIT(KC_LEFT_CTRL) | MOD_BIT(KC_LEFT_SHIFT), MOD_BIT(KC_LEFT_CTRL) }

#define TAPPING_TERM 210
#define COMBO_TERM 30
#define QUICK_TAP_TERM 0

// A mod-tap only holds when chorded with the opposite hand, and never during
// fast typing. Together these keep rolls off the mod-taps on the alpha rows.
#define CHORDAL_HOLD
#define FLOW_TAP_TERM 150

// A mod-tap held past the tapping term still emits its tap on release, so a
// slow finger cannot swallow the keystroke entirely.
#define RETRO_TAPPING

#define NO_AUTO_SHIFT_ALPHA
#define AUTO_SHIFT_TIMEOUT 120

#define SPLIT_LAYER_STATE_ENABLE
#define MASTER_RIGHT
