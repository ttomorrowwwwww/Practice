/*
* Copyright (C) 2012 Realtek Semiconductor Corp.
* All Rights Reserved.
*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTEHR THAN AS AUTHORIZED UNDER 
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
* 
* $Revision: v1.0.1 $
* $Date: 2012-10-23 11:18:41 +0800 $
*
* Purpose : MDC/MDIO example code
*
* Feature :  This file consists of following modules:
*                
*
*/

#ifndef _GPIO_H_
#define _GPIO_H_
#define GPIO_DIR_OUT 	"out"
#define GPIO_DIR_IN 	"in"
#define HIGH 					"1"
#define LOW 					"0"
#define GPIO_INT_DISABLE	0
#define GPIO_PERI_GPIO 1
#include "rtl8309n_types.h"
uint32 _rtl865x_initGpioPin( uint32 gpioId, uint32 gpio_pre,char *a , uint32 gpin_int);

void _rtl865x_setGpioDataBit(uint32 smi_MDC, uint32 data);

uint32 _rtl865x_getGpioDataBit(uint32 smi_MDIO, uint32 *u);
#endif