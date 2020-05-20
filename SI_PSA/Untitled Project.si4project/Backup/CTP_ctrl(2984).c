#include "project_cfg.h" 
#ifdef  CTP_MODULE
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
#include <string.h>
#include "CTP_if.h"
#include "CTP_ctrl.h"
#include "CTP_data.h"
#include "atmel_1189.h"
#include "Command/command.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
// static UINT8 Touch_Int = false;

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/


/**********************************************************************************************
* Local types
**********************************************************************************************/
static UINT8 Chip_Detected_Flag = false;
TOUCH_COORDINATE_BUF Touch_Coordinate_Buf;

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/


/**********************************************************************************************
* Local functions
**********************************************************************************************/


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
void CTP_Ctrl_Init(void)
{
    CTP_Data_Init();//浣胯兘渚涚數CTP Pin,鍒濆鍖朇TP GPIO,鍒濆鍖朚CU IIC妯″潡

    Touch_Coordinate_Buf.write_pos=0;
    Touch_Coordinate_Buf.read_pos=COORDINATE_BUF_NUMBER-1;
    memset(Touch_Coordinate_Buf.buf, 0x00, sizeof(Touch_Coordinate_Buf.buf));
    Touch_Coordinate_Buf.action=0;
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Standby_Set(UINT8 standby_state)
{
    if(STANDBY_ON == standby_state)
    {
        CTP_Data_Reset_Clear();
        Chip_Detected_Flag = false;
    }
    else
    {
        CTP_Ctrl_Init();
        CTP_Ctrl_Touch_Init();
    }
}
/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Touch_Init(void)
{
    UINT32 i;
    
    i=0x000B0000;/*90.2ms*/
    while(i--);//  delay 
    
    Chip_Detected_Flag = init_touch_app();  // find and initialise QT device 
    if(Chip_Detected_Flag == true)
    {
        while(ChangeLineStatus() == CHANGELINE_NEGATED)
        {
            get_message();
        }
    }
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Task(void)
{
#if 0
    UINT32  temp_time=0;
    //------- 用查询CHG 的方式读取信号 --------------------      
    if(Chip_Detected_Flag == true)
    {  
        if(ChangeLineStatus() == CHANGELINE_NEGATED)
//                if(Touch_Int == true)
        {
//            get_touch_message();
            CTP_Ctrl_Des_Set_Cds(1);
            get_touch_message_for_PSA1();
            temp_time=0xC350;/* delay 10ms*/
            while(temp_time--);
            CTP_Ctrl_Des_Set_Cds(0);
        }
    }
#endif  
    if(Chip_Detected_Flag == true)
    {  
        if(ChangeLineStatus() == CHANGELINE_NEGATED)
        {
            get_touch_message_for_PSA(&Touch_Coordinate_Buf);
        }
    }
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Cmd_Make(void)
{
    CTP_Data_Cmd_make(&Touch_Coordinate_Buf);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Cmd_Send(UINT8* data, UINT8 data_size)
{
    CTP_If_Cmd_Send(data, data_size);
}

/***********************************************************************************************
*
* @brief    CTP_If_Init() - Program entry function
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_Ctrl_Des_Set_Cds(UINT8 level)
{
    CTP_If_Des_Set_Cds(level);
}
#endif /* CTP_MODULE */
