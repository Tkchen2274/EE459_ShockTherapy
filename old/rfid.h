// Define SPI Pins
#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SS_PIN PB2
#define MOSI_PIN PB3
#define MISO_PIN PB4
#define SCK_PIN PB5

// MFRC522 Commands
#define CMD_IDLE	0x00
#define CMD_MEM		0x01
#define CMD_GEN_RANDOM_ID 0x02
#define CMD_CALC_CRC	0x03
#define CMD_TRANSMIT	0x04
#define CMD_NOCMDCHANGE	0x07
#define CMD_RECEIVE	0x08
#define CMD_TRANSCEIVE	0x0C
#define CMD_MFAUTHENT	0x0E
#define CMD_SOFTRESET	0x0F

// MFRC522 Registers
#define REG_ERROR	0x06
#define REG_BIT_MASK    0x07
#define REG_STATUS1     0x07
#define REG_COLLISION   0x0E
#define REG_FIFO_DATA   0x09
#define REG_FIFO_LEVEL	0x0A
#define REG_RFCFG       0x26   // RF configuration register address
#define REG_MODE        0x11   // Mode register address
#define REG_TX_MODE	0x12	
#define REG_RX_MODE	0x13
#define REG_TX_CONTROL	0x14
#define REG_TXASK       0x15   // TXASK register address

#define REG_MOD_WIDTH	0x24
#define REG_T_MODE	0x2A
#define REG_T_PRESCALAR	0x2B
#define REG_T_RELOAD_H	0x2C
#define REG_T_RELOAD_L	0x2D
#define REG_AUTO_TEST	0x36


#define MAX_UID_LENGTH 10  // Maximum length of UID

// Function prototypes
void spi_init();
uint8_t spi_transfer(uint8_t data);
void mfrc522_write_register(uint8_t reg, uint8_t value);
uint8_t mfrc522_read_register(uint8_t reg);
void mfrc522_send_command(uint8_t cmd);
void mfrc522_soft_reset();
void mfrc522_configure_antenna_gain(uint8_t gain);
void mfrc522_configure_agc(uint8_t enable);
void mfrc522_configure_tx_ask(uint8_t enable);
void mfrc522_configure_sensitivity(uint8_t level);
uint8_t mfrc522_check_card_present();
void mfrc522_test();
