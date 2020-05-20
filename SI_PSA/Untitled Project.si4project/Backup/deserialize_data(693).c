#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
/******************************************************************************								
*  Name: deserialize_data.c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		

/*******************************************************************************
* Includes
********************************************************************************/
#include "deserialize_ctrl.h"
#include "deserialize_data.h"
#include "GPIO.h"

/*******************************************************************************
* Constants
********************************************************************************/


/*******************************************************************************
* Macros
********************************************************************************/
#define DESER_SET_BYTE_NUMBER   0x01                
/*******************************************************************************
* Types
********************************************************************************/
typedef struct
{
    UINT8 sync;
    UINT8 device_address;
    UINT8 register_address;
    UINT8 byte_number;
    UINT8 byte[DESER_SET_BYTE_NUMBER];
}DESERIALIZE_SET;

/*******************************************************************************
* Variables
********************************************************************************/
DESERIALIZE_BLACK_STA Deserialize_Data_Backlight_sta={100,0};//backlight data update from uart receive 
/*******************************************************************************
* Global Variables
********************************************************************************/


/*******************************************************************************
* static Functions
********************************************************************************/
//static void Deserialize_Data_Register_Set(void);
static void Deserialize_Data_GPIO_Init(void);

/***********************************************************************************************
*
* @brief    deserialize_data_init - driver init and Initalize deserialize register
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Data_Init(void)
{
    /*UART driver init */
    Deserialize_UART_Init();
    /*GPIO init*/
    Deserialize_Data_GPIO_Init();
    /* initialize deserialize register */
//    Deserialize_Data_Register_Set();
}
/***********************************************************************************************
*
* @brief    deserialize_data_init - driver init and Initalize deserialize register
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Data_Standby(void)
{
    
}
/***********************************************************************************************
*
* @brief    deserialize_data_init - driver init and Initalize deserialize register
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Data_Send(UINT8* data, UINT8 data_size)
{
    UINT32  temp_time=0;
    
    Deserialize_Data_Set_Cds(1);
    Deserialize_UART_Send_Data(data, data_size);
    temp_time=0x2FF;/*0.096ms*/
    while(temp_time--);
    Deserialize_Data_Set_Cds(0);
}

/***********************************************************************************************
*
* @brief    deserialize_data_init - driver init and Initalize deserialize register
* @param    none
* @return   none
*
************************************************************************************************/  
UINT8 Deserialize_Data_Send_En(void)
{
    return GPIO_READ_INPUT(PTD,PTD6);
}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Data_Set_Cds(UINT8 level)
{
    if(level)
        OUTPUT_SET(PTD,PTD5);
    else
        OUTPUT_CLEAR(PTD,PTD5);
}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Data_Set_Backlight(UINT8 value,UINT8 flag)
{
 	Deserialize_Data_Backlight_sta.backlight=value;
	Deserialize_Data_Backlight_sta.update_flag=flag;
}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
 DESERIALIZE_BLACK_STA * Deserialize_Data_Get_Backlight(void)
{
 	return &Deserialize_Data_Backlight_sta;
}

#if 0
/***********************************************************************************************
*
* @brief    deserialize_data_register_set - set the register of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
static void Deserialize_Data_Register_Set(void)
{
    DESERIALIZE_SET des_set;
    /* initialize des_set */    
    des_set.sync = 0x79;
    des_set.device_address = 0x90;
    des_set.register_address = 0x04;
    des_set.byte_number = DESER_SET_BYTE_NUMBER;
    des_set.byte[0] = 0x07;
    /* send data to deserialize */
    Deserialize_Data_Send((UINT8*)&des_set, sizeof(DESERIALIZE_SET));
}
#endif

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
static void Deserialize_Data_GPIO_Init(void)
{
CONFIG_PIN_AS_GPIO(PTH,PTH0,OUTPUT); /* deserializer power up */
OUTPUT_SET(PTH,PTH0);

//BWS硬件open，高带宽模式
//    CONFIG_PIN_AS_GPIO(PTD, PTD5, OUTPUT);  /* Configure deserialize as master */
//    OUTPUT_SET(PTD,PTD5);
//    CONFIG_PIN_AS_GPIO(PTD,PTD0,OUTPUT); /* Configure DES_BWS(PTD0) as an output */	
//    OUTPUT_SET(PTD,PTD1);
    CONFIG_PIN_AS_GPIO(PTD,PTD1,OUTPUT); /* Configure DES_DRS(PTD1) as an output,fast TXCLKOUT_frequencies */	
    OUTPUT_CLEAR(PTD,PTD1);
    CONFIG_PIN_AS_GPIO(PTD,PTD2,INPUT);  /* Configure DES_LOCK(PTD2) as an input */
    GPIO_ENABLE_INPUT(PTD,PTD2);
    CONFIG_PIN_AS_GPIO(PTD,PTD3,INPUT);  /* Configure DES_ERR(PTD3) as an input */	
    GPIO_ENABLE_INPUT(PTD,PTD3);
    CONFIG_PIN_AS_GPIO(PTD,PTD4,OUTPUT); /* Configure DES_PWDN(PTD4) as an output */
    OUTPUT_SET(PTD,PTD4);
    CONFIG_PIN_AS_GPIO(PTD,PTD5,OUTPUT); /* Configure DES_CDS(PTD5) as an output,low when a control-channel master μC
                                           is connected at the serializer. */	
    OUTPUT_CLEAR(PTD,PTD5);
										   
    CONFIG_PIN_AS_GPIO(PTD,PTD6,INPUT);  /* Configure DES_INTOUT(PTD6) as an input */
    GPIO_ENABLE_INPUT(PTD,PTD6);
	
    CONFIG_PIN_AS_GPIO(PTD,PTD7,OUTPUT); /* Configure DES_GPI(PTD7) as an output */
    OUTPUT_SET(PTD,PTD7);
    CONFIG_PIN_AS_GPIO(PTA,PTA1,OUTPUT); /* Configure DES_HIM(PTA1) as an output,High-Immunity Mode active-high */
    OUTPUT_CLEAR(PTA,PTA1);
	
    CONFIG_PIN_AS_GPIO(PTF,PTF0,OUTPUT); /* Configure DES_SSEN(PTF0) as an output,
                                           low to use the LVDS and control outputs without spread spectrum. */
    OUTPUT_CLEAR(PTF,PTF0);
										   
    CONFIG_PIN_AS_GPIO(PTF,PTF2,OUTPUT); /* Configure DES_EQS(PTF2) as an output,Equalizer Select,
                                          EQS = low selects 10.7dB boost. EQS = high selects 5.7dB boost.*/
    OUTPUT_SET(PTF,PTF2);
#if 0										  		
	CONFIG_PIN_AS_GPIO(PTH,PTH0,OUTPUT); /* deserializer power up */
    OUTPUT_SET(PTH,PTH0);
#endif
}


#endif /* DESERIALIZE_MODULE */

