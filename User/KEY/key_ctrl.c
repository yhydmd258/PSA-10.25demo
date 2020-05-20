/******************************************************************************
*  Name: .c
*  Description:
*  Project: SGM
*  Auther:  shanghai tianma
*              R&D center
*              penghu
*  MCU: S9KEAZ128AMLH
*  Comment:
******************************************************************************/
#include "project_cfg.h" 
#ifdef	KEY_MODULE
#include <string.h>
#include "gpio.h"
#include "../Command/command.h"

#include "key_if.h"
#include "key_ctrl.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
enum KEY_BACKLIGHT_LEVEL
{
    KEY_BACKLIGHT_LEVEL0=0x00,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL1,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL2,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL3,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL4,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL5,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL6,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL7,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL8,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL9,    /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL10,   /* key backlight brightness level*/
    KEY_BACKLIGHT_LEVEL_END
};
    
/* the key backlight value */
const uint8_t Key_Pwm[KEY_BACKLIGHT_LEVEL_END]=
    {
        0,  /* KEY_BACKLIGHT_LEVEL0 */
        1, /* KEY_BACKLIGHT_LEVEL1 */
        8, /* KEY_BACKLIGHT_LEVEL2 */
        20, /* KEY_BACKLIGHT_LEVEL3 */
        31, /* KEY_BACKLIGHT_LEVEL4 */
        43, /* KEY_BACKLIGHT_LEVEL5 */
        54, /* KEY_BACKLIGHT_LEVEL6 */
        66, /* KEY_BACKLIGHT_LEVEL7 */
        77, /* KEY_BACKLIGHT_LEVEL8 */
        89, /* KEY_BACKLIGHT_LEVEL9 */
        100 /* KEY_BACKLIGHT_LEVEL10 */
    };

static uint8_t Key_Backlight_Level = KEY_BACKLIGHT_LEVEL5;
static uint8_t Key_Backlight_Response_Flag = FALSE;

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
static void Key_Ctrl_GPIO_Init(void);
static void Key_Ctrl_GPIO_Deinit(void);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief      Init the Key pwm
* @param    none
* @return   none
*
************************************************************************************************/
void Key_Ctrl_Init(void)
{
    Key_Ctrl_GPIO_Init();
    Key_If_Pwm_Set((uint16_t)Key_Pwm[Key_Backlight_Level]);
}

/***********************************************************************************************
*
* @brief      Deinit the Key pwm
* @param    none
* @return   none
*
************************************************************************************************/
void Key_Ctrl_Deinit(void)
{
    Key_Backlight_Response_Flag = FALSE;
    Key_If_Pwm_Set(0x0000);
    Key_Ctrl_GPIO_Deinit();
}

/***********************************************************************************************
*
* @brief      analyze the command received from the MMI
* @param    none
* @return   none
*
************************************************************************************************/
void Key_Ctrl_Pwm_Cmd_Analyze(uint8_t* cmd_data)
{
    uint8_t key_pwm_level = KEY_BACKLIGHT_LEVEL0;

    switch(cmd_data[0])
    {
        case CMD_REV_ID_KEY_BACKLIGHT:
            if(KEY_BACKLIGHT_OFF == cmd_data[1])
            {
                /* close the key backlight */
                key_pwm_level = KEY_BACKLIGHT_LEVEL0;
            }
            else
            {
                /* open the key backlight */
                if(KEY_BACKLIGHT_LEVEL_END>cmd_data[2])
                {
                    /* the backlight value is valid */
                    key_pwm_level = cmd_data[2];
                }
                else
                {
                    /* the backlight value is invalid, */
                    key_pwm_level = KEY_BACKLIGHT_LEVEL5;
                }
            }

            /* set key PWM */
            Key_If_Pwm_Set((uint16_t)Key_Pwm[key_pwm_level]);
            /* storage the level */
            Key_Backlight_Level = key_pwm_level;
            /* set the response flag */
            Key_Backlight_Response_Flag = TRUE;
            break;
        default:
            break;
    }
}

/***********************************************************************************************
*
* @brief      make the response command of the key pwm and send it
* @param    none
* @return   none
*
************************************************************************************************/
void Key_Ctrl_Pwm_Cmd_Make(void)
{
    #define KEY_PWM_SEND_CMD_SIZE   0x05
    
    uint8_t send_cmd[KEY_PWM_SEND_CMD_SIZE]={0};

    if(TRUE == Key_Backlight_Response_Flag)
    {
        /* command ID */
        send_cmd[0] = CMD_SEND_ID_KEY_BACKLIGHT;
        
        if(KEY_BACKLIGHT_LEVEL0 == Key_Backlight_Level)
        {
            send_cmd[1] = KEY_BACKLIGHT_OFF;
        }
        else
        {
            send_cmd[1] = KEY_BACKLIGHT_ON;
            send_cmd[2] = Key_Backlight_Level;
        }
        
        /* send the command */
        Key_If_Pwm_Cmd_Send_Save(send_cmd, KEY_PWM_SEND_CMD_SIZE);

        /* clear the flag */
        Key_Backlight_Response_Flag = FALSE;
    }    
}

/***********************************************************************************************
*
* @brief      Init the GPIO
* @param    none
* @return   none
*
************************************************************************************************/
static void Key_Ctrl_GPIO_Init(void)
{
    CONFIG_PIN_AS_GPIO(PTG,PTG2,OUTPUT);  /* PWR-KEY-3V3(PTG2) */
    OUTPUT_SET(PTG,PTG2);
    CONFIG_PIN_AS_GPIO(PTE,PTE2,OUTPUT);  /* VOLUME+_3V3(PTE2) */
    OUTPUT_SET(PTE,PTE2);
    CONFIG_PIN_AS_GPIO(PTE,PTE3,OUTPUT);  /* VOLUME-_3V3(PTE3) */
    OUTPUT_SET(PTE,PTE3);    
}

/***********************************************************************************************
*
* @brief      Deinit the GPIO
* @param    none
* @return   none
*
************************************************************************************************/
static void Key_Ctrl_GPIO_Deinit(void)
{
    /* PWR-KEY-3V3(PTG2) */
    OUTPUT_CLEAR(PTG,PTG2);
    /* VOLUME+_3V3(PTE2) */
    OUTPUT_CLEAR(PTE,PTE2);
    /* VOLUME-_3V3(PTE3) */
    OUTPUT_CLEAR(PTE,PTE3);    
}
#endif
