// License:MIT License
// copyright-holders:aburi6800 (Hitoshi Iwai)
// Compiler:zcc (z88dk)
// Include path:{$Z88DK_HOME}/include/* , {$Z88DK_HOME}/include/**/*

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <msx.h>
#include <msx/gfx.h>

#include "../include/resource.h"
#include "../include/music.h"
#include "../include/const.h"
#include "../include/unpack.h"
#include "../include/psgdriver.h"
#include "../include/asmsub.h"
#include "../include/logo.h"
#include "./scene.c"


// Reference to font.asm
//extern uint8_t FONT_COL_TBL[];
extern uint8_t FONT_PTN_TBL_JP_EN[];
extern uint8_t FONT_PTN_TBL_SP_PR[];

extern uint8_t REGION;

// Prompt message
uint8_t promptMessage[] = { 0x3E, 0x00 };

// Cursor character
uint8_t cursor[] = {0x5f, 0x00};

// Help command
uint8_t helpCommand[] = {0x48, 0x45, 0x4C, 0x50, 0x00};

// Blank character
uint8_t *blank = " ";

// Debug character
uint8_t *debug = "*";

// Command input buffer
uint8_t input_buffer[32 - sizeof(promptMessage)];

// Key input buffer
uint8_t key_buffer;

// Flag management (handled with bitwise operations)
uint16_t game_flags = 0;

// Expansion destination work area
uint8_t temp[2048];

// Reference to scene data
Scene *scene;

// Choice data
Choice choice;


void input_command()
{
    // RETURN key input flag
    bool_t enter_flg = false;

    // Input buffer index
    uint8_t buffer_ix = 0;

    // Clear buffer
    for (uint8_t i = 0; i < sizeof(input_buffer); i++) {
        input_buffer[i] = 0x00;
    }
    buffer_ix = 0;

    // Display prompt
    put_message_direct(0, PROMPT_LINE, promptMessage);

    // Clear key buffer
    buffer_check();

    // Start loop
    while(enter_flg == false) {
        // Display buffer contents
        put_message_direct(sizeof(promptMessage) - 1, PROMPT_LINE , input_buffer);
        put_message_direct(sizeof(promptMessage) + buffer_ix - 1, PROMPT_LINE , cursor);

        // Clear key buffer
//        msx_clearkey();

        // Wait for key input
//        key_buffer = getkeycode();    // TODO: I'd really like to use this (without stdio)
        key_buffer = getkey();

        // If RETURN key, exit loop
        if (key_buffer == 0x0a) {
            if (buffer_ix > 0) {
                enter_flg = true;
            }
            continue;
        }
/*
        // If ESC key, clear buffer
        if (key_buffer == 0x1b) {
            clear_inputBuffer();
            continue;
        }
*/
        // Delete one character from buffer with DEL key if buffer exists
        if (key_buffer == 0x08) {
            if (buffer_ix > 0) {
                buffer_ix--;
                put_message_direct(sizeof(promptMessage) + buffer_ix, PROMPT_LINE, blank);
            }
            input_buffer[buffer_ix] = 0x00;
            continue;
        }

        // Ignore cursor keys
        if (key_buffer >= 0x1c && key_buffer <= 0x1f) {
            continue;
        }

        // Convert lowercase letters to uppercase
        if (key_buffer >= 'a' && key_buffer <= 'z') {
            key_buffer -= 0x20;
        }

        // Otherwise add to buffer (← When doing romaji-kana input, you can call conversion processing here)
        if (buffer_ix < 31 - sizeof(promptMessage)) {
            input_buffer[buffer_ix++] = key_buffer;
        }
    }

    return;
}


// Get array index of scene from scene enumeration type
uint8_t getSceneIdx(SceneId sceneId)
{
    uint8_t idx = 0;

    for (uint8_t i = 0; i < SCENE_NUM; i++) {
        if (scenes[i]->sceneId == sceneId) {
            idx = i;
            break;
        }
    }

    return idx;
}


// Scene execution processing
void run_scene(SceneId start_scene_id)
{
    // Array index of current scene
    uint8_t scene_idx = getSceneIdx(start_scene_id);
    // Array index of previous scene
    uint8_t previous_scene_idx = 0xff;
    // Loop end determination flag
    bool_t end_flg = false;
    // Command match flag
    bool_t matched = false;

    while (!end_flg) {

        // Check if scene has changed from before
        if (scene_idx != previous_scene_idx) {

            // If changed, switch the scene
            scene = scenes[scene_idx];

            // When only performing scene branching based on flags
            if (scene->flag_to_check) {
                if (game_flags & scene->flag_to_check) {
                    scene_idx = getSceneIdx(scene->next_sceneId_if_set);
                } else {
                    scene_idx = getSceneIdx(scene->next_sceneId_if_unset);
                }

            // In case of normal scenes
            } else {
                // Replace previous scene ID with current scene ID
                previous_scene_idx = scene_idx;

                if (scene->graphic_ptn0 != NULL) {
                    // When graphic data is set
                    // Block 1/2 color table setting (blank)
                    for (uint16_t i = 0; i < 2048; i++) {
                        temp[i] = 0x00;
                    }
                    vdp_vwrite(temp, VRAM_COLOR_TBL1, VRAM_COLOR_TBL_SIZE);
                    vdp_vwrite(temp, VRAM_COLOR_TBL2, VRAM_COLOR_TBL_SIZE);
                    // Unpack and display data
                    switch_bank(scene->graphic_bank);
                    unpack(scene->graphic_ptn0, temp);
                    vdp_vwrite(temp, VRAM_PTN_GENR_TBL1, 0x04c0);
                    unpack(scene->graphic_ptn1, temp);
                    vdp_vwrite(temp, VRAM_PTN_GENR_TBL2, 0x0390);
                    unpack(scene->graphic_col0, temp);
                    vdp_vwrite(temp, VRAM_COLOR_TBL1, 0x04c0);
                    unpack(scene->graphic_col1, temp);
                    vdp_vwrite(temp, VRAM_COLOR_TBL2, 0x0390);
                }

                // Display message
                clear_message();
                put_message(0, 17, scene->message);

                if (scene->sceneId == OVER) {
                    // If game over, end processing
                    end_flg = true;
                }

                if (scene->next_sceneId_if_unset != NOSCENE) {
                    // If only the next scene setting is done, wait for key input and change scenes
                    keywait();
                    scene_idx = getSceneIdx(scene->next_sceneId_if_unset);
                }
            }

        } else {
            // Command input
            input_command();

            // Clear message display area
            clear_message();

            // Command match flag OFF
            matched = false;

            if ((strcmp(helpCommand, input_buffer) == 0) &&
                (scene->sceneId != TITLE) &&
                (scene->sceneId != PROLOGUE)) {

                put_message(0, 17, HELPMESSAGE);
                matched = true;

            } else {

                Choice *choices = scene->choices;
                uint8_t i = 0;

                while (choices[i] != NULL) {

                    // Get reference to choice data from scene data
                    choice = choices[i];

                    // If choice data command is not set, exit loop
                    if (choice.commands[0] == NULL) {
                        break;
                    } 

                    // Compare against all command candidates
                    for (int j = 0; choice.commands[j] != NULL; j++) {

                        // Choice data command = input command AND
                        // Condition flag = not set OR condition flag target = ON
                        if ((strcmp(choice.commands[j], input_buffer) == 0) &&
                            ((choice.required_flag == 0) || (game_flags & choice.required_flag))) {

                            // Command match flag ON
                            matched = true;

                            // Check target flag in choice data = set AND
                            // Check target flag = ON
                            if ((choice.flag_to_check) && (game_flags & choice.flag_to_check)) {
                                // Display message
                                if (choice.message_if_set != NULL) {
                                    put_message(0, 17, choice.message_if_set);
                                }
                                // Set flag
                                if (choice.set_flag_if_set) {
                                    game_flags |= choice.set_flag_if_set;
                                }
                                // Scene transition
                                if (choice.next_sceneId_if_set) {
                                    if (choice.message_if_set != NULL) {
                                        keywait();
                                    }
                                    scene_idx = getSceneIdx(choice.next_sceneId_if_set);
                                }

                            // Otherwise
                            } else {
                                // Display message
                                if (choice.message_if_unset != NULL) {
                                    put_message(0, 17, choice.message_if_unset);
                                }
                                // Set flag
                                if (choice.set_flag_if_unset) {
                                    game_flags |= choice.set_flag_if_unset;
                                }
                                // Scene transition
                                if (choice.next_sceneId_if_unset) {
                                    if (choice.message_if_unset != NULL) {
                                        keywait();
                                    }
                                    scene_idx = getSceneIdx(choice.next_sceneId_if_unset);
                                }
                            }

                            break;
                        }
                    }

                    if (matched) {
                        break;
                    }

                    i++;
                }
                
            }

            // Did command match?
            if (!matched) {
                // If command did not match, display a fixed message
                put_message(0, 17, DONTMESSAGE);
            }
        }
    }
}


void init()
{
    // Language detection
    check_region();

    // Change palette
    set_palette();

    // Sound driver initialization
    sounddrv_init();

    // Display logo
    boot_logo();

    // Key click switch OFF
    *(uint8_t *)MSX_CLIKSW = 0;

    // Time interval before key auto-repeat starts
    // C-BIOS initial value is 1, so explicitly set (but will be overwritten immediately and cannot be changed)
    *(uint8_t *)MSX_REPCNT = 50;

    // Block 3 pattern generator table setting (font pattern)
    switch_bank(1);
    if (REGION == 0 || REGION == 1) {
        unpack(FONT_PTN_TBL_JP_EN, temp);
    } else {
        unpack(FONT_PTN_TBL_SP_PR, temp);
    }
    vdp_vwrite(temp, VRAM_PTN_GENR_TBL3, VRAM_PTN_GENR_TBL_SIZE);

    // Block 3 color table setting (font pattern)
    for (uint16_t i = 0; i < 0x0800; i++) {
        temp[i] = 0xf0;
    }
    vdp_vwrite(temp, VRAM_COLOR_TBL3, VRAM_PTN_GENR_TBL_SIZE);

    // Pattern name table initialization
    uint8_t code = 0;
    for (uint16_t i = 0; i < 256; i++) {
        temp[i] = 254;
    }
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 19; j++) {
            temp[i * 32 + j + 6] = code++;
        }
    }
    // Block 1
    vdp_vwrite(temp, VRAM_PTN_NAME_TBL1, 0x100);
    // Block 2
    vdp_vwrite(temp, VRAM_PTN_NAME_TBL2, 0x100);
}

void main()
{
    init();

    while (0 == 0) {
        // Flag initialization
        game_flags = 0;

        // Game start
        run_scene(TITLE);
    }
}
