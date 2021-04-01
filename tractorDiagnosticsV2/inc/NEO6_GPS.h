/*
 * NEO6_GPS.h
 *
 *  Created on: Mar 14, 2018
 *      Author: Zach
 */

#define GPS_LEN 256

#define QUALITY		6
#define SPEED		7

char gpsIn[GPS_LEN];
char gpsGGA[GPS_LEN];
char gpsVTG[GPS_LEN];
int gpsPtr;

extern void gpsGetSpeed(char * speedNow);
extern void gpsQualityCheck(char * qualitySpeed);
