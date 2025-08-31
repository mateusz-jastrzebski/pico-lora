#ifndef LORA_H
#define LORA_H

// // #include <Arduino.h>
// #include <SPI.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "string.h"
#include "Print.h"

// Default LoRa SPI pin assignments (renamed to avoid collision with project-wide PIN_* macros)
#define LORA_PIN_MISO_DEFAULT 16
#define LORA_PIN_CS_DEFAULT   8
#define LORA_PIN_SCK_DEFAULT  18
#define LORA_PIN_MOSI_DEFAULT 19

#define SPI_PORT spi0
#define READ_BIT 0x80

#define LORA_DEFAULT_SPI           spi0
#define LORA_DEFAULT_SPI_FREQUENCY 8E6
#define LORA_DEFAULT_SS_PIN        8
#define LORA_DEFAULT_RESET_PIN     9
#define LORA_DEFAULT_DIO0_PIN      7
#endif

#define PA_OUTPUT_RFO_PIN          0
#define PA_OUTPUT_PA_BOOST_PIN     1

static void __empty();

//class LoRaClass : public Stream {
class LoRaClass : public Print {
public:
  LoRaClass();

  int begin(long frequency);
  void end();

  int beginPacket(int implicitHeader = false);
  int endPacket(bool async = false);

  int parsePacket(int size = 0);
  int packetRssi();
  float packetSnr();
  long packetFrequencyError();

  int rssi();

  // from Print
  virtual size_t write(uint8_t byte);
  virtual size_t write(const uint8_t *buffer, size_t size);

  // from Stream
  virtual int available();
  virtual int read();
  virtual int peek();
  virtual void flush();

  void onCadDone(void (*callback)(bool));
  void onReceive(void (*callback)(int));
  void onTxDone(void (*callback)());

  void receive(int size = 0);
  void channelActivityDetection(void);

  void idle();
  void sleep();

  // size_t print(const char* c);

  void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
  void setFrequency(long frequency);
  void setSpreadingFactor(int sf);
  void setSignalBandwidth(long sbw);
  void setCodingRate4(int denominator);
  void setPreambleLength(long length);
  void setSyncWord(int sw);
  void enableCrc();
  void disableCrc();
  void enableInvertIQ();
  void disableInvertIQ();

  void setOCP(uint8_t mA); // Over Current Protection control

  void setGain(uint8_t gain); // Set LNA gain

  // deprecated
  void crc() { enableCrc(); }
  void noCrc() { disableCrc(); }

  uint8_t random();

  void setPins(int ss = LORA_DEFAULT_SS_PIN, int reset = LORA_DEFAULT_RESET_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
  void setSPI(spi_inst_t &spi);
  void setSPIFrequency(uint32_t frequency);
  void setSPIPins(int miso, int sck, int mosi); // configure which GPIOs are used for SPI (must match already-initialised bus)

  void dumpRegisters();
  uint8_t getVersion(); // read silicon version register (0x42)

private:
  void explicitHeaderMode();
  void implicitHeaderMode();

  void handleDio0Rise();
  bool isTransmitting();

  int getSpreadingFactor();
  long getSignalBandwidth();

  void setLdoFlag();

  uint8_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint8_t value);
  uint8_t singleTransfer(uint8_t address, uint8_t value);

  static void onDio0Rise(uint, uint32_t);

private:
  // SPISettings _spiSettings;
  spi_inst_t *_spi;
  int _ss;
  int _reset;
  int _dio0;
  int _pin_miso;
  int _pin_sck;
  int _pin_mosi;
  long _frequency;
  int _packetIndex;
  int _implicitHeaderMode;
  void (*_onReceive)(int);
  void (*_onCadDone)(bool);
  void (*_onTxDone)();
};

extern LoRaClass LoRa;