
## Description

The objective of this demo is to control an LED based on the state of a push button. Instead of relying solely on high-level abstraction layers, this implementation manipulates the processor's registers directly to configure pin modes and handle I/O operations, providing a deeper understanding of the ARM Cortex-M3 architecture.

## Technical Specifications

### Hardware Configuration

| Component | Pin | Mode |
| :--- | :--- | :--- |
| LED | PA0 | General purpose output push-pull |
| Button | PA1 | Input with pull-up resistor |

### Implementation Details

* **Clock Management**: Uses `RCC_APB2PeriphClockCmd` from the Standard Peripheral Library to enable the clock for GPIO Port A.
* **Register Configuration**:
    * **CRL (Control Register Low)**: Manages configuration for pins PA0 through PA7. The code performs bitwise operations to set the mode and configuration bits.
    * **BSRR (Bit Set/Reset Register)**: Used for atomic bit manipulation to set or reset the output state of PA0.
    * **IDR (Input Data Register)**: Used to read the logical state of the input pin PA1.

