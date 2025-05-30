#include <readings.h>
#include <stdio.h>
#include <string.h>


Readings newR(){
	Readings new_R;
	new_R.s1_size = 0;
	new_R.s2_size = 0;
	new_R.s3_size = 0;
	new_R.size = 24;

	return new_R;
}

void addData(Readings* r, int sensor, int t, int h, int p){
	switch(sensor){
		case 1:
			if(r->s1_size == r->size){
				break;
			}
			r->s1_temp[r->s1_size] = t;
			r->s1_humi[r->s1_size] = h;
			r->s1_pres[r->s1_size] = p;
		    r->s1_size++;
			break;
		case 2:
			if(r->s2_size == r->size){
				break;
			}
			r->s2_temp[r->s1_size] = t;
			r->s2_humi[r->s1_size] = h;
			r->s2_pres[r->s1_size] = p;
			r->s2_size++;
			break;
		case 3:
			if(r->s3_size == r->size){
				break;
			}
			r->s3_temp[r->s1_size] = t;
			r->s3_humi[r->s1_size] = h;
			r->s3_pres[r->s1_size] = p;
			r->s3_size++;
			break;
	}
}



