/*
 *   itg3200.cpp
 *
 *   Copyright (C) 2010  Ricardo Arturo Cabral <ing dot cabral dot mejia at gmail dot com>. All rights reserved.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  See <license.txt>, if not, see 
 *   <http://www.gnu.org/licenses/>.
 */
#include "I2C.h"
//#include "WConstants.h" 
#include "itg3200.h"

ITG3200::ITG3200()
{
  I2c.begin();
};

void ITG3200::begin(int address)
{
  _gyro_address = address;
  this->write(GYRO_REG_PWR_MGM, 0);
  this->write(GYRO_REG_SMPLRT_DIV, 0xFF);
  this->write(GYRO_REG_DLPF_FS, 0x1E);
  this->write(GYRO_REG_INT_CFG, 0);
}

void ITG3200::begin(int address,byte pwr_mgm, byte fs_lpf, byte smplrt_div, byte int_cfg) {
  // Power Management
  _gyro_address = address;
  this->write(GYRO_REG_PWR_MGM, pwr_mgm);
  
  // Sample rate divider
  //_smplrt_div = smplrt_div;
  this->write(GYRO_REG_SMPLRT_DIV, smplrt_div);
  
  //Frequency select and digital low pass filter
  //_fs_lpf = fs_lpf;
  this->write(GYRO_REG_DLPF_FS, 0x1F & fs_lpf);
  
  //Interrupt configuration
  //_int_cfg = int_cfg;
  this->write(GYRO_REG_INT_CFG, 0xF5 & int_cfg);
}

float ITG3200::getX() {
  bool error = false;
  float result = (float)(this->read(GYRO_REG_X_L, &error) | this->read(GYRO_REG_X_H, &error)<<8)/GYRO_SENSITIVITY; 
  if(error) return NAN;
  return result;
}

float ITG3200::getY() {
  bool error = false;
  float result = (float)(this->read(GYRO_REG_Y_L, &error) | this->read(GYRO_REG_Y_H, &error)<<8)/GYRO_SENSITIVITY; 
  if(error) return NAN;
  return result;
}

float ITG3200::getZ() {
  bool error = false;
  float result = (float)(this->read(GYRO_REG_Z_L, &error) | this->read(GYRO_REG_Z_H, &error)<<8)/GYRO_SENSITIVITY; 
  if(error) return NAN;
  return result;
}

float ITG3200::getTemperature(){
  return (((float)((this->read(GYRO_REG_TEMP_L) | this->read(GYRO_REG_TEMP_H)<<8) + GYRO_TEMP_OFFSET))/GYRO_TEMP_SENSITIVITY) + GYRO_TEMP_OFFSET_CELSIUS;
}

void ITG3200::reset() {
  this->write(GYRO_REG_PWR_MGM, GYRO_RESET);
}

void ITG3200::sleep() {
  byte t = this->read(GYRO_REG_PWR_MGM);
  this->write(GYRO_REG_PWR_MGM, t | GYRO_SLEEP);
}

void ITG3200::wake(){
  byte t = this->read(GYRO_REG_PWR_MGM);
  this->write(GYRO_REG_PWR_MGM, t & ~GYRO_SLEEP);
}

void ITG3200::standBy(byte axis) {
  byte t = this->read(GYRO_REG_PWR_MGM);
  this->write(GYRO_REG_PWR_MGM, t & ~axis);
}

byte ITG3200::getAddress()
{
  return this->read(GYRO_REG_WHOAMI);
}

void ITG3200::setAddress(byte newAddress)
{
  this->write(GYRO_REG_WHOAMI, newAddress);
}

void ITG3200::setInterruptConfig(byte config)
{
  // bit 3 and 1 must be zero
  this->write(GYRO_REG_INT_CFG, 0xF5 & config);
}

bool ITG3200::isInterruptRawDataReady()
{
  byte result = this->read(GYRO_REG_INT_STS);
  return (result & GYRO_INT_DATA) == GYRO_INT_DATA;
}

bool ITG3200::isInterruptReady()
{
  byte result = this->read(GYRO_REG_INT_STS);
  return (result & GYRO_INT_READY) == GYRO_INT_READY;
}

byte ITG3200::getInterruptConfig()
{
  return this->read(GYRO_REG_INT_CFG);
}

void ITG3200::setClockSource(byte clockSource)
{
  if (clockSource >= 6) // 6 and 7 are reserved
    return;
  this->write(GYRO_REG_PWR_MGM, 0xF8 & clockSource);
}

void ITG3200::write(byte reg, byte val, bool *error) {
  byte errorcode = I2c.write(_gyro_address, reg, val);
  if(error != NULL && errorcode >= 1 && errorcode <= 7) *error = true;
}

byte ITG3200::read(byte reg, bool *error) {
  byte errorcode = I2c.write(_gyro_address, reg);
  if(error != NULL && errorcode >= 1 && errorcode <= 7) *error = true;

  I2c.read(_gyro_address, byte(1));
  return I2c.receive();
}
