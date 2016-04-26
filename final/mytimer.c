#include "mytimer.h"

void MYTIMER_init()
{
    // we don't have to do anything.
}

void MYTIMER_enable()
{
    MYTIMER->control |= MYTIMER_ENABLE_MASK;
}

void MYTIMER_disable()
{
    MYTIMER->control &= ~MYTIMER_ENABLE_MASK;
}

void MYTIMER_setOverflowVal(uint32_t value)
{
	// Yes it's inefficient, but it's written this way to
	// show you the C to assembly mapping.
    uint32_t * timerAddr = (uint32_t*)(MYTIMER);
    *timerAddr = value; // overflowReg is at offset 0x0
}

uint32_t MYTIMER_getCounterVal()
{

// Yes it's inefficient, but it's written this way to
// show you the C to assembly mapping.
    uint32_t * timerAddr = (uint32_t*)(MYTIMER);
    return *(timerAddr+1); // counterReg is at offset 0x4
}

/**
 * Enable all interrupts
 */
void MYTIMER_enable_allInterrupts() {
	uint32_t * controlAddr = (uint32_t*) (MYTIMER);
	*(controlAddr+2) |= 0x2;
}
/**
 * Disable all interrupts
 */

void MYTIMER_disable_allInterrupts() {
	uint32_t * controlAddr = (uint32_t*) (MYTIMER);
	*(controlAddr+2) &= 0xD;
}
/**
 * Enable compare interrupt
 */

void MYTIMER_enable_compareInt() {
	uint32_t * controlAddr = (uint32_t*) (MYTIMER);
	*(controlAddr+2) |= 0x4;
}
/**
 * Disable compare interrupt
 */

void MYTIMER_disable_compareInt() {
	uint32_t * controlAddr = (uint32_t*) (MYTIMER);
	*(controlAddr+2) &= 0xB;
}
/**
 * Set Compare value
 */

void MYTIMER_setCompareVal(uint32_t compare) {
	uint32_t * compareAddr = (uint32_t*) (MYTIMER);
	*(compareAddr+3) = compare;
}
/**
 * Enable overflow interrupt
 */

void MYTIMER_enable_overflowInt() {
	uint32_t * controlAddr = (uint32_t*) (MYTIMER);
	*(controlAddr+2) |= 0x8;
}
/**
 * Disable overflow interrupt
 */

void MYTIMER_disable_overflowInt() {
	uint32_t * controlAddr = (uint32_t*) (MYTIMER);
	*(controlAddr+2) &= 0x7;
}
/**
  * Interrupt status
*/

uint32_t MYTIMER_getInterrupt_status() {
	uint32_t * statusAddr = (uint32_t*) (MYTIMER);
	return *(statusAddr+4);
}
