#include "stm32f10x_adc.h"
#include "stdint.h"

#define FULL_CHARGE 237600000

volatile int32_t oldAmps, newAmps, avgAmps, amp_usecs, amp_msecs, total_A_ms, maxAmps;
volatile uint32_t avgVolts;
volatile uint16_t newVolts;
volatile uint32_t pwr;
volatile uint8_t charge;

void initBMS( void );
void getAmps( int32_t * maxAmps );
uint8_t batterySOC( void );
void batteryRunTime( int * minutes );
uint16_t getVolts( void );
uint32_t getPower( uint32_t deezAmps, uint32_t deezVolts );
