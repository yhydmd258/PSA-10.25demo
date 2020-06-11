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

#include "CTP_if.h"
#include "CTP_ctrl.h"
#include "../Command/command.h"
#include "../Timer/timer_if.h"

#ifdef  DESERIALIZE_MODULE
#include "../Deserialize/deserialize_if.h"
#endif  
#ifdef LED_MODULE
#include "../LED/LED_if.h"
#endif

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/


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
void CTP_If_Init(void)
{
    CTP_Ctrl_Init();
}

/***********************************************************************************************
*
* @brief    Deinitialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_If_Deinit(void)
{
    CTP_Ctrl_Deinit();
}

/***********************************************************************************************
*
* @brief    KBI interrupt, CTP IC notices the module to read the message
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_If_Touch_Interrupt_Notice(void)
{
//    CTP_Ctrl_Int_Flag_Set(CTP_Ctrl_Int_Flag_Get()+1);
    CTP_Ctrl_Int_Flag_Set(1);
    CTP_Ctrl_Msg_Read();
}

/***********************************************************************************************
*
* @brief    return the version of the touch IC
* @param    none
* @return   uint32_t&0xFF000000 : Firmware Major
                uint32_t&0x00FF0000 : Firmware Minor
                uint32_t&0x0000FF00 : Configuration version MSB
                uint32_t&0x000000FF : Configuration version LSB
*
************************************************************************************************/
uint32_t CTP_If_Version_Read(void)
{
    return CTP_Ctrl_Version_Read();
}

/***********************************************************************************************
*
* @brief    task function, make the command and send to deserializer module
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_If_Task(void)
{
    CTP_Ctrl_Status_Check();
    /* read the touch report */
//    CTP_Ctrl_Msg_Read();
    
    /* make the command according to the touch report, and send it to deserizlizer */
    CTP_Ctrl_Cmd_Make();

    /* analyse the command and response the command */
    CTP_Ctrl_Cmd_Response();
}

/***********************************************************************************************
*
* @brief    this task is called every 8ms, read the message from the touch IC
* @param    none
* @return   none
*
************************************************************************************************/
void CTP_If_8ms_Task(void)
{
//    CTP_Ctrl_Msg_Read();
}

/***********************************************************************************************
*
* @brief    check the CTP is OK or not.
* @param    none
* @return   TRUE: OK
*               FALSE: abnormal
*
************************************************************************************************/
uint8_t CTP_If_Fault_Check(void)
{
    /* CTP IC is CYAT81688-100AS61, Error pin36(P1[4]) in CTP IC don't connected to PCBA by Hardware,
           so the status is TRUE when check fault */
    return CTP_Ctrl_Fault_Check();
}

/***********************************************************************************************
*
* @brief      save the command data which will be sent
* @param     data: the data need to be made into standard frame
                data_size:  the size of the data need to be made into standard frame
                repeat: the repeat times of the data need to be sent
                extend_data: the data need to be made into extended frame
                extend_size: the size of the data need to be made into extended frame
* @return   none
*
************************************************************************************************/
void CTP_If_Cmd_Send_Save(uint8_t* data,uint8_t data_size,uint8_t repeat,uint8_t *extend_data,uint8_t extend_size)
{
#ifdef DESERIALIZE_MODULE
    Deserialize_If_Send_Cmd_Save(data, data_size, repeat, extend_data, extend_size);
#endif
}

/***********************************************************************************************
*
* @brief    analyse the command
* @param    cmd_buf: the pointer of command
                  cmd_size: command size
* @return   none
*
************************************************************************************************/
void CTP_If_Cmd_Analyse(uint8_t *cmd_buf, uint8_t cmd_size)
{
    CTP_Ctrl_Cmd_Analyse(cmd_buf, cmd_size);
}

/***********************************************************************************************
*
* @brief      set the key of VOLUME+_3V3(PTE2)
* @param    level, 0: set low; other: set high;
* @return   none
*
************************************************************************************************/
void CTP_If_Vol_Increase_Set(uint8_t level)
{
#ifdef KEY_MODULE
    Key_If_Vol_Increase_Set(level);
#endif
}

/***********************************************************************************************
*
* @brief      set the key of VOLUME-_3V3(PTE2)
* @param    level, 0: set low; other: set high;
* @return   none
*
************************************************************************************************/
void CTP_If_Vol_Decrease_Set(uint8_t level)
{
#ifdef KEY_MODULE
    Key_If_Vol_Decrease_Set(level);
#endif
}

#endif /* CTP_MODULE */
