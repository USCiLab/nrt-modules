/*
 * itg3200.h
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
#ifndef ITG3200_H_
#define ITG3200_H_

typedef uint8_t uint8_t;

class ITG3200
{
  public:
    static const float GYRO_SENSITIVITY = 14.375;
    static const uint8_t GYRO_RESET=0x80;
    static const uint8_t GYRO_SLEEP=0x40;
    static const uint8_t GYRO_STBY_X=0x20;
    static const uint8_t GYRO_STBY_Y=0x10;
    static const uint8_t GYRO_STBY_Z=0x08;
    static const uint8_t GYRO_CLK_INT=0x0;
    static const uint8_t GYRO_CLK_X=0x1;
    static const uint8_t GYRO_CLK_Y=0x2;
    static const uint8_t GYRO_CLK_Z=0x3;
    static const uint8_t GYRO_CLK_EXT32=0x4;
    static const uint8_t GYRO_CLK_EXT19=0x5;
    
    static const uint8_t GYRO_X=0x1;
    static const uint8_t GYRO_Y=0x2;
    static const uint8_t GYRO_Z=0x4;
    
    static const uint8_t GYRO_ADDR_0=0x68;
    static const uint8_t GYRO_ADDR_1=0x69;
    
   
    static const uint8_t GYRO_FS_2000=0x18;
    static const uint8_t GYRO_LPF_256_8K=0x0;
    static const uint8_t GYRO_LPF_188_1K=0x1;
    static const uint8_t GYRO_LPF_98_1K=0x2;
    static const uint8_t GYRO_LPF_42_1K=0x3;
    static const uint8_t GYRO_LPF_20_1K=0x4;
    static const uint8_t GYRO_LPF_10_1K=0x5;
    static const uint8_t GYRO_LPF_5_1K=0x6; // 5Hz LPF
    
    static const uint8_t GYRO_INT_ACTLO=0x80; // INT logic level = Active Low
    static const uint8_t GYRO_INT_ACTHI=0x00; // INT logic level = Active High
    static const uint8_t GYRO_INT_OPEND=0x40; // INT drive type = Open Drain
    static const uint8_t GYRO_INT_PUSHP=0x00; // INT drive type = Push Pull
    static const uint8_t GYRO_INT_LATCH=0x20; // INT Latch mode = until interrupt is cleared
    static const uint8_t GYRO_INT_PULSE=0x00; // INT Latch mode = 50us pulse
    static const uint8_t GYRO_INT_CLRRD=0x10; // INT clear method = any register ready
    static const uint8_t GYRO_INT_STSRD=0x00; // INT clear method = status reg. read
    static const uint8_t GYRO_INT_RDYEN=0x04; // INT Ready Enable
    static const uint8_t GYRO_INT_RAWEN=0x01; // INT Raw Data Ready Enable 
    
    ITG3200();
    void begin(int gyro_address);
    void begin(int gyro_address, uint8_t pwr_mgm, uint8_t fs_lpf, uint8_t smplrt_div, uint8_t int_cfg);
    void reset();
    void sleep();
    void standBy(uint8_t axis);
    void wake();
    void setInterruptConfig(uint8_t config);
    uint8_t getInterruptConfig();
    bool isInterruptRawDataReady();
    bool isInterruptReady();
    float getX();
    float getY();
    float getZ();
    float getTemperature();
    uint8_t getAddress();
    void setAddress(uint8_t newAddress);
    void setClockSource(uint8_t clockSource);
    

  private:
    static const uint8_t GYRO_REG_WHOAMI=0x00;
    static const uint8_t GYRO_REG_SMPLRT_DIV=0x15;
    static const uint8_t GYRO_REG_DLPF_FS=0x16;
    static const uint8_t GYRO_REG_INT_CFG=0x17;
    static const uint8_t GYRO_REG_INT_STS=0X1A;

    static const uint8_t GYRO_REG_TEMP_H=0x1B;
    static const uint8_t GYRO_REG_TEMP_L=0x1C;
    static const uint8_t GYRO_REG_X_H=0x1D;
    static const uint8_t GYRO_REG_X_L=0x1E;
    static const uint8_t GYRO_REG_Y_H=0x1F;
    static const uint8_t GYRO_REG_Y_L=0x20;
    static const uint8_t GYRO_REG_Z_H=0x21;
    static const uint8_t GYRO_REG_Z_L=0x22;
    static const uint8_t GYRO_REG_PWR_MGM=0x3E;

    static const uint8_t GYRO_INT_READY=0x04; // Enable interrupt when device is ready
    static const uint8_t GYRO_INT_DATA=0x01; // Enable interrupt when data is available
    
    static const float GYRO_TEMP_SENSITIVITY = 280.0;
    static const int GYRO_TEMP_OFFSET = 13200;
    static const float GYRO_TEMP_OFFSET_CELSIUS = 35.0;
    uint8_t _gyro_address;
    void write(uint8_t reg, uint8_t val);
    uint8_t read(uint8_t reg);
};


#endif
