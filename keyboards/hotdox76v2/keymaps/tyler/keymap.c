// Copyright 2021 JasonRen(biu)
// Copyright 2022 Drashna Jael're (@Drashna Jael're)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "host.h"

enum layers { _MAC, _LINUX, _FUNC, _OTHER };

enum custom_keycodes {
    FUNC_TOGGLE = SAFE_RANGE,
    MAC_COPY, 
    MAC_PASTE,
    CTRL_COPY,
    CTRL_PASTE,
    MAC_END,
    MAC_HOME,
    SPACE_CANCEL_CAPS
};

enum combos{
    OS_LSFT,
    OS_RSFT,
    LENTER,
    RENTER,
    RENTERM,
    LENTERM
};

const uint16_t PROGMEM lsft_combo[] = {KC_F, KC_D, COMBO_END};
const uint16_t PROGMEM rsft_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM renter_combo[] = {KC_J, KC_K, RCTL_T(KC_L), COMBO_END};
const uint16_t PROGMEM renter_combo_mac[] = {KC_J, KC_K, RGUI_T(KC_L), COMBO_END};
const uint16_t PROGMEM lenter_combo[] = {LCTL_T(KC_S), KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM lenter_combo_mac[] = {LGUI_T(KC_S), KC_D, KC_F, COMBO_END};
combo_t key_combos[] = {
    [OS_LSFT] = COMBO_ACTION(lsft_combo),
    [OS_RSFT] = COMBO_ACTION(rsft_combo),
    [LENTER] = COMBO_ACTION(lenter_combo),
    [RENTER] = COMBO_ACTION(renter_combo),
    [LENTERM] = COMBO_ACTION(lenter_combo_mac),
    [RENTERM] = COMBO_ACTION(renter_combo_mac),
};

bool combo_held = false;
static uint16_t combo_timer;
void process_combo_event(uint16_t combo_index, bool pressed) {
    switch(combo_index) {
        case OS_LSFT:
            if (pressed) {
                combo_timer = timer_read();
                register_mods(MOD_BIT(KC_LSFT));  // Hold starts now
            } else {
                if (timer_elapsed(combo_timer) < TAPPING_TERM) {
                    // Tap: oneshot shift
                    set_oneshot_mods(MOD_BIT(KC_LSFT));
                }
                unregister_mods(MOD_BIT(KC_LSFT));  // Always unregister after release
            }
            break;
        case OS_RSFT:
            if (pressed) {
                combo_timer = timer_read();
                register_mods(MOD_BIT(KC_RSFT));  // Hold starts now
            } else {
                if (timer_elapsed(combo_timer) < TAPPING_TERM) {
                    // Tap: oneshot shift
                    set_oneshot_mods(MOD_BIT(KC_RSFT));
                }
                unregister_mods(MOD_BIT(KC_RSFT));  // Always unregister after release
            }
            break;
        case LENTERM:
        case RENTERM:
        case LENTER:
        case RENTER:
            if (pressed) {
                tap_code(KC_ENTER);
                combo_held = true;
            }
            break;
    }
};
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t func_layer_timer;
    static uint16_t macro_timer;

    switch (keycode) {
        case KC_SPACE:
            if (!record->event.pressed){ //using the ! indicates "on key up" while removing it indicates "on key down"
                if (host_keyboard_led_state().caps_lock){
                    tap_code(KC_CAPS);
                }
            }
            return true; //continue normal processing
        case FUNC_TOGGLE:
            if (record->event.pressed) {
                func_layer_timer = timer_read(); // Record the press time
            } else {
                if (timer_elapsed(func_layer_timer) < TAPPING_TERM) {
                    // TAP: send one-shot layer
                    layer_on(_FUNC);
                    set_oneshot_layer(_FUNC, ONESHOT_START);
                } else {
                    // HOLD: activate momentary layer
                    layer_off(_FUNC);
                }
            }
            return false; // Skip normal processing
        case CTRL_COPY:
            if (record->event.pressed) {
                macro_timer = timer_read(); // Record the press time
            } else {
                if (timer_elapsed(macro_timer) < TAPPING_TERM) {
                    tap_code(KC_C);  // Tap: send normal key
                }else{
                    register_code(KC_RIGHT_CTRL);
                    tap_code(KC_C);
                    unregister_code(KC_RIGHT_CTRL);
                } 
            }
            return false;
        case CTRL_PASTE:
            if (record->event.pressed) {
                macro_timer = timer_read(); // Record the press time
            } else {
                if (timer_elapsed(macro_timer) < TAPPING_TERM) {
                       tap_code(KC_V);  // Tap: send normal key
                }else{
                    register_code(KC_RIGHT_CTRL);
                    tap_code(KC_V);
                    unregister_code(KC_RIGHT_CTRL);
                } 
            }
            return false;
        case MAC_PASTE:
            if (record->event.pressed) {
                macro_timer = timer_read(); // Record the press time
            } else {
                if (timer_elapsed(macro_timer) < TAPPING_TERM) {
                    tap_code(KC_V);  // Tap: send normal key
                }else{                
                    register_code(KC_RIGHT_GUI);
                    tap_code(KC_V);
                    unregister_code(KC_RIGHT_GUI);
                }
            }
            return false;
        case MAC_COPY:
            if (record->event.pressed) {
                macro_timer = timer_read(); // Record the press time
            } else {
                if (timer_elapsed(macro_timer) < TAPPING_TERM) {
                    tap_code(KC_C);  // Tap: send normal key
                }else{  
                    register_code(KC_RIGHT_GUI);
                    tap_code(KC_C);
                    unregister_code(KC_RIGHT_GUI);
                }
            }   
            return false;
        case MAC_END:
            if (record->event.pressed) {
                register_code(KC_LGUI);   // Command down
                tap_code(KC_RIGHT);
                unregister_code(KC_LGUI); // Command up
            }
            return false; // Skip default behavior
        case MAC_HOME:
            if (record->event.pressed) {
                register_code(KC_LGUI);
                tap_code(KC_LEFT);
                unregister_code(KC_LEFT_GUI);
            }
            return false;
    }
    return true; 
};
void keyboard_post_init_user(void) {
    // Set RGB to solid white at full brightness
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(1);
    rgblight_sethsv_noeeprom(0, 0, 255); // HSV: hue=0, sat=0 (white), val=255 (max brightness)
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record){
    switch (keycode) {
        case LGUI_MT_A:
        case LALT_MT_A: 
            return 320;
        case MAC_COPY: 
        case MAC_PASTE:
        case CTRL_COPY:
        case CTRL_PASTE:
            return 190; 
        default: 
            return TAPPING_TERM; 
    }
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  //KeymapCEditor - keymap.c viewer and editor - I use this extension to view the keymaps in a more ui friendly way for quick mental parsing.  
    [_MAC] = LAYOUT_ergodox_pretty(
        KC_GRV,      KC_1,               KC_2,           KC_3,         KC_4,          KC_5,    KC_6,               KC_PSCR,          KC_7,    KC_8,         KC_9,         KC_0,             KC_MINS,               KC_EQL,
        KC_DEL,      KC_Q,               KC_W,           KC_E,         KC_R,          KC_T,    KC_LEFT_BRACKET,    KC_RIGHT_BRACKET, KC_Y,    KC_U,         KC_I,         KC_O,             KC_P,                  KC_BSLS,
        KC_ESC,      KC_A,          LGUI_T(KC_S),   KC_D,         KC_F,          KC_G,                                          KC_H,    KC_J,         KC_K,         RGUI_T(KC_L), RALT_T(KC_SCLN),   KC_QUOT,
        OSL(_OTHER), MT(MOD_LCTL, KC_Z), KC_X,           MAC_COPY,     MAC_PASTE,     KC_B,    OSL(_FUNC),         KC_N,             KC_N,    KC_M,         KC_COMM,      KC_DOT,           MT(MOD_RCTL, KC_SLSH), KC_RSFT,
        KC_CAPS,     KC_F4,              KC_F5,          KC_LEFT,      KC_RIGHT,                                                                              KC_DOWN,      KC_UP,        KC_LBRC,      TO(_LINUX),           TO(_LINUX),
                                                                                                        KC_LALT, KC_LGUI,             KC_RALT, KC_A,
                                                                                                                 KC_PGUP,             MAC_HOME,
                                                                                           KC_BSPC, OSL(_OTHER), KC_TAB,              MAC_END, OSL(_FUNC), KC_SPACE
    ),

    [_LINUX] = LAYOUT_ergodox_pretty(
        KC_GRV,      KC_1,              KC_2,         KC_3,         KC_4,          KC_5,    KC_6,               KC_PSCR,           KC_7,    KC_8,         KC_9,         KC_0,             KC_MINS,               KC_EQL,
        KC_DEL,      KC_Q,              KC_W,         KC_E,         KC_R,          KC_T,    KC_LEFT_BRACKET,    KC_RIGHT_BRACKET,  KC_Y,    KC_U,         KC_I,         KC_O,             KC_P,                  KC_BSLS,
        KC_ESC,      LGUI_MT_A,        LCTL_T(KC_S), KC_D,         KC_F,          KC_G,                                                   KC_H,    KC_J,         KC_K,         RCTL_T(KC_L), KC_SCLN,           KC_QUOT,
        OSL(_OTHER), MT(MOD_LCTL, KC_Z),KC_X,         CTRL_COPY,    CTRL_PASTE,    KC_B,    OSL(_FUNC),         KC_N,              KC_N,    KC_M,         KC_COMM,      KC_DOT,           MT(MOD_RCTL, KC_SLSH), KC_RSFT,
        KC_CAPS,     KC_F4,             KC_F5,        KC_LEFT,      KC_RIGHT,                                                                               KC_DOWN,      KC_UP,        TO(_FUNC),    TO(_FUNC),       TO(_MAC),
                                                                                                        KC_LALT, KC_LGUI,            KC_RALT, KC_A,
                                                                                                                     KC_PGUP,            KC_HOME,
                                                                                       KC_BSPC, OSL(_OTHER), KC_TAB,             KC_END, OSL(_FUNC), KC_SPACE
    ),

    [_FUNC] = LAYOUT_ergodox_pretty(
        KC_GRV,  KC_F1,              KC_F2,   KC_F3,   KC_F4,    KC_F5,   KC_F6,               KC_PSCR, KC_F7,   KC_F8,   KC_F9,   KC_F10,      KC_F11,                  KC_F12,
        KC_DEL,  KC_Q,               KC_W,    KC_E,    KC_R,     KC_T,    TO(_OTHER),          TO(1),   KC_Y,    KC_U,    KC_LBRC, KC_RBRC,     KC_P,                    KC_BSLS,
        KC_ESC,  KC_LGUI,            _______, KC_D,    KC_F,     KC_G,                                          KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,KC_SCLN,             KC_QUOT,
        _______, MT(MOD_LCTL, KC_Z), KC_X,    KC_C,    KC_V,     KC_B,    _______,             KC_N,    KC_N,    KC_M,    KC_COMM, KC_DOT,      MT(MOD_RCTL, KC_SLSH),   KC_RSFT,
        _______, KC_F4,              KC_F5,   KC_LEFT, KC_RIGHT,                                                                 KC_DOWN, KC_UP,   KC_LBRC, TO(_OTHER),             KC_RGUI,
                                                                                     KC_LALT, KC_LGUI,             KC_RALT, KC_A,
                                                                                                  KC_PGUP,             KC_PGDN,
                                                                        KC_BSPC, KC_ENT, _______,           KC_RCTL, _______, _______
    ),

    [_OTHER] = LAYOUT_ergodox_pretty(
        KC_GRV,  KC_1,               KC_2,    KC_3,    KC_4,     KC_5,    KC_6,               KC_PSCR, KC_7,    KC_8,    KC_9,    KC_0,       KC_MINS,               KC_EQL,
        KC_DEL,  KC_EXCLAIM,               KC_W,    KC_E,    KC_R,     KC_T,    TO(_MAC),           TO(1),   KC_Y,    KC_7,    KC_8,    KC_9,       KC_P,                  KC_BSLS,
        KC_ESC,  KC_LGUI,            _______, KC_D,    KC_F,     KC_G,                                         KC_H,    KC_4,    KC_5,    KC_6,   KC_SCLN,           KC_QUOT,
        _______, MT(MOD_LCTL, KC_Z), KC_X,    KC_UP,    KC_DOWN, KC_B,    _______,            KC_N,    KC_N,    KC_1,    KC_2,    KC_3,       MT(MOD_RCTL, KC_SLSH), KC_RSFT,
        _______, KC_F4,              KC_F5,   KC_LEFT, KC_RIGHT,                                                                KC_0,    KC_0,    KC_DOT, TO(_MAC),          KC_RGUI,
                                                                                     KC_LALT, KC_LGUI,            KC_RALT, KC_A,
                                                                                                  KC_PGUP,            KC_PGDN,
                                                                        KC_BSPC, KC_ENT, _______,             KC_RCTL, KC_ENT, _______
    ),
};
