
#include "gpio.h"
#include "mdcmdio.h"      /*RTL8651B file*/
//#include "asicRegs.h"
#include "rtl8309n_types.h"
#include <stdio.h>
#include <stdlib.h>
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"


uint32 _initGpioPin( uint32 gpioId)
{
	FILE *fp = NULL;
	int value;
	int gpio_num;
	char buf[10];
	fp = fopen("/sys/class/gpio/export","w");
	fprintf(fp,"%d",gpioId);
	fclose(fp);
}

uint32 _initGpioPinDirection( uint32 gpioId, char *a)
{

	FILE *fp = NULL;
	char file_name[50];
	sprintf(file_name, "/sys/class/gpio/gpio%d/direction", gpioId);

	fp = fopen(file_name,"rb+");

	fprintf(fp, a);
	fclose(fp);

}

uint32 _rtl865x_initGpioPin( uint32 gpioId, uint32 gpio_pre,char *a , uint32 gpin_int)
{
	uint32 ret = 0;
	static uint32 smi_MDC_init = 0;
	static uint32 smi_MDIO_init = 0;
	static uint32 smi_MRESET_init = 0;
	
	if(gpioId == smi_MDC)
	{
		if(smi_MDC_init)
		{
			_initGpioPinDirection(gpioId, a);
		}
		else
		{
			printf("%s %d init smi_MDC pin,gpioId = %d %s gpin_int:%d\r\n",__func__,__LINE__,gpioId,  a, gpin_int);
			smi_MDC_init = 1;
			_initGpioPin(gpioId);
			_initGpioPinDirection(gpioId, a);
		}
	}
	else if(gpioId == smi_MDIO)
	{
		if(smi_MDIO_init)
		{
			_initGpioPinDirection(gpioId, a);
		}
		else
		{
			printf("%s %d init smi_MDIO pin,gpioId = %d %s gpin_int:%d\r\n",__func__,__LINE__,gpioId,  a, gpin_int);
			smi_MDIO_init = 1;
			_initGpioPin(gpioId);
			_initGpioPinDirection(gpioId, a);
		}
	}
	else if(gpioId == smi_MRESET)
	{
		if(smi_MRESET_init)
		{
			printf("%s %d init reset pin\r\n.",__func__,__LINE__);
		}
		else
		{
			printf("%s %d init reset pin,gpioId = %d %s gpin_int:%d\r\n",__func__,__LINE__,gpioId,  a, gpin_int);
			smi_MRESET_init = 1;
			_initGpioPin(gpioId);
			_initGpioPinDirection(gpioId, a);
			_rtl865x_setGpioDataBit(gpioId, 1);
			sleep(1);
			_rtl865x_setGpioDataBit(gpioId, 0);
			sleep(1);
			_rtl865x_setGpioDataBit(gpioId, 1);
			sleep(2);
		}
	}

	return ret;
}

void _rtl865x_setGpioDataBit(uint32 smi_MDC, uint32 data)
{
	char file_name[50];
	char value = (char)(data+48);
	FILE *fp = NULL;
	//JL_DBG_MSG(JL_FLAG_PHY, _DBG_ERROR,"%s pin:%d, val = %d.\r\n",__func__, __LINE__,gpio->pin, val);
  sprintf(file_name, "/sys/class/gpio/gpio%d/value", smi_MDC);
  fp = fopen(file_name,"rb+");
  if(data)
  {
	  fprintf(fp, "1");
  }
  else
  {
		fprintf(fp, "0");
  }

  fclose(fp);
}

uint32 _rtl865x_getGpioDataBit(uint32 smi_MDIO, uint32 *u)
{
	uint32 ret = 0;

	FILE *fp = NULL;
	char buf[10];
	char file_name[50];
  sprintf(file_name, "/sys/class/gpio/gpio%d/value", smi_MDIO);
  fp = fopen(file_name,"rb+");
  memset(buf,0,10);
  fread(buf,1,sizeof(buf)-1,fp); //读取输入值
  ret = buf[0]-48;
  *u = ret;
  //printf("read value = %d\n",*u);
  fclose(fp);
	return ret;
}
