#include <stdint.h>

#include "driverlib.h"

#include "clock.h"
#include "spi.h"

#include "cypress_fram.h"

/**
 * Initialize the external FRAM interface.
 */
void EXTFRAM_init(void) {
	// configure SPI as master
	EUSCI_A_SPI_initMasterParam spi_init_struct = {
		.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK,
		.clockSourceFrequency = SMCLK_Freq,
//		.desiredSpiClock = FRAM_SPI_FREQUENCY,
		.desiredSpiClock = SMCLK_Freq,
		.msbFirst = EUSCI_A_SPI_MSB_FIRST,
//		.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
		.clockPhase = EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
		.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
		.spiMode = EUSCI_A_SPI_3PIN,
	};

	EUSCI_A_SPI_initMaster(FRAM_SPI_PERIPHERAL, &spi_init_struct);

	// enable SPI
	EUSCI_A_SPI_enable(FRAM_SPI_PERIPHERAL);
}

/**
 * Initialize GPIOs used for FRAM interface.
 */
void EXTFRAM_initGPIO(void) {
	// configure SPI pins
	GPIO_setAsPeripheralModuleFunctionOutputPin(FRAM_PORT_CLK, FRAM_PIN_CLK,
			GPIO_SECONDARY_MODULE_FUNCTION);
	GPIO_setAsPeripheralModuleFunctionOutputPin(FRAM_PORT_MOSI, FRAM_PIN_MOSI,
			GPIO_SECONDARY_MODULE_FUNCTION);
	GPIO_setAsPeripheralModuleFunctionInputPin(FRAM_PORT_MISO, FRAM_PIN_MISO,
			GPIO_SECONDARY_MODULE_FUNCTION);

	GPIO_setAsOutputPin(FRAM_PORT_CS, FRAM_PIN_CS);
	GPIO_setOutputHighOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	GPIO_setAsOutputPin(FRAM_PORT_POWER, FRAM_PIN_POWER);
	GPIO_setOutputHighOnPin(FRAM_PORT_POWER, FRAM_PIN_POWER);
}

/**
 * Enable power supply of external FRAM chip.
 */
void EXTFRAM_powerOn(void) {
	// power on
	GPIO_setOutputHighOnPin(FRAM_PORT_POWER, FRAM_PIN_POWER);

	// configure GPIO to default
	EXTFRAM_initGPIO();
}

/**
 * Disable power supply of external FRAM chip.
 */
void EXTFRAM_powerOff(void) {
	// configure output
	GPIO_setAsOutputPin(FRAM_PORT_CLK, FRAM_PIN_CLK);
	GPIO_setAsOutputPin(FRAM_PORT_MOSI, FRAM_PIN_MOSI);
	GPIO_setAsOutputPin(FRAM_PORT_MISO, FRAM_PIN_MISO);

	// set outputs low
	GPIO_setOutputLowOnPin(FRAM_PORT_POWER, FRAM_PIN_POWER);
	GPIO_setOutputLowOnPin(FRAM_PORT_CLK, FRAM_PIN_CLK);
	GPIO_setOutputLowOnPin(FRAM_PORT_MOSI, FRAM_PIN_MOSI);
	GPIO_setOutputLowOnPin(FRAM_PORT_MISO, FRAM_PIN_MISO);
	GPIO_setOutputLowOnPin(FRAM_PORT_CS, FRAM_PIN_CS);
}

/**
 * Read data form external FRAM.
 * @param address The address of the memory location where the first byte to
 * read is stored (byte addressed)
 * @param data The local data buffer where the read data is written to
 * @param count The number of bytes to read
 * @return The number of bytes read from external memory
 */
uint16_t EXTFRAM_read(uint32_t address, void* data, uint16_t count) {
	uint8_t* data_ptr = (uint8_t*)data;

	// CS low
	GPIO_setOutputLowOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	// 1. transmit read command
	SPI_transceive(FRAM_SPI_PERIPHERAL, FRAM_CMD_READ);

	// 2. transmit 3 byte read address, MSB first
	SPI_transceive(FRAM_SPI_PERIPHERAL, (address >> 16) & 0xff);
	SPI_transceive(FRAM_SPI_PERIPHERAL, (address >> 8) & 0xff);
	SPI_transceive(FRAM_SPI_PERIPHERAL, address & 0xff);

	// 3. receive data
	uint16_t i = 0;
	while(i < count) {
		data_ptr[i++] = SPI_transceive(FRAM_SPI_PERIPHERAL, FRAM_SPI_DUMMY_MESSAGE);
	}

	// CS high
	GPIO_setOutputHighOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	return i;
}

/**
 * Write data to external FRAM.
 * @param address The address of the memory location where the first byte is written to (byte addressed)
 * @param data The local data to write to external memory
 * @param count The number of bytes to write
 * @return The number of bytes written to the external memory
 */
uint16_t EXTFRAM_write(uint32_t address, const void* data, uint16_t count) {
	const uint8_t* data_ptr = (uint8_t*)data;

	// CS low
	GPIO_setOutputLowOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	// 1. transmit write enable command
	SPI_transceive(FRAM_SPI_PERIPHERAL, FRAM_CMD_WREN);

	// 2. confirm write enable by pulling CS high for a short period
	GPIO_setOutputHighOnPin(FRAM_PORT_CS, FRAM_PIN_CS);
	GPIO_setOutputLowOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	// 3. transmit write command
	SPI_transceive(FRAM_SPI_PERIPHERAL, FRAM_CMD_WRITE);

	// 4. transmit 3 byte write address, MSB first
	SPI_transceive(FRAM_SPI_PERIPHERAL, (address >> 16) & 0xff);
	SPI_transceive(FRAM_SPI_PERIPHERAL, (address >> 8) & 0xff);
	SPI_transceive(FRAM_SPI_PERIPHERAL, address & 0xff);

	// 5. transmit the register values to set
	uint16_t i = 0;
	while(i < count) {
		SPI_transceive(FRAM_SPI_PERIPHERAL, data_ptr[i++]);
	}

	// CS high
	GPIO_setOutputHighOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	return i;
}

/**
 * Set the external FRAM to sleep mode.
 * @note This will introduce a delay on the next commmand that needs to be
 * handeled manually.
 */
void EXTFRAM_sleep(void) {
	// CS low
	GPIO_setOutputLowOnPin(FRAM_PORT_CS, FRAM_PIN_CS);

	// 1. transmit write enable command
	SPI_transceive(FRAM_SPI_PERIPHERAL, FRAM_CMD_SLEEP);

	// CS high
	GPIO_setOutputHighOnPin(FRAM_PORT_CS, FRAM_PIN_CS);
}

/**
 * Trigger a wakeup of the external FRAM when in sleep mode.
 * @note Delay needs to be added manually after wakeup trigger to guarantee
 * reliable operation.
 */
void EXTFRAM_wakeup(void) {
	// toggle CS
	GPIO_setOutputLowOnPin(FRAM_PORT_CS, FRAM_PIN_CS);
	GPIO_setOutputHighOnPin(FRAM_PORT_CS, FRAM_PIN_CS);
}
