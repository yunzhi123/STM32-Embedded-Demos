## Description
This project implements a Software-based I2C (bit-banging) protocol on the STM32F103 microcontroller to interface with an SSD1306 OLED display. Unlike using a dedicated hardware peripheral, this implementation manually toggles GPIO pins to recreate the I2C timing diagrams. This approach demonstrates a deep understanding of the I2C physical layer, including Start/Stop conditions, data bit transitions, and Acknowledge (ACK) bit handling, all achieved through direct register manipulation.

## Technical Specifications

### Hardware Configuration
| Peripheral | Pin | Mode | Function |
| :--- | :--- | :--- | :--- |
| **Software SCL** | PA0 | General Purpose Open-Drain | Serial Clock Line |
| **Software SDA** | PA1 | General Purpose Open-Drain | Serial Data Line |
| **OLED Display** | N/A | Slave (Address: 0x78) | Target device for command sequence |

### Implementation Details
The firmware simulates the I2C protocol by directly controlling the state of the processor's GPIO registers:

* **Register-Level GPIO Initialization**: Configures PA0 and PA1 using the `GPIOA->CRL` register. The pins are set to **Open-Drain Output** mode (`0x6` configuration bits) to properly support the I2C bus requirements where lines are pulled high by external resistors.
* **Atomic Bit Manipulation**: Utilizes the **BSRR** (Bit Set Reset Register) for high-speed toggling of the SCL and SDA lines. Macros are used to perform atomic writes to the upper 16 bits (Reset) or lower 16 bits (Set) of the register, ensuring precise control over the pin states.
* **Input Data Sampling**: Reads the **IDR** (Input Data Register) to sample the SDA line during the 9th clock pulse to detect the Acknowledge (ACK) signal from the OLED slave device.
* **Protocol Timing**: Implements a manual microsecond delay function (`delay_u`) to satisfy the minimum setup and hold times required by the I2C specification.
* **Bit-Banging Logic**:
    * **Start/Stop Conditions**: Manually sequences SDA and SCL transitions (e.g., pulling SDA low while SCL is high for a Start condition).
    * **Byte Transmission**: Iterates through 8 bits of data (MSB first), toggling the clock for every bit.
    * **Application Layer**: Wraps the bit-banged functions into an `OLED_SendCommands` routine that follows the SSD1306 communication protocol (Address -> Control Byte -> Data).

---

### Register-Based Bit-Banging Analysis
The following code snippet demonstrates the implementation of the I2C clock and data transition logic using direct register access:

```c
// Macro definitions for direct BSRR and IDR access
#define scl_w(v) ((v) ? (GPIOA->BSRR = (1<<0)) : (GPIOA->BSRR = (1<<16)))
#define sda_w(v) ((v) ? (GPIOA->BSRR = (1<<1)) : (GPIOA->BSRR = (1<<17)))
#define sda_r ((GPIOA->IDR & (1<<1)) ? 1 : 0)

uint8_t send_byte(uint8_t byte) {
    for (int i=7; i>=0; i--) {
        scl_w(0); // Pull SCL low via BSRR upper 16 bits
        if ((byte & (1<<i)) != 0) sda_w(1);
        else sda_w(0);
        delay_u(1);
        scl_w(1); // Pull SCL high via BSRR lower 16 bits
    }
    
    // Check for ACK/NACK bit
    scl_w(0);
    sda_w(1); // Release SDA for slave control
    delay_u(1);
    scl_w(1);
    return sda_r; // Read state from IDR
}
```