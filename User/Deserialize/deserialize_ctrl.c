#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
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

/*******************************************************************************
* Includes
********************************************************************************/
#include "derivative.h" 

#include "deserialize_ctrl.h"
#include "deserialize_data.h"
#include "deserialize_if.h"

/***********************************************************************************************
*
* @brief    deserialize_ctrl_init - Initalize data layer
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Ctrl_Init(void)
{
    Deserialize_Data_Init();
}

/***********************************************************************************************
*
* @brief    deserialize_ctrl_init - Initalize data layer
* @param    none
* @return   none
*
************************************************************************************************/  
extern void Deserialize_Ctrl_Cmd_Send(UINT8* data, UINT8 data_size)
{
    Deserialize_Data_Send(data, data_size);    
}

/***********************************************************************************************
*
* @brief    deserialize_data_init - driver init and Initalize deserialize register
* @param    none
* @return   none
*
************************************************************************************************/  
UINT8 Deserialize_Ctrl_Send_En(void)
{
    return Deserialize_Data_Send_En();
}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_Ctrl_Set_Cds(UINT8 level)
{
    Deserialize_Data_Set_Cds(level);
}

/***********************************************************************************************
*
* @brief    Deserialize_Data_GPIO_INIT - set the GPIO of deserialize
* @param    none
* @return   none
*
************************************************************************************************/  
UINT8 Deserialize_Ctrl_Get_Backlight(void)
{
	return Deserialize_Data_Backlight_sta.backlight;
}
UINT8 Deserialize_Ctrl_Backlight_Flag(void)
{
	return Deserialize_Data_Backlight_sta.update_flag;
}
void Deserialize_Ctrl_Backlight_Flag_update(UINT8 sta)
{
	Deserialize_Data_Backlight_sta.update_flag=sta;
}

#endif /* DESERIALIZE_MODULE */
