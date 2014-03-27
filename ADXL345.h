/*
 * ADXL345.h
 *
 *  Created on: Mar 16, 2013
 *      Author: jhatch
 */

#ifndef ADXL345_H_
#define ADXL345_H_

typedef struct XYZ
{
    uint8_t xyz[6];
} XYZ_t;

typedef struct XYZCon
{
	uint16_t xyz[3];
} XYZCon_t;

void writeADXL345(uint8_t reg, uint8_t val);
uint8_t readADXL345(uint8_t reg);
// void    readADXL345XYZ( XYZ_t *xyz);
void readADXL345XYZ( uint8_t *xyz_data);
void ADXL345ReadFIFODataSet(uint32_t portnum, uint8_t *buf);
void initialize_accel();
void CalcMAvg(XYZCon_t *XYZCon, int32_t output[], int samples);
void ConvertRawXYZ(XYZ_t *XYZRaw, XYZCon_t *XYZCon);
int GetAngle(int x, int y);

#endif /* ADXL345_H_ */
