#ifndef AUDIO_STUB_H
#define AUDIO_STUB_H
#include <stdint.h>
typedef struct { int dummy; } SAI_HandleTypeDef;
typedef struct { uint32_t PeriphClockSelection; struct { uint32_t PLLI2SN, PLLI2SQ; } PLLI2S; uint32_t PLLI2SDivQ; } RCC_PeriphCLKInitTypeDef;
#define __DMB() __asm__ volatile("" ::: "memory")
#define CODEC_PDWN_SW 1
#define SAI1_IRQn 1
#define LED3 3
#define RCC_PERIPHCLK_SAI_PLLI2S 1
#define HAL_OK 0
#define AUDIO_OK 0
#define OUTPUT_DEVICE_HEADPHONE 2
#define CODEC_AUDIOFRAME_SLOT_02 5
#define AUDIO_OUT_IRQ_PREPRIO 5
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t n);
void HAL_NVIC_DisableIRQ(int x);
void HAL_NVIC_EnableIRQ(int x);
void HAL_NVIC_SetPriority(int x,int y,int z);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef *c);
int HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef *c);
void BSP_LED_On(int x);
int BSP_AUDIO_OUT_Stop(int x);
int BSP_AUDIO_OUT_Init(int d,int v,int r);
void BSP_AUDIO_OUT_SetAudioFrameSlot(int x);
int BSP_AUDIO_OUT_Play(uint16_t *p,uint32_t n);
#endif
