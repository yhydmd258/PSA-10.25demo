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
#ifdef  CTP_MODULE
    
#include <stdio.h>
#include <string.h>
#include "CTP_ctrl.h"
#include "CTP_data.h"
#include "Command/command.h"
#include "I2C.h"
#include "GPIO.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
typedef enum
{
    UNGRIP_EVENT    =0x01
    , SUPPRESS_EVENT=0x02
    , AMP_EVENT     =0x04
    , VECTOR_EVENT =0x08
    , MOVE_EVENT    =0x10
    , RELEASE_EVENT =0x20
    , PRESS_EVENT   =0x40
    , DETECT_EVENT  =0x80
}TOUCH_EVENT_ID;

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
// static void CTP_Data_GPIO_Cfg(void);

/**********************************************************************************************
* Global functions
**********************************************************************************************/



/***********************************************************************************************
*
* @brief    initialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Init(void)
{
    I2C_ConfigType  I2C_Config = {{0}};    
    /* Initialize I2C module with interrupt mode */
    I2C_Config.u16Slt = 0x0000;
//    I2C_Config.u16F = 0x0090; /* Baud rate at 100Kbps, MULT=4,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.u16F = 0x0010; /* Baud rate at 400Kbps, MULT=1,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.sSetting.bMSTEn = 1;
    I2C_Config.sSetting.bIntEn = 0;
    I2C_Config.sSetting.bI2CEn = 1;

    I2C_Init(I2C1, &I2C_Config);
//  I2C_ClearStatus(CTP_IC,I2C_S_IICIF_MASK);
}

/***********************************************************************************************
*
* @brief    Deinitialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Deinit(void)
{
    I2C_Deinit(CTP_IC);
//    CTP_Data_Reset_Set(0);
//    OUTPUT_CLEAR(PTG,PTG1); 

//    CONFIG_PIN_AS_GPIO(PTE, PTE0, INPUT);
//    CONFIG_PIN_AS_GPIO(PTE, PTE1, INPUT);

//    CONFIG_PIN_AS_GPIO(PTG, PTG0, INPUT);
}

/***********************************************************************************************
*
* @brief    send the data to the CTP IC
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   0: OK; other: error
*
************************************************************************************************/
uint8_t CTP_Data_Master_Send(uint8_t *send_data, uint32_t data_size)
{
    if((NULL==send_data) || (0 == data_size))
        return 1;

    /* send data */
//    I2C_IntDisable(CTP_IC);
    return I2C_MasterSendWait(CTP_IC,CTP_IC_SLAVE_ADDRESS,&send_data[0],data_size);
//    I2C_IntEnable(CTP_IC);
}
/***********************************************************************************************
*
* @brief    reg_addr : the address of the register
                read_data : the pointer of the read data
                data_size : the size of the read data
* @param    none
* @return   0: read OK
                other: read error
*
************************************************************************************************/
uint8_t CTP_Data_Master_Read(uint8_t reg_addr, uint8_t *read_data, uint32_t data_size)
{
    uint8_t result=1;
    
    if((NULL==read_data) || (0 == data_size))
        return result;
    
    result = I2C_MasterSendWait(CTP_IC,CTP_IC_SLAVE_ADDRESS,&reg_addr,1);
    if(I2C_ERROR_NULL == result)
        result = I2C_MasterReadWait(CTP_IC,CTP_IC_SLAVE_ADDRESS,read_data,data_size);
    
    return result;
}

/***********************************************************************************************
*
* @brief    the touch information is made the command according to the protocol of SX12  
* @param    touch_data : the pointer of touch information
* @return   none
*
************************************************************************************************/
void CTP_Data_Cmd_Make(TOUCH_COORDINATE_BUF *touch_data)
{
    /*Event codes*/
	#define EVENT_CODE_NO_EVENT 	 0x00/*No event*/   
	#define EVENT_CODE_MOVE 		 0x01/*Significant displacement (>= active distance)*/
	#define EVENT_CODE_DOWN 		 0x04/*Touchdown*/
	#define EVENT_CODE_UP			 0x05/*Liftoff (record reports last known coordinates)*/
	/*touch type*/
 	#define TOUCH_TYPE_RESERVED		 0x00/*Reserved for future use*/
 	#define TOUCH_TYPE_FINGER		 0x01/*Standard finger or thin glove*/
	#define TOUCH_TYPE_GLOVE		 0x05/*Thick glove*/
	#define TOUCH_TYPE_LARGE_TOUCH   0x06/*Large Object touch (if detailed Large Object Report is enabled)*/
	
	#define COMMAND_SIZE_MAX			13

    UINT8   cmd_data[COMMAND_SIZE_MAX]={0};
    UINT8   cmd_data_postion=0;
    TOUCH_REPORT_STRUCT  temp_touch;
	if( (touch_data->read_pos+1)%COORDINATE_BUF_NUMBER == touch_data->write_pos)
        return;
	/* start to communication with deserializer */
    while((touch_data->read_pos+1)%COORDINATE_BUF_NUMBER != touch_data->write_pos)
    {
    	memset(cmd_data,0x00,COMMAND_SIZE_MAX);
	    memcpy(&temp_touch, &(touch_data->buf[touch_data->read_pos]),sizeof(touch_data->buf[touch_data->read_pos]));
		touch_data->read_pos=(touch_data->read_pos+1)%COORDINATE_BUF_NUMBER;	
		/* make  touch position info standard command */
        /*       t[0] = 0x79    sync
                        t[1] = 0x6A   Device Adress 
                        t[2] = 0x02   Register or information type
                        t[3]             length of the follow data
                        t[4]             report ID
                        t[5]             touch type
                        t[6]             event codes
                        t[7] = 0x00   LSB of x coordinate
                        t[8] = 0x00   MSB of x coordinate
                        t[9] = 0x00   LSB of y coordinate
                        t[10] = 0x00  MSB of y coordinate
                        t[11] = 0x6A   end of data
                        t[12] = 0x6A   end of data   */	
                 // make the header of packet
        cmd_data[cmd_data_postion]=COMMAND_SYNC; //D0
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_DEV_ADDR; //D1
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_ID_TOUCH; //D2
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=0x00; //temp set ,after adding message, modify it,D3
        cmd_data_postion++;
		/*report ID*/
		cmd_data[cmd_data_postion]=temp_touch.touch_ID_str.touch_ID;
		/*touch type*/
		cmd_data_postion++;
		switch(temp_touch.touch_ID_str.touch_type2)
		{
			case FINGER_OR_THIN_GLOVE_TYPE:
				cmd_data[cmd_data_postion]=TOUCH_TYPE_FINGER;
				break;
			case LARGE_OBJECT_TYPE:
				cmd_data[cmd_data_postion]=TOUCH_TYPE_LARGE_TOUCH;
				break;
			case THICK_GLOVE_TYPE:
				cmd_data[cmd_data_postion]=TOUCH_TYPE_GLOVE;
				break;
			case RESERVED_TYPE:
			default:
				cmd_data[cmd_data_postion] = TOUCH_TYPE_RESERVED;
				break;
		}
		/* event codes */
		cmd_data_postion++;
		switch (temp_touch.touch_ID_str.event_ID)
		{   
			case TOUCH_DOWN:
				cmd_data[cmd_data_postion] = EVENT_CODE_DOWN;
				break;		
			case MOVING:
				cmd_data[cmd_data_postion] = EVENT_CODE_MOVE;
				break;
			case LIFT_OFF:
				cmd_data[cmd_data_postion] = EVENT_CODE_UP;
				break;
			case NO_EVENT:
			default:
				cmd_data[cmd_data_postion] = EVENT_CODE_NO_EVENT;
				break;
		}
		cmd_data_postion++;
		cmd_data[cmd_data_postion]=temp_touch.touch_y_LSB;
		cmd_data_postion++;
		cmd_data[cmd_data_postion]=temp_touch.touch_y_MSB;
		cmd_data_postion++;
		cmd_data[cmd_data_postion]=temp_touch.touch_x_LSB;
		cmd_data_postion++;
		cmd_data[cmd_data_postion]=temp_touch.touch_x_MSB;
		cmd_data[3] += 7;
		// make the end of packet
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_END_DATA;
        cmd_data_postion++;
        cmd_data[cmd_data_postion]=COMMAND_END_DATA;
        cmd_data[3] += 2;
		CTP_Ctrl_Cmd_Send(cmd_data, cmd_data_postion+1);
    }

	

}

/***********************************************************************************************
*
* @brief    level : 1( CTP IC power on )
                        0( CTP IC power off )
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Reset_Set(uint8_t level)
{
    if(level)
        OUTPUT_SET(PTG,PTG1);
    else
        OUTPUT_CLEAR(PTG,PTG1);
}

/***********************************************************************************************
*
* @brief    CTP power enable
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Power_En_Init(void)
{
    CONFIG_PIN_AS_GPIO(PTH, PTH6, OUTPUT);    /* CTP_EN , the enable pin of the CTP power */
    OUTPUT_SET(PTH, PTH6);
}

/***********************************************************************************************
*
* @brief    CTP power disenable
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Data_Power_En_Deinit(void)
{
    /* CTP_EN , the enable pin of the CTP power */
    OUTPUT_CLEAR(PTH, PTH6);
    /* CTP _reset, low */
    CTP_Data_Reset_Set(0);
}


#if 0
/***********************************************************************************************
*
* @brief    configure the GPIO
* @param    none
* @return   none
*
************************************************************************************************/
static void CTP_Data_GPIO_Cfg(void)
{
    /* CTP IC reset  set high*/
    CONFIG_PIN_AS_GPIO(PTG,PTG1,OUTPUT); 
    OUTPUT_SET(PTG,PTG1); 
}
#endif
#endif /* CTP_MODULE */
