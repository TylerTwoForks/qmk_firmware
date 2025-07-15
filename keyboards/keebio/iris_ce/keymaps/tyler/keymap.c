// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "config.h"

#if defined(OS_DETECTION_ENABLE)
    #include "os_detection.h"
#endif

enum custom_layers {
     _HDP,
     _LOWER,
     _RAISE
};

enum custom_keycodes {
    KC_CYCLE_LAYERS= SAFE_RANGE,
    COPY_OS_AWARE,
    PASTE_OS_AWARE,
    CUT_OS_AWARE,
    FIND_OS_AWARE,
    UNDO_OS_AWARE,
    HOME_OS_AWARE,
    END_OS_AWARE,
    MAC_END,
    MAC_HOME,
    SPACE_CANCEL_CAPS,
};

enum combos{
    LENTERHD,
    RENTERHD,
    LSHIFTHD,
    RSHIFTHD,
};

// 1st layer on the cycle
#define LAYER_CYCLE_START 0
// Last layer on the cycle
#define LAYER_CYCLE_END 4

bool cycle_layer(void) {
    uint8_t current_layer = get_highest_layer(layer_state);

    // Check if we are within the range, if not quit
    if (current_layer > LAYER_CYCLE_END || current_layer < LAYER_CYCLE_START) {
        return false;
    }

    uint8_t next_layer = current_layer + 1;
    if (next_layer > LAYER_CYCLE_END) {
        next_layer = LAYER_CYCLE_START;
    }
    layer_move(next_layer);
    return false;
}

const uint16_t PROGMEM lenter_combo_hd[] = {LCTL_MT_N, KC_T, KC_H, COMBO_END};
const uint16_t PROGMEM renter_combo_hd[] = {KC_A, KC_E, RCTL_MT_I, COMBO_END};
const uint16_t PROGMEM lsft_combo_hd[] = {KC_T, KC_H, COMBO_END};
const uint16_t PROGMEM rsft_combo_hd[] = {KC_A, KC_E, COMBO_END};

combo_t key_combos[] = {
    [LENTERHD] = COMBO_ACTION(lenter_combo_hd),
    [RENTERHD] = COMBO_ACTION(renter_combo_hd),
    [RENTERHD] = COMBO_ACTION(renter_combo_hd),
    [LSHIFTHD] = COMBO_ACTION(lsft_combo_hd),
    [RSHIFTHD] = COMBO_ACTION(rsft_combo_hd),
};

bool combo_held = false;
static uint16_t combo_timer;
void process_combo_event(uint16_t combo_index, bool pressed) {
    switch(combo_index) {
        case LSHIFTHD:
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
        case RSHIFTHD:
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
        case RENTERHD:
        case LENTERHD:
            if (pressed) {
                tap_code(KC_ENTER);
                combo_held = true;
            }
            break;
    }
};

// Some helper C macros
    #define GENERAL_MODIFIER_KEY_DELAY_MS 10
    #define GENERAL_KEY_ACTION_DELAY_MS   20

    #define KEY_MODIFIER_ACTION(keycode, modifier) \
        SS_DOWN(modifier) \
        SS_DELAY(GENERAL_MODIFIER_KEY_DELAY_MS) \
        SS_TAP(keycode) \
        SS_DELAY(GENERAL_KEY_ACTION_DELAY_MS) \
        SS_UP(modifier) \
        SS_DELAY(GENERAL_MODIFIER_KEY_DELAY_MS)

    #define KEY_CTRL_ACTION(keycode) \
        KEY_MODIFIER_ACTION(keycode,X_LCTL)

    #define KEY_APPLE_KEY_ACTION(keycode) \
        KEY_MODIFIER_ACTION(keycode,X_LCMD)

        
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // static uint16_t macro_timer;

    #if defined(OS_DETECTION_ENABLE)
        os_variant_t host = OS_UNSURE;
        host = detected_host_os();
    #else
        uint8_t host = 0;
    #endif

    switch (keycode) {
        case END_OS_AWARE:
            if (record->event.pressed){
                #if defined (OS_DETECTION_ENABLED)
                    if(host == OS_MACOS){
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_RIGHT));
                    }else{
                        tap_code(KC_END)
                    }
                #endif
            }
        case HOME_OS_AWARE:
            if (record->event.pressed){
                #if defined (OS_DETECTION_ENABLED)
                    if(host == OS_MACOS){
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_LEFT));
                    }else{
                        tap_code(KC_HOME)
                    }
                #endif
            }
        case COPY_OS_AWARE:
            if (record->event.pressed){
                #if defined(OS_DETECTION_ENABLE)
                    if (host == OS_MACOS || host == OS_IOS){
                        // Mac: Cmd + C
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_C));
                    }else{
                        // Linux, Windows, etc.: Ctrl + C
                        SEND_STRING(KEY_CTRL_ACTION(X_C));
                    }
                #endif
            } 
            break;
        case PASTE_OS_AWARE:
            if (record->event.pressed){
                #if defined(OS_DETECTION_ENABLE)
                    if (host == OS_MACOS || host == OS_IOS){
                        // Mac: Cmd + V
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_V));
                    }else{
                        // Linux, Windows, etc.: Ctrl + V
                        SEND_STRING(KEY_CTRL_ACTION(X_V));
                    }
                #endif
            } 
            break;
        case CUT_OS_AWARE:
            if (record->event.pressed){
                #if defined(OS_DETECTION_ENABLE)
                    if (host == OS_MACOS || host == OS_IOS){
                        // Mac: Cmd + X
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_X));
                    }else{
                        // Linux, Windows, etc.: Ctrl + X
                        SEND_STRING(KEY_CTRL_ACTION(X_X));
                    }
                #endif
            } 
            break;
         case FIND_OS_AWARE:
            if (record->event.pressed){
                #if defined(OS_DETECTION_ENABLE)
                    if (host == OS_MACOS || host == OS_IOS){
                        // Mac: Cmd + X
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_F));
                    }else{
                        // Linux, Windows, etc.: Ctrl + X
                        SEND_STRING(KEY_CTRL_ACTION(X_F));
                    }
                #endif
            } 
            break;
        case UNDO_OS_AWARE:
            if (record->event.pressed){
                #if defined(OS_DETECTION_ENABLE)
                    if (host == OS_MACOS || host == OS_IOS){
                        // Mac: Cmd + Z
                        SEND_STRING(KEY_APPLE_KEY_ACTION(X_Z));
                    }else{
                        // Linux, Windows, etc.: Ctrl + Z
                        SEND_STRING(KEY_CTRL_ACTION(X_Z));
                    }
                #endif
            } 
            break;
        case KC_CYCLE_LAYERS:
            if (!record->event.pressed) {
                // We've already handled the keycode (doing nothing), let QMK know so no further code is run unnecessarily
                return false;
            };
            cycle_layer();
        case KC_SPACE://this is used to remove CAPS when hitting space
            if (!record->event.pressed) { // using the ! indicates "on key up" while removing it indicates "on key down"
                if (host_keyboard_led_state().caps_lock) {
                    tap_code(KC_CAPS);
                }
            }
            return true; // continue normal processing
        
    }

    return true;
};
void keyboard_post_init_user(void) {
    // Set RGB to solid white at full brightness
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(1);
    rgblight_sethsv_noeeprom(0, 0, 100); // HSV: hue=0, sat=0 (white), val=255 (max brightness)
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record){
    switch (keycode) {
        case LGUI_MT_S:
        case LCTL_MT_N:
            return 320;
        default: 
            return TAPPING_TERM; 
    }
};



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_HDP] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_GRV, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_CAPS,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_DEL,  KC_F,    KC_P,    KC_D,    KC_L,    KC_X,                               KC_SCLN, KC_U,    KC_O,    KC_Y,    KC_B,    KC_Z,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_ESC,  LGUI_MT_S,LCTL_MT_N, KC_T, KC_H,    KC_K,                               KC_COMM, KC_A,    KC_E,    RCTL_MT_I, KC_C,  KC_Q,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_BSLS, KC_V,    KC_W,    KC_G,    KC_M,    KC_J,    KC_TAB,           KC_RALT, KC_MINS, KC_DOT,  KC_QUOT, KC_EQL,  KC_SLSH, CYCL,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    OSL(_LOWER),KC_R, KC_BSPC,                   KC_SPC,  KC_Z,    OSL(_RAISE)
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_LOWER] = LAYOUT(//symbol - other - num pad
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_PSCR, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PGUP,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_DEL,  _______, _______, KC_LBRC, KC_RBRC, _______,                            _______, KC_7,    KC_8,    KC_9,    KC_P0,   KC_PGDN,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_ESC,  _______, _______, KC_LPRN, KC_RPRN, _______,                            HOSA,    KC_4,    KC_5,    KC_6,    KC_PLUS, KC_PIPE,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_BSLS, UOSA,    CTOA,    CPOA,    POSA,    FOSA,    KC_LPRN,          HOSA,    EOSA,    KC_1,    KC_2,    KC_3,    KC_MINS, CYCL,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    KC_F5,   KC_LGUI, KC_DEL,                    TO(_HDP),  KC_0,  _______
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_RAISE] = LAYOUT( //movement - func - gaming
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     RM_TOGG, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, QK_BOOT,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     RM_NEXT, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, RM_VALU, KC_BSLS,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_LCTL,          HOSA,    EOSA,    KC_END,  RM_HUED, RM_SATD, RM_VALD, CYCL,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, KC_SPACE,                  TO(_HDP), _______, _______
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  )
};
