# stm32f469-discovery-nes
NES emulator for the STM32F469 Discovery board, featuring touchscreen controls and CS43L22 audio output. Built with STM32CubeIDE.

## STM32F469 Discovery NES Changelog

1. v0.1: No audio support.
2. v0.2: Added audio support (CS43L22 headphone output).

## Test Hardware

1. STM32F4 Discovery Board (MCU: STM32F469NIH6).

## Programming and Testing

1. Connect headphones or powered speakers to the board's 3.5 mm headphone output jack.
2. Use STM32CubeProgrammer to program `Debug/STM32F469_Disco_NES.hex`. After programming, press the Reset button on the board.
3. Press USER or the on-screen START button to start the game. Test the background music, jumping sounds, and coin collection sounds.
4. The default codec volume is 75/100. Adjust `NES_AUDIO_VOLUME` in `main/nes_config.h` to change it.

## Building the Project

1. Build the project using STM32CubeIDE (version 1.16.1 is used for this project).
2. After a successful build, a `Debug` directory is generated containing the following files:

   - `STM32F469_Disco_NES.bin`
   - `STM32F469_Disco_NES.elf`
   - `STM32F469_Disco_NES.hex`
   - `STM32F469_Disco_NES.map`

3. Use STM32CubeProgrammer to program `STM32F469_Disco_NES.hex` onto the development board for testing.

## How to Switch Test Games
1. Modify NES_GAME_INDEX in main/nes_config.h.
2. Set NES_GAME_INDEX to 0 for Super Mario.
3. Set NES_GAME_INDEX to 1 for yingzichuanshuo.

## Audio Processing and Synchronization

For each emulated frame: `InfoNES_pAPUVsync` generates five APU waveforms → `InfoNES_SoundOutput` → `NES_AudioOutput`.

- The sample rate is 44,100 Hz, with 735 samples per frame, corresponding to approximately 60 emulated frames per second.
- The initial audio implementation uses linear mixing, DC removal, and signed 16-bit PCM. The left and right channels carry identical audio; this is not native stereo.
- The main program writes to a two-block mono queue. DMA half-transfer and transfer-complete interrupts copy completed blocks into the buffer half that has finished playing.
- The circular DMA buffer has two halves. Each half contains 735 left/right sample pairs, representing approximately 16.67 ms of audio.
- Two frames are prepared before playback starts. When the queue is full, the main program waits for DMA to consume data, using the audio clock to pace the game.
- The queue and DMA buffer together can hold approximately four frames of audio (66.7 ms), introducing a corresponding audio delay.
- If data is unavailable, silence is inserted and the underrun counter is incremented. Repeated underruns indicate that audio generation and/or game rendering cannot keep up.
- The maximum wait for DMA is 100 ms. If audio fails, output is stopped and the game falls back to approximately 60 Hz pacing using the HAL tick.
- `FrameSkip = 3` is retained, so the screen is rendered once every four emulated frames. Audio is generated for every emulated frame.

## Configurable Settings

In `main/nes_config.h`:

| Setting | Default | Description |
|---|---|---|
| NES_AUDIO_ENABLE | 1 | Set to 0 to disable hardware audio while retaining approximately 60 Hz pacing. |
| NES_AUDIO_VOLUME | 75 | Codec volume, from 0 to 100. Rebuild after changing this value. |
| NES_AUDIO_TEST_TONE | 0 | Set to 1 to replace game audio with a 440 Hz square-wave test tone. The game display continues running. |
| NES_GAME_INDEX | 0 | 0: SuperMario; 1: yingzichuanshuo. |
| NES_DISPLAY_SCALE | 2 | Integer display scaling factor. Set to 1 to reduce the display workload for comparison. |
| NES_TOUCH_FLIP_X/Y | 0 | Reverse the horizontal/vertical touch coordinates. |

If there is no sound, start the game first (the title screen may be silent), then check the headphone connection and volume. To isolate the issue, build with `NES_AUDIO_TEST_TONE = 1`. If the test tone is audible, the basic codec/DMA path is working; then investigate game audio generation.

## Debug Variables

| Variable | Meaning |
|---|---|
| nes_audio_ready | 1: Audio initialization succeeded. |
| nes_audio_running | 1: DMA playback has started. |
| nes_audio_frames | Number of times NES has called the audio output function. |
| nes_audio_callbacks | Number of DMA half-transfer/transfer-complete callbacks. This should keep increasing during normal playback. |
| nes_audio_underruns | Number of buffer halves filled with silence because no completed audio data was available at the callback. |
| nes_audio_error | Error code listed below. 0 means no error has been recorded. |

| Error Code | Meaning |
|---|---|
| 1 | The sample rate or samples per frame does not match 44100/735. |
| 2 | Audio PLLI2S configuration failed. |
| 3 | BSP/codec initialization failed. |
| 4 | DMA playback failed to start. |
| 5 | SAI/DMA error callback occurred. |
| 6 | The queue was full, but DMA did not free any space within 100 ms. |

LED3 turns on after an audio failure. Check `nes_touch_ok` and `nes_audio_error` to distinguish the cause. The DMA ISR does not perform blocking I2C operations or call `HAL_Delay`.

## Retained Board Debug Information

- LED1: The LCD/SDRAM/touch initialization sequence has completed.
- LED2: Toggles after every 30 game frames displayed.
- LED4: Repeating blink counts indicate the following: 1 = HSE/PLL; 2 = system clock/OverDrive; 3 = LCD; 4 = SDRAM; 5 = ROM loading failed or NES returned.
- `nes_boot_stage`: 1 = before clock configuration; 2 = clock configuration complete; 3 = LCD initialization; 4 = SDRAM check; 5 = touch initialization; 6 = NES execution.
- `nes_error_code`: 0 = normal; 1–5 = as listed above; 10 = HardFault; 11 = MemManage; 12 = BusFault; 13 = UsageFault. Fault handlers halt execution for debugger inspection.

## Memory and Hardware

- Flash: 2 MiB; SRAM: 320 KiB; CCM heap: 64 KiB; stack: 24 KiB.
- The audio DMA buffer and queue are placed in the SRAM `.bss` section, not in CCM, which DMA cannot access.
- LCD framebuffer: `0xC0000000`; NES WorkFrame: `0xC0200000`; SDRAM test area: `0xC0300000`.
- SAI1 Block A: PG7 = MCLK, PE4 = FS, PE5 = SCK, PE6 = SD; PE2 = codec reset.
- DMA2 Stream3 Channel0 with half-transfer/transfer-complete interrupts; SAI1 error interrupt.
- Audio uses PLLI2S. The LCD retains its original PLLSAI configuration.
- CS43L22 uses slots 0/2 of ST's 64-bit SAI frame, carrying one 16-bit PCM sample for each of the left and right channels.

## Validation Scope

A clean build with Arm GNU Toolchain 12.3.Rel1 and `-O2` completed with 0 errors and 0 warnings.

Host tests verify the DMA startup transfer length, left/right channel data, DC decay, underrun recovery, pacing, counter wraparound, timeouts, and error paths. AddressSanitizer/UBSan were used; physical codec/SAI testing was not performed as part of this validation.

Test source files are in `tests/`. Build logs, memory addresses, and the HEX SHA256 checksum are in `Firmware/`.

The linker's 100% RAM/CCMRAM usage results from placing the stack at the top of RAM and reserving the heap. It does not mean that `.bss` has exhausted RAM. The linker includes an overlap check.

See `DEPENDENCIES.txt` for dependency versions and details of the audio BSP subset. License information is provided in `Licenses/` and the source file headers.
