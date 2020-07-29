#include "bsp_pca9539a.h"	

#define PCA9539A_DEBUG

#ifdef 	PCA9539A_DEBUG
#define PCA9539A_TRACE         printf
#else
#define PCA9539A_TRACE(...)  				     
#endif


void iic_pca9539a_init(pca9539a_t dev)
{
	SoftI2c.pin_init(dev.pins);
	
	if (SoftI2c.check(dev.pins,dev.devAddress) == 1)
	{
		PCA9539A_TRACE("iic device pca9539a addr=%d......no found\r\n",dev.devAddress);
	}
	else
	{
		PCA9539A_TRACE("iic device pca9539a addr=%d......found\r\n",dev.devAddress);
//		PCA9539A_TRACE("out_data=%d\r\n",dev.out_data);
//		PCA9539A_TRACE("&out_data=%d\r\n",&dev.out_data);
//		PCA9539A_TRACE("(uint8_t *)dev.out_data=%d\r\n",(uint8_t *)dev.out_data);
//		PCA9539A_TRACE("(uint8_t *)1=%d\r\n",(uint8_t *)1);	
		
		u8 reg_data=0xff; SoftI2c.writes(dev.pins,1,dev.devAddress,6,&reg_data,1); //Set p0 as input
		
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,4,&reg_data,1);		 //set input Polarity
									 SoftI2c.writes(dev.pins,1,dev.devAddress,3,&dev.out_data,1);//set all outputs off
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,7,&reg_data,1);		 //set p1 as output
	}
}

