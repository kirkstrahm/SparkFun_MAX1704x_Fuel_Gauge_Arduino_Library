/******************************************************************************
SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h
By: Paul Clark
October 23rd 2020

Based extensively on:
SparkFunMAX17043.h
SparkFun MAX17043 Library Header File
Jim Lindblom @ SparkFun Electronics
Original Creation Date: June 22, 2015
https://github.com/sparkfun/SparkFun_MAX17043_Particle_Library

This file implements all functions of the MAX17043 class. Functions here range
from higher level stuff, like reading/writing MAX17043 registers to low-level,
hardware reads and writes.

This code is released under the MIT license.

Distributed as-is; no warranty is given.
******************************************************************************/
#ifndef MAX1704X_ARDUINO_LIBRARY_H
#define MAX1704X_ARDUINO_LIBRARY_H

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

//#include "application.h"

///////////////////////////////////
// MAX1704x Register Definitions //
///////////////////////////////////
// All registers contain two bytes of data and span two addresses.
#define MAX17043_VCELL 0x02     // R - 12-bit A/D measurement of battery voltage
#define MAX17043_SOC 0x04       // R - 16-bit state of charge (SOC)
#define MAX17043_MODE 0x06      // W - Sends special commands to IC
#define MAX17043_VERSION 0x08   // R - Returns IC version
#define MAX17048_HIBRT 0x0A     // R/W - (MAX17048/49) Thresholds for entering hibernate
#define MAX17043_CONFIG 0x0C    // R/W - Battery compensation (default 0x971C)
#define MAX17048_CVALRT 0x14    // R/W - (MAX17048/49) Configures vcell range to generate alerts (default 0x00FF)
#define MAX17048_CRATE 0x16     // R - (MAX17048/49) Charge rate 0.208%/hr
#define MAX17048_VRESET_ID 0x18 // R/W - (MAX17048/49) Reset voltage and ID (default 0x96__)
#define MAX17048_STATUS 0x1A    // R/W - (MAX17048/49) Status of ID (default 0x01__)
#define MAX17043_COMMAND 0xFE   // W - Sends special comands to IC

///////////////////////////////////
// MAX17043 Config Register Bits //
///////////////////////////////////
#define MAX17043_CONFIG_SLEEP 7
#define MAX17043_CONFIG_ALERT 5
#define MAX17043_CONFIG_THRESHOLD 0

/////////////////////////////////////
// MAX17043 Mode Register Commands //
/////////////////////////////////////
#define MAX17043_MODE_QUICKSTART 0x4000

/////////////////////////////////////
// MAX17048/9 Status Register Bits //
/////////////////////////////////////
#define MAX1704x_STATUS_RI (1 << 0)
#define MAX1704x_STATUS_VH (1 << 1)
#define MAX1704x_STATUS_VL (1 << 2)
#define MAX1704x_STATUS_VR (1 << 3)
#define MAX1704x_STATUS_HD (1 << 4)
#define MAX1704x_STATUS_SC (1 << 5)

////////////////////////////////////////
// MAX17043 Command Register Commands //
////////////////////////////////////////
#define MAX17043_COMMAND_POR 0x5400

////////////////////////////////
// MAX17043 7-Bit I2C Address //
////////////////////////////////
#define MAX1704x_ADDRESS 0x36 // Unshifted I2C address. Becomes 0x6C for write and 0x6D for read.

// Generic error:
// Wire.endTransmission will return:
// 0:success
// 1:data too long to fit in transmit buffer
// 2:received NACK on transmit of address
// 3:received NACK on transmit of data
// 4:other error
// So, let's use "5" as a generic error value
#define MAX17043_GENERIC_ERROR 5

class SFE_MAX1704X
{
public:
  SFE_MAX1704X(int full_scale = 5); // Default to a full-scale of 5V for the MAX17043. Select 10 for the MAX17044.

  // begin() - Initializes the MAX17043.
  boolean begin(TwoWire &wirePort = Wire); //Returns true if module is detected

  //Returns true if device answers on MAX1704x_ADDRESS
  boolean isConnected(void);

  // Debug
  void enableDebugging(Stream &debugPort = Serial); // enable debug messages
  void disableDebugging();                          // disable debug messages

  // quickStart() - Restarts the MAX17043 to allow it to re-"guess" the
  // parameters that go into its SoC algorithms. Calling this in your setup()
  // usually results in more accurate SoC readings.
  // Output: 0 on success, positive integer on fail.
  uint8_t quickStart();

  // getVoltage() - Get the MAX17043's voltage reading.
  // Output: floating point value between 0-5V in 1.25mV increments.
  float getVoltage();

  // getSOC() - Get the MAX17043's state-of-charge (SOC) reading, as calculated
  // by the IC's "ModelGauge" algorithm.
  // Output: floating point value between 0-100, representing a percentage of
  // full charge.
  float getSOC();

  // getVersion() - Get the MAX17043's production version number.
  // Output: 3 on success
  uint16_t getVersion();

  // getThreshold() - Get the MAX17043's current percentage threshold that will
  // trigger an alert.
  // Output: An integer value between 1 and 32, representing a % that will
  // trigger an alert interrupt.
  uint8_t getThreshold();

  // setThreshold([percent]) - Set the MAX17043's percentage threshold that will
  // trigger an alert.
  // Input: [percent] - Percentage value that will trigger an alert interrupt.
  // Any value between 1 and 32 is valid.
  // Output: 0 on success, positive integer on fail.
  uint8_t setThreshold(uint8_t percent);

  // getAlert([clear]) - Check if the MAX17043's alert interrupt has been
  // triggered.
  // INPUT: [clear] - If [clear] is true, the alert flag will be cleared if it
  // was set.
  // OUTPUT: Returns 1 if interrupt is/was triggered, 0 if not.
  uint8_t getAlert(bool clear = false);

  // clearAlert() - Clear the MAX17043's alert flag.
  // Output: 0 on success, positive integer on fail.
  uint8_t clearAlert();

  // sleep() - Set the MAX17043 into sleep mode.
  // Output: 0 on success, positive integer on fail.
  uint8_t sleep();

  // wake() - Wake the MAX17043 up from sleep.
  // Output: 0 on success, positive integer on fail.
  uint8_t wake();

  // reset() - Issue a Power-on-reset command to the MAX17043. This function
  // will reset every register in the MAX17043 to its default value.
  // Output: Positive integer on success, 0 on fail.
  uint8_t reset();

  // getConfigRegister() - Read the 16-bit value of the CONFIG Register.
  // Output: 16-bit integer value representing the msb and lsb bytes of the
  // CONFIG register.
  uint16_t getConfigRegister();

  // getCompensation() - Get the ModelGauge compensation value - an obscure
  // 8-bit value set to 0x97 by default.
  // Output: 8-bit value read from the CONFIG register's MSB.
  uint8_t getCompensation();

  // setCompensation([newCompensation]) - Set the 8-bit compensation value. This
  // is an obscure 8-bit value that has some effect on Maxim's ModelGauge
  // algorithm.
  // From the datasheet: "Contact Maxim for instructions for optimization."
  // Input: [newCompensation] - Should be a value between 0-255.
  // Output: 0 on success, positive integer on fail.
  uint8_t setCompensation(uint8_t newCompensation);

  // getID() - (MAX17048/49) Returns 8-bit OTP bits set at factory. Can be used to
  // 'to distinguish multiple cell types in production'.
  // Writes to these bits are ignored.
  uint8_t getID(void);

  // setResetVoltage([threshold]) - (MAX17048/49) Set the 7-bit VRESET value.
  // A 7-bit value that controls the comparator for detecting when
  // a battery is detached and re-connected. 40mV per bit. Default is 3.0V.
  // Input: [threshold] - Should be a value between 0-127.
  // Output: 0 on success, positive integer on fail.
  uint8_t setResetVoltage(uint8_t threshold);

  // getResetVoltage() - (MAX17048/49) Get the 7-bit VRESET value
  // Output: 7-bit value read from the VRESET/ID register's MSB.
  uint8_t getResetVoltage(void);

  // enableComparator() - (MAX17048/49) Set bit in VRESET/ID reg
  // Comparator is enabled by default. (Re)enable the analog comparator, uses 0.5uA.
  // Output: 0 on success, positive integer on fail.
  uint8_t enableComparator(void);

  // disableComparator() - (MAX17048/49) Clear bit in VRESET/ID reg
  // Disable the analog comparator, saves 0.5uA in hibernate mode.
  // Output: 0 on success, positive integer on fail.
  uint8_t disableComparator(void);

  // getChangeRate() - (MAX17048/49) Get rate of change per hour in %
  // Output: (signed) Float (that is the 0.208% * CRATE register value)
  // A positive rate is charging, negative is discharge.
  float getChangeRate();

  // getStatus() - (MAX17048/49) Get the 7 bits of status register
  // Output: 7-bits indicating various alerts
  uint8_t getStatus();

  //(MAX17048/49) Various helper functions to check bits in status register
  bool isReset();       //True after POR
  bool isVoltageHigh(); //True when VCELL is above VALRTMAX (see setAlertMinVoltage)
  bool isVoltageLow();
  bool isVoltageReset();
  bool isLow();    //True when SOC crosses the value in ATHD (see setThreshold)
  bool isChange(); //True when SOL changes by at least 1%

  // softReset() - (MAX17048/49) Writes 0x5400 to CMD register (0xFE)
  // Initiates a power on reset. May also work with MAX17043/44 but requires 0x0054?
  // Output: None
  void softReset();

  // enableAlert() - (MAX17048/49) Set bit in STATUS register 0x1A
  // Enable/disable the assertion of the !ALRT! pin when a voltage-reset
  // event occurs under the conditions set by VRESET/ID register (see setResetVoltage)
  // Output: 0 on success, positive integer on fail.
  uint8_t enableAlert();
  uint8_t disableAlert();

  //Lower level functions but exposed incase user wants them

  // write16([data], [address]) - Write 16 bits to the requested address. After
  // writing the address to be written, two sequential 8-bit writes will occur.
  // the msb is written first, then lsb.
  // Input: [data] - A 16-bit integer to be written to the device.
  //        [address] - An 8-bit address to be written to.
  // Output: 0 on success, positive integer on fail.
  uint8_t write16(uint16_t data, uint8_t address);

  // read16([address]) - Read 16-bits from the requested address of a device.
  // Input: [address] - An 8-bit address to be read from.
  // Output: A 16-bit value read from the device's address will be returned.
  uint16_t read16(uint8_t address);

private:
  //Variables
  TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware

  Stream *_debugPort;          //The stream to send debug messages to if enabled. Usually Serial.
  boolean _printDebug = false; //Flag to print debugging variables

  int _full_scale = 5; // Default to a full-scale of 5V for the MAX17043. Select 10 for the MAX17044.
};

#endif
