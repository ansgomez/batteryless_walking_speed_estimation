/*
 * serial.h
 *
 *  Created on: 14.03.2016
 *      Author: thomas
 */

#ifndef SERIAL_H_
#define SERIAL_H_

/* Use http://processors.wiki.ti.com/index.php/USCI_UART_Baud_Rate_Gen_Mode_Selection
 * for proper selection of prescaler etc.
 */

// SMCLK 3MHz, 115200baud
/*
#define SERIAL_CLOCK_SOURCE			EUSCI_A_UART_CLOCKSOURCE_SMCLK
#define SERIAL_PRESCALER 			26
#define SERIAL_UCxBRF				0
#define SERIAL_UCxBRS				0
#define SERIAL_MODE					EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION
*/

// SMCLK 12MHz, 115200baud
/*
#define SERIAL_CLOCK_SOURCE			EUSCI_A_UART_CLOCKSOURCE_SMCLK
#define SERIAL_PRESCALER 			104
#define SERIAL_UCxBRF				0
#define SERIAL_UCxBRS				1
#define SERIAL_MODE					EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION
*/

// SMCLK 24MHz, 115200baud
#define SERIAL_CLOCK_SOURCE			EUSCI_A_UART_CLOCKSOURCE_SMCLK		// 12 MHz
#define SERIAL_PRESCALER 			208		//26		// 104
#define SERIAL_UCxBRF				0		//1		// 0
#define SERIAL_UCxBRS				2		//0		// 1
#define SERIAL_MODE					EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION	//EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION		// EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION


void Serial_init(void);

void Serial_sendChar(char);
void Serial_sendString(char*);

#endif /* SERIAL_H_ */
