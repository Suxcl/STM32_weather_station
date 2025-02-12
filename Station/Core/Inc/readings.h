#include <stdio.h>
#include <string.h>

typedef struct Readings{
	int s1_temp[24];
	int s1_humi[24];
	int s1_pres[24];
	int s2_temp[24];
	int s2_humi[24];
	int s2_pres[24];
	int s3_temp[24];
	int s3_humi[24];
	int s3_pres[24];

	int s1_size;
	int s2_size;
	int s3_size;

	int size;

} Readings;

Readings newR(void);
void addData(Readings* r, int sensor, int t, int h, int p);
