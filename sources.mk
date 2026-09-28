C_SOURCES := \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dsi.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
  Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c \
  Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.c \
  Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_sdram.c \
  Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_lcd.c \
  Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_ts.c \
  Drivers/BSP/Components/otm8009a/otm8009a.c \
  Drivers/BSP/Components/nt35510/nt35510.c \
  Drivers/BSP/Components/ft6x06/ft6x06.c \
  nes/src/InfoNES.c \
  nes/src/InfoNES_Mapper.c \
  nes/src/InfoNES_pAPU.c \
  nes/src/K6502.c \
  nes/port/nes_port.c \
  nes/games/nes_game.c \
  main/main.c \
  main/nes_board.c \
  main/stm32f4xx_it.c \
  GCC/system_stm32f4xx.c \
  GCC/sysmem.c
ASM_SOURCES := GCC/startup_stm32f469xx.s GCC/game_resources.S
INCLUDES := -Imain -ICMSIS/Include -ICMSIS/Device/ST/STM32F4xx/Include -IDrivers/STM32F4xx_HAL_Driver/Inc -IDrivers/BSP/STM32469I-Discovery -IUtilities/Fonts -Ines/src -Ines/games -Ines/port

C_SOURCES += Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.c \
 Drivers/BSP/Components/cs43l22/cs43l22.c \
 Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai.c \
 Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai_ex.c \
 main/nes_audio.c
