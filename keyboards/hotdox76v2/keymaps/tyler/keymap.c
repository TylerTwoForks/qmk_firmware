// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#if defined(OS_DETECTION_ENABLE)
#    include "os_detection.h"
#endif

enum custom_layers {
    _QWERTY,
    _LOWER,
    _RAISE,
};

enum custom_keycodes {
    KC_CYCLE_LAYERS = SAFE_RANGE,
    COPY_OS_AWARE,
    PASTE_OS_AWARE,
    CUT_OS_AWARE,
    FIND_OS_AWARE,
    UNDO_OS_AWARE,
    HOME_OS_AWARE,
    END_OS_AWARE,
    DESK_RIGHT,
    DESK_LEFT,
    DOUBLE_UNDERSCORE,
};

enum combo_events {
    LENTERQ,
    RENTERQ,
};

#define UOSA UNDO_OS_AWARE
#define CTOA CUT_OS_AWARE
#define CPOA COPY_OS_AWARE
#define POSA PASTE_OS_AWARE
#define FOSA FIND_OS_AWARE
#define HOSA HOME_OS_AWARE
#define EOSA END_OS_AWARE
#define CYCL KC_CYCLE_LAYERS
#define DBLUS DOUBLE_UNDERSCORE
#define LCTL_MT_B LCTL_T(KC_B)
#define RCTL_MT_N RCTL_T(KC_N)

const uint16_t PROGMEM lenter_combo_qwerty[] = {KC_S, KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM renter_combo_qwerty[] = {KC_J, KC_K, KC_L, COMBO_END};

combo_t key_combos[] = {
    [LENTERQ] = COMBO(lenter_combo_qwerty, KC_ENT),
    [RENTERQ] = COMBO(renter_combo_qwerty, KC_ENT),
};

static bool host_is_apple(void) {
#if defined(OS_DETECTION_ENABLE)
    const os_variant_t host = detected_host_os();
    return host == OS_MACOS || host == OS_IOS;
#else
    return false;
#endif
}

static void tap_os_shortcut(uint16_t keycode) {
    tap_code16((host_is_apple() ? QK_LGUI : QK_LCTL) | keycode);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case COPY_OS_AWARE:
            if (record->event.pressed) {
                tap_os_shortcut(KC_C);
            }
            return false;
        case PASTE_OS_AWARE:
            if (record->event.pressed) {
                tap_os_shortcut(KC_V);
            }
            return false;
        case CUT_OS_AWARE:
            if (record->event.pressed) {
                tap_os_shortcut(KC_X);
            }
            return false;
        case FIND_OS_AWARE:
            if (record->event.pressed) {
                tap_os_shortcut(KC_F);
            }
            return false;
        case UNDO_OS_AWARE:
            if (record->event.pressed) {
                tap_os_shortcut(KC_Z);
            }
            return false;
        case HOME_OS_AWARE:
            if (record->event.pressed) {
                if (host_is_apple()) {
                    tap_code16(G(KC_LEFT));
                } else {
                    tap_code(KC_HOME);
                }
            }
            return false;
        case END_OS_AWARE:
            if (record->event.pressed) {
                if (host_is_apple()) {
                    tap_code16(G(KC_RIGHT));
                } else {
                    tap_code(KC_END);
                }
            }
            return false;
        case DESK_RIGHT:
            if (record->event.pressed) {
                tap_code16(LCA(KC_RIGHT));
            }
            return false;
        case DESK_LEFT:
            if (record->event.pressed) {
                tap_code16(LCA(KC_LEFT));
            }
            return false;
        case DOUBLE_UNDERSCORE:
            if (record->event.pressed) {
                SEND_STRING("__");
            }
            return false;
        case KC_CYCLE_LAYERS:
            if (record->event.pressed) {
                const uint8_t current_layer = get_highest_layer(layer_state);
                layer_move(current_layer >= _RAISE ? _QWERTY : current_layer + 1);
            }
            return false;
        case KC_SPACE:
            if (!record->event.pressed && host_keyboard_led_state().caps_lock) {
                tap_code(KC_CAPS);
            }
            return true;
        default:
            return true;
    }
}

void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, 100);
}

// Rows 0-5 are the left half and 6-11 the right, with each half's thumb cluster
// on its lowest row. Thumbs are exempt so same-hand chords like Ctrl+Backspace
// still hold.
char chordal_hold_handedness(keypos_t key) {
    if (key.row == 0 || key.row == MATRIX_ROWS / 2) {
        return '*';
    }
    return key.row < MATRIX_ROWS / 2 ? 'L' : 'R';
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_SCLN:
        case KC_SLSH:
            return 250;
        default:
            return TAPPING_TERM;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_ergodox_pretty(
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,     KC_MINS,       KC_PSCR, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_DEL,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     KC_LBRC,       KC_RBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                              KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_BSLS, KC_Z,    KC_X,    KC_C,    KC_V,    LCTL_MT_B, KC_TAB,        KC_TAB,  RCTL_MT_N, KC_M, KC_COMM, KC_DOT,  KC_SLSH, KC_EQL,
        UOSA,    CTOA,    CPOA,    POSA,    OSL(_LOWER),                                      OSL(_RAISE), KC_PGDN, KC_PGUP, KC_END, KC_PSCR,
                                                               KC_LALT, KC_LGUI,      KC_LGUI, KC_RALT,
                                                                        FOSA,          HOSA,
                                                      KC_BSPC, KC_LSFT, KC_LCTL,      KC_ENT, KC_RSFT, KC_SPACE
    ),

    [_LOWER] = LAYOUT_ergodox_pretty(
        KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,  _______,       _______, KC_PSCR, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PGUP,
        KC_DEL,  _______, _______, KC_LBRC, KC_RBRC, _______,  _______,       _______, KC_ENT,  KC_7,    KC_8,    KC_9,    KC_P0,   KC_PGDN,
        TO(_QWERTY), TO(_QWERTY), DESK_LEFT, KC_LPRN, KC_RPRN, DESK_RIGHT,                    KC_HOME, KC_4, KC_5, KC_6, KC_PLUS, KC_PIPE,
        KC_BSLS, UOSA,    CTOA,    CPOA,    POSA,    FOSA,     KC_LPRN,       HOSA,    EOSA,    KC_1,    KC_2,    KC_3,    KC_MINS, CYCL,
        _______, _______, _______, _______, DBLUS,                                            KC_0,    _______, _______, _______, _______,
                                                               _______, _______,      _______, _______,
                                                                        _______,      _______,
                                                      KC_F5,   KC_LGUI, _______,      _______, KC_0, TO(_QWERTY)
    ),

    [_RAISE] = LAYOUT_ergodox_pretty(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,       _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        RM_TOGG, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    _______,       _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, QK_BOOT,
        RM_NEXT, TO(_QWERTY), KC_S, KC_D,   KC_F,    KC_G,                             KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, RM_VALU, KC_BSLS,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_LCTL,       HOSA,    EOSA,    KC_END,  RM_HUED, RM_SATD, RM_VALD, CYCL,
        _______, _______, _______, _______, _______,                                          KC_HOME, KC_END,  _______, _______, _______,
                                                               _______, _______,      _______, _______,
                                                                        _______,      _______,
                                                      KC_SPACE, _______, _______,     _______, KC_HOME, TO(_QWERTY)
    ),
};
