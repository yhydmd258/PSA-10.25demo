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
#include "project_cfg.h" 
#ifdef LCD_MODULE

#include "spi.h"
#include "GPIO.h"
#include "../Timer/timer_if.h"
#include "LCD_ctrl.h"
#include "LCD_data.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/


/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define LCD_SPI         SPI0
    
#define LCD_READ_MODE   1
#define LCD_WRITE_MODE  0
#define LCD_SPI_CS_LOW      OUTPUT_CLEAR(PTB, PTB5)
#define LCD_SPI_CS_HIGH     OUTPUT_SET(PTB, PTB5)

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/


/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void LCD_Data_GPIO_Init(void);
static void LCD_Data_Cs_Init(void);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    init SPI module
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Data_Init(void)
{
    SPI_ConfigType sSPIConfig = {{0}};

    LCD_Data_GPIO_Init();

#if 0
    /* 4 wires of SPI */
    sSPIConfig.u32BitRate  = 1000000; // 1Mbps
    sSPIConfig.u32BusClkHz = 20000000;
    sSPIConfig.sSettings.bModuleEn          = 1; // enable SPI module
    sSPIConfig.sSettings.bMasterMode        = 1; // master mode
    sSPIConfig.sSettings.bClkPhase1         = 0; // clock phase
    sSPIConfig.sSettings.bClkPolarityLow    = 0; // clock polarity
#else
    /* 3 wires of SPI */
    sSPIConfig.u32BitRate  = 1000000; // 1Mbps
    sSPIConfig.u32BusClkHz = 20000000;
    sSPIConfig.sSettings.bModuleEn          = 1; // enable SPI module
    sSPIConfig.sSettings.bMasterMode        = 1; // master mode
    sSPIConfig.sSettings.bClkPhase1         = 1; // clock phase
    sSPIConfig.sSettings.bClkPolarityLow    = 1; // clock polarity
    sSPIConfig.sSettings.bBidirectionModeEn = 1; // Bidirectional mode output enable 
    sSPIConfig.sSettings.bPinAsOuput       = 1; // enables bidirectional in same line
#endif

    SPI_Init(LCD_SPI, &sSPIConfig);
		//set cs pin high
    LCD_Data_Cs_Init();
    
//    Timer_If_Delay(TIMER_DELAY_10MS);
}

/***********************************************************************************************
*
* @brief    Deinit SPI module
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Data_Deinit(void)
{
    SPI_DeInit(LCD_SPI);
    
    GPIO_DISABLE_INPUT(PTC,PTC0);   /* LCD_FAIL_T(PTC0): fail feedback */
}

/***********************************************************************************************
*
* @brief    Deinit SPI module
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Data_GPIO_Standby_High(void)
{
    OUTPUT_SET(PTB,PTB1);
}
/***********************************************************************************************
*
* @brief    write the register of LCD IC(NT51625TT)/(RM5365B)
* @param    IC_SID: the ID of IC
*               reg: register address
*               register_data: register data
* @return   none
*
************************************************************************************************/
void LCD_Data_Register_Write(LCD_IC_SID IC_SID, uint8_t reg, uint8_t register_data)
{
    uint8_t send_reg=0;
    
    /* set write mode */
    send_reg += (LCD_WRITE_MODE&0x01)<<7;
    /* set sid */
    send_reg += (IC_SID&0x03)<<5;
    send_reg += reg&0x1F;

    /* delay 10us */
    Timer_If_Delay(TIMER_DELAY_10US);
    LCD_SPI_CS_LOW;

    /* output */
    SPI_BidirOutEnable(LCD_SPI);

    while(!SPI_IsSPTEF(LCD_SPI));
    SPI_WriteDataReg(LCD_SPI, send_reg);
    while(!SPI_IsSPRF(LCD_SPI));
    SPI_ReadDataReg(LCD_SPI);
    while(!SPI_IsSPTEF(LCD_SPI));
    SPI_WriteDataReg(LCD_SPI, register_data);
    while(!SPI_IsSPRF(LCD_SPI));
    SPI_ReadDataReg(LCD_SPI);

    /* delay 20us */
    Timer_If_Delay(TIMER_DELAY_20US);
    LCD_SPI_CS_HIGH;
}

/***********************************************************************************************
*
* @brief    read the register of LCD IC(NT51625TT)
* @param    IC_SID: the ID of IC
*               reg: register address
*               register_data: the pointer of register data which need to be read
* @return   none
*
************************************************************************************************/
void LCD_Data_Register_Read( LCD_IC_SID IC_SID, uint8_t reg, uint8_t *register_data)
{
    uint8_t send_reg=0;
    
    /* set write mode */
    send_reg += (LCD_READ_MODE&0x01)<<7;
    /* set sid */
    send_reg += (IC_SID&0x03)<<5;
    send_reg += reg&0x1F;

    Timer_If_Delay(TIMER_DELAY_10US);
    LCD_SPI_CS_LOW;

    /* output */
    SPI_BidirOutEnable(LCD_SPI);

    while(!SPI_IsSPTEF(LCD_SPI));
    SPI_WriteDataReg(LCD_SPI, send_reg);
    while(!SPI_IsSPRF(LCD_SPI));
    *register_data = SPI_ReadDataReg(LCD_SPI);
        
    /* intput */
    SPI_BidirOutDisable(LCD_SPI);
    
    while(!SPI_IsSPTEF(LCD_SPI));
    SPI_WriteDataReg(LCD_SPI, 0xFF);
    while(!SPI_IsSPRF(LCD_SPI));
    *register_data = SPI_ReadDataReg(LCD_SPI);

    Timer_If_Delay(TIMER_DELAY_20US);
    LCD_SPI_CS_HIGH;
}

/***********************************************************************************************
*
* @brief    into the standby mode
* @param    none
* @return   none
*
************************************************************************************************/
void LCD_Data_Standby(void)
{
    OUTPUT_CLEAR(PTB,PTB0); /* LCD_RESET(PTB0):*/
    OUTPUT_CLEAR(PTB,PTB1);   /* LCD_STBYB(PTB1):  request LCD into standby */
}

/***********************************************************************************************
*
* @brief    detect the error pin
* @param    none
* @return   1: error
                 0: normal
*
************************************************************************************************/
uint8_t LCD_Data_Fault_Detect(void)
{
    return GPIO_READ_INPUT(PTC,PTC0);
}


/***********************************************************************************************
*
* @brief    LCD_Spi_GPIO_Init() - GPIO init
* @param    none
* @return   none
*
************************************************************************************************/
static void LCD_Data_GPIO_Init(void)
{
    SIM_SOPT0 &=~(SIM_SOPT0_NMIE_MASK);   /* set PTB4 as not NMI */
#if 0
    /* SPI */
    CONFIG_PIN_AS_GPIO(PTB,PTB5,OUTPUT);  /* LCD_CSB(PTB5) */
    OUTPUT_SET(PTB,PTB5);
    CONFIG_PIN_AS_GPIO(PTB,PTB2,OUTPUT);  /* LCD_SCL(PTB2) */
    OUTPUT_SET(PTB,PTB2);
    CONFIG_PIN_AS_GPIO(PTB,PTB3,OUTPUT);  /* LCD_SDI(PTB3) */
    OUTPUT_SET(PTB,PTB3);
    CONFIG_PIN_AS_GPIO(PTB,PTB4,INPUT);  /* Configure LCD_SDO(PTB4) as an input */
    GPIO_ENABLE_INPUT(PTB,PTB4);
#endif	
	
	//Control signal
    CONFIG_PIN_AS_GPIO(PTB,PTB0,OUTPUT);  /* LCD_RESET(PTB0):*/
    OUTPUT_SET(PTB,PTB0);
    CONFIG_PIN_AS_GPIO(PTB,PTB1,OUTPUT);  /* LCD_STBYB(PTB1):  request LCD into standby */
    OUTPUT_CLEAR(PTB,PTB1);
//    OUTPUT_SET(PTB,PTB1);
    CONFIG_PIN_AS_GPIO(PTA,PTA7,OUTPUT);  /* LCD_IFSELO:  request LCD into standby */
    OUTPUT_SET(PTA,PTA7);
    CONFIG_PIN_AS_GPIO(PTC,PTC2,OUTPUT); /* Configure LCD_UPDN(PTC2) as an output */
    OUTPUT_CLEAR(PTC,PTC2);
    CONFIG_PIN_AS_GPIO(PTC,PTC3,OUTPUT); /* Configure LCD_SHLR(PTC3) as an output */
    OUTPUT_CLEAR(PTC,PTC3);
    CONFIG_PIN_AS_GPIO(PTC,PTC0,INPUT);   /* LCD_FAIL_T(PTC0): fail feedback */
    GPIO_ENABLE_INPUT(PTC,PTC0);
	
}

/***********************************************************************************************
*
* @brief    LCD_Spi_Cs_Init() - Cs pin of 4 wires SPI
* @param    none
* @return   none
*
************************************************************************************************/
static void LCD_Data_Cs_Init(void)
{
    CONFIG_PIN_AS_GPIO(PTB,PTB5,OUTPUT);  /* LCD_CSB(PTB5) */
    OUTPUT_SET(PTB,PTB5);
}

#endif /* LCD_MODULE */
