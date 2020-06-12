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

#ifndef _TIMER_IF_H_
#define _TIMER_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

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
typedef enum
{
    EN_SYSTEM_STARTUP_CMD_TIMER=0x00    /* 2s, used for startup command */
    ,EN_DESER_SEND_DATA_TIMER       /* 100ms, used for interrupt pin, CRQ */
    ,EN_CTP_IC_STARTUP_TIMER        /* 40ms, wait for 40ms after reset, CTP can work */
    ,EN_LCD_STABDBY_LOW_KEEP        /* 70ms, when the module deinit */
    ,EN_DESER_PDB_LOW_KEEP          /* 70ms, when the module deinit */
    ,EN_UPGRADE_DIS_RESET           /* 1s, if it receives the command of status query command after received the reset commnad, we will send the status command, the timeout is 1s */
    ,EN_UPGRADE_DESER_SEND_DATA    /* 100ms, used for interrupt pin, CRQ */
    ,EN_LED_PREPARE_CLOSE           /* 10s, wait the MMI to close the LED */
    ,EN_DESER_LOCK_LAST_STATUS      /* 5s, the time of deserializer error last foe 5s */
    ,END_OF_TIMER_BYTE 

}EN_TIMER_BYTE;

/* It's counted by 2ms task,  Asynchronous timing */
#define TIMER_UP            0
#define TIMER_COUNT_8MS     4   /* 8/2: 8ms*/
#define TIMER_COUNT_16MS    8   /* 16/2: 16ms*/
#define TIMER_COUNT_20MS    10  /* 20/2: 20ms*/
#define TIMER_COUNT_40MS    20  /* 40/2: 40ms */
#define TIMER_COUNT_32MS    16  /* 32/2: 32ms*/
#define TIMER_COUNT_64MS    32  /* 64/2: 64ms*/
#define TIMER_COUNT_70MS    35  /* 70/2: 70ms*/
#define TIMER_COUNT_80MS    40  /* 80/2: 80ms*/
#define TIMER_COUNT_90MS    45  /* 90/2: 90ms*/
#define TIMER_COUNT_100MS   50  /* 100/2: 100ms*/
#define TIMER_COUNT_180MS   90  /* 180/2: 180ms*/
#define TIMER_COUNT_500MS   250  /* 500/2: 500ms*/
#define TIMER_COUNT_1S      500  /* 1000/2: 1000ms*/
#define TIMER_COUNT_2S      1000  /* 2000/2: 2000ms*/
#define TIMER_COUNT_5S      2500   /* 5000/2: 5000ms */
#define TIMER_COUNT_10S     5000  /* 10000/2: 10000ms*/
#define TIMER_COUNT_120S    60000  /* 120000/2: 120s*/



/* synchronous timing, the bus clock is 20M  */
#define TIMER_DELAY_1US     8       /* 125ns*8 = 1us */
#define TIMER_DELAY_2US     16      /* 125ns*16 = 2us */
#define TIMER_DELAY_3US     24      /* 125ns*24 = 3us */
#define TIMER_DELAY_4US     32      /* 125ns*32 = 4us */
#define TIMER_DELAY_5US     40      /* 125ns*40 = 5us */
#define TIMER_DELAY_10US    80      /* 125ns*80 = 10us */
#define TIMER_DELAY_20US    160     /* 125ns*160 = 20us */
#define TIMER_DELAY_30US    240     /* 125ns*240 = 30us */
#define TIMER_DELAY_40US    320     /* 125ns*320 = 40us */
#define TIMER_DELAY_50US    400     /* 125ns*400 = 50us */
#define TIMER_DELAY_100US   800     /* 125ns*800 = 100us */
#define TIMER_DELAY_200US   1600    /* 125ns*1600 = 200us */
#define TIMER_DELAY_300US   2400    /* 125ns*2400 = 300us */
#define TIMER_DELAY_400US   3200    /* 125ns*3200 = 400us */
#define TIMER_DELAY_500US   4000    /* 125ns*4000 = 500us */
#define TIMER_DELAY_1MS     8000    /* 125ns*8000 = 1ms */
#define TIMER_DELAY_2MS     16000   /* 125ns*16000 = 2ms */
#define TIMER_DELAY_3MS     24000   /* 125ns*24000 = 3ms */
#define TIMER_DELAY_4MS     32000   /* 125ns*32000 = 4ms */
#define TIMER_DELAY_5MS     40000   /* 125ns*40000 = 5ms */
#define TIMER_DELAY_6MS     48000   /* 125ns*48000 = 5ms */
#define TIMER_DELAY_10MS    80000   /* 125ns*80000 = 10ms */
#define TIMER_DELAY_20MS    160000  /* 125ns*160000 = 20ms */
#define TIMER_DELAY_30MS    240000  /* 125ns*240000 = 30ms */
#define TIMER_DELAY_40MS    320000  /* 125ns*320000 = 40ms */
#define TIMER_DELAY_50MS    400000  /* 125ns*400000 = 50ms */
#define TIMER_DELAY_70MS    560000  /* 125ns*700000 = 70ms */
#define TIMER_DELAY_100MS   800000  /* 125ns*800000 = 100ms */


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
extern void Timer_If_Count(void);
extern void Timer_If_Set(EN_TIMER_BYTE en_timer,uint32_t time);
extern void Timer_If_Clear(EN_TIMER_BYTE en_timer);
extern uint8_t Timer_If_Check(EN_TIMER_BYTE en_timer);
extern void Timer_If_Delay(uint32_t delay_time);

#ifdef __cplusplus
}
#endif

#endif
