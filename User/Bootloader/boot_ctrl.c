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
#ifdef  BOOTLOADER
#include <string.h>
#include "flash.h"
#include "../Command/command.h"
#include "../Timer/timer_if.h"

#include "boot_if.h"
#include "boot_ctrl.h"
#include "boot_data.h"

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
    BOOT_UPGRADE_PREPARE    /* just entered upgrade mode, no command communication */
    ,BOOT_UPGRADE_REQUEST   /* wait to receive the start upgrade command from MMI */
    ,BOOT_UPGRADE_START     /* wait to receive the data transfer start command from MMI */     
    ,BOOT_FIRST_DATA_FRAMES /* wait to receive the first data command from MMI and determine the upgrade program is CTP or application  */
    ,BOOT_CTP_UPGRADING     /* upgrade the CTP program */
    ,BOOT_APP_UPGRADING     /* upgrade the Application program */
    ,BOOT_UPGRADE_END       /* upgrade is end, and restart the application */
    ,BOOT_SWITCH_TO_APP     /* switch to application */
}BOOT_UPGRADE_STATUS;


typedef struct
{
    uint8_t read_buf[BOOT_REC_BUF_SIZE];
    uint8_t read_size;
}UPGRADE_DATA_STRUCT;

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static uint8_t Boot_Cmd_Sending_Status = FALSE; /* FALSE: the data does not wait to being sent; TRUE: the data wait to being sent*/
static UPGRADE_DATA_STRUCT Boot_Upgrade_Data = {0};
static UPGRADE_FLAG_STRUCT Boot_Upgrade_Flag = {0};
static BOOT_UPGRADE_STATUS Boot_Upgrade_Status; /* TRUE: updating, FALSE: start to update or update end */
/* These data is used in bootloader module, checksum = CMD_ID+data1+...+dataN+EXT_length +1 */
static uint8_t Dis_To_MMI_Req_Upgrade[7]={CMD_SEND_ID_UPGRADE,0x01,0x81,0x05,0x09,0x02,0x01};/*CMD_ID + Packet Header + packet data*/
static uint8_t MMI_To_Dis_Req_Upgrade[7]={CMD_REV_ID_UPGRADE,0x01,0x80,0x02,0x0D,0xC2,0xC2};/*CMD_ID + Packet Header + packet data*/
static uint8_t Dis_To_MMI_Req_Upgrade_Res[3]={CMD_SEND_ID_UPGRADE,0xAA,0x81};/*CMD_ID + Packet Header + packet data*/
static uint8_t MMI_To_Dis_Upgrade_Start[4]={CMD_REV_ID_UPGRADE,0x00,0x01,0x10};/*CMD_ID + Packet Header + packet data*/
static uint8_t Dis_To_MMI_Upgrade_Start_Res[4]={CMD_SEND_ID_UPGRADE,0x80,0x01,0x10};/*CMD_ID + Packet Header + packet data*/
static uint8_t MMI_To_Dis_Upgrade_End[4]={CMD_REV_ID_UPGRADE,0x00,0x01,0x20};/*CMD_ID + Packet Header + packet data*/
static uint8_t Dis_To_MMI_Upgrade_End_Res[4]={CMD_SEND_ID_UPGRADE,0x80,0x01,0x20};/*CMD_ID + Packet Header + packet data*/

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void Boot_Ctrl_Upgrade_Status_Update(void);
static void Boot_Ctrl_App_Image_Write(uint8_t *image_buf, uint8_t buf_size);
static void Boot_Ctrl_Update_Uncompress_Read(uint8_t *cmd_data, uint32_t *cmd_size);
static void Boot_Ctrl_Update_Compress_Save(uint8_t *cmd_data, uint8_t cmd_size);
static void Boot_Ctrl_Fun_Defect(void);
static void Boot_Ctrl_Voltage_Range(void);
static void Boot_Ctrl_Version(void);
static void Boot_Ctrl_Diag_Cmd_Save(uint8_t* cmd_data, uint8_t cmd_size);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/***********************************************************************************************
*
* @brief    initialize the Boot module
* @param    none
* @return   none
*
************************************************************************************************/
void Boot_Ctrl_Init(void)
{
    /* init deserializer I2C, init eeprom(CTP) I2C  */
    Boot_Data_Init();
    /* read the data*/
    Boot_Data_Eeprom_Random_Read(PERIPHERAL_EEPROM_START_ADDR, (uint8_t*)&Boot_Upgrade_Flag, sizeof(Boot_Upgrade_Flag));
    /* update the upgrade status */
    Boot_Ctrl_Upgrade_Status_Update();
    /* set send status */
    Boot_Cmd_Sending_Status = FALSE;
}

/***********************************************************************************************
*
* @brief    deinitialize the Boot module
* @param    none
* @return   none
*
************************************************************************************************/
void Boot_Ctrl_Deinit(void)
{
    Boot_Data_Deinit();
    
    Boot_Cmd_Sending_Status = FALSE;
}
/***********************************************************************************************
*
* @brief      check the application upgrade is complete or not
* @param    none
* @return   TRUE: updating, 
                FALSE: upgrade is complete
*
************************************************************************************************/
uint8_t Boot_Ctrl_Upgrade_Result_Check(void)
{
    if(BOOT_SWITCH_TO_APP == Boot_Upgrade_Status)
        return FALSE;
    else
        return TRUE;
}

/***********************************************************************************************
*
* @brief      the read_buf and read_size is empty, we will analyse the command received from MMI; if not, direct return 
* @param    read_buf: the pointer of buf which store read data
                  read_size: the size of read data
* @return   void
*
************************************************************************************************/
void Boot_Ctrl_Cmd_Read(void)
{
    uint8_t read_cmd_buf[BOOT_REC_BUF_SIZE] = {0};
    uint32_t cmd_size = 0;
    
    if((0!=Boot_Upgrade_Data.read_buf[0]) || (0!=Boot_Upgrade_Data.read_size))
        return;

    /* read command */
    Boot_Ctrl_Update_Uncompress_Read(read_cmd_buf, &cmd_size);
    
    if(0 < cmd_size)
    {
        /* classify the command type, temp code */
        if(CMD_REV_ID_STATUS_QUESTION == read_cmd_buf[0])
        {
            /* deal with dignosis command */
            Boot_Ctrl_Fun_Defect();
            Boot_Ctrl_Voltage_Range();
            Boot_Ctrl_Version();
        }
        else if(CMD_REV_ID_UPGRADE == read_cmd_buf[0])
        {
            /* save the command */
            memcpy(Boot_Upgrade_Data.read_buf, read_cmd_buf, cmd_size);
            Boot_Upgrade_Data.read_size = cmd_size;
        }
        else
        {
            /* do nothing */
        }
    }
}

/***********************************************************************************************
*
* @brief      check the send buf, it sets interrupt pin low if there is data to be sent and time out 
* @param   void
* @return   void
*
************************************************************************************************/
void Boot_Ctrl_Cmd_Send_Check(void)
{
    /* there is data to be sent and the time is up */
    if(TRUE == Timer_If_Check(EN_UPGRADE_DESER_SEND_DATA))
    {
        if(TRUE == Boot_Cmd_Sending_Status)
        {
            /* set interrupt pin(CRQ) high */
            Boot_Data_Interrupt_Set(1);
            /* set send status as FALSE */
            Boot_Cmd_Sending_Status =FALSE;
        }
        else if(TRUE == Boot_Data_Interrupt_Low_Check())
        {
            /* set interrupt pin(CRQ) low */
            Boot_Data_Interrupt_Set(0);
            /* set the timer */
            Timer_If_Set(EN_UPGRADE_DESER_SEND_DATA, TIMER_COUNT_100MS);
            /* set send status as TRUE */
            Boot_Cmd_Sending_Status =TRUE;
        }
        else
        {
            /* set interrupt pin(CRQ) high */
            Boot_Data_Interrupt_Set(1);
        }
    }
}

/***********************************************************************************************
*
* @brief      FALSE: the data does not wait to being sent; TRUE: the data wait to being sent
* @param   void
* @return   void
*
************************************************************************************************/
void Boot_Ctrl_Cmd_Send_Status_Clear(void)
{
    Boot_Cmd_Sending_Status =FALSE;
}

/***********************************************************************************************
*
* @brief      analyse the command received from MMI, and switch the upgrade status
* @param    void
* @return   void
*
************************************************************************************************/
void Boot_Ctrl_Upgrade_Status_Manage(void)
{
#define REQ_UPGRADE_TIME_MAX    120
#define DATA_FRAMES_ID           0x01
#define DATA_FRAMES_ID_POSTION  0x01
#define DATA_FRAMES_TYPE_MCU    0x30
#define DATA_FRAMES_TYPE_TP     0x40
#define DATA_FRAMES_TYPE_POSTION 0x03

    static uint8_t req_upgrade_time = 0;
#if 1
    if((0<Boot_Upgrade_Data.read_size)&&
        (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Req_Upgrade, Boot_Upgrade_Data.read_size)))
    {
        req_upgrade_time = 0;
//                    Timer_If_Clear(EN_UPGRADE_DIS_REQ_PER);
        /* receive the upgrade request command, switch to next status */
        Boot_Upgrade_Status = BOOT_UPGRADE_REQUEST;
    }
#endif    
    if((Boot_Upgrade_Data.read_size > 0) 
        || (BOOT_UPGRADE_PREPARE == Boot_Upgrade_Status) 
        || (BOOT_UPGRADE_END == Boot_Upgrade_Status))
    {
        switch(Boot_Upgrade_Status)
        {
            /* just entered upgrade mode, no command communication, send request upgrade command;
                            if receive the upgrade request command, switch to next status */
            case BOOT_UPGRADE_PREPARE:
                if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Req_Upgrade, Boot_Upgrade_Data.read_size)))
                {
                    req_upgrade_time = 0;
//                    Timer_If_Clear(EN_UPGRADE_DIS_REQ_PER);
                    /* receive the upgrade request command, switch to next status */
                    Boot_Upgrade_Status = BOOT_UPGRADE_REQUEST;
                }
                else if(TRUE == Timer_If_Check(EN_UPGRADE_DIS_REQ_PER))
                {
                    if(req_upgrade_time < REQ_UPGRADE_TIME_MAX)
                    {
                        /* send request upgrade command */
                        Boot_Ctrl_Update_Compress_Save(Dis_To_MMI_Req_Upgrade, sizeof(Dis_To_MMI_Req_Upgrade));
                        /* request to send data */
                        Boot_Ctrl_Cmd_Send_Check();
                        Timer_If_Set(EN_UPGRADE_DIS_REQ_PER, TIMER_COUNT_1S);
                        req_upgrade_time++;
                    }
                    else
                    {
                        /* if request upgrade time out, MCU reset, set SYSRESETREQ bit, activate external SYSRESETREQ signal  */
                        NVIC_SystemReset();
                    }
                }
                break;
            /* receive the request upgrade command from MMI, send response to MMI; 
                        If receive the request upgrade command, switch to next status; */
            case BOOT_UPGRADE_REQUEST:
                if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Req_Upgrade, Boot_Upgrade_Data.read_size)))
                {
                    /* receive the request upgrade command from MMI, send response to MMI */
                    Boot_Ctrl_Update_Compress_Save(Dis_To_MMI_Req_Upgrade_Res, sizeof(Dis_To_MMI_Req_Upgrade_Res));
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    Boot_Upgrade_Data.read_size = 0;
                }
                else if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Upgrade_Start, Boot_Upgrade_Data.read_size)))
                {
                    /* receive the request upgrade command, switch to next status; */
                    Boot_Upgrade_Status = BOOT_UPGRADE_START;
                }
                else
                {
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    Boot_Upgrade_Data.read_size = 0;
                }
                break;
            /*  receive the data transfer start command from MMI, send response to MMI;
                            if received the data frames, switch to next status */
            case BOOT_UPGRADE_START:
                if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Upgrade_Start, Boot_Upgrade_Data.read_size)))
                {
                    /* receive the data transfer start command from MMI, send response to MMI */
                    Boot_Ctrl_Update_Compress_Save(Dis_To_MMI_Upgrade_Start_Res, sizeof(Dis_To_MMI_Upgrade_Start_Res));
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    Boot_Upgrade_Data.read_size = 0;
                }
                else if(DATA_FRAMES_ID == Boot_Upgrade_Data.read_buf[1])
                {
                    if(DATA_FRAMES_TYPE_MCU == Boot_Upgrade_Data.read_buf[3])
                    {
                        /* if received the MCU data frames, switch to next status */
                        Boot_Upgrade_Status = BOOT_APP_UPGRADING;
//                        /* erase the flash of application program */
//                        Boot_Data_App_Image_Erase();
                        /* start to update the MCU(application), updata the flag, write it to EEPROM */
                        Boot_Upgrade_Flag.app_integrity_flag = FALSE;
                        Boot_Data_Eeprom_Random_Write(PERIPHERAL_EEPROM_START_ADDR, (uint8_t*)&Boot_Upgrade_Flag, sizeof(Boot_Upgrade_Flag));
                    }
                    else if(DATA_FRAMES_TYPE_TP == Boot_Upgrade_Data.read_buf[3])
                    {
                        /* if received the CTP data frames, switch to next status */
                        Boot_Upgrade_Status = BOOT_CTP_UPGRADING;
                        /* enter into CTP update mode */
                        
                    }
                    else
                    {
                        /* do nothing  */
                    }
                }
                else
                {
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    Boot_Upgrade_Data.read_size = 0;
                }
                break;
            /* If received the upgrade data, upgrade the CTP program;
                        If received the end upgrade command, switch to next status */
            case BOOT_CTP_UPGRADING:
                if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Upgrade_End, Boot_Upgrade_Data.read_size)))
                {
                    /* If received the end upgrade command, switch to next status */
                    Boot_Upgrade_Status = BOOT_UPGRADE_END;
                }
                else if((DATA_FRAMES_ID == Boot_Upgrade_Data.read_buf[DATA_FRAMES_ID_POSTION]) && (DATA_FRAMES_TYPE_TP == Boot_Upgrade_Data.read_buf[DATA_FRAMES_TYPE_POSTION]))
                {
                    /*If received the CTP upgrade data, write to CTP */

                    /* send response to MMI */
                    
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    Boot_Upgrade_Data.read_size = 0;
                }
                break;
            /* If received the upgrade data, upgrade the Application program;
                        If received the end upgrade command, switch to next status */
            case BOOT_APP_UPGRADING:
                if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Upgrade_End, Boot_Upgrade_Data.read_size)))
                {
                    Boot_Upgrade_Status = BOOT_UPGRADE_END;
                    /* the MCU(application) upgrade is end, updata the flag, write it to EEPROM */
                    Boot_Upgrade_Flag.app_integrity_flag = TRUE;
                    Boot_Data_Eeprom_Random_Write(PERIPHERAL_EEPROM_START_ADDR, (uint8_t*)&Boot_Upgrade_Flag, sizeof(Boot_Upgrade_Flag));
                }
                else if((DATA_FRAMES_ID == Boot_Upgrade_Data.read_buf[DATA_FRAMES_ID_POSTION]) && (DATA_FRAMES_TYPE_MCU == Boot_Upgrade_Data.read_buf[DATA_FRAMES_TYPE_POSTION]))
                {
                    /* write to flash, and send response to MMI */
                    Boot_Ctrl_App_Image_Write(Boot_Upgrade_Data.read_buf, Boot_Upgrade_Data.read_size);
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    Boot_Upgrade_Data.read_size = 0;
                }
                break;
            /* upgrade is end, send response and wait 500ms, if no received data, restart the application */
            case BOOT_UPGRADE_END:
                if((0<Boot_Upgrade_Data.read_size)&&
                    (0 == memcmp(Boot_Upgrade_Data.read_buf, MMI_To_Dis_Upgrade_End, Boot_Upgrade_Data.read_size)))
                {
                    /* if received the upgrade end command, we will send the response command */
                    Boot_Ctrl_Update_Compress_Save(Dis_To_MMI_Upgrade_End_Res, sizeof(Dis_To_MMI_Upgrade_End_Res));
                    /* clear the buf which store the data received from the MMI */
                    memset(Boot_Upgrade_Data.read_buf, 0x00, sizeof(Boot_Upgrade_Data.read_buf));
                    /* set timer */
                    Timer_If_Set(EN_UPGRADE_END, TIMER_COUNT_500MS);
                }
                else if(TRUE == Timer_If_Check(EN_UPGRADE_END))
                {
                    Boot_Upgrade_Status = BOOT_SWITCH_TO_APP;
                }
                break;
            default:
                break;
                    
        }
    }
}

/***********************************************************************************************
*
* @brief    update the upgrade status according to the data stored in EEPROM
* @param    none
* @return   TRUE: request to upgrade
                FALSE: not request to upgrade
*
************************************************************************************************/
static void Boot_Ctrl_Upgrade_Status_Update(void)
{
    if(TRUE == Boot_Upgrade_Flag.upgrade_req_flag)
    {
        Boot_Upgrade_Status = BOOT_UPGRADE_REQUEST;
        /* update the flag, write it to eeprom */
        Boot_Upgrade_Flag.upgrade_req_flag = FALSE;
        Boot_Data_Eeprom_Random_Write(PERIPHERAL_EEPROM_START_ADDR, (uint8_t*)&Boot_Upgrade_Flag, sizeof(Boot_Upgrade_Flag));

        /* send response to MMI */
        Boot_Ctrl_Update_Compress_Save(Dis_To_MMI_Req_Upgrade_Res, sizeof(Dis_To_MMI_Req_Upgrade_Res));
        
    }
    else if(TRUE != Boot_Upgrade_Flag.app_integrity_flag)
    {
        Boot_Upgrade_Status = BOOT_UPGRADE_PREPARE;
    }
    else
    {
        Boot_Upgrade_Status = BOOT_SWITCH_TO_APP;
    }
}

/***********************************************************************************************
*
* @brief      write the application program to flash;
                 CMD_ID(1 byte) + Packet Header(2 bytes, 0x01+type) + type(1 byte) + package_num(1 byte) + data(S19 file) + 0x1C + ...
* @param    image_buf: the pointer of buf which will be written into MCU flash
                  read_size: the size of image_buf
* @return   void
*
************************************************************************************************/
static void Boot_Ctrl_App_Image_Write(uint8_t *image_buf, uint8_t buf_size)
{
#define UPGRADE_LENGTH_POSTION  0x02    /* CMD_ID + 0x01( the first byte of Packet Header) */
#define UPGRADE_PACKAGE_NUM_POS 0x04    /* CMD_ID + Packet Header( 2 byte ) + type */
#define UPGRADE_S19_DATA_POS  0x05    /* CMD_ID + Packet Header( 2 byte ) + type+ package_num */
#define IMAGE_RESPONSE_SIZE     0x04    /* CMD_ID + Packet Header( 2 byte, 0x01 or 0x02 +package_num )  */

    ALIGN(4) static uint8_t local_image_buf[FLASH_SECTOR_SIZE];
    static uint32_t package_num = 0;
    uint32_t image_addr_start = 0; /* the start address of application image data per packet data*/
    uint32_t image_addr_size = 0; /* the size of address */
    uint32_t S19_checksum = 0; /* calculate the checksum of S19 file */
    uint32_t S19_data_addr_start = 8; /* the data position of command, 8 + the size of address;
                                                                        8: CMD_ID(1 byte) + Packet Header(2 bytes, 0x01+type) + type(1 byte) + package_num(1 byte) + Sn(2byte) +count(1byte)*/
    uint8_t S19_data_size = 0; /* the data size of S19 file */
    uint8_t response_cmd[BOOT_SEND_BUF_SIZE] = {0};
    uint8_t number = 0;
    
    /* CMD_ID */    
    response_cmd[0] = CMD_SEND_ID_UPGRADE;
    response_cmd[1] = 0x81;
    /* set package_num */
    response_cmd[3] = package_num;

//    if(((uint8_t)package_num == image_buf[UPGRADE_PACKAGE_NUM_POS]) && ('S' == image_buf[UPGRADE_S19_DATA_POS]))
    if((uint8_t)package_num == image_buf[UPGRADE_PACKAGE_NUM_POS])
    {
        /* the structure of S19 file: type(Sn, 'n' is number, 2bytes) + count(1 byte) + address + data + checksum */
        /* calculate the checksum, start from count(1 byte) */
        for(number=0; number<image_buf[UPGRADE_S19_DATA_POS+2]; number++)
        {
            S19_checksum = S19_checksum+image_buf[UPGRADE_S19_DATA_POS+2+number];
        }
        /* confirm the checksum is OK or not */
        if((0xFF - (S19_checksum&0x000000FF)) == image_buf[UPGRADE_S19_DATA_POS+2+number])
        {
            switch(image_buf[UPGRADE_S19_DATA_POS+1])
            {
                /* type S1, the size of address is 2 bytes, the data is written into MCU */
                case 1:
                    image_addr_size = 2;
                    break;
                /* type S2, the size of address is 3 bytes, the data is witten into MCU */
                case 2:
                    image_addr_size = 3;
                    break;
                /* type S3, the size of address is 4 bytes, the data is written into MCU */
                case 3:
                    image_addr_size = 4;
                    break;
                case 0:/* type S0, the size of address is 2 bytes, record the name(20 bytes) , version(2 bytes), reversion(2 bytes), description(0-36 bytes)  */
                case 5:/* type S5, the size of address is 4 bytes, record the count of S1, S2, S3, the data is not written into MCU */
                case 7:/* type S7, the size of address is 4 bytes, the data is the start address of MCU */
                case 8:/* type S8, the size of address is 3 bytes, the data is the start address of MCU */
                case 9:/* type S9, the size of address is 2 bytes, the data is the start address of MCU */
                default:
                    break;
            }
            /* the data from the MMI need to be written into MCU */
            if(image_addr_size > 0)
            {
                /* set the position of data which will be written to MCU */
                S19_data_addr_start += image_addr_size;
                /* set image data address */
                for(number=0; number<image_addr_size; number++)
                    image_addr_start = (image_addr_start<<8)+image_buf[UPGRADE_S19_DATA_POS+3+number];
                /* write data into the MCU */
                S19_data_size = image_buf[UPGRADE_S19_DATA_POS+2]-1-image_addr_size;/* 2: Sn, 1: the size of checksum */

                __disable_irq();
                if((image_addr_start & (FLASH_SECTOR_SIZE-1)) == 0)
                {
                    /* erase section every time */
                    FLASH_EraseSector(image_addr_start);
                }
                memcpy(local_image_buf, &(image_buf[S19_data_addr_start]), S19_data_size);
                Boot_Data_App_Image_Write(image_addr_start, local_image_buf, S19_data_size);
                __enable_irq();
            }
            /* set the correct flag */
            response_cmd[2] = 0x01;

            /* update the package_num */
            package_num = (package_num+1)%0xFF;            
        }
        else
        {
            /* set the incorrect respoonse, the checksum is error */
            response_cmd[2] = 0x02;
        }
    }
    else
    {
        /* set the incorrect respoonse, the package_num is error or the first byte S19 is not 'S' */
        response_cmd[2] = 0x02;
    }

    /* send the response */
    Boot_Ctrl_Update_Compress_Save(response_cmd, IMAGE_RESPONSE_SIZE);
}

/***********************************************************************************************
*
* @brief    uncompress the data
* @param    cmd_data: the pointer of command
                  cmd_size: the size of command
* @return   void
*
************************************************************************************************/
static void Boot_Ctrl_Update_Uncompress_Read(uint8_t *cmd_data, uint32_t *cmd_size)
{
    #define REC_PACKET_DATA_START_POS   3

    uint8_t read_buf[BOOT_REC_BUF_SIZE]={0};
    uint32_t read_size = 0;
    uint32_t checksum = 0;
    uint32_t number = 0;

    /* copy command to buf in the current file */
    Boot_Data_Cmd_Read(read_buf, &read_size);
    if(1 < read_size)
    {
        /* checksum calibration */
        for(number=0; number<(read_size-1); number++)
        {
            checksum += read_buf[number];
        }
        checksum = (checksum+1)&0xFF;
        
        if(checksum == read_buf[read_size-1])
        {
            /* the checksum is OK */
            if((REC_PACKET_DATA_START_POS < read_size) && (CMD_REV_ID_UPGRADE == read_buf[0]))
            {
                /* copy the data, the size is REC_PACKET_DATA_START_POS */
                for(number=0; number < REC_PACKET_DATA_START_POS; number++)
                    cmd_data[number] = read_buf[number];
                
                /* unescape operation, 0x1b + 0x1e->0x1c, 0x1b+0x1d->0x1b */
                for(number=REC_PACKET_DATA_START_POS, *cmd_size=REC_PACKET_DATA_START_POS; number<read_size-3; number++)
                {
                    cmd_data[*cmd_size] = read_buf[number];
                    if((0x1b == read_buf[number]) && (0x1d == read_buf[number+1]))
                    {
                        cmd_data[*cmd_size] = read_buf[number];
                        number++;
                    }
                    else if((0x1b == read_buf[number]) && (0x1e == read_buf[number+1]))
                    {
                        cmd_data[*cmd_size] = 0x1c;
                        number++;
                    }
                    else
                    {
                        cmd_data[*cmd_size] = read_buf[number];
                    }
                    (*cmd_size)++;
                }
            }
            else
            {
                /* copy the data from read_buf to cmd_data */
                memcpy(cmd_data, read_buf, read_size);
                *cmd_size = read_size;
            }
        }
        else
        {
            /* thechecksum is error, the size set 0 */
            read_size = 0;
        }
    }
    else
    {
        /* size is 0 */
        read_size = 0;
    }
}
/***********************************************************************************************
*
* @brief    make data package , and save it which to be sent to MMI
* @param    cmd_data: the pointer of command(cmd_data : CMD_ID+Packet Header+ packet data
                                                                      or CMD_ID +Packet Header )
                  cmd_size: the size of command
* @return   void
*
************************************************************************************************/
static void Boot_Ctrl_Update_Compress_Save(uint8_t *cmd_data, uint8_t cmd_size)
{
    #define SEND_PACKET_DATA_START_POS   3
    #define SEND_PACKET_HEADER_SIZE       2
    
    uint8_t escape_buf1[BOOT_SEND_BUF_SIZE*2]={0};
    uint8_t escape_buf2[BOOT_SEND_BUF_SIZE*3]={0};
    uint8_t send_buf[BOOT_SEND_BUF_SIZE*3]={0};
    uint32_t escape_buf1_postion = 0;
    uint32_t escape_buf2_postion = 0;
    uint32_t send_size = 0;
    uint32_t checksum = 0;
    uint32_t number = 0;
    
    if((NULL == cmd_data) || (0 == cmd_size) || (BOOT_SEND_BUF_SIZE < cmd_size ))
        return;
    else
    {
        /* check the data and escape operation */
        /* cmd_data : CMD_ID+Data[n], Data[n] : Header0+Header1+Packet data[n] */
        if(cmd_size > SEND_PACKET_DATA_START_POS)
        {
            /* escape operation, 0x1b -> 0x1b + 0x1d */
            for(escape_buf1_postion=0,number=SEND_PACKET_DATA_START_POS; number<cmd_size; number++,escape_buf1_postion++)
            {
                escape_buf1[escape_buf1_postion] = cmd_data[number];
                if(0x1B == cmd_data[number])
                {
                    escape_buf1_postion++;
                    escape_buf1[escape_buf1_postion] = 0x1D;
                }
            }
            
            /* escape operation, 0x1c -> 0x1b + 0x1e */
            for(number=0,escape_buf2_postion=0; number<escape_buf1_postion; number++,escape_buf2_postion++)
            {
                if(0x1C == escape_buf1[number])
                {
                    escape_buf2[escape_buf2_postion] = 0x1B;
                    escape_buf2_postion++;
                    escape_buf2[escape_buf2_postion] = 0x1E;
                }
                else
                {
                    escape_buf2[escape_buf2_postion] = escape_buf1[number];
                }
            }

            /* copy data to send_buf */
            for(number=0; number<SEND_PACKET_DATA_START_POS; number++)
                send_buf[number] = cmd_data[number];
            send_size = number;
            memcpy(&(send_buf[number]), escape_buf2, escape_buf2_postion);
            send_size += escape_buf2_postion;
        }
        else
        {
            memcpy(send_buf, cmd_data, cmd_size);
            send_size = cmd_size;
        }

            
        send_buf[send_size] = BOOR_DESER_RECEIVE_SEPARATOR;
        send_size++;
        /* add data at the end of the packet  */
        send_buf[send_size] = 0; /* EXT_len*/
        send_size++;
        /* calculate the checksum */
        for(number=0; number<send_size; number++)
        {
            checksum = (checksum+send_buf[number])&0xFF;
        }
        checksum = (checksum+1)&0xFF;
        send_buf[send_size] = checksum&0xFF;
        send_size++;
        
        /* save the command */
        Boot_Data_Send_Cmd_Save(send_buf, send_size);
    }
}

/***********************************************************************************************
*
* @brief    This is not corrent result, only response to commnd ID 0x81;
                function defect, include the module status, display status, touch function, temperature status
* @param    none
* @return   none
*
************************************************************************************************/
static void Boot_Ctrl_Fun_Defect(void)
{
    #define FUNC_DEFECT_CMD_SIZE   0x09
    
    uint8_t send_cmd[FUNC_DEFECT_CMD_SIZE]={0};
    uint8_t LED_fault = 0;/* record the LED fault status */
    
    send_cmd[0] = CMD_SEND_ID_FUNCTION_DEFECT;
    /* display status(LCD), 0:ok, 1: abnormal */
    send_cmd[2] = 0;
    /* touch function, 0:ok, 1: abnormal */
    send_cmd[3] = 0;
    /* temperature status, 0:ok, 1: abnormal */
    send_cmd[4] = 0;
    /* LED status, 0:ok, 1: abnormal */
    LED_fault = 0;
    
    /* the module status, 0:ok, 1: abnormal */
    send_cmd[1] = LED_fault | send_cmd[2] | send_cmd[3] | send_cmd[4];
    /* send data */
    Boot_Ctrl_Diag_Cmd_Save(send_cmd, FUNC_DEFECT_CMD_SIZE);
}

/***********************************************************************************************
*
* @brief    This is not corrent result, only response to commnd ID 0x81;
                defect voltage range.
* @param    none
* @return   none
*
************************************************************************************************/
static void Boot_Ctrl_Voltage_Range(void)
{
    #define VOLTAGE_RANGE_CMD_SIZE   0x09
    
    uint8_t send_cmd[VOLTAGE_RANGE_CMD_SIZE]={0};
    
    send_cmd[0] = CMD_SEND_ID_VOLTAGE_RANGE;
    /* input voltage status, 0:(10V-15V), 1:(15V-16V), 2:(9V-10V), 3:other */
    send_cmd[1] = 0;
    
    /* send data */
    Boot_Ctrl_Diag_Cmd_Save(send_cmd, VOLTAGE_RANGE_CMD_SIZE);
}

/***********************************************************************************************
*
* @brief    The flag of bootprogram is right,the other is wrong;
                defect Version.
* @param    none
* @return   none
*
************************************************************************************************/
static void Boot_Ctrl_Version(void)
{
    #define VERSION_CMD_SIZE   0x09
    
    uint8_t send_cmd[VERSION_CMD_SIZE]={0};

    send_cmd[0] = CMD_SEND_ID_VERSION;
    /* HW Version number before the dot, for example, 1.2, send_cmd[1]=1, send_cmd[2]=2 */
    send_cmd[1] = 0;
    send_cmd[2] = 0;
    /* SW Version number before the dot, for example, 1.2, send_cmd[3]=1, send_cmd[4]=2 */
    send_cmd[3] = 0;
    send_cmd[4] = 0;
    /* Is SW UserProgram or BootProgram, 0:userprogram, 1:bootprogram */
    send_cmd[5] = 1;

    /* send data */
    Boot_Ctrl_Diag_Cmd_Save(send_cmd, VERSION_CMD_SIZE);
}

/***********************************************************************************************
*
* @brief    encapsulated command format 
* @param    send_data: the data need to be made into standard or extended frame
                data_size:  the size of the data need to be made into standard or extended frame
* @return   none
*
************************************************************************************************/
static void Boot_Ctrl_Diag_Cmd_Save(uint8_t* cmd_data, uint8_t cmd_size)
{
#define SEND_CMD_SIZE  11

    uint8_t number=0;
    uint32_t checksum=0;
    uint8_t send_data[SEND_CMD_SIZE] = {0};
    
    /* make standard frame */
    for(number=0; number<cmd_size; number++)
    {
        send_data[number] = cmd_data[number];
        checksum += cmd_data[number];
    }
    /* Extended length assignment*/
    send_data[number] = 0;
    checksum += send_data[number]+1;
    number++;
    /* checksum assignment*/
    send_data[number] = checksum;
    number++;

    /* save the command */
    Boot_Data_Send_Cmd_Save(send_data, number);
}

#endif /* BOOTLOADER */
