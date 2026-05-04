## Description
This project demonstrates the implementation of External Interrupts (EXTI) on the STM32F103 microcontroller using direct register access. The firmware configures two physical buttons as interrupt sources to control the state of an onboard LED. By bypassing high-level libraries, the code illustrates the complete interrupt signal path, starting from the GPIO hardware through the Alternate Function I/O (AFIO) mapper and the EXTI controller, finally reaching the Nested Vectored Interrupt Controller (NVIC).

## Technical Specifications

### Hardware Configuration
| Peripheral | Pin | Mode | Function |
| :--- | :--- | :--- | :--- |
| **LED** | PA5 | General Purpose Output (2MHz) | Status indicator |
| **Button 1** | PA6 | Input with Pull-up (IPU) | Triggers Interrupt (LED ON) |
| **Button 2** | PA7 | Input with Pull-up (IPU) | Triggers Interrupt (LED OFF) |

### Interrupt Architecture
| Component | Configuration | Description |
| :--- | :--- | :--- |
| **EXTI Line** | Line 6 and Line 7 | Mapped to Port A via AFIO |
| **Trigger** | Rising Edge | Interrupt fires when button signal rises |
| **IRQ Channel** | EXTI9_5_IRQn | Shared vector for EXTI lines 5 through 9 |
| **Priority** | 0x00 (Highest) | Configured in NVIC priority register |

### Implementation Details
The project utilizes precise bitwise operations to configure the hardware interrupt pipeline:

* **Clock and AFIO Routing**:
    * Enables the AFIO clock via `RCC->APB2ENR` to allow for interrupt line mapping.
    * Configures the `AFIO->EXTICR[1]` register to route GPIOA pins 6 and 7 to EXTI lines 6 and 7 respectively (setting bits to `0000` for Port A).
* **EXTI Controller Setup**:
    * **IMR (Interrupt Mask Register)**: Unmasks lines 6 and 7 to allow interrupt requests to reach the processor.
    * **RTSR (Rising Trigger Selection Register)**: Configures the hardware to detect rising edges on the specified lines.
* **NVIC Integration**:
    * Enables the shared `EXTI9_5_IRQn` interrupt vector by modifying the `NVIC->ISER` (Interrupt Set-Enable Register).
    * Sets the execution priority to the highest level through the `NVIC->IP` register array.
* **Interrupt Service Routine (ISR)**:
    * Implements `EXTI9_5_IRQHandler` to handle events from both buttons.
    * Uses the **PR (Pending Register)** to identify which specific pin triggered the interrupt.
    * Clears the interrupt flag by writing a '1' to the corresponding bit in `EXTI->PR` (RC_W1 logic) and updates the LED state using the `BSRR` register.

---

### Register-Level Interrupt Logic Snippet
The following snippet highlights the manual routing and handling of the external interrupt signals:
```c
void button_init() {
    // Map EXTI6 and EXTI7 to GPIO Port A
    AFIO->EXTICR[1] &= ~(0xF << 8);  // EXTI6 = PA6
    AFIO->EXTICR[1] &= ~(0xF << 12); // EXTI7 = PA7
    
    // Enable Interrupt Mask and Rising Edge Trigger
    EXTI->IMR |= (1 << 6) | (1 << 7);
    EXTI->RTSR |= (1 << 6) | (1 << 7);
}

void EXTI9_5_IRQHandler() {
    // Identify trigger source and clear pending bit
    if (EXTI->PR & (1 << 6)) {
        GPIOA->BSRR = (1 << 5);  // Set PA5 High
        EXTI->PR = (1 << 6);     // Clear pending flag
    } 
    if (EXTI->PR & (1 << 7)) {
        GPIOA->BSRR = (1 << 21); // Set PA5 Low (Reset)
        EXTI->PR = (1 << 7);     // Clear pending flag
    }
}
```
### Demo Video Link
https://youtube.com/shorts/5GF7uIScRXQ?feature=share