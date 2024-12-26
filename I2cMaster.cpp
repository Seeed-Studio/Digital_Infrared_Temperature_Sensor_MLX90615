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
#include <I2cMaster.h>
#include <TwiMap.h>
//==============================================================================
// WARNING don't change SoftI2cMaster unless you verify the change with a scope
//------------------------------------------------------------------------------
/**
    Initialize SCL/SDA pins and set the bus high.

    \param[in] sdaPin The software SDA pin number.

    \param[in] sclPin The software SCL pin number.
*/
SoftI2cMaster::SoftI2cMaster(uint8_t sdaPin, uint8_t sclPin) {
    sdaPin_ = sdaPin;
    pinMode(sdaPin_, OUTPUT);
    digitalWrite(sdaPin_, HIGH);
    sclPin_ = sclPin;
    pinMode(sclPin_, OUTPUT);
    digitalWrite(sclPin_, HIGH);
}
//------------------------------------------------------------------------------
/** Read a byte and send Ack if more reads follow else Nak to terminate read.

    \param[in] last Set true to terminate the read else false.

    \return The byte read from the I2C bus.
*/
uint8_t SoftI2cMaster::read(uint8_t last) {
    uint8_t b = 0;
    // make sure pull-up enabled
    digitalWrite(sdaPin_, HIGH);
    pinMode(sdaPin_, INPUT);
    // read byte
    for (uint8_t i = 0; i < 8; i++) {
        // don't change this loop unless you verify the change with a scope
        b <<= 1;
        delayMicroseconds(I2C_DELAY_USEC);
        digitalWrite(sclPin_, HIGH);
        if (digitalRead(sdaPin_)) {
            b |= 1;
        }
        digitalWrite(sclPin_, LOW);
    }
    // send Ack or Nak
    pinMode(sdaPin_, OUTPUT);
    digitalWrite(sdaPin_, last);
    digitalWrite(sclPin_, HIGH);
    delayMicroseconds(I2C_DELAY_USEC);
    digitalWrite(sclPin_, LOW);
    digitalWrite(sdaPin_, LOW);
    return b;
}
//------------------------------------------------------------------------------
/** Issue a restart condition.

    \param[in] addressRW I2C address with read/write bit.

    \return The value true, 1, for success or false, 0, for failure.
*/
bool SoftI2cMaster::restart(uint8_t addressRW) {
    digitalWrite(sdaPin_, HIGH);
    digitalWrite(sclPin_, HIGH);
    delayMicroseconds(I2C_DELAY_USEC);
    return start(addressRW);
}
//------------------------------------------------------------------------------
/** Issue a start condition.

    \param[in] addressRW I2C address with read/write bit.

    \return The value true, 1, for success or false, 0, for failure.
*/
bool SoftI2cMaster::start(uint8_t addressRW) {
    digitalWrite(sdaPin_, LOW);
    delayMicroseconds(I2C_DELAY_USEC);
    digitalWrite(sclPin_, LOW);
    return write(addressRW);
}
//------------------------------------------------------------------------------
/**  Issue a stop condition. */
void SoftI2cMaster::stop(void) {
    digitalWrite(sdaPin_, LOW);
    delayMicroseconds(I2C_DELAY_USEC);
    digitalWrite(sclPin_, HIGH);
    delayMicroseconds(I2C_DELAY_USEC);
    digitalWrite(sdaPin_, HIGH);
    delayMicroseconds(I2C_DELAY_USEC);
}
//------------------------------------------------------------------------------
/**
    Write a byte.

    \param[in] data The byte to send.

    \return The value true, 1, if the slave returned an Ack or false for Nak.
*/
bool SoftI2cMaster::write(uint8_t data) {
    // write byte
    for (uint8_t m = 0X80; m != 0; m >>= 1) {
        // don't change this loop unless you verify the change with a scope
        digitalWrite(sdaPin_, m & data);
        digitalWrite(sclPin_, HIGH);
        delayMicroseconds(I2C_DELAY_USEC);
        digitalWrite(sclPin_, LOW);
    }

    // get Ack or Nak
    pinMode(sdaPin_, INPUT);
    // enable pullup
    digitalWrite(sdaPin_, HIGH);
    digitalWrite(sclPin_, HIGH);
    uint8_t rtn = digitalRead(sdaPin_);
    digitalWrite(sclPin_, LOW);
    pinMode(sdaPin_, OUTPUT);
    digitalWrite(sdaPin_, LOW);
    return rtn == 0;
}
//==============================================================================

#if defined(ARDUINO_ARCH_AVR)

void TwiMaster::execCmd(uint8_t cmdReg) {
    // send command
    TWCR = cmdReg;
    // wait for command to complete
    while (!(TWCR & (1 << TWINT)));
    // status bits.
    status_ = TWSR & 0xF8;
}
//------------------------------------------------------------------------------
/**
    Initialize hardware TWI.

    \param[in] enablePullup Enable the AVR internal pull-ups. The internal
    pull-ups can, in some systems, eliminate the need for external pull-ups.
*/
TwiMaster::TwiMaster(bool enablePullup) {
    // no prescaler
    TWSR = 0;
    // set bit rate factor
    TWBR = (F_CPU / F_TWI - 16) / 2;
    // enable pull-ups if requested
    if (enablePullup) {
        digitalWrite(TWI_SDA_PIN, HIGH);
        digitalWrite(TWI_SCL_PIN, HIGH);
    }
}
//------------------------------------------------------------------------------
/** Read a byte and send Ack if more reads follow else Nak to terminate read.

    \param[in] last Set true to terminate the read else false.

    \return The byte read from the I2C bus.
*/
uint8_t TwiMaster::read(uint8_t last) {
    execCmd((1 << TWINT) | (1 << TWEN) | (last ? 0 : (1 << TWEA)));
    return TWDR;
}
//------------------------------------------------------------------------------
/** Issue a restart condition.

    \param[in] addressRW I2C address with read/write bit.

    \return The value true, 1, for success or false, 0, for failure.
*/
bool TwiMaster::restart(uint8_t addressRW) {
    return start(addressRW);
}
//------------------------------------------------------------------------------
/** Issue a start condition.

    \param[in] addressRW I2C address with read/write bit.

    \return The value true for success or false for failure.
*/
bool TwiMaster::start(uint8_t addressRW) {
    // send START condition
    execCmd((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
    if (status() != TWSR_START && status() != TWSR_REP_START) {
        return false;
    }

    // send device address and direction
    TWDR = addressRW;
    execCmd((1 << TWINT) | (1 << TWEN));
    if (addressRW & I2C_READ) {
        return status() == TWSR_MRX_ADR_ACK;
    } else {
        return status() == TWSR_MTX_ADR_ACK;
    }
}
//------------------------------------------------------------------------------
/** Issue a stop condition. */
void TwiMaster::stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

    // wait until stop condition is executed and bus released
    while (TWCR & (1 << TWSTO));
}
//------------------------------------------------------------------------------
/**
    Write a byte.

    \param[in] data The byte to send.

    \return The value true, 1, if the slave returned an Ack or false for Nak.
*/
bool TwiMaster::write(uint8_t data) {
    TWDR = data;
    execCmd((1 << TWINT) | (1 << TWEN));
    return status() == TWSR_MTX_DATA_ACK;
}

#elif defined(ARDUINO_ARCH_ESP8266) 
#include <twi.h>
//------------------------------------------------------------------------------
/**
    Initialize hardware TWI.

    \param[in] enablePullup Enable the AVR internal pull-ups. The internal
    pull-ups can, in some systems, eliminate the need for external pull-ups.
*/
TwiMaster::TwiMaster(bool /* enablePullup */) {

}
//------------------------------------------------------------------------------
/** Read a byte and send Ack if more reads follow else Nak to terminate read.

    \param[in] last Set true to terminate the read else false.

    \return The byte read from the I2C bus.
*/
uint8_t TwiMaster::read(uint8_t last) {
    uint8_t rxBuffer;
    twi_readFrom(addressRW_, &rxBuffer, 1, last);
    return rxBuffer;
}
//------------------------------------------------------------------------------
/** Issue a restart condition.

    \param[in] addressRW I2C address with read/write bit.

    \return The value true, 1, for success or false, 0, for failure.
*/
bool TwiMaster::restart(uint8_t addressRW) {
    return start(addressRW);
}
//------------------------------------------------------------------------------
/** Issue a start condition.

    \param[in] addressRW I2C address with read/write bit.

    \return The value true for success or false for failure.
*/
bool TwiMaster::start(uint8_t addressRW) {
    addressRW_ = addressRW;
    twi_init(TWI_SDA_PIN, TWI_SCL_PIN);
    // TODO: set frequency/conversion etc.
}
//------------------------------------------------------------------------------
/** Issue a stop condition. */
void TwiMaster::stop(void) {
    twi_stop();
}
//------------------------------------------------------------------------------
/**
    Write a byte.

    \param[in] data The byte to send.

    \return The value true, 1, if the slave returned an Ack or false for Nak.
*/
bool TwiMaster::write(uint8_t data) {
    twi_writeTo(addressRW_, &data, 1, true);
}

#else
// #error unknown CPU
#endif
