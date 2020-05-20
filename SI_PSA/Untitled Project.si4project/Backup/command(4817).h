#ifndef  _COMMAND_H_
#define _COMMAND_H_

#include "derivative.h" 

#ifndef ALIGN
/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#endif /* Compiler Related Definitions */
#endif

#define FALSE 0
#define TRUE 1

#define DisableInterrupts __asm(" CPSID i");
#define EnableInterrupts __asm(" CPSIE i");

/* bus clock */
#define BUS_CLK_HZ      20000000

typedef void (*CALLBACK_FUNCTION)(void);

/* CMD ID, these are used by client "SX12" */
#define CMD_REV_ID_UPGRADE          0x7E /* This command is sent from MMI to display and used in upgrade */
#define CMD_REV_ID_STARTUP_DONE     0x80 /* this command indicates the MMI has receive the command 0x40 */
#define CMD_REV_ID_STATUS_QUESTION  0x81 /* check the display status,display return CMD_ID: 0x48, 0x49, 0x4B */
#define CMD_REV_ID_KEY_BACKLIGHT    0x91 /* key backlight mode, from MMI to display, display return CMD_ID:0x4C */
#define CMD_SEND_ID_UPGRADE         0x7F /* This command is sent from display to MMI and used in upgrade */
#define CMD_SEND_ID_SELF_CHECK_INFO 0x40 /* display self check info, MMI return CMD_ID:0x80 */
#define CMD_SEND_ID_TP_POSITION     0x41 /* send the touch position information */
#define CMD_SEND_ID_EXTENDED_TP_POS 0x42 /* send the extended touch position information*/
#define CMD_SEND_ID_FUNCTION_DEFECT 0x48 /* detect the module status, display status, touch function, temperature status */
#define CMD_SEND_ID_VOLTAGE_RANGE   0x49 /* input voltage status */
#define CMD_SEND_ID_VERSION          0x4B /* HW and SW version, this command is supported both in userprogram and bootload */
#define CMD_SEND_ID_KEY_BACKLIGHT   0x4C /* respond key backlight mode, from display to MMI */

/* CMD_ID, there are used by the factory */
#define CMD_REV_ID_LCD_VCOM_TEST    0x1C  /* write the vcom, test the effect of the LCD */
#define CMD_REV_ID_LCD_VCOM_STORAGE 0x1D  /* find the best the value of vcom, storage it and write it to LCD IC */

/* CMD ID: CMD_SEND_ID_TP_POSITION */
#define TP_INFO_NUMBER_MAX          2
#define EXTEND_TP_INFO_NUMBER_MAX  8
#define EVERY_TP_INFO_SIZE          9

/* CMD ID: CMD_REV_ID_KEY_BACKLIGHT 0x4C */
#define KEY_BACKLIGHT_ON            0x00    /* key backlight state: ON */
#define KEY_BACKLIGHT_OFF           0x01    /* key backlight state: OFF */

#define CMD_STARTUP_NUMBER_MAX     9

/* These data is used in bootloader module, checksum = CMD_ID+data1+...+dataN+EXT_length +1 */
//#define APP_UPGRADE_REQ_FLAG_ADDR   0x1400  /* 0x1400 = 5K, the size of bootloader is less than 5K, if not, we msut modify it more than 0x1400 */    
//#define APP_UPGRADE_REQ_CMD_DATA    0x88    /* It is defined by the engineer himself */
//#define APP_UPGRADE_FINISH_FLAG_ADDR 0x1600 /* 0x1600 = 5.5K, the address used to record the whether the application has completed */
//#define APP_UPGRADE_FINISH_DATA     0x44    /*  It is defined by the engineer himself  */
//#define APP_UPGRADE_DEFAULT_DATA    0xFF    /* default value, if the data need to clear, write it */
#define APP_IMAGE_START_ADDR        0x4000  /* 0x2800 = 10K, the application image address*/
#define MCU_FLASH_SIZE_MAX           0x20000 /* 0x20000=128K, the flash is 128K */

/* the peripheral eeprom starts storing address */
#define PERIPHERAL_EEPROM_START_ADDR    0
/* the structure of upgrade data flag in the first ten bytes of EEPROM  */
typedef struct
{
    uint8_t upgrade_req_flag;       /* record the flag of upgrade request,TRUE(1):request to update, other: no request */
    uint8_t app_integrity_flag;     /* record the flag of application is integrity or not, TRUE(1): application is intergrity, other: application is not intergrity*/
    uint8_t error_restart_times;   /* record the restart times because of error */
    uint8_t reserve[7];              /* reserved */
}UPGRADE_FLAG_STRUCT;
typedef enum
{
    MCU_UPGRADE_STATUS_ENUM     /* record the upgrade flag and Application is OK or not and so on */
    ,LCD_VCOM_ENUM               /*  record the vcom data and flag */
    ,CTP_CFG_FLAG
}EEPROM_STORAGE_TYPE;

#define EEPROM_I2C                      I2C1
#define EEPROM_ADDRESS                  0xA0
#define EEPROM_PAGE_LENGTH_MAX        16
#define CTP_IC  I2C1

/*command start*/
#define COMMAND_SYNC            0x79
#define COMMAND_DEV_ADDR        0x6A

/* command type ID */
#define COMMAND_ID_BACKLIGHT    0x01    /* receive */
#define COMMAND_ID_TOUCH        0x02    /* send touch message */
#define COMMAND_ID_LED_FAULT    0x03    /* send LED fault message */
#define COMMAND_ID_LCD_FAULT    0x04    /* send LCD fault message */

/*command end*/
#define COMMAND_END_DATA        0x6A

/* standby state */
#define STANDBY_ON              0x01
#define STANDBY_OFF             0x00

/* state */
#define ERROR_NULL              0x00
#define ERROR                   0x01

/********************************************************* 
communication protocol data 
**********************************************************/
/* head and end */
#define COMMAND_REV_HEAD_D0          0x79 /*The first data received from the host, fixed*/
#define COMMAND_REV_HEAD_D1          0x66 /*The second data received from the host, fixed*/
#define COMMAND_REV_END_D1           0x66 /*The last second data received from the host, fixed*/
#define COMMAND_REV_END_D0           0xAA /*The last data received from the host, fixed*/
#define COMMAND_SEND_HEAD_D0         0x79 /*The first data sent to the host, fixed*/
#define COMMAND_SEND_HEAD_D1         0x6A /*The second data sent to the host, fixed*/
#define COMMAND_SEND_END_D1          0x6A /*The last second data sent to the host, fixed*/
#define COMMAND_SEND_END_D0          0x6A /*The last data sent to the host, fixed*/

/* D2 */
#define CMD_REV_TYPE_POSITION        0x02 /*The position of D2, the start is D0*/
#define CMD_REV_TYPE_FACTORY         0x10 /*The value of the D2, indicates that this command is used by factory*/
#define CMD_REV_TYPE_DIAGNOSIS       0x20 /*The value of the D2, indicates that this command is used to diagnose by the host*/
#define CMD_REV_TYPE_OTHER           0x40 /*The value of the D2, indicates that this command is used by the host */
#define CMD_SEND_TYPE_FACTORY        0x11 /*The value of the data2, indicates that this command is used to reply*/
#define CMD_SEND_TYPE_DIAGNOSIS      0x21 /*The value of the data2, indicates that this command is used to reply*/
#define CMD_SEND_TYPE_OTHER          0x41  /*The value of the data2, indicates that this command is used to reply*/

/*D3: the length of the command, start from D4 to the last, for example: 0x79 0x66 0x20 0x03 0x10 0x66 0xAA */
#define CMD_REV_lENGTH_POSITION      0x03

/*D4: command ID */
/*D2=0x20, D4 as follow:*/
#define CMD_REV_ID_ENTER_DIAGNOSIS  0x10 /*The value of the data4, require to enter diagnosis status*/
#define CMD_REV_ID_READ_SW_VERSION  0x11 /*The value of the data4, require to read the software version*/
#define CMD_REV_ID_READ_LCD_FAULT   0x12 /*The value of the data4, require to read the LCD fault*/
#define CMD_REV_ID_EXIT_DIAGNOSIS   0x2F /*The value of the data4, require to exit the diagnosis status*/

/*D2=21, D4 as follow:*/
#define CMD_SEND_ID_ENTER_DIAGNOSIS 0x50 /*The value of the data4, reply the result of entering diagnosis status*/
#define CMD_SEND_ID_SW_VERSION       0x51 /*The value of the data4, reply the software version*/
#define CMD_SEND_ID_LCD_FAULT        0x52 /*The value of the data4, reply the LCD fault information*/
#define CMD_SEND_ID_EXIT_DIAGNOSIS  0x6F /**/


#endif/* _COMMAND_H_ */
