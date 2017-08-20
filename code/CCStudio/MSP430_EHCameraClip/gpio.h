#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_PORT_START_BUTTON			GPIO_PORT_P4
#define GPIO_PIN_START_BUTTON			GPIO_PIN5

#define GPIO_PORT_LED_GREEN				GPIO_PORT_P1
#define GPIO_PIN_LED_GREEN				GPIO_PIN0

#define GPIO_PORT_CUSTOM1				GPIO_PORT_P1
#define GPIO_PIN_CUSTOM1				GPIO_PIN2
#define GPIO_PORT_CUSTOM2				GPIO_PORT_P3
#define GPIO_PIN_CUSTOM2				GPIO_PIN0
#define GPIO_PORT_CUSTOM3				GPIO_PORT_P4
#define GPIO_PIN_CUSTOM3				GPIO_PIN0
#define GPIO_PORT_CUSTOM4				GPIO_PORT_P4
#define GPIO_PIN_CUSTOM4				GPIO_PIN1

//#define GPIO_PORT_ACTIVITY_INDICATOR	GPIO_PORT_P1
//#define GPIO_PIN_ACTIVITY_INDICATOR		GPIO_PIN6

#define GPIO_PORT_LPM_WAKEUP			GPIO_PORT_P4
#define GPIO_PIN_LPM_WAKEUP				GPIO_PIN6

void GPIO_init(void);
void GPIO_reset(void);

#endif /* GPIO_H_ */