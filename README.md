# STM32F103 Register-Level Demos

This repository is a small collection of STM32F103 learning projects. Each folder is a separate Keil uVision 5 project focused on one basic peripheral or embedded concept.

The main goal is to learn the STM32F103 by reading the datasheet/reference manual and configuring peripheral registers by hand. Instead of relying on high-level HAL-style APIs, the demo code mostly works directly with memory-mapped registers such as `RCC`, `GPIO`, `USART`, `I2C`, `SPI`, `TIM`, `EXTI`, and `NVIC`.

## What Is Included

| Folder | Topic |
| --- | --- |
| `GPIO_LED_test` | Basic GPIO input/output with LED and button control |
| `UART_Tx_test` | USART transmit and `printf` redirection |
| `UART_Rx_test` | USART receive experiments |
| `Interrupt_test` | Interrupt-driven UART / LED behavior |
| `EXTI_test` | External interrupt configuration with buttons |
| `I2C_test` | Hardware I2C communication, including OLED usage |
| `SI2C_test` | Software I2C / bit-banged I2C for OLED output |
| `SPI_test` | Basic SPI1 register configuration |
| `Timer_test` | TIM3-based millisecond tick and delay |
| `BreathLED_test` | PWM breathing LED using TIM1 |

Some folders also contain a short local `ReadMe.md` with notes, pin assignments, or demo images/videos for that specific experiment.

## Development Environment

- MCU: STM32F103 series, mainly STM32F103R8
- IDE/build tool: Keil uVision 5
- Language: C
- Style: register-level peripheral configuration based on the STM32F103 documentation

## How To Build

1. Open the desired `.uvprojx` file in Keil uVision 5.
2. Select the target/project configuration.
3. Build the project in Keil.
4. Flash the generated firmware to the STM32F103 board with your usual programmer/debugger setup.

## Notes

This repo is intended for study and experimentation rather than as a polished driver library. The code is organized as independent demos, so repeated startup files, CMSIS/StdPeriph files, build outputs, and local helper libraries may appear in multiple folders.

The interesting part is the peripheral initialization and control logic: clock setup, GPIO modes, alternate functions, interrupt routing, timer configuration, serial buses, and status-flag polling are all written close to the hardware so the behavior can be traced back to the STM32F103 registers.
