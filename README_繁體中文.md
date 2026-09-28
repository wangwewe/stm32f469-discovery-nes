# STM32F469 Discovery NES LOG
1. v0.1 沒有音效
2. V0.2 加入音效 (支援 CS43L22 耳機輸出)

## 測試硬體

1. STM32F4 Discovery Board (MCU: STM32F469NIH6)。

## 燒錄測試

1. 接上開發板的 3.5 mm 耳機輸出孔，使用耳機或有源喇叭。
2. 使用STM32CubeProgrammer燒錄 `Debug/STM32F469_Disco_NES.hex`，燒錄完畢後按下開發版上的Reset按鈕。
3. 按 USER 或觸控 START 開始，測試背景音樂、跳躍、吃金幣音效。
4. 預設 codec 音量 75/100。可在 `main/nes_config.h` 修改 `NES_AUDIO_VOLUME`。

## 如何編譯專案

1. 使用STM32CubeIDE (本專案使用v1.16.1)編譯.
2. 編譯成功後, 會生成一個Debug目錄, 底下會有以下的檔案:
    a. STM32F469_Disco_NES.bin
    b. STM32F469_Disco_NES.elf
    c. STM32F469_Disco_NES.hex
    d. STM32F469_Disco_NES.map
3. 可以使用使用STM32CubeProgrammer燒錄STM32F469_Disco_NES.hex到開發版進行測試。

## 如何切換測試遊戲
1. 在main/nes_config.h修改NES_GAME_INDEX參數。
2. NES_GAME_INDEX => 0, Super Mario（超級瑪利歐)。
3. NES_GAME_INDEX => 1, yingzichuanshuo（影子傳說)。

## 音訊流程與同步

每個模擬影格：InfoNES_pAPUVsync 產生五路 APU 波形 → InfoNES_SoundOutput → NES_AudioOutput。
- 44,100 Hz；每影格 735 個取樣，約 60 個模擬影格/秒。
- 首版使用線性混音、DC 去除、16-bit 有號 PCM；左右聲道相同，不是原生立體聲。
- 主程式寫入兩格 mono queue；DMA half/full IRQ 把完成的區塊複製到已播放完的半區。
- DMA circular buffer 共兩個半區，每半區 735 組左右取樣，約 16.67 ms。
- 開始播放前先準備兩個影格；queue 填滿時主程式等待 DMA 消耗，以音訊時鐘控制遊戲節奏。
- queue 與 DMA 合計最多約四個影格的儲存量（66.7 ms），會有相應聲音延遲。
- 資料不足時補靜音並累加 underruns；若持續發生，代表產生聲音/遊戲畫面的速度跟不上。
- 等候 DMA 最長 100 ms；音訊故障後停止輸出，遊戲改用 HAL tick 約 60 Hz 節拍。
- 保留 FrameSkip=3，因此每4個模擬影格畫一次螢幕；音訊每個模擬影格都有產生。

## 可調設定

在 `main/nes_config.h`：

| 設定 | 預設 | 用途 |
|---|---|---|
| NES_AUDIO_ENABLE | 1 | 改為 0 可停用硬體音訊，保留約 60 Hz 節拍 |
| NES_AUDIO_VOLUME | 75 | 0–100 codec 音量；更改後重新編譯 |
| NES_AUDIO_TEST_TONE | 0 | 改為 1，將遊戲聲音替換為 440 Hz 測試方波；遊戲畫面仍執行 |
| NES_GAME_INDEX | 0 | 0 SuperMario；1 yingzichuanshuo |
| NES_DISPLAY_SCALE | 2 | 畫面整數放大倍率；改 1 可降低顯示負擔作比較 |
| NES_TOUCH_FLIP_X/Y | 0 | 觸控左右/上下反轉 |

若沒有聲音，先啟動遊戲（標題畫面不一定有聲音），再檢查耳機接孔與音量。需要隔離問題時可編譯 NES_AUDIO_TEST_TONE=1：能聽到測試音代表基本 codec/DMA 路徑可工作，再回頭檢查遊戲音訊。

## 除錯變數

| 變數 | 意義 |
|---|---|
| nes_audio_ready | 1：音訊初始化成功 |
| nes_audio_running | 1：已啟動 DMA 播放 |
| nes_audio_frames | NES 呼叫聲音輸出的次數 |
| nes_audio_callbacks | DMA half/full 回呼次數，正常會持續增加 |
| nes_audio_underruns | 回呼時無完成資料可取用，補靜音的半區次數 |
| nes_audio_error | 下表錯誤碼；0 表示尚無已記錄錯誤 |

| 錯誤碼 | 意義 |
|---|---|
| 1 | 取樣率或每影格取樣數不符 44100/735 |
| 2 | 音訊 PLLI2S 設定失敗 |
| 3 | BSP/codec 初始化失敗 |
| 4 | 啟動 DMA 播放失敗 |
| 5 | SAI/DMA 錯誤回呼 |
| 6 | queue 已滿，但 100 ms 內 DMA 沒有釋出空間 

音訊失敗後 LED3 會亮；需搭配 nes_touch_ok 與 nes_audio_error 分辨。DMA ISR 不執行阻塞式 I2C 或 HAL_Delay。

## 保留的板子除錯資訊

LED1：LCD/SDRAM/觸控初始化流程已結束。LED2：每輸出 30 張遊戲画面翻轉一次。
LED4 重複閃爍：1=HSE/PLL、2=系統時鐘/OverDrive、3=LCD、4=SDRAM、5=ROM 載入失敗或 NES 返回。
`nes_boot_stage`：1 時鐘前、2 時鐘完成、3 LCD、4 SDRAM 檢查、5 觸控、6 NES。
`nes_error_code`：0 正常；1–5 同上；10 HardFault、11 MemManage、12 BusFault、13 UsageFault（Fault 停住供 debugger 查看）。

## 記憶體與硬體

- Flash 2 MiB，SRAM 320 KiB，CCM heap 64 KiB，stack 24 KiB。
- 音訊 DMA buffer 與 queue 放在 SRAM 的 .bss，沒有放在 DMA 無法存取的 CCM。
- LCD framebuffer：0xC0000000；NES WorkFrame：0xC0200000；SDRAM 測試：0xC0300000。
- SAI1 Block A：PG7 MCLK、PE4 FS、PE5 SCK、PE6 SD；PE2 codec reset。
- DMA2 Stream3 Channel0，half/full interrupt；SAI1 error interrupt。
- 音訊使用 PLLI2S，LCD 保留原本 PLLSAI 設定。
- CS43L22 使用 ST 的 64-bit SAI frame 中 slots 0/2，對應左右各一個 16-bit PCM。

## 驗證範圍

Arm GNU Toolchain 12.3.Rel1、-O2 乾淨編譯，0 errors、0 warnings。
主機測試驗證 DMA 啟動資料長度、左右聲道、DC 衰減、缺資料恢復、節奏控制、計數器回繞、逾時與錯誤路徑。使用 AddressSanitizer/UBSan；沒有實體 codec/SAI 測試。
測試來源在 tests/，建置紀錄、記憶體位置和 HEX SHA256 在 Firmware/。
Linker 的 RAM/CCMRAM 100% 是固定放置頂端 stack 與預留 heap 的結果，不表示 .bss 用完 RAM；linker 有重疊檢查。
相依版本與音訊 BSP 裁切說明見 DEPENDENCIES.txt，授權见 Licenses/ 和各檔頭。