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


//#include "rtl_types.h"
#include "gpio.h"
#include "mdcmdio.h"      /*RTL8651B file*/
//#include "asicRegs.h"
#include "rtl8309n_types.h"
#define DELAY 500

#if (!SMI_HIGHSPEED)
#define CLK_DURATION(clk)   { volatile int i; for (i=0; i< (clk); i++); }

uint32 smi_MDC = 116;         /* GPIO used for SMI Clock Generation */
uint32 smi_MDIO = 115;       /* GPIO used for SMI Data signal */
uint32 smi_MRESET = 65;       /* GPIO used for SMI Data signal */
#endif


#if (!SMI_HIGHSPEED)

/*
 *Low speed smi is a general MDC/MDIO interface, it is realized by call gpio api 
 *function, could specified any gpio pin as MDC/MDIO
 */
static void _smiZbit(void)
{

    _rtl865x_initGpioPin(smi_MDIO, GPIO_PERI_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);
    _rtl865x_setGpioDataBit(smi_MDC, 0);
    _rtl865x_setGpioDataBit(smi_MDIO, 0);
    CLK_DURATION(DELAY);
}

/*Generate  1 -> 0 transition and sampled at 1 to 0 transition time,
 *should not sample at 0->1 transition because some chips stop outputing
 *at the last bit at rising edge
 */
static void _smiReadBit(uint32 * pdata) 
{
    uint32 u;
    
    _rtl865x_initGpioPin(smi_MDIO, GPIO_PERI_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);
    _rtl865x_setGpioDataBit(smi_MDC, 1);
    CLK_DURATION(DELAY);
    _rtl865x_setGpioDataBit(smi_MDC, 0);
    _rtl865x_getGpioDataBit(smi_MDIO, &u);
    *pdata = u;
	CLK_DURATION(DELAY);
}



/* Generate  0 -> 1 transition and put data ready during 0 to 1 whole period */
static void _smiWriteBit(uint32 data) 
{
    _rtl865x_initGpioPin(smi_MDIO, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (data) 
    {  /*write 1*/
        _rtl865x_setGpioDataBit(smi_MDIO, 1);
        _rtl865x_setGpioDataBit(smi_MDC, 0);
        CLK_DURATION(DELAY);
        _rtl865x_setGpioDataBit(smi_MDC, 1);
		CLK_DURATION(DELAY);
    } 
    else 
    {
        _rtl865x_setGpioDataBit(smi_MDIO, 0);
        _rtl865x_setGpioDataBit(smi_MDC, 0);
        CLK_DURATION(DELAY);
        _rtl865x_setGpioDataBit(smi_MDC, 1);
		CLK_DURATION(DELAY);
    }
}


/* Function Name:
 *      smiRead
 * Description:
 *      Read data from phy register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31) 
 * Output:
 *      data    -  Register value 
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 *     This function could read register through MDC/MDIO serial 
 *     interface, and it is platform  related. It use two GPIO pins 
 *     to simulate MDC/MDIO timing. MDC is sourced by the Station Management 
 *     entity to the PHY as the timing reference for transfer of information
 *     on the MDIO signal. MDC is an aperiodic signal that has no maximum high 
 *     or low times. The minimum high and low times for MDC shall be 160 ns each, 
 *     and the minimum period for MDC shall be 400 ns. Obeying frame format defined
 *     by IEEE802.3 standard, you could access Phy registers. If you want to 
 *     port it to other CPU, please modify static functions which are called 
 *      by this function.
 */
int32 smiRead(uint32 phyad, uint32 regad, uint32 * data) 
{

    int32 i;
    uint32 readBit;

    if ((phyad > 8) || (regad > 31) || (data == NULL))  
        return FAILED;

    /*it lock the resource to ensure that SMI opertion is atomic, 
     *the API is based on RTL865X, it is used to disable CPU interrupt,
     *if porting to other platform, please rewrite it to realize the same function
     */

 //   rtlglue_drvMutexLock();   

    /* 32 continuous 1 as preamble*/
    for(i=0; i<32; i++)
        _smiWriteBit(1);

    /* ST: Start of Frame, <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);
    
    /* OP: Operation code, read is <10>*/
    _smiWriteBit(1);
    _smiWriteBit(0);
    
    /* PHY Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((phyad>>i)&0x1);
    
    /* Register Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((regad>>i)&0x1);
    
    /* TA: Turnaround <z0>, zbit has no clock in order to steal a clock to
     *  sample data at clock falling edge 
     */
    _smiZbit();
	//_smiWriteBit(0);
    _smiReadBit(&readBit);

    /* Data */
    *data = 0;
    for(i=15; i>=0; i--) 
    {
        _smiReadBit(&readBit);
        *data = ((*data) << 1) | readBit;
    }
   // printf("smiRead = %#x.\r\n", *data);

    /*add  an extra clock cycles for robust reading , ensure partner stop 
     *output signal and not affect the next read operation, because TA 
     *steal a clock*/     
    _smiWriteBit(1);
    _smiZbit();

    /*unlock the source, enable interrupt*/    
 //   rtlglue_drvMutexUnlock();
    
    return  SUCCESS;
}

/* Function Name:
 *      smiWrite
 * Description:
 *      Write data to Phy register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31)
 *      data    -  Data to be written into Phy register
 * Output:
 *      none
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 *     This function could read register through MDC/MDIO serial 
 *     interface, and it is platform  related. It use two GPIO pins 
 *     to simulate MDC/MDIO timing. MDC is sourced by the Station Management 
 *     entity to the PHY as the timing reference for transfer of information
 *     on the MDIO signal. MDC is an aperiodic signal that has no maximum high 
 *     or low times. The minimum high and low times for MDC shall be 160 ns each, 
 *     and the minimum period for MDC shall be 400 ns. Obeying frame format defined
 *     by IEEE802.3 standard, you could access Phy registers. If you want to 
 *     port it to other CPU, please modify static functions which are called 
*      by this function.
 */

int32 smiWrite(uint32 phyad, uint32 regad, uint32 data)
{
    int32 i;

    if ((phyad > 31) || (regad > 31) || (data > 0xFFFF))  
        return FAILED;

    /*it lock the resource to ensure that SMI opertion is atomic, 
      *the API is based on RTL865X, it is used to disable CPU interrupt,
      *if porting to other platform, please rewrite it to realize the same function
      */
   // rtlglue_drvMutexLock();   

    /* 32 continuous 1 as preamble*/
    for(i=0; i<32; i++)
        _smiWriteBit(1);
    
    /* ST: Start of Frame, <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);

    /* OP: Operation code, write is <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);

    /* PHY Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((phyad>>i)&0x1);

    /* Register Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((regad>>i)&0x1);

    /* TA: Turnaround <10>*/
    _smiWriteBit(1);
    _smiWriteBit(0);

    /* Data */
    for(i=15; i>=0; i--) 
        _smiWriteBit((data>>i)&0x1);
	
    _smiZbit();

    /*unlock the source, enable interrupt*/        
  //  rtlglue_drvMutexUnlock();
            
    return SUCCESS; 
}


/* Function Name:
 *      smiInit
 * Description:
 *      Init Rtl8651B smi interface
 * Input:
 *      port        - Specify Rtl8651B GPIO port
 *      pinMDC    - Set which gpio pin as MDC 
 *      pinMDIO   - Set which gpio pin as MDIO
 * Output:
 *      none
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 *      This function is only for Rtl8651B, use it to specify
 *      GPIO pins as MDC/MDIO signal. It should be called at first.
 */
int32 smiInit(uint32 port, uint32 pinMDC, uint32 pinMDIO)
{
    uint32 gpioId;
    int32 res;

    /* Initialize GPIO smi_MDC  as SMI MDC signal */
    gpioId = smi_MDC;//GPIO_ID(port, pinMDC);
    res = _rtl865x_initGpioPin(gpioId, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (res != SUCCESS)
        return res;


    /* Initialize GPIO smi_MDIO  as SMI MDIO signal */
    gpioId = smi_MDIO;//GPIO_ID(port, pinMDIO);
    res = _rtl865x_initGpioPin(gpioId, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (res != SUCCESS)
        return res;

    /* Initialize GPIO smi_MDIO  as SMI MDIO signal */
    gpioId = smi_MRESET;//GPIO_ID(port, pinMDIO);
    res = _rtl865x_initGpioPin(gpioId, GPIO_PERI_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
    if (res != SUCCESS)
        return res;
    return SUCCESS;
}


#else  /*high speed smi*/

/*
 *high speed smi is a special interface, it is realsized by configuring rtl865x gpio register directly,
 *it specifies GPIO port C pin1 as MDC, pin 0 as MDIO
 */
/* Change clock to 1 */
static void _smiZBit(void) 
{
    uint32 i;
    REG32(PABCDIR) = (REG32(PABCDIR)& 0xFFFFFCFF) | 0x200;
    REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF);
    for(i=0; i< DELAY; i++);
}

/* Generate  1 -> 0 transition and sampled at 1 to 0 transition time,
 *should not sample at 0->1 transition because some chips stop outputing
 *at the last bit at rising edge
 */

static void _smiReadBit(uint32 * pdata) 
{
    uint32 i;
    REG32(PABCDIR) = (REG32(PABCDIR)& 0xFFFFFCFF) | 0x200;
    REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF) | 0x200;
    for(i=0; i< DELAY; i++);
    REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF);
    *pdata = (REG32(PABCDAT) & 0x100)?1:0;
}

/* Generate  0 -> 1 transition and put data ready during 0 to 1 whole period */
static void _smiWriteBit(uint32 data) 
{
    uint32 i;

    REG32(PABCDIR) = REG32(PABCDIR) | 0x300;
    if(data) 
    {  /* Write 1 */
        REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF) | 0x100;
        for(i=0; i< DELAY; i++);
        REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF) | 0x300;
    } 
    else 
    {
        REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF);
        for(i=0; i< DELAY; i++);
        REG32(PABCDAT) = (REG32(PABCDAT) & 0xFFFFFCFF) | 0x200;
    }
}

int32 smiRead(uint32 phyad, uint32 regad, uint32 * data) 
{
    int32 i;
    uint32 readBit;

    /* Configure port C pin 1, 0 to be GPIO and disable interrupts of these two pins */
    REG32(PABCCNR) = REG32(PABCCNR) & 0xFFFFFCFF;
    REG32(PCIMR) = REG32(PCIMR) & 0xFFFFFFF;
    
    /* 32 continuous 1 as preamble*/
    for(i=0; i<32; i++)
        _smiWriteBit(1);
    
    /* ST: Start of Frame, <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);
    
    /* OP: Operation code, read is <10> */
    _smiWriteBit(1);
    _smiWriteBit(0);

    /* PHY Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((phyad>>i)&0x1);

    /* Register Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((regad>>i)&0x1);
    
    /* TA: Turnaround <z0> */
    _smiZBit();
    _smiReadBit(&readBit);
    
    /* Data */
    *data = 0;
    for(i=15; i>=0; i--) 
    {
        _smiReadBit(&readBit);
        *data = (*data<<1) | readBit;
    }
    
    /*add  an extra clock cycles for robust reading , ensure partner stop output signal
      *and not affect the next read operation, because TA steal a clock
      */     
    _smiWriteBit(1);
    _smiZbit();
    
    return SUCCESS ;
}

int32 smiWrite(uint32 phyad, uint32 regad, uint32 data) 
{
    int32 i;

    /*Configure port C pin 1, 0 to be GPIO and disable interrupts of these two pins */
    REG32(PABCCNR) = REG32(PABCCNR) & 0xFFFFFCFF;
    REG32(PCIMR) = REG32(PCIMR) & 0xFFFFFFF;
    
    /* 32 continuous 1 as preamble*/
    for(i=0; i<32; i++)
        _smiWriteBit(1);
    
    /* ST: Start of Frame, <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);
    
    /* OP: Operation code, write is <01> */
    _smiWriteBit(0);
    _smiWriteBit(1);
    
    /* PHY Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((phyad>>i)&0x1);

    /* Register Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((regad>>i)&0x1);
    
    /* TA: Turnaround <10> */
    _smiWriteBit(1);
    _smiWriteBit(0);
    
    /* Data */
    for(i=15; i>=0; i--) 
        _smiWriteBit((data>>i)&0x1);
    _smiZBit();
    
    return SUCCESS;
}


#endif /* if  SMI_HIGHSPEED*/


/* Function Name:
 *      smiReadBit
 * Description:
 *      Read one bit of PHY register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31) 
 *      bit       -  Register bit (0~15)   
 * Output:
 *      pdata    - the pointer of  Register bit value 
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 */

int32 smiReadBit(uint32 phyad, uint32 regad, uint32 bit, uint32 * pdata) 
{
    uint32 regData;

    if ((phyad > 31) || (regad > 31) || (bit > 15) || (pdata == NULL) ) 
        return  FAILED;
    
    if(bit>=16)
        * pdata = 0;
    else 
    {
        smiRead(phyad, regad, &regData);
        if(regData & (1<<bit)) 
            * pdata = 1;
        else
            * pdata = 0;
    }
    return SUCCESS;
}

/* Function Name:
 *      smiWriteBit
 * Description:
 *      Write one bit of PHY register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31) 
 *      bit       -  Register bit (0~15)   
 *      data     -  Bit value to be written
 * Output:
 *      none
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 */

int32 smiWriteBit(uint32 phyad, uint32 regad, uint32 bit, uint32 data) 
{
    uint32 regData;
    
    if ((phyad > 31) || (regad > 31) || (bit > 15) || (data > 1) ) 
        return  FAILED;
    smiRead(phyad, regad, &regData);
    if(data) 
        regData = regData | (1<<bit);
    else
        regData = regData & ~(1<<bit);
    smiWrite(phyad, regad, regData);
    return SUCCESS;
}




















