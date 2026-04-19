## Description
This project demonstrates the implementation of the I2C (Inter-Integrated Circuit) communication protocol on the STM32F103 microcontroller through direct register manipulation. The firmware configures the I2C1 peripheral in Master Mode to interface with an SSD1306 OLED display. Key features include hardware pin remapping, Fast Mode clock configuration, and manual handling of the I2C state machine (Start, Address, Data, and Stop sequences) without the use of high-level libraries.

## Technical Specifications

### Hardware Configuration
| Peripheral | Pin | Mode | Function |
| :--- | :--- | :--- | :--- |
| **I2C1 SCL** | PB8 | Alternate Function Open Drain | Serial Clock (Remapped) |
| **I2C1 SDA** | PB9 | Alternate Function Open Drain | Serial Data (Remapped) |
| **Board LED** | PB10 | General Purpose Push-Pull | Status indicator based on I2C read |
| **OLED Display** | N/A | Slave (Address: 0x78) | Target device for data transfer |

### Implementation Details
The implementation emphasizes low-level hardware control and timing accuracy by accessing the MCU's internal registers directly:

* **Peripheral Remapping**: Utilizes the **AFIO->MAPR** register to remap I2C1 signals from their default pins to PB8 and PB9. This involves enabling the AFIO clock and setting the `I2C1_REMAP` bit.
* **Clock and Reset Control**:
    * Directly enables clocks for AFIO, GPIOB, and I2C1 via **RCC->APB1ENR** and **RCC->APB2ENR**.
    * Performs a hardware reset of the I2C1 peripheral using **RCC->APB1RSTR** to ensure a clean initial state.
* **I2C Protocol Engine Configuration**:
    * **CR2 Register**: Sets the peripheral clock frequency to 36 MHz.
    * **CCR Register**: Configures the bus for Fast Mode (400 kHz) by setting the `FS` bit and calculating the appropriate clock control value.
    * **TRISE Register**: Manually defines the maximum allowed rise time for the SCL/SDA signals to comply with I2C electrical specifications.
* **Master Communication Logic**:
    * **Transmit (I2C_Send)**: Manually generates the Start bit, waits for the `SB` (Start Bit) flag, transmits the 7-bit address, and monitors the `ADDR` and `TXE` flags in **SR1** for data flow control.
    * **Receive (I2C_Receive)**: Handles the reception of data by managing `ACK` generation and `STOP` bit timing, specifically ensuring the NACK is sent before the final byte to signal the end of the transfer.
    * **Error Handling**: Implements polling for the `AF` (Acknowledge Failure) bit in the Status Register to detect and recover from communication errors with the slave device.

---

### Register-Level I2C Transaction Snippet
The following code highlights the manual flag-checking and data register operations required for a master transmitter:

```c
// Direct Register check for Start Bit (SB) and Address (ADDR) flags
I2C1->CR1 |= (1 << 8);           // Generate START
while (!(I2C1->SR1 & (1 << 0))); // Wait for SB flag

I2C1->DR = (Addr & 0xFE);        // Load Address + Write bit into Data Register

// Wait for Address Sent (ADDR) or Acknowledge Failure (AF)
while (!(I2C1->SR1 & (1 << 1))) {
    if (I2C1->SR1 & (1 << 10)) { // Check for Ack Failure
        I2C1->CR1 |= (1 << 9);   // Generate STOP
        return -1;
    }
}
```

### Demo Image
![alt text](image-1.png)