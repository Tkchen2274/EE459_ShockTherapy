#include "rfid.h"
#include <avr/io.h>
#include <util/delay.h>

void spi_init() {
  // Set MOSI, SCK, and SS pins as output
  SPI_DDR |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);

  // Set MISO pin as input
  SPI_DDR &= ~(1 << MISO_PIN);

  // Enable SPI, Set as Master, Set clock rate fck/16
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

  mfrc522_write_register(REG_TX_MODE, 0x00);
  mfrc522_write_register(REG_RX_MODE, 0x00);
  mfrc522_write_register(REG_MOD_WIDTH, 0x26);
  mfrc522_write_register(REG_T_MODE, 0x80);
  mfrc522_write_register(REG_T_PRESCALAR, 0xA9);
  mfrc522_write_register(REG_T_RELOAD_H, 0x00);
  mfrc522_write_register(REG_T_RELOAD_L, 0xE8);
  mfrc522_write_register(REG_TXASK, 0x40);
  mfrc522_write_register(REG_MODE, 0x3D);

  // Turn antenna on
  mfrc522_write_register(REG_TX_CONTROL,
                         mfrc522_read_register(REG_TX_CONTROL) | 0x03);
}

uint8_t spi_transfer(uint8_t data) {
  // Start transmission
  SPDR = data;

  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)))
    ;

  // Return received data
  return SPDR;
}

// Function that writes only 1
void mfrc522_write_register(uint8_t reg, uint8_t value) {
  SPI_PORT &= ~(1 << SS_PIN); // Select MFRC522

  spi_transfer(((reg << 1) & 0x7E) |
               0x80);  // Send register address with read command
  spi_transfer(value); // Write single byte to register

  SPI_PORT |= (1 << SS_PIN); // Deselect MFRC522
}

// function that writes multiple bytes
void mfrc522_write_registers(uint8_t reg, uint8_t *values, uint8_t num_bytes) {
  SPI_PORT &= ~(1 << SS_PIN); // Select MFRC522

  spi_transfer(((reg << 1) & 0x7E) |
               0x80); // Send register address with read command
  for (uint8_t i = 0; i < num_bytes; i++) {
    spi_transfer(values[i]); // Write data to register
  }

  SPI_PORT |= (1 << SS_PIN); // Deselect MFRC522
}

uint8_t mfrc522_read_register(uint8_t reg) {
  SPI_PORT &= ~(1 << SS_PIN); // Select MFRC522

  spi_transfer(((reg << 1) & 0x7E) |
               0x80);                 // Send register address with read command
  uint8_t value = spi_transfer(0x00); // Receive data

  SPI_PORT |= (1 << SS_PIN); // Deselect MFRC522

  return value;
}

void mfrc522_read_registers(uint8_t reg, uint8_t *buffer, uint8_t num_bytes) {
  SPI_PORT &= ~(1 << SS_PIN); // Select MFRC522

  spi_transfer(((reg << 1) & 0x7E) |
               0x80); // Send register address with read command
  for (uint8_t i = 0; i < num_bytes; i++) {
    buffer[i] = spi_transfer(0x00); // Read data into buffer
  }

  SPI_PORT |= (1 << SS_PIN); // Deselect MFRC522
}

void mfrc522_send_command(uint8_t cmd) {
  mfrc522_write_register(0x01,
                         (mfrc522_read_register(0x01) & 0xF0) |
                             (cmd & 0x0F)); // Write to CommandReg register
}

void mfrc522_soft_reset() {
  mfrc522_send_command(CMD_SOFTRESET); // Send soft reset command
  _delay_ms(50);                       // Wait for reset to complete
}

void mfrc522_configure_agc(uint8_t enable) {
  // Read current RF configuration register value
  uint8_t rf_value = mfrc522_read_register(REG_RFCFG);

  // Clear AGC bit (bit 5) in the RF configuration register
  rf_value &= ~(1 << 5);

  // Set AGC enable/disable based on input parameter
  if (enable) {
    rf_value |= (1 << 5); // Enable AGC
  }

  // Write the updated value back to the RF configuration register
  mfrc522_write_register(REG_RFCFG, rf_value);
}

void mfrc522_configure_tx_ask(uint8_t enable) {
  // Read current TXASK register value
  uint8_t txask_value = mfrc522_read_register(REG_TXASK);

  // Clear bit 6 (TXASK) in the TXASK register
  txask_value &= ~(1 << 6);

  // Set TXASK enable/disable based on input parameter
  if (enable) {
    txask_value |= (1 << 6); // Enable TXASK
  }

  // Write the updated value back to the TXASK register
  mfrc522_write_register(REG_TXASK, txask_value);
}

void mfrc522_configure_sensitivity(uint8_t level) {
  // Read current mode register value
  uint8_t mode_value = mfrc522_read_register(REG_MODE);

  // Clear bits 6-5 (RxGain) in the mode register
  mode_value &= ~(0x60);

  // Set new sensitivity level (0 to 3) in bits 6-5 (RxGain) of mode register
  mode_value |= ((level << 5) & 0x60);

  // Write the updated value back to the mode register
  mfrc522_write_register(REG_MODE, mode_value);
}

void mfrc522_read_uid(uint8_t *uid_buffer) {
  // Activate the RFID reader
  mfrc522_send_command(CMD_TRANSCEIVE);

  // Read tag value (assuming a 4-byte UID for demonstration)
  uint8_t tag_uid[4];
  mfrc522_send_command(CMD_MEM);        // Store 25 bytes into internal buffer
  _delay_ms(1);                         // Wait for stabilization
  mfrc522_read_register(REG_FIFO_DATA); // Dummy read to clear FIFO buffer
  mfrc522_send_command(CMD_GEN_RANDOM_ID); // Generate random 10-byte ID
  for (uint8_t i = 0; i < 4; i++) {
    tag_uid[i] = mfrc522_read_register(REG_FIFO_DATA); // Read UID bytes
  }

  // Write tag UID to FIFO buffer
  mfrc522_send_command(CMD_MEM); // Store 25 bytes into internal buffer
  for (uint8_t i = 0; i < 4; i++) {
    mfrc522_write_register(REG_FIFO_DATA,
                           tag_uid[i]); // Write UID bytes to FIFO
  }

  // Read from FIFO buffer
  mfrc522_send_command(CMD_MEM); // Store 25 bytes into internal buffer
  _delay_ms(1);                  // Wait for stabilization
  uint8_t fifo_data[25];
  for (uint8_t i = 0; i < 25; i++) {
    fifo_data[i] = mfrc522_read_register(REG_FIFO_DATA); // Read FIFO data
  }
  // Deactivate the RFID reader and put MFRC522 in idle mode
  mfrc522_send_command(CMD_IDLE);
}

uint8_t mfrc522_check_card_present() {
  uint8_t status;
  mfrc522_send_command(CMD_IDLE); // Put MFRC522 into idle state
  mfrc522_send_command(
      CMD_RECEIVE); // Activate the transmitter to start scanning
  _delay_ms(1);     // Wait for scanning to stabilize
  status = mfrc522_read_register(REG_STATUS1); // Read Status1 register
  return (status & (1 << 5))
             ? 1
             : 0; // Check CRCReady bit (bit 5) for card presence
}

void mfrc522_reset() {
  mfrc522_send_command(CMD_SOFTRESET);
  _delay_ms(200);
}

void mfrc522_test() {
  mfrc522_reset();
  uint8_t ZEROES[25] = {0x00};
  mfrc522_write_register(REG_FIFO_LEVEL, 0x80);       // flush FIFO
  mfrc522_write_registers(REG_FIFO_DATA, ZEROES, 25); // write 25 bytes of 0s
  mfrc522_send_command(CMD_MEM);                      // transfer to FIFO

  // enable test
  mfrc522_write_register(REG_AUTO_TEST, 0x09);

  mfrc522_write_register(REG_FIFO_DATA, 0x00);

  // start self test
  mfrc522_send_command(CMD_CALC_CRC);
  _delay_ms(100);
  uint8_t result[64];
  mfrc522_read_registers(REG_FIFO_DATA, result, 64);

  mfrc522_send_command(CMD_IDLE);
}
