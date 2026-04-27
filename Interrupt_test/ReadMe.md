## Description
This project demonstrates interrupt-driven serial communication on the STM32F103 microcontroller. By utilizing the Nested Vectored Interrupt Controller (NVIC) and USART2 peripheral, the firmware allows a PC to dynamically control the LED blinking frequency in real-time. Unlike polling-based methods, this implementation uses hardware interrupts to capture incoming data immediately, ensuring the main execution loop for LED toggling remains responsive to user input.

## Technical Specifications

### Hardware Configuration
| Peripheral | Pin | Mode | Function |
| :--- | :--- | :--- | :--- |
| **USART2 TX** | PA2 | Alternate Function Push-Pull | Serial data transmission |
| **USART2 RX** | PA3 | Input with Pull-up (IPU) | Serial data reception |
| **LED** | PA5 | General Purpose Output (2MHz) | Frequency-controlled visual output |

### Interrupt Configuration
| Component | Setting | Description |
| :--- | :--- | :--- |
| **Interrupt Source** | USART2_IRQn (Position 38) | Triggers on RXNE (Receive Data Register Not Empty) |
| **Priority** | High (0x00) | Configured via `NVIC->IP` |
| **Enable Register** | ISER[1] Bit 6 | Enabled via `NVIC->ISER` (Interrupt Set-Enable Register) |

### Implementation Details
The project focuses on low-level configuration of the Interrupt Controller and USART status management:

* **NVIC Manual Configuration**: 
    * The interrupt priority is set by directly writing to the `NVIC->IP` array.
    * The USART2 interrupt (IRQ 38) is enabled by calculating the correct bit position in the `NVIC->ISER` register array (`38 >> 5` for index and `1 << (38 & 0x1F)` for the bit).
* **USART2 Interrupt Setup**:
    * The `RXNEIE` (Receive data register not empty interrupt enable) bit in the `USART2->CR1` register is set to allow the hardware to trigger an interrupt request whenever a byte arrives.
* **Interrupt Service Routine (ISR)**:
    * The `USART2_IRQHandler` is implemented to handle the event. It performs a secondary safety check on the `RXNE` flag in the Status Register (`USART2->SR`).
    * Incoming characters ('0', '1', '2') update a `volatile` global variable `blinkInterval`, which dictates the timing of the main loop.
* **Register-Level I/O**:
    * Utilizes the **BSRR** (Bit Set Reset Register) for atomic LED toggling to prevent interference with interrupt-driven memory access.

---

### Register-Level NVIC and ISR Implementation
The following snippet highlights the manual interrupt enabling and the logic within the Interrupt Service Routine:

```c
void NVIC_init() {
    // Set priority for USART2 (IRQ 38, both 0 for preemptive priority and sub-priority)
    NVIC->IP[USART2_IRQn] = (uint8_t)(0x00 << 4);
    
    // Enable IRQ 38 in the Interrupt Set-Enable Register
    // Index: 38 / 32 = 1, Bit: 38 % 32 = 6
    NVIC->ISER[USART2_IRQn >> 5] |= (1 << 6);
}

void USART2_IRQHandler() {
    // Verify RXNE flag in Status Register
    if (USART2->SR & (1 << 5)) {
        uint8_t data = (uint8_t)(USART2->DR & 0xFF);
        // Map received characters to specific millisecond intervals
        if (data == '0')      blinkInterval = 1000;
        else if (data == '1') blinkInterval = 200;
        else if (data == '2') blinkInterval = 60;
    }
}
```
### Demo Link
https://youtube.com/shorts/izHsCs8ztqw?si=hTx6y6CIGiNmVY-A