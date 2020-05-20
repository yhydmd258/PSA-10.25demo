/******************************************************************************
*  Name: .h
*  Description:
*  Project: SGM
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/
#include "project_cfg.h" 
#ifdef	POWER_MANAGEMENT_MODULE

#ifndef _POWER_IF_H_
#define _POWER_IF_H_
#include "derivative.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
/* input voltage, theoretical value of ADC = input voltage*(20K/(150K+20K))/(3.3V/2^12)= 146.025*input voltage */
#define POWER_INPUT_ADC_5V      0x02B2   /* 690 */
#define POWER_INPUT_ADC_7V      0x03B1   /* 945 */
#define POWER_INPUT_ADC_8_5V    0x04DD  /* 1245 */
#define POWER_INPUT_ADC_12V     0x0702  /* 1794 */
#define POWER_INPUT_ADC_18_2V   0x0ACE  /* 2766 */
#define POWER_INPUT_ADC_18_7V   0x0B0F  /* 2831 */
#define POWER_INPUT_ADC_26_5V   3871

typedef enum
{
    STANDBY_TO_ACTIVE       /* from standby to active */
    , ACTIVE_STATUS         /* wakeup pin is ok, initialization of all modules are ok */
    , ACTIVE_TO_STANDBY     /* from active to standby */
    , STANDBY_ENTERING      /* realize the power off sequence */
    , STANDBY_STATUS        /* standby mode, wait for wakeup */
}POWER_WAKEUP_STATUS;

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
extern void Power_If_Init(void);
extern void Power_If_Deinit(void);
extern void Power_If_Task(void);
extern uint32_t Power_If_Wakeup_Status_read(void);
extern uint16_t Power_If_Voltage_ADC(void);
extern void Power_If_Cmd_Analyse(uint8_t *cmd_buf);
extern void Power_If_Startup_Cmd_Send(uint8_t* data, uint8_t data_size);
extern uint8_t Power_If_Startup_Status_Read(void);
extern void Power_If_Wakeup_KBI_Init(void);
extern void Power_If_Not_Oper_To_Full_Oper(void);
extern void Power_If_Abnormal_To_Full_Oper(void);
extern void Power_If_Full_Oper_To_Not_Operational(void);
extern void Power_If_Abnormal_To_Not_Operational(void);
extern void Power_If_Full_Oper_To_Abnormal(void);
extern void Power_If_Not_Operational_To_Abnormal(void);

#endif /* _POWER_IF_H_ */

#endif /* POWER_MANAGEMENT_MODULE */
