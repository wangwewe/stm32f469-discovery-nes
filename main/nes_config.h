#ifndef NES_CONFIG_H
#define NES_CONFIG_H
/* First version: STM32F469I-DISCO, production board (8-MHz HSE). */
#define NES_GAME_INDEX        0 /* 0: SuperMario, 1: yingzichuanshuo */
#define NES_LCD_WIDTH         800U
#define NES_LCD_HEIGHT        480U
#define NES_DISPLAY_SCALE     2U
#define NES_FRAMEBUFFER_ADDR  0xC0000000UL
#define NES_WORK_FRAME_ADDR   0xC0200000UL
#define NES_SDRAM_TEST_ADDR   0xC0300000UL
/* FT6x06 BSP already supplies landscape coordinates. */
#define NES_TOUCH_FLIP_X      0
#define NES_TOUCH_FLIP_Y      0
/* Audio: headphones; 0..100 codec volume. Test tone replaces game audio only. */
#define NES_AUDIO_ENABLE 1
#define NES_AUDIO_VOLUME 75
#define NES_AUDIO_TEST_TONE 0
#endif
