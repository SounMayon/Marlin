/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../inc/MarlinConfig.h"
#include <string.h>

enum {
  ID_EN = 1,
  ID_FR,
  ID_L_RETURN
};

#define SELECTED    1
#define UNSELECTED  0

static void disp_language(uint8_t language, uint8_t state);

extern lv_group_t *g;
static lv_obj_t *scr;
static lv_obj_t *buttonEN, *buttonFR;

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  switch (obj->mks_obj_id) {
    case ID_EN:
      gCfgItems.language = LANG_ENGLISH;
      update_spi_flash();
      disp_language_init();
      disp_language(LANG_ENGLISH, SELECTED);
      disp_language(LANG_FRENCH, UNSELECTED);
      break;
    case ID_FR:
      gCfgItems.language = LANG_FRENCH;
      update_spi_flash();
      disp_language_init();
      disp_language(LANG_FRENCH, SELECTED);
      disp_language(LANG_ENGLISH, UNSELECTED);
      break;
    case ID_L_RETURN:
      buttonEN = nullptr;
      buttonFR = nullptr;
      lv_clear_language();
      lv_draw_set();
      break;
  }
}

static void disp_language(const uint8_t language, const uint8_t state) {
  lv_obj_t *obj;
  uint16_t id;
  const char *image_path;

  if (language == LANG_FRENCH) {
    id = ID_FR;
    obj = buttonFR;
    image_path = state == SELECTED ? "F:/bmp_french_sel.bin" : "F:/bmp_french.bin";
  }
  else {
    id = ID_EN;
    obj = buttonEN;
    image_path = state == SELECTED ? "F:/bmp_english_sel.bin" : "F:/bmp_english.bin";
  }

  lv_obj_set_event_cb_mks(obj, event_handler, id, "", 0);
  // Use persistent string literals: LVGL keeps the source pointer for later redraws.
  lv_imgbtn_set_src_both(obj, image_path);
  if (state == UNSELECTED) lv_obj_refresh_ext_draw_pad(obj);
}

void lv_draw_language() {
  scr = lv_screen_create(LANGUAGE_UI);
  buttonEN = lv_big_button_create(scr, "F:/bmp_english.bin", "English",
                                  INTERVAL_V, titleHeight, event_handler, ID_EN);
  buttonFR = lv_big_button_create(scr, "F:/bmp_french.bin", "Français",
                                  BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_FR);
  lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back,
                       BTN_X_PIXEL * 3 + INTERVAL_V * 4,
                       BTN_Y_PIXEL + INTERVAL_H + titleHeight,
                       event_handler, ID_L_RETURN);
  disp_language(gCfgItems.language == LANG_FRENCH ? LANG_FRENCH : LANG_ENGLISH, SELECTED);
}

void lv_clear_language() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
