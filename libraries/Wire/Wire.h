
#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>
#include "Stream.h"

#define BUFFER_LENGTH     64

#define IDLE 0
#define MASTER_TX 1
#define MASTER_RX 2
#define SLAVE_RX 3

#if defined(ENERGIA_EK_TM4C1294XL)
#define BOOST_PACK_WIRE 0
#define Wire Wire0
#elif defined(ENERGIA_EK_TM4C123GXL)
#define BOOST_PACK_WIRE 1
#define Wire Wire1
#else
#error "LauncPad not supported"
#endif

class TwoWire : public Stream
{

	private:
		uint8_t rxBuffer[BUFFER_LENGTH];
		uint8_t rxReadIndex;
		uint8_t rxWriteIndex;

		uint8_t txAddress;
		uint8_t txBuffer[BUFFER_LENGTH];
		uint8_t txReadIndex;
		uint8_t txWriteIndex;

		uint8_t i2cModule;
		uint8_t slaveAddress;

		uint8_t transmitting;
		uint8_t currentState;
		uint32_t highSpeed;
		void (*user_onRequest)(void);
		void (*user_onReceive)(int);
		void onRequestService(void);
		void onReceiveService(uint8_t*, int);
		
		uint8_t getRxData(unsigned long cmd);
		uint8_t sendTxData(unsigned long cmd, uint8_t data);
		void forceStop(void);

    public:
		TwoWire(void);
		TwoWire(unsigned long);
		void begin();
		void begin(uint8_t);
		void begin(int);
		void beginTransmission(uint8_t);
		void beginTransmission(int);
		uint8_t endTransmission(void);
		uint8_t endTransmission(uint8_t);
		uint8_t requestFrom(uint8_t, uint8_t);
		uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
		uint8_t requestFrom(int, int);
		uint8_t requestFrom(int, int, int);
		virtual size_t write(uint8_t);
		virtual size_t write(const uint8_t *, size_t);
		virtual int available(void);
		virtual int read(void);
		virtual int peek(void);
		virtual void flush(void);
		void onReceive( void (*)(int) );
		void onRequest( void (*)(void) );
		void setClock(uint32_t);

		inline size_t write(unsigned long n) { return write((uint8_t)n); }
		inline size_t write(long n) { return write((uint8_t)n); }
		inline size_t write(unsigned int n) { return write((uint8_t)n); }
		inline size_t write(int n) { return write((uint8_t)n); }
		using Print::write;

		//Stellarpad-specific functions
		void I2CIntHandler(void);
		void setModule(unsigned long);

};


#if WIRE_INTERFACES_COUNT > 0
  extern TwoWire Wire0;
  extern "C" void I2CIntHandler0(void);
#endif
#if WIRE_INTERFACES_COUNT > 1
  extern TwoWire Wire1;
  extern "C" void I2CIntHandler1(void);
#endif
#if WIRE_INTERFACES_COUNT > 2
  extern TwoWire Wire2;
  extern "C" void I2CIntHandler2(void);
#endif
#if WIRE_INTERFACES_COUNT > 3
  extern TwoWire Wire3;
  extern "C" void I2CIntHandler3(void);
#endif
#endif
