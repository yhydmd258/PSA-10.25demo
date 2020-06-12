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

#ifndef CTP_I2C_H_
#define CTP_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
* External objects
**********************************************************************************************/
#include "derivative.h" 

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define COORDINATE_BUF_NUMBER 200
#define TOUCH_FINGER_NUMBER_MAX     10

/* HST_MODE */
#define DATA_TOGGLE_MASK    0x80    
#define DATA_TOGGLE_SHIFT   7
#define DEVICE_MODE_MASK    0x70
#define DEVICE_MODE_SHIFT   4
#define MODE_CHANGE_RQ_MASK     0x08
#define MODE_CHANGE_RQ_SHIFT    3
#define LOW_POWER_ENABLE_MASK   0x04
#define LOW_POWER_ENABLE_SHIFT  2
#define DEEP_SLEEP_MASK     0x02
#define DEEP_SLEEP_SHIFT    1
#define RESET_MASK          0x01
#define RESET_SHIFT         0

#define CTP_IC        I2C1

/* TOUCH TYPE */
typedef enum
{
    FINGER_OR_THIN_GLOVE_TYPE
    ,LARGE_OBJECT_TYPE
    ,THICK_GLOVE_TYPE
    ,RESERVED_TYPE
}TOUCH_TYPE_ENUM;

/* Event ID */
typedef enum
{
    NO_EVENT        /* no move, or no touch */
   ,TOUCH_DOWN
   ,MOVING
   ,LIFT_OFF
}EVENT_ID_ENUM;

/* operating mode register map */
typedef enum
{
    HST_MODE=0
   ,RESET_DETECT=1
   ,COMMAND=2
   ,CMD_DATA=3
   ,REP_LEN=9
   ,REP_STAT=10
   ,BUTTONS=11
   ,TT_STAT=27
   ,TOUCH_REPORT=28
}REGISTER_ADRESS_OFFSET;

typedef enum
{
     HST_MODE_SIZE=1
    ,RESET_DETECT_SIZE=1
    ,COMMAND_SIZE=1
    ,CMD_DATA_SIZE=6    /* from 'CMD_DATA1' to 'CMD_DATA6' */
    ,REP_LEN_SIZE=1
    ,REP_STAT_SIZE=1
    ,BUTTONS_SIZE=1
    ,TT_STAT_SIZE=1
    ,TOUCH_REPORT_SIZE=10
}REGISTER_SIZE;

typedef enum
{
    POWER_BUTTON                =0x01   /* power */
    ,VOLUME_INCREASE_BUTTON   =0x10  /* Volume+ */
    ,VOLUME_DECREASE_BUTTON   =0x04  /* Volume- */
}TOUCH_BUTTON_ENUM;

/* HST_MODE_STRUCT */
typedef struct
{
    uint8_t reset               :1; /* bit0 */
    uint8_t deep_sleep          :1; /* bit1 */
    uint8_t low_power_enable    :1; /* bit2*/
    uint8_t mode_change_req     :1; /* bit3 */
    uint8_t device_mode         :3; /* bit4-bit6 */
    uint8_t data_toggle         :1; /* bit7 */
}HST_MODE_STRUCT;

/* TT_STAT structure */
typedef struct
{
    uint8_t touch_record_number :5; /* number of touch records */
    uint8_t object_detected     :3; /* object detected */
}TT_STAT_STRUCT;

/* touch ID structure */
typedef struct
{
    uint8_t touch_ID    :4; /* bit0-bit3 */
    uint8_t event_ID    :2; /* bit4-bit5 */
    uint8_t touch_type2 :2; /* bit6-bit7 */
}TOUCH_ID_STRUCT;

/* touch report record structure */
typedef struct
{
    uint8_t touch_x_MSB;
    uint8_t touch_x_LSB;
    uint8_t touch_y_MSB;
    uint8_t touch_y_LSB;
    uint8_t touch_z;
    TOUCH_ID_STRUCT touch_ID_str;
    uint8_t touch_object;
    uint8_t touch_major;
    uint8_t touch_minor;
    uint8_t touch_orientation;
}TOUCH_REPORT_STRUCT;

/**********************************************************************************************
* Global variables
**********************************************************************************************/
typedef struct
{
    uint8_t write_pos;
    uint8_t read_pos;
    TOUCH_REPORT_STRUCT buf[COORDINATE_BUF_NUMBER];
}TOUCH_COORDINATE_BUF;

/**********************************************************************************************
* Local types
**********************************************************************************************/


/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CTP_Data_Init(void);
extern void CTP_Data_Deinit(void);
extern uint8_t CTP_Data_Master_Send(uint8_t *send_data, uint32_t data_size);
extern uint8_t CTP_Data_Master_Read(uint8_t reg_addr, uint8_t *read_data, uint32_t data_size);
extern void CTP_Data_Cmd_Make(TOUCH_COORDINATE_BUF *data);
extern void CTP_Data_Reset_Set(uint8_t level);
extern void CTP_Data_Power_En_Init(void);
extern void CTP_Data_Power_En_Deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _CTP_I2C_H_ */

#endif /* CTP_MODULE */
