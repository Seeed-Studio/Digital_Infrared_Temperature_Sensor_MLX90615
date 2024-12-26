/*  Arduino I2cMaster Library
    Copyright (C) 2010 by William Greiman

    This file is part of the Arduino I2cMaster Library

    This Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Arduino I2cMaster Library.  If not, see
    <http://www.gnu.org/licenses/>.
*/
#ifndef I2C_MASTER_H
#define I2C_MASTER_H
/**
    \file
    \brief Software I2C and hardware TWI library
*/
#if ARDUINO < 100
    #include <WProgram.h>
#else  // ARDUINO
    #include <Arduino.h>
#endif  // ARDUINO
/** hardware I2C clock in Hz */
uint32_t const F_TWI = 400000L;

/** Delay used for software I2C */
uint8_t const I2C_DELAY_USEC = 4;

/** Bit to or with address for read start and read restart */
uint8_t const I2C_READ = 1;

/** Bit to or with address for write start and write restart */
uint8_t const I2C_WRITE = 0;
//------------------------------------------------------------------------------
// Status codes in TWSR - names are from Atmel TWSR.h with TWSR_ added

/** start condition transmitted */
uint8_t const TWSR_START = 0x08;

/** repeated start condition transmitted */
uint8_t const TWSR_REP_START = 0x10;

/** slave address plus write bit transmitted, ACK received */
uint8_t const TWSR_MTX_ADR_ACK = 0x18;

/** data transmitted, ACK received */
uint8_t const TWSR_MTX_DATA_ACK = 0x28;

/** slave address plus read bit transmitted, ACK received */
uint8_t const TWSR_MRX_ADR_ACK = 0x40;

//------------------------------------------------------------------------------
/**
    \class I2cMasterBase
    \brief Base class for SoftI2cMaster and TwiMaster
*/
class I2cMasterBase {
  public:
    /** Read a byte
        \param[in] last send Ack if last is false else Nak to terminate read
        \return byte read from I2C bus
    */
    virtual uint8_t read(uint8_t last) = 0;
    /** Send new address and read/write bit without sending a stop.
        \param[in] addressRW i2c address with read/write bit
        \return true for success false for failure
    */
    virtual bool restart(uint8_t addressRW) = 0;
    /** Issue a start condition
        \param[in] addressRW i2c address with read/write bit
        \return true for success false for failure
    */
    virtual bool start(uint8_t addressRW) = 0;
    /** Issue a stop condition. */
    virtual void stop(void) = 0;
    /** Write a byte
        \param[in] data byte to write
        \return true for Ack or false for Nak */
    virtual bool write(uint8_t data) = 0;
};
//------------------------------------------------------------------------------
/**
    \class SoftI2cMaster
    \brief Software I2C master class
*/
class SoftI2cMaster : public I2cMasterBase {
  public:
    SoftI2cMaster(uint8_t sdaPin, uint8_t sclPin);
    uint8_t read(uint8_t last);
    bool restart(uint8_t addressRW);
    bool start(uint8_t addressRW);
    void stop(void);
    bool write(uint8_t b);
  private:
    SoftI2cMaster() {}
    uint8_t sdaPin_;
    uint8_t sclPin_;
};
//------------------------------------------------------------------------------
/**
    \class TwiMaster
    \brief Hardware I2C master class

    Uses ATmega TWI hardware port
*/
class TwiMaster : public I2cMasterBase {
  public:
    explicit TwiMaster(bool enablePullup);
    uint8_t read(uint8_t last);
    bool restart(uint8_t addressRW);
    bool start(uint8_t addressRW);
    /** \return status from last TWI command - useful for library debug */
    #if defined(ARDUINO_ARCH_AVR)
    uint8_t status(void) {
        return status_;
    }
    #endif
    void stop(void);
    bool write(uint8_t data);
  private:
    TwiMaster() {}

    #if defined(ARDUINO_ARCH_AVR)

    uint8_t status_;
    void execCmd(uint8_t cmdReg);

    #elif defined(ARDUINO_ARCH_ESP8266) 
    uint8_t addressRW_ = 0;
    #else
// #error unknown CPU
    #endif
};
#endif  // I2C_MASTER_H
