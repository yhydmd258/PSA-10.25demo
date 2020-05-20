/******************************************************************************
*  Name: Main.c
*  Description:
*  Project: SGM
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/
#include "project_cfg.h"
#ifdef  DIAGNOSIS_MODULE

#ifndef _DIAGNOSIS_CTRL_H_
#define  _DIAGNOSIS_CTRL_H_
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

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Diagnosis_Ctrl_Init(void);
extern void Diagnosis_Ctrl_Cmd_Response(void);
extern void Diagnosis_Ctrl_Cmd_Analyse(uint8_t *cmd_buf);
extern void Diagnosis_Ctrl_Fun_Defect(void);
extern void Diagnosis_Ctrl_Voltage_Range(void);
extern void Diagnosis_Ctrl_Version(void);

#endif/* _DIAGNOSIS_IF_H_ */

#endif/*DIAGNOSIS_MODULE*/
