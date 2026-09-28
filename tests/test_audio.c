/* Host behavioral tests: no physical SAI/codec validation. */
#include <assert.h>
#include <stdio.h>
#include "../main/nes_audio.c"
SAI_HandleTypeDef haudio_out_sai;
static uint32_t ticks, next_irq;
static int auto_irq, half, init_error, play_error, clock_error;
uint32_t HAL_GetTick(void) { return ticks; }
void HAL_Delay(uint32_t n) {
    while (n--) {
        ++ticks;
        if (auto_irq && nes_audio_running && ticks >= next_irq) {
            if (half++ & 1) BSP_AUDIO_OUT_TransferComplete_CallBack();
            else BSP_AUDIO_OUT_HalfTransfer_CallBack();
            next_irq = ticks + 17;
        }
    }
}
void HAL_NVIC_DisableIRQ(int x) { (void)x; }
void HAL_NVIC_EnableIRQ(int x) { (void)x; }
void HAL_NVIC_SetPriority(int x,int y,int z) { (void)x;(void)y;(void)z; }
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef *c) { memset(c,0,sizeof *c); }
int HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef *c) {
    assert(c->PLLI2S.PLLI2SN == 429 && c->PLLI2SDivQ == 19);
    return clock_error;
}
void BSP_LED_On(int x) { assert(x==LED3); }
int BSP_AUDIO_OUT_Stop(int x) { (void)x; return 0; }
int BSP_AUDIO_OUT_Init(int d,int v,int r) {
    assert(d==OUTPUT_DEVICE_HEADPHONE && v==NES_AUDIO_VOLUME && r==44100);
    BSP_AUDIO_OUT_ClockConfig(&haudio_out_sai,r,0); return init_error;
}
void BSP_AUDIO_OUT_SetAudioFrameSlot(int x) { assert(x==5); }
int BSP_AUDIO_OUT_Play(uint16_t *p,uint32_t n) {
    assert(p==(uint16_t *)dma_pcm && n==5880); next_irq=ticks+17; return play_error;
}
static uint8_t wave[SAMPLES];
static void push(void) { NES_AudioOutput(SAMPLES,wave,wave,wave,wave,wave); }
int main(void) {
    NES_AudioReset(); assert(NES_AudioOpen(735,44100));
    memset(wave,255,sizeof wave);
    push(); assert(!nes_audio_running);
    push(); assert(nes_audio_running && produced==consumed);
    assert(dma_pcm[0][0]==30600);
    for (int h=0;h<2;++h) for (unsigned i=0;i<SAMPLES;++i)
        assert(dma_pcm[h][2*i]==dma_pcm[h][2*i+1]);
    assert(dma_pcm[1][1468] < 200); /* constant DC decays */
    BSP_AUDIO_OUT_HalfTransfer_CallBack();
    assert(nes_audio_underruns==1);
    for(unsigned i=0;i<SAMPLES*2;++i) assert(dma_pcm[0][i]==0);
    push(); BSP_AUDIO_OUT_TransferComplete_CallBack();
    assert(nes_audio_underruns==1); /* recovery consumes real block */
    auto_irq=1;
    for(int i=0;i<120;++i) push();
    assert(!nes_audio_error && ticks > 1500 && produced-consumed <= QUEUE_BLOCKS);
    /* Counter wrap must retain FIFO slot order. */
    produced=consumed=UINT32_MAX;
    push(); assert(produced==0);
    BSP_AUDIO_OUT_HalfTransfer_CallBack(); assert(consumed==0);
    /* No callbacks: bounded timeout, no permanent game hang. */
    auto_irq=0; push(); push(); push(); assert(nes_audio_error==6 && !nes_audio_running);
    uint32_t t=ticks; push(); assert(ticks>t);
    NES_AudioReset(); init_error=1; assert(!NES_AudioOpen(735,44100) && nes_audio_error==3);
    init_error=0; NES_AudioReset(); clock_error=1;
    assert(!NES_AudioOpen(735,44100) && nes_audio_error==2);
    clock_error=0; NES_AudioReset(); assert(NES_AudioOpen(735,44100));
    play_error=1; push(); push(); assert(nes_audio_error==4);
    play_error=0; NES_AudioReset(); assert(NES_AudioOpen(735,44100)); push(); push();
    BSP_AUDIO_OUT_Error_CallBack(); push(); assert(nes_audio_error==5 && !codec_initialized);
    NES_AudioReset(); assert(!NES_AudioOpen(367,22050) && nes_audio_error==1);
    puts("PASS: DMA startup/bytes/stereo, DC decay, underrun/recovery, pacing, counter wrap, timeout, init/clock/play/IRQ failures");
}
