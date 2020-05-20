#include "project_cfg.h" 
#ifdef LED_MODULE
/******************************************************************************								
*  Name: .c
*  Description: 
*  Project: C_demo
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/		
#include "LED_I2C.h"
#include "GPIO.h"
#include "I2C.h"
#include "LED_data.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/


/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/* SDA SDL */
#define LED_SCL_HIGH    //OUTPUT_SET(PTC,PTC7)
#define LED_SCL_LOW     //OUTPUT_CLEAR(PTC,PTC7)
#define LED_SDA_HIGH    //OUTPUT_SET(PTC,PTC6)
#define LED_SDA_LOW     //OUTPUT_CLEAR(PTC,PTC6)

/* time */
#define TIME_ONE_UNIT       0x00000001  /*the core clock is 40MHz, 0x01 need 125.12ns to count */
#define TIME_TWO_UNIT       0x00000002
#define TIME_THREE_UNIT     0x00000003
#define TIME_FOUR_UNIT      0x00000004
#define TIME_FIVE_UNIT      0x00000005
#define TIME_SIX_UNIT       0x00000006
#define TIME_SEVEN_UNIT     0x00000007
#define TIME_EIGHT_UNIT     0x00000008
#define TIME_NINE_UNIT      0x00000009
#define TIME_TEN_UNIT       0x0000000A
#define TIME_ELEVEN_UNIT    0x0000000B
#define TIME_FOURTEEN_UNIT  0x0000000E
/* ACK */
typedef enum 
{
    I2C_NACK=0
    ,I2C_ACK
}LED_I2C_ACK;

/**********************************************************************************************
* Local types
**********************************************************************************************/


/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static UINT8 Power_state;

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void LED_I2C_GPIO_Init(void);
static void LED_I2C_Start(void);
static void LED_I2C_Restart(void);
static void LED_I2C_Stop(void);
static UINT8 LED_I2C_Send_One_Byte(UINT8 byte);
static void LED_I2C_Read_One_Byte(UINT8 *byte, LED_I2C_ACK ack);
static void LED_I2C_Delay(UINT32  timer);
static void LED_I2C_Enable_Input(GPIO_Type *port, UINT32 port_pin);
static void LED_I2C_Unable_Input(GPIO_Type *port, UINT32 port_pin);
static UINT8 LED_I2C_Read_Input(GPIO_Type *port, UINT32 port_pin);
static void LED_I2C_Cfg_GPIO_Direction(GPIO_Type *port, UINT32 port_pin, UINT8 direction);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_I2C_Init(void)
{
    /* the communication rate of I2C with backlight IC LP8860 is 400Kbps */
    LED_I2C_GPIO_Init();
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_I2C_Master_Write(UINT8 *reg_addr, UINT8 *reg_data, UINT32 data_size)
{
    UINT32  number=0;
    UINT8   u8ErrorStatus;
    
    if((NULL==reg_data) || (0==data_size))
        return;

    LED_I2C_Start();
    
    /* send slave address */
    u8ErrorStatus = LED_I2C_Send_One_Byte(LED_SLAVE_ADDRESS_WRITE);
    
    if( I2C_ERROR_NULL == u8ErrorStatus )
    {   
        for(number=0; number<data_size; number++)
        {
            /* send register address */
            u8ErrorStatus = LED_I2C_Send_One_Byte(reg_addr[number]);
            
            if( I2C_ERROR_NULL == u8ErrorStatus )
            {
                LED_I2C_Send_One_Byte(reg_data[number]);
            }
        }
    }

    LED_I2C_Stop();
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_I2C_Master_Read(UINT8 reg_addr, UINT8 *read_data, UINT32 data_size)
{
    UINT32  number=0;
    UINT8   u8ErrorStatus;

    if((NULL==read_data) || (0==data_size))
        return;

    LED_I2C_Start();

    /* send slave address */
    u8ErrorStatus = LED_I2C_Send_One_Byte(LED_SLAVE_ADDRESS_WRITE);

    if( I2C_ERROR_NULL == u8ErrorStatus )
    {
        u8ErrorStatus = LED_I2C_Send_One_Byte(reg_addr);
        if( I2C_ERROR_NULL == u8ErrorStatus )
        {
            LED_I2C_Restart();
            u8ErrorStatus = LED_I2C_Send_One_Byte(LED_SLAVE_ADDRESS_READ);
            if( I2C_ERROR_NULL == u8ErrorStatus )
            {
                for(number=0; number<data_size-1; number++)
                {
                    LED_I2C_Read_One_Byte(&(read_data[number]), I2C_ACK);
                }
                LED_I2C_Read_One_Byte(&(read_data[number]), I2C_NACK);
            }
        }
    }

    LED_I2C_Stop();
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_I2C_Set_PWM(UINT8 pwm_level)
{
	UINT16 bri_reg=0;
	UINT8 bri[3]={0x28};
	if(pwm_level>100)
		pwm_level=100;
	bri_reg=(UINT16)(pwm_level*655);
	bri[1]=(UINT8)(bri_reg&0x0F);
	bri[2]=(UINT8)(bri_reg>>0x08);
	I2C_MasterSendWait(I2C1, LED_SLAVE_ADDRESS_WRITE, bri,3);
#if 0
    if(pwm_level)
        OUTPUT_SET(PTE,PTE5);
    else
        OUTPUT_CLEAR(PTE,PTE5);
#endif

}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void LED_I2C_Set_Power_En(UINT8 power_level)
{
    if(power_level)
    {
        OUTPUT_SET(PTE,PTE6);
        Power_state=1;
    }
    else
    {
        OUTPUT_CLEAR(PTE,PTE6);
        Power_state=0;
    }
}
    
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Start(void)
{
    LED_SCL_HIGH;
    LED_SDA_HIGH;
    LED_I2C_Delay(TIME_TWO_UNIT);
    LED_SDA_LOW;
    LED_I2C_Delay(TIME_SIX_UNIT); 
    LED_SCL_LOW;
    LED_I2C_Delay(TIME_FIVE_UNIT); 
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Restart(void)
{
    LED_I2C_Delay(TIME_THREE_UNIT); 
    LED_SDA_HIGH;
    LED_I2C_Delay(TIME_TWO_UNIT); 
    LED_SCL_HIGH;
    LED_I2C_Delay(TIME_THREE_UNIT); 
    LED_SDA_LOW;
    LED_I2C_Delay(TIME_TWO_UNIT); 
    LED_SCL_LOW;
    LED_I2C_Delay(TIME_TWO_UNIT); 
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Stop(void)
{
    LED_SDA_LOW;
    LED_I2C_Delay(TIME_FIVE_UNIT);
    LED_SCL_HIGH;
    LED_I2C_Delay(TIME_FIVE_UNIT);
    LED_SDA_HIGH;
    LED_I2C_Delay(TIME_ELEVEN_UNIT);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static UINT8 LED_I2C_Send_One_Byte(UINT8 byte)
{
    UINT32  timer=0;
    UINT8   bit_number=0;
    UINT8   u8ErrorStatus;

    /* total time of  a cycle is 6.441us*/
    for(bit_number=8; bit_number>0; bit_number--)
    {
        LED_I2C_Delay(TIME_THREE_UNIT);
        if(byte&(0x01<<(bit_number-1)))
            LED_SDA_HIGH;
        else
            LED_SDA_LOW;
        LED_I2C_Delay(TIME_TWO_UNIT);
        LED_SCL_HIGH;   /* start to sample */
        LED_I2C_Delay(TIME_TEN_UNIT);
        LED_SCL_LOW;    /* stop to sample */
        LED_I2C_Delay(TIME_SIX_UNIT); 
    }
    
    /* wait ACK responce */
//    LED_SDA_HIGH;
//    OUTPUT_CLEAR(PTC, PTC6);
//    CONFIG_PIN_AS_GPIO(PTC, PTC6, INPUT);  /* Configure LED_SDA(PTC6) as an input */
//    GPIO_ENABLE_INPUT(PTC, PTC6);
//    PTC->PDDR &= ~(0x00000001 << PTC6);
    LED_SDA_LOW;
//    LED_I2C_Cfg_GPIO_Direction(PTC, PTC6, INPUT);
    LED_I2C_Enable_Input(PTC, PTC6);
//    OUTPUT_CLEAR(PTC, PTC6);
    LED_I2C_Delay(TIME_FIVE_UNIT); 

    LED_SCL_HIGH; /* start to sample */
    timer=0;
    while( (0!=LED_I2C_Read_Input(PTC, PTC6)) && (timer++<TIME_FIVE_UNIT));
    if(timer >=TIME_TEN_UNIT)
        u8ErrorStatus = I2C_ERROR_NO_GET_ACK;   /* no ack */
    else
        u8ErrorStatus = I2C_ERROR_NULL;         /* get ack */

    while(timer++<TIME_FIVE_UNIT);/* In order to align at clock */
    LED_SCL_LOW; /* stop to sample */
    
//    LED_I2C_Delay(TIME_TWO_UNIT);     
//    LED_I2C_Cfg_GPIO_Direction(PTC, PTC6, OUTPUT);
    LED_I2C_Unable_Input(PTC, PTC6);
//    PTC->PDDR |= 0x00000001 << PTC6;
//    CONFIG_PIN_AS_GPIO(PTC, PTC6, OUTPUT);  /* Configure LED_SDA(PTC6) as an output */
//    LED_SDA_HIGH;
    return u8ErrorStatus;
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Read_One_Byte(UINT8 *byte, LED_I2C_ACK ack)
{
    UINT8   bit_number=0;

//    OUTPUT_TOGGLE(PTC, PTC6);
    LED_SDA_LOW;
//    OUTPUT_CLEAR(PTC, PTC6);
    LED_I2C_Cfg_GPIO_Direction(PTC, PTC6, INPUT);
//    CONFIG_PIN_AS_GPIO(PTC, PTC6, INPUT);  /* Configure LED_SDA(PTC6) as an input */
//    GPIO_ENABLE_INPUT(PTC, PTC6);
    for(bit_number=0; bit_number<8; bit_number++)
    {
        *byte = (*byte)<<1;
        LED_I2C_Enable_Input(PTC, PTC6);
        LED_I2C_Delay(TIME_FOUR_UNIT); 
//        LED_SDA_LOW;
        /* start to sample */
        LED_SCL_HIGH;
        LED_I2C_Delay(TIME_FOUR_UNIT); 
        *byte += LED_I2C_Read_Input(PTC, PTC6);
        LED_I2C_Delay(TIME_FIVE_UNIT); 
        LED_SCL_LOW;

        LED_I2C_Unable_Input(PTC, PTC6);
        
        LED_I2C_Delay(TIME_FOURTEEN_UNIT); 
    }

    /* send ack */
    LED_I2C_Cfg_GPIO_Direction(PTC, PTC6, OUTPUT);
//    CONFIG_PIN_AS_GPIO(PTC, PTC6, OUTPUT);  /* Configure LED_SDA(PTC6) as an input */
//    PTC->PIDR |= 0x00000001 << PTC6;
//    PTC->PDDR |= 0x00000001 << PTC6;

//    LED_I2C_Delay(TIME_TWO_UNIT); 
    if(I2C_ACK == ack)
        LED_SDA_LOW;
    else
        LED_SDA_HIGH;
//    LED_I2C_Delay(TIME_TWO_UNIT);
    LED_SCL_HIGH;   /* slave start to sample */
    LED_I2C_Delay(TIME_TEN_UNIT);
    LED_SCL_LOW;    /* slave stop to sample */
    LED_I2C_Delay(TIME_SIX_UNIT);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_GPIO_Init(void)
{
	UINT32 i;

    /* I2C */
//    CONFIG_PIN_AS_GPIO(PTC, PTC7, OUTPUT);  /* Configure LED_SCL(PTC7) as an output */
//    CONFIG_PIN_AS_GPIO(PTC, PTC6, OUTPUT);  /* Configure LED_SDA(PTC6) as an output */
//    OUTPUT_SET(PTC,PTC6);                   /* High Level */
//    OUTPUT_SET(PTC,PTC7);                   /* High Level */
//    ENABLE_PULLUP(PTC,PTC6);
    Power_state=1;
    /* PWM dimming input. */
    CONFIG_PIN_AS_GPIO(PTE,PTE5,OUTPUT); /* Configure LED_PWM as an output */
    OUTPUT_SET(PTE,PTE5);
	i=0x000C3500;	
    while(i--);//  delay 
    Power_state=1;
    /* LED_FAULT */
    CONFIG_PIN_AS_GPIO(PTG,PTG3,INPUT);
    GPIO_ENABLE_INPUT(PTG,PTG3);
	    /* VDDIO/EN */
    CONFIG_PIN_AS_GPIO(PTE,PTE6,OUTPUT); /* Configure LED_EN as an output */
    OUTPUT_SET(PTE,PTE6); 
		i=0x000C3500;	
    while(i--);//  delay 
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
UINT8 LED_I2C_Fault_Status_Read(void)
{
    return GPIO_READ_INPUT(PTG,PTG3);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
UINT8 LED_I2C_Power_En_Check(void)
{
    return Power_state;
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static UINT8 LED_I2C_Read_Input(GPIO_Type *port, UINT32 port_pin)
{
    return ((port->PDIR)>>port_pin)&0x00000001;
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Delay(UINT32  timer)
{
    while(timer--);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Enable_Input(GPIO_Type *port, UINT32 port_pin)
{
    port->PIDR &= ~(0x00000001 << port_pin);
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Unable_Input(GPIO_Type *port, UINT32 port_pin)
{
    port->PIDR |= 0x00000001 << port_pin;
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
static void LED_I2C_Cfg_GPIO_Direction(GPIO_Type *port, UINT32 port_pin, UINT8 direction)
{
    if( INPUT == direction )
        port->PDDR &= ~(0x00000001 << port_pin);
    else
        port->PDDR |= 0x00000001 << port_pin;
}
#endif /* LED_MODULE */
