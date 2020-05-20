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

#ifndef _DIAGNOSIS_IF_H_
#define  _DIAGNOSIS_IF_H_
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
#define VOLTAGE_ADC_9V      0x0500
#define VOLTAGE_ADC_10V     0x0600
#define VOLTAGE_ADC_12V     0x0702
#define VOLTAGE_ADC_15V     0x0900
#define VOLTAGE_ADC_16V     0x09CE

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Diagnosis_If_Init(void);
extern void Diagnosis_If_First_Report(void);
extern void Diagnosis_If_Task(void);
extern void Diagnosis_If_Cmd_Analyse(uint8_t *cmd_buf);
extern void Diagnosis_If_Cmd_Send_Save(uint8_t* data, uint8_t data_size);
extern uint16_t Diagnosis_If_Volt_ADC_Get(void);
extern void Diagnosis_If_Cmd_Send_Save(uint8_t* data, uint8_t data_size);
extern uint8_t Diagnosis_If_LED_Check(void);
extern uint8_t Diagnosis_If_LCD_Check(void);
extern uint8_t Diagnosis_If_CTP_Check(void);
extern uint8_t Diagnosis_If_Display_Temper_Check(void);

#endif/* _DIAGNOSIS_IF_H_ */

#endif/*DIAGNOSIS_MODULE*/
