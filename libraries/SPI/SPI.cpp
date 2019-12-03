/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "wiring_private.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "SPI.h"
#include "part.h"

#define SSIBASE g_ulSSIBase[SSIModule]
#define NOT_ACTIVE 0xA

/* variants
   stellarpad - LM4F120H5QR, TM4C123GH6PM, aka TARGET_IS_BLIZZARD_RB1
    i base  port
    0 SSI0 PA
    1 SSI1 PF
    2 SSI2 PB
    3 SSI3 PD

   dktm4c129 - TM4C129XNCZAD 
    i base  port
    0 SSI0  PA
    1 SSI1  PB/PE
    2 SSI2  PD
    3 SSI3  PF 
    4 SSI2  PG 
    5 SSI3  PQ

   ektm4c12944XL - TM4C1294NCPDT
    i base  port
    0 SSI0  PA
    1 SSI1  PB/PE
    2 SSI2  PD
    3 SSI3  PF 
    4 SSI3  PQ
*/

static const unsigned long g_ulSSIBase[] = {
#if defined(TARGET_IS_BLIZZARD_RB1)
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE
#elif defined(__TM4C129XNCZAD__)
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE, SSI2_BASE, SSI3_BASE
#elif defined(__TM4C1294NCPDT__)
    SSI0_BASE, SSI1_BASE, SSI2_BASE, SSI3_BASE, SSI3_BASE
#endif
};

//*****************************************************************************
//
// The list of SSI peripherals.
//
//*****************************************************************************
static const unsigned long g_ulSSIPeriph[] = {
#if defined(TARGET_IS_BLIZZARD_RB1)
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3
#elif defined(__TM4C129XNCZAD__)
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3
#elif defined(__TM4C1294NCPDT__)
    SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1, SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3, SYSCTL_PERIPH_SSI3
#endif

};

//*****************************************************************************
//
// The list of SSI gpio configurations.
//
//*****************************************************************************
static const unsigned long g_ulSSIConfig[][4] = {
#if defined(TARGET_IS_BLIZZARD_RB1)
    {GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0RX, GPIO_PA5_SSI0TX},
    {GPIO_PF2_SSI1CLK, GPIO_PF3_SSI1FSS, GPIO_PF0_SSI1RX, GPIO_PF1_SSI1TX},
    {GPIO_PB4_SSI2CLK, GPIO_PB5_SSI2FSS, GPIO_PB6_SSI2RX, GPIO_PB7_SSI2TX},
    {GPIO_PD0_SSI3CLK, GPIO_PD1_SSI3FSS, GPIO_PD2_SSI3RX, GPIO_PD3_SSI3TX}
#elif defined(__TM4C129XNCZAD__)
// from Table 20-1. SSI Signals (212BGA)
    {GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0XDAT0, GPIO_PA5_SSI0XDAT1},
    {GPIO_PB5_SSI1CLK, GPIO_PB4_SSI1FSS, GPIO_PE4_SSI1XDAT0, GPIO_PE5_SSI1XDAT1},
    {GPIO_PD3_SSI2CLK, GPIO_PD2_SSI2FSS, GPIO_PD1_SSI2XDAT0, GPIO_PD0_SSI2XDAT1},
    {GPIO_PF3_SSI3CLK, GPIO_PF2_SSI3FSS, GPIO_PF1_SSI3XDAT0, GPIO_PF0_SSI3XDAT1},
    {GPIO_PG7_SSI2CLK, GPIO_PG6_SSI2FSS, GPIO_PG5_SSI2XDAT0, GPIO_PG4_SSI2XDAT1},
    {GPIO_PQ0_SSI3CLK, GPIO_PQ1_SSI3FSS, GPIO_PQ2_SSI3XDAT0, GPIO_PQ3_SSI3XDAT1}
#elif defined(__TM4C1294NCPDT__)
// from Table 17-1. SSI Signals (128TQFP)
    {GPIO_PA2_SSI0CLK, GPIO_PA3_SSI0FSS, GPIO_PA4_SSI0XDAT0, GPIO_PA5_SSI0XDAT1},
    {GPIO_PB5_SSI1CLK, GPIO_PB4_SSI1FSS, GPIO_PE4_SSI1XDAT0, GPIO_PE5_SSI1XDAT1},
    {GPIO_PD3_SSI2CLK, GPIO_PD2_SSI2FSS, GPIO_PD1_SSI2XDAT0, GPIO_PD0_SSI2XDAT1},
    {GPIO_PF3_SSI3CLK, GPIO_PF2_SSI3FSS, GPIO_PF1_SSI3XDAT0, GPIO_PF0_SSI3XDAT1},
    {GPIO_PQ0_SSI3CLK, GPIO_PQ1_SSI3FSS, GPIO_PQ2_SSI3XDAT0, GPIO_PQ3_SSI3XDAT1}
#endif
,};

//*****************************************************************************
//
// The list of SSI gpio port bases.
//
//*****************************************************************************
static const unsigned long g_ulSSIPort[] = {
#if defined(TARGET_IS_BLIZZARD_RB1)
    GPIO_PORTA_BASE, GPIO_PORTF_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE
#elif defined(__TM4C129XNCZAD__)
    GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE, GPIO_PORTF_BASE, GPIO_PORTG_BASE, GPIO_PORTQ_BASE
#elif defined(__TM4C1294NCPDT__)
    GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE, GPIO_PORTF_BASE, GPIO_PORTQ_BASE
#endif
};

//*****************************************************************************
//
// The list of SSI gpio configurations.
//
//*****************************************************************************
static const unsigned long g_ulSSIPins[] = {
#if defined(TARGET_IS_BLIZZARD_RB1)
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
	GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#elif defined(__TM4C129XNCZAD__)
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#elif defined(__TM4C1294NCPDT__)
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_4 | GPIO_PIN_5,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
	GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
#endif
};

uint8_t SPIClass::initialized = 0;
uint8_t SPIClass::interruptMode = 0;
uint8_t SPIClass::interruptMask[] = {};
uint8_t SPIClass::interruptSave[] = {};

SPIClass::SPIClass(void) {
	SSIModule = NOT_ACTIVE;
	SSIBitOrder = MSBFIRST;
}

SPIClass::SPIClass(uint8_t module) {
	SSIModule = module;
	SSIBitOrder = MSBFIRST;
}

void SPIClass::beginTransaction(SPISettings settings) {
    if (interruptMode > 0) {
        if (interruptMode == 1) {
            uint8_t i;
            for(i = 1; i < NUM_PORTS; i++) {
                if(interruptMask[i]) {
                    interruptSave[i] = HWREG((uint32_t) portBASERegister(i) + GPIO_O_IM);
                    // disable the registered interrupts
                    GPIOIntDisable((uint32_t) portBASERegister(i), interruptMask[i]);
                }
            }
        } else {
            noInterrupts();
        }
    }
    setBitOrder(settings._SSIBitOrder);
    setDataMode(settings._SSIMode);
    setClockDivider(settings._divider);
}

void SPIClass::endTransaction(void) {
    if (interruptMode > 0) {
        noInterrupts();
        if (interruptMode == 1) {
            uint8_t i;
            for(i = 1; i < NUM_PORTS; i++) {
                if(interruptSave[i]) {
                    // disable the registered interrupts
                    GPIOIntEnable((uint32_t) portBASERegister(i), interruptSave[i]);
                }
            }
            interrupts();
        } else {
            interrupts();
        }
    }
}

void SPIClass::begin() {
    if (!initialized) {
        unsigned long initialData = 0;

        if(SSIModule == NOT_ACTIVE) {
            SSIModule = BOOST_PACK_SPI;
        }

        ROM_SysCtlPeripheralEnable(g_ulSSIPeriph[SSIModule]);
        ROM_SSIDisable(SSIBASE);
        ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][0]);
        ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][1]);
        ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][2]);
        ROM_GPIOPinConfigure(g_ulSSIConfig[SSIModule][3]);
    #if defined(TARGET_IS_BLIZZARD_RB1)
        ROM_GPIOPinTypeSSI(g_ulSSIPort[SSIModule], g_ulSSIPins[SSIModule]);
    #elif defined(__TM4C129XNCZAD__) || defined(__TM4C1294NCPDT__)
        if (SSIModule == 1) { // 1 is a split port 
            ROM_GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_5 | GPIO_PIN_4);
            ROM_GPIOPinTypeSSI(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
        } else {
            ROM_GPIOPinTypeSSI(g_ulSSIPort[SSIModule], g_ulSSIPins[SSIModule]);
        }
    #endif


        /*
        Polarity Phase        Mode
            0 	   0   SSI_FRF_MOTO_MODE_0
            0     1   SSI_FRF_MOTO_MODE_1
            1     0   SSI_FRF_MOTO_MODE_2
            1     1   SSI_FRF_MOTO_MODE_3
        */

        /*
        * Default to
        * System Clock, SPI_MODE_0, MASTER,
        * 4MHz bit rate, and 8 bit data
        */
        ROM_SSIClockSourceSet(SSIBASE, SSI_CLOCK_SYSTEM);

    #if defined(TARGET_IS_BLIZZARD_RB1)
        SSIConfigSetExpClk(SSIBASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);
    #else
        SSIConfigSetExpClk(SSIBASE, F_CPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);
    #endif

        ROM_SSIEnable(SSIBASE);

        //clear out any initial data that might be present in the RX FIFO
        while(ROM_SSIDataGetNonBlocking(SSIBASE, &initialData));
        }
    initialized++; // reference count
}

void SPIClass::end() {
    if (initialized)
        initialized--;
    if (!initialized) {
    	ROM_SSIDisable(SSIBASE);
        interruptMode = 0;
    }
}

void SPIClass::usingInterrupt(uint8_t interruptNumber) {
    noInterrupts();
    // handle interruptMode
    uint8_t bit = digitalPinToBitMask(interruptNumber);
	uint8_t port = digitalPinToPort(interruptNumber);
    
    if(!port || !bit) {
        interruptMode = 2;
    }

    if (!interruptMode) {
        interruptMode = 1;
    }

    interruptMask[port] |= bit;

    interrupts();
}

void SPIClass::notUsingInterrupt(uint8_t interruptNumber) {
  if (interruptMode == 2) {
    return;
  }

    uint8_t haveInterrupts = false;
    uint8_t i;

    noInterrupts();
    // handle interruptMode

    uint8_t bit = digitalPinToBitMask(interruptNumber);
	uint8_t port = digitalPinToPort(interruptNumber);
    interruptMask[port] &= ~bit;

    for(i = 1; i < NUM_PORTS; i++) {
        if(interruptMask[i]) {
            haveInterrupts = true;
            break;
        }
    }

    if (!haveInterrupts)
        interruptMode = 0;

    interrupts();
}

void SPIClass::setBitOrder(uint8_t ssPin, uint8_t bitOrder) {
	SSIBitOrder = bitOrder;
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
	SSIBitOrder = bitOrder;
}

void SPIClass::setDataMode(uint8_t mode) {
	HWREG(SSIBASE + SSI_O_CR0) &= ~(SSI_CR0_SPO | SSI_CR0_SPH);
	HWREG(SSIBASE + SSI_O_CR0) |= mode;
}

void SPIClass::setClockDivider(uint8_t divider){

    uint32_t ui32MaxBitRate;
    uint32_t ui32BitRate;
    uint32_t ui32RegVal;
    uint32_t ui32PreDiv;
    uint32_t ui32SCR;

    ui32BitRate = 16000000 / divider;
    //
    // Set the clock predivider.
    //
    ui32MaxBitRate = F_CPU / ui32BitRate;
    ui32PreDiv = 0;
    do
    {
        ui32PreDiv += 2;
        ui32SCR = (ui32MaxBitRate / ui32PreDiv) - 1; 
    }
    while(ui32SCR > 255);
    HWREG(SSIBASE + SSI_O_CPSR) = ui32PreDiv;

    ui32RegVal = HWREG(SSIBASE + SSI_O_CR0);
    ui32RegVal &= ~(SSI_CR0_SCR_M);
    ui32RegVal |= (ui32SCR << SSI_CR0_SCR_S) & (SSI_CR0_SCR_M);
    HWREG(SSIBASE + SSI_O_CR0) = ui32RegVal;
}

uint8_t SPIClass::transfer(uint8_t data) {
	uint32_t rxtxData;

	rxtxData = data;
	if(SSIBitOrder == LSBFIRST) {
		asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of 32 bits 
		asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of bytes to get original bits into lowest byte 
	}
	ROM_SSIDataPut(SSIBASE, (uint8_t) rxtxData);

	while(ROM_SSIBusy(SSIBASE));

	ROM_SSIDataGet(SSIBASE, &rxtxData);
	if(SSIBitOrder == LSBFIRST) {
		asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of 32 bits 
		asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of bytes to get original bits into lowest byte 
	}

	return (uint8_t) rxtxData;
}

uint16_t SPIClass::transfer16(uint16_t data) {
    uint32_t rxtxData;
    uint32_t ui32RegVal;

    // Set datawidth to 16 bit
    ui32RegVal = HWREG(SSIBASE + SSI_O_CR0);
    ui32RegVal &= ~(SSI_CR0_DSS_M);
    ui32RegVal |= (SSI_CR0_DSS_16);

    HWREG(SSIBASE + SSI_O_CR0) = ui32RegVal;

	rxtxData = data;
	if(SSIBitOrder == LSBFIRST) {
		asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of 32 bits 
		asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of bytes to get original bits into lowest byte 
	}
	ROM_SSIDataPut(SSIBASE, (uint16_t) rxtxData);

	while(ROM_SSIBusy(SSIBASE));

	ROM_SSIDataGet(SSIBASE, &rxtxData);
	if(SSIBitOrder == LSBFIRST) {
		asm("rbit %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of 32 bits 
		asm("rev %0, %1" : "=r" (rxtxData) : "r" (rxtxData));	// reverse order of bytes to get original bits into lowest byte 
	}

    // Set datawidth to 8 bit
    ui32RegVal = HWREG(SSIBASE + SSI_O_CR0);
    ui32RegVal &= ~(SSI_CR0_DSS_M);
    ui32RegVal |= (SSI_CR0_DSS_8);

    HWREG(SSIBASE + SSI_O_CR0) = ui32RegVal;

	return (uint16_t) rxtxData;
}

void SPIClass::transfer(void *buf, size_t count) { 
    uint32_t ui32RegVal;
    uint32_t txData;
    uint8_t *_buf = (uint8_t *) buf;
    size_t i;
    
    if(count == 1) {
        transfer(_buf[0]);
    }

    // Transfer 16 bytes at the time
    for(i = 0; i < count; i = i+ 2) {
        if(count - i == 1) {
            transfer(_buf[i]);
            break;
        }
        txData = ((uint8_t)_buf[i]) << 8 | ((uint8_t)_buf[i + 1]);
        transfer16(txData);
    }
}

void SPIClass::setModule(uint8_t module) {
	SSIModule = module;
	begin();
}

#if SPI_INTERFACES_COUNT > 0
  SPIClass SPI0(0);
#endif
#if SPI_INTERFACES_COUNT > 1
  SPIClass SPI1(1);
#endif
#if SPI_INTERFACES_COUNT > 2
  SPIClass SPI2(2);
#endif
#if SPI_INTERFACES_COUNT > 3
  SPIClass SPI3(3);
#endif
