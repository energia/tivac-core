/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdio.h>
#include <Energia.h>

#define SPI_CLOCK_DIV2 2
#define SPI_CLOCK_DIV4 4
#define SPI_CLOCK_DIV8 8
#define SPI_CLOCK_DIV16 16
#define SPI_CLOCK_DIV32 32
#define SPI_CLOCK_DIV64 64
#define SPI_CLOCK_DIV128 128

#define SPI_MODE0 0x00
#define SPI_MODE1 0x80
#define SPI_MODE2 0x40
#define SPI_MODE3 0xC0

#if defined(ENERGIA_EK_TM4C1294XL)
#define BOOST_PACK_SPI 2
#define SPI SPI2
#elif defined(ENERGIA_EK_TM4C123GXL)
#define BOOST_PACK_SPI 2
#define SPI SPI2
#else
#error "LauncPad not supported"
#endif

#define MSBFIRST 1
#define LSBFIRST 0

class SPISettings {
public:
  uint8_t _SSIBitOrder;
  uint8_t _SSIMode;
  uint8_t _divider;
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
      init_AlwaysInline(clock, bitOrder, dataMode);
  }
  SPISettings() {
    init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
  }
private:

  void init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
    __attribute__((__always_inline__)) {

    // Pack into the SPISettings class
    _SSIBitOrder = bitOrder;
    _SSIMode     = dataMode ;
    _divider     =  20000000/clock;
  }
  friend class SPIClass;  
};

class SPIClass {

private:
	uint8_t SSIModule;
	uint8_t SSIBitOrder;
  static uint8_t inTransactionFlag;
  static uint8_t initialized;
  static uint8_t interruptMode; // 0=none, 1=mask, 2=global
  static uint8_t interruptMask[NUM_PORTS]; // which interrupts to mask
  static uint8_t interruptSave[NUM_PORTS]; // temp storage, to restore state
public:

  SPIClass(void);
  SPIClass(uint8_t);
  void begin(); // Default
  void end();
  void usingInterrupt(uint8_t interruptNumber);
  void notUsingInterrupt(uint8_t interruptNumber);
  void endTransaction(void);
  void beginTransaction(SPISettings settings);
  void setBitOrder(uint8_t);
  void setBitOrder(uint8_t, uint8_t);

  void setDataMode(uint8_t);

  void setClockDivider(uint8_t);

  uint8_t transfer(uint8_t);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);

  //Stellarpad-specific functions
  void setModule(uint8_t);

};

#if SPI_INTERFACES_COUNT > 0
  extern SPIClass SPI0;
#endif
#if SPI_INTERFACES_COUNT > 1
  extern SPIClass SPI1;
#endif
#if SPI_INTERFACES_COUNT > 2
  extern SPIClass SPI2;
#endif
#if SPI_INTERFACES_COUNT > 3
  extern SPIClass SPI3;
#endif
#endif
