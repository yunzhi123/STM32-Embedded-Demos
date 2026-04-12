## Description
This project demonstrates bidirectional serial communication (USART) and peripheral control on the STM32F103 microcontroller using direct register access. The firmware implements full-duplex UART communication, allowing the device to transmit strings via the standard `printf` library and receive control commands to manipulate onboard hardware. By bypassing the Standard Peripheral Library and HAL, this demo showcases low-level hardware initialization and efficient polling-based I/O handling.

## Technical Specifications

### Hardware Configuration
| Peripheral | Pin | Mode | Function |
| :--- | :--- | :--- | :--- |
| **USART2 TX** | PA2 | Alternate Function Push-Pull | Serial data transmission |
| **USART2 RX** | PA3 | Floating Input | Serial data reception |
| **LED** | PA5 | General Purpose Output Push-Pull | Visual status indicator |
| **Button** | PC13 | Input with Pull-up | Hardware trigger for transmission |

### Implementation Details
The firmware is implemented using direct memory-mapped I/O operations to ensure minimal overhead and precise control over the hardware:

* **Clock and Peripheral Reset**: Direct manipulation of `RCC->APB1ENR` and `RCC->APB2ENR` to enable clocks for USART2, GPIOA, and GPIOC simultaneously.
* **GPIO Port Configuration**:
    * Uses the **CRL** (Control Register Low) to configure PA2 as an Alternate Function output (speed 10MHz) and PA3 as an input.
    * Uses the **CRH** (Control Register High) to configure PC13 as an input and initializes its state using the **BSRR** register to enable the internal pull-up resistor.
* **USART2 Initialization**:
    * **Baud Rate (BRR)**: Manually calculated and loaded into the `USART2->BRR` register.
    * **Control Registers**: Direct bit-setting in `CR1` to enable the Transmitter (TE), Receiver (RE), and the UART peripheral (UE).
* **Data Handling**:
    * **Transmission**: Redirection of `fputc` by polling the `TXE` (Transmit data register empty) flag in the `USART2->SR` register.
    * **Reception**: Implements a polling logic in the main loop that monitors the `RXNE` (Read data register not empty) flag. When data is received, it evaluates the byte to toggle PA5 (LED).
    * **Interrupt-free Polling**: The logic relies on checking the `IDR` (Input Data Register) for button presses and the `SR` register for incoming serial bytes, demonstrating a non-blocking style within a super-loop architecture.

---

### Direct Register Logic
The following code fragment illustrates the low-level approach used for command processing and I/O control:

```c
// Monitoring the Status Register (SR) for incoming data
if (USART2->SR & (1 << 5)) { // Check RXNE bit
    uint8_t byteRcvd = USART2->DR; // Read Data Register directly
    if (byteRcvd == '0') {
        GPIOA->BSRR = (1 << 5);  // Atomic bit set for LED
    } else if (byteRcvd == '1') {
        GPIOA->BRR = (1 << 5);   // Atomic bit reset for LED
    }
}

// Monitoring Input Data Register (IDR) for button state
if (!(GPIOC->IDR & (1 << 13))) { // Check if PC13 is Low
    speak(); // Trigger UART transmission
    Delay(1000);
}
```
### Demo Link
https://youtube.com/shorts/UeKZCP0hCGg?si=Xi7GdWp_JyPBGRij