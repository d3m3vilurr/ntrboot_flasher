#include "menu.h"

#include "device.h"

#include "hid.h"
#include "protocol.h"
#include "ui.h"

void menu_wait_cart_insert()
{
    if (REG_CARDCONF2 & 0x1)
        ShowString(TOP_SCREEN, "No cartridge inserted\n(insert a cartridge to continue)");
    while (REG_CARDCONF2 & 0x1) { };
}

bool menu_show_intro_warning()
{
    ClearScreen(TOP_SCREEN, COLOR_RED);
    
    DrawStringF(TOP_SCREEN, 10, 10, COLOR_WHITE, COLOR_RED, "WARNING: READ THIS BEFORE CONTINUING");
    DrawStringF(TOP_SCREEN, 10, 20, COLOR_WHITE, COLOR_RED, "------------------------------------");

    DrawStringF(TOP_SCREEN, 10, 40, COLOR_WHITE, COLOR_RED, "If you don't know what you're doing: STOP. Open your browser to");
    DrawStringF(TOP_SCREEN, 10, 50, COLOR_WHITE, COLOR_RED, "http://3ds.guide and follow the steps provided there.");

    DrawStringF(TOP_SCREEN, 10, 70, COLOR_WHITE, COLOR_RED, "This software writes directly to your flashcart. It's possible");
    DrawStringF(TOP_SCREEN, 10, 80, COLOR_WHITE, COLOR_RED, "you may brick your flashcart, rendering it unusable.");

    DrawStringF(TOP_SCREEN, 10, 100, COLOR_WHITE, COLOR_RED, "ALWAYS KEEP A BACKUP");

    DrawStringF(TOP_SCREEN, 10, 120, COLOR_WHITE, COLOR_RED, "<A> Continue <B> Exit");

    while (HID_STATE != 0); // bug fix: wait for the HID_STATE to reset
    bool retval = (WaitButton(BUTTON_A | BUTTON_B) & BUTTON_A) == BUTTON_A;
    ClearScreen(TOP_SCREEN, COLOR_BLACK);
    return retval;
}

int8_t menu_select_flashcart()
{
    ClearScreen(TOP_SCREEN, STD_COLOR_BG);

    int deviceOption = 0;    
    while(true)
    {
        DrawRectangle(TOP_SCREEN, 0, 0, SCREEN_WIDTH_TOP, 12, COLOR_BLUE);

        DrawStringF(TOP_SCREEN, 10,  1, COLOR_WHITE, COLOR_BLUE, "Select your flashcart:");
        DrawStringF(TOP_SCREEN, 270, 1, COLOR_WHITE, COLOR_BLUE, "<A> Select  <B> Exit");

        DrawStringF(TOP_SCREEN, 10, SCREEN_HEIGHT-23, COLOR_BLACK, COLOR_LIGHTGREY, "ntrboot_flasher: %s", NTRBOOT_FLASHER_VERSION);
        DrawStringF(TOP_SCREEN, 10, SCREEN_HEIGHT-11, COLOR_BLACK, COLOR_LIGHTGREY, "flashcart_core:  %s", FLASHCART_CORE_VERSION);

        // todo: scroll through this, we can only have 23 on the screen at once
        int i = 0;
        for (std::vector<Flashcart*>::iterator it = flashcart_list->begin() ; it != flashcart_list->end(); ++it, i++) {
            DrawStringF(TOP_SCREEN, 10, 20 + i*10, (i == deviceOption) ? COLOR_RED : STD_COLOR_FONT, STD_COLOR_BG, (*it)->getName());

            if (i == deviceOption)
            {
                ClearScreen(BOTTOM_SCREEN, STD_COLOR_BG);

                DrawRectangle(BOTTOM_SCREEN, 0, 0, SCREEN_WIDTH_TOP, 12, COLOR_BLUE);
                DrawStringF(BOTTOM_SCREEN, 106, 0, COLOR_WHITE, COLOR_BLUE, "Selected Cart Info");

                DrawStringF(BOTTOM_SCREEN, 10, 20, STD_COLOR_FONT, STD_COLOR_BG, "Name: %s", (*it)->getName());
                DrawStringF(BOTTOM_SCREEN, 10, 30, STD_COLOR_FONT, STD_COLOR_BG, "Author: %s", (*it)->getAuthor());

                DrawStringF(BOTTOM_SCREEN, 10, 50, STD_COLOR_FONT, STD_COLOR_BG, (*it)->getDescription());
            }
        }

        uint32_t keys = WaitButton(BUTTON_ANY);
        if (keys & BUTTON_UP) deviceOption--;
        if (keys & BUTTON_DOWN) deviceOption++;

        if(deviceOption < 0) deviceOption = 0;
        else if(deviceOption >= flashcart_list->size()) deviceOption = flashcart_list->size() - 1;

        if (keys & BUTTON_A) return deviceOption;
        if (keys & BUTTON_B) return -1;        
    }

    return -1;
}


uint8_t menu_flashcart_menu(const char* flashcart_name)
{
    ClearScreen(TOP_SCREEN, STD_COLOR_BG);    

    DrawStringF(TOP_SCREEN, 10, 10, STD_COLOR_FONT, STD_COLOR_BG, "Flashcart: %s", flashcart_name);

    int menuOption = 0;    
    while(true)
    {        
        DrawStringF(TOP_SCREEN, 10, 30, (menuOption == MENU_DUMP   )          ? COLOR_RED : STD_COLOR_FONT, STD_COLOR_BG, "Dump Flash");
        DrawStringF(TOP_SCREEN, 10, 40, (menuOption == MENU_RESTORE)          ? COLOR_RED : STD_COLOR_FONT, STD_COLOR_BG, "Restore Flash");
        DrawStringF(TOP_SCREEN, 10, 50, (menuOption == MENU_INJECT )          ? COLOR_RED : STD_COLOR_FONT, STD_COLOR_BG, "Inject Ntrboot");
        DrawStringF(TOP_SCREEN, 10, 70, (menuOption == MENU_SELECT_FLASHCART) ? COLOR_RED : STD_COLOR_FONT, STD_COLOR_BG, "Reselect Flashcart");
        DrawStringF(TOP_SCREEN, 10, 90, (menuOption == MENU_EXIT)             ? COLOR_RED : STD_COLOR_FONT, STD_COLOR_BG, "Exit");
        
        uint32_t keys = WaitButton(BUTTON_ANY);
        if (keys & BUTTON_UP) menuOption--;
        if (keys & BUTTON_DOWN) menuOption++;

        if(menuOption < 0) menuOption = 0;
        else if(menuOption >= MENU_MAX) menuOption = MENU_MAX - 1;

        if (keys & BUTTON_A) return menuOption;
        if (keys & BUTTON_B) return MENU_EXIT;
    }

    return -1;
}