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
#include "i2c.h"
#include "gpio.h"
#include "flash.h"
#include "../Timer/timer_if.h"
#include "../Command/command.h"

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
#define BOOT_DESER_I2C_SLAVE_ADRESS   0x34
#define BOOT_DESER_I2C I2C0

/**********************************************************************************************
* Local types
**********************************************************************************************/
/* the struct of sending data*/
typedef struct
{
    uint8_t write_pos;
    uint8_t read_pos;
    /* the first byte of every buf is the length of valid data which need to be sent */
    uint8_t buf[BOOT_SEND_BUF_NUMBER][BOOT_SEND_BUF_SIZE];
} BOOT_SEND_BUF;

/* the struct of received data */
typedef struct
{
    uint8_t write_pos;
    uint8_t read_pos;
    /* the first byte of every buf is the length of valid received data  */
    uint8_t buf[BOOT_REC_BUF_NUMBER][BOOT_REC_BUF_SIZE];
} BOOT_RECEIVE_BUF;

/* the struct of initializing deserialize(948) */
typedef struct
{
    uint8_t device_address;
    uint8_t register_address;
    uint8_t byte;
}TI_SERIALIZE_I2C_SET;

/**********************************************************************************************
* Local function prototypes
*********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static BOOT_SEND_BUF       Boot_Send_Buf = {0};
static BOOT_RECEIVE_BUF    Boot_Receive_Buf = {0};

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static uint8_t Boot_Data_Eeprom_Master_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size);
static uint8_t Boot_Data_Eeprom_Master_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size);
static void Boot_Data_GPIO_Init(void);
static void Boot_Data_ISR(void);
static uint8_t Boot_Data_Deser_Comm_Enable(void);
static uint8_t Boot_Data_Deser_I2C_Write(uint8_t *send_buf, uint8_t buf_size);
static uint8_t Boot_Data_Deser_I2C_Read(uint8_t dev_add, uint8_t reg_add, uint8_t *read_data);

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
void Boot_Data_Init(void)
{
    uint8_t result=0;
    uint8_t init_time = 3;
    I2C_ConfigType  sI2C_Config0 = {{0}};
    I2C_ConfigType  I2C_Config = {{0}};

    /* init GPIO */
    Boot_Data_GPIO_Init();

    for(; init_time>0; init_time--)
    {
        /* Init I2C as master, send the data to register, after that, Deinit it and  init I2C as slave.*/
        /* Initialize I2C module as master */
        sI2C_Config0.u16Slt = 0x0000;
    //    I2C_Config.u16F = 0x0090; /* Baud rate at 100Kbps, MULT=4,  SCL divider=48, 20M/(MULT*divider) */
        sI2C_Config0.u16F = 0x0010; /* Baud rate at 400Kbps, MULT=1,  SCL divider=48, 20M/(MULT*divider) */
        sI2C_Config0.sSetting.bMSTEn = 1;/* master mode */
        sI2C_Config0.sSetting.bIntEn = 0;
        sI2C_Config0.sSetting.bI2CEn = 1;

        I2C0_SetCallBack(NULL);
        I2C_Init(BOOT_DESER_I2C, &sI2C_Config0);

        result = Boot_Data_Deser_Comm_Enable();
        /* send the init data to the deserializer IC (948) */
        if(!result)
            break;
    }
        
    I2C_Deinit(BOOT_DESER_I2C);
    
    /* Initialize I2C module as slave for deserializer */
    sI2C_Config0.u16Slt = 0;
    sI2C_Config0.sSetting.bMSTEn = 0; /* the slave of I2C */
    sI2C_Config0.u16OwnA1 = BOOT_DESER_I2C_SLAVE_ADRESS;
    sI2C_Config0.sSetting.bIntEn = 1;
    sI2C_Config0.sSetting.bI2CEn = 1;
    I2C0_SetCallBack(Boot_Data_ISR);
    I2C_Init(BOOT_DESER_I2C, &sI2C_Config0);

    
    /* Initialize I2C module as master for eeprom and CTP*/
    I2C_Config.u16Slt = 0x0000;
//    I2C_Config.u16F = 0x0090; /* Baud rate at 100Kbps, MULT=4,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.u16F = 0x0010; /* Baud rate at 400Kbps, MULT=1,  SCL divider=48, 20M/(MULT*divider) */
    I2C_Config.sSetting.bMSTEn = 1;
    I2C_Config.sSetting.bIntEn = 0;
    I2C_Config.sSetting.bI2CEn = 1;

    I2C_Init(EEPROM_I2C, &I2C_Config);

    /* Flash Init */
    FLASH_Init(BUS_CLK_HZ);

    
    Boot_Send_Buf.read_pos = BOOT_SEND_BUF_NUMBER - 1;
    Boot_Send_Buf.write_pos = 0;

    Boot_Receive_Buf.read_pos = BOOT_REC_BUF_NUMBER - 1;
    Boot_Receive_Buf.write_pos = 0;
}

/***********************************************************************************************
*
* @brief    Deinitialize the Boot module
* @param    none
* @return   none
*
************************************************************************************************/
void Boot_Data_Deinit(void)
{
    OUTPUT_CLEAR(PTE, PTE5);  /*PDB*/
    OUTPUT_CLEAR(PTE, PTE6);  /* 1V2_EN*/
    OUTPUT_CLEAR(PTH, PTH0);  /* BISTC */

    Timer_If_Delay(TIMER_DELAY_70MS);

    OUTPUT_CLEAR(PTH, PTH6);    /*LCM/BL/DES_POWER_EN*/

    /* Deinit I2C*/
    I2C_Deinit(BOOT_DESER_I2C);
}

/***********************************************************************************************
*
* @brief    read the command from the receive buf
* @param    cmd_data: the pointer of command
                  cmd_size: the size of command
* @return   none
*
************************************************************************************************/
void Boot_Data_Cmd_Read(uint8_t *cmd_data, uint32_t *cmd_size)
{
    uint8_t  read_position = 0;

    if((NULL == cmd_data) || (NULL == cmd_size))
        return;
    else
    {
        read_position = (Boot_Receive_Buf.read_pos + 1) % BOOT_REC_BUF_NUMBER;
        if(read_position != Boot_Receive_Buf.write_pos)
        {
            *cmd_size = Boot_Receive_Buf.buf[read_position][BOOT_BUF_LENGTH_POS];
            memcpy(cmd_data, &(Boot_Receive_Buf.buf[read_position][BOOT_BUF_LENGTH_POS_SIZE]), *cmd_size);
//            memset(Boot_Receive_Buf.buf[read_position], 0x00, BOOT_REC_BUF_SIZE);
            Boot_Receive_Buf.read_pos = read_position;
        }
        else
        {
            *cmd_size = 0;
        }
    }
}

/***********************************************************************************************
*
* @brief      save the command which will be sent
* @param    send_data: the data will be sent to MMI
                data_size: the size of the data will be sent to MMI
* @return   none
*
************************************************************************************************/
void Boot_Data_Send_Cmd_Save(uint8_t* send_data, uint32_t data_size)
{
    uint8_t  write_position = 0;
    uint8_t  number = 0;

    if((NULL == send_data) || (0 == data_size))
        return;
    else
    {
        for(number=Boot_Send_Buf.write_pos; (number+1)%BOOT_SEND_BUF_NUMBER != Boot_Send_Buf.read_pos; number=(number+1)%BOOT_SEND_BUF_NUMBER)
        {
            if(CMD_SEND_ID_UPGRADE == Boot_Send_Buf.buf[write_position][1])
                return;
        }
            
        write_position = Boot_Send_Buf.write_pos;
        if(((write_position + 1) % BOOT_SEND_BUF_NUMBER) != Boot_Send_Buf.read_pos)
        {
            memset(Boot_Send_Buf.buf[write_position], 0x00, BOOT_SEND_BUF_SIZE);
            Boot_Send_Buf.buf[write_position][0] = (data_size > (BOOT_SEND_BUF_SIZE - 1)) ? (BOOT_SEND_BUF_SIZE - 1) : data_size;
            memcpy(&(Boot_Send_Buf.buf[write_position][1]), send_data, Boot_Send_Buf.buf[write_position][0]);
            Boot_Send_Buf.write_pos = (write_position + 1) % BOOT_SEND_BUF_NUMBER;
        }
    }
}

/***********************************************************************************************
*
* @brief    Erase the flash where save application image
* @param    none
* @return   0: erase success, other: error
*
************************************************************************************************/
uint16_t Boot_Data_App_Image_Erase(void)
{
    uint16_t erase_result = 0;
    uint32_t erase_start_addr = 0;

    __disable_irq();

    for(erase_start_addr=APP_IMAGE_START_ADDR; erase_start_addr<MCU_FLASH_SIZE_MAX; erase_start_addr+=FLASH_SECTOR_SIZE)
    {
        /* erase section every time */
        erase_result = FLASH_EraseSector(erase_start_addr);
        if(FLASH_ERR_SUCCESS != erase_result)
            break;
    }

    __enable_irq();

    return erase_result;
}

/***********************************************************************************************
*
* @brief    write the flash where save application image
* @param    none
* @return   0: write success, other: error
*
************************************************************************************************/
uint16_t Boot_Data_App_Image_Write(uint32_t u32NVMTargetAddress, uint8_t *pData, uint16_t u16SizeBytes)
{
    uint16_t u16Err = FLASH_ERR_SUCCESS;
    uint16_t u16w2LongWordCount = u16SizeBytes>>3;
    uint8_t  u8WrLeftBytes = (u16SizeBytes & 0x07);
    uint16_t u16WrLeftLongWords = u8WrLeftBytes>>2;
    uint32_t u32WrTargetAddress = u32NVMTargetAddress;
    uint32_t u32DwData0,u32DwData1;
    uint32_t *pDwData = (uint32_t*)pData;
    uint32_t  i;
    
    // Check address to see if it is aligned to 4 bytes
    // Global address [1:0] must be 00.
    if(u32NVMTargetAddress & 0x03)
    {
        u16Err = FLASH_ERR_INVALID_PARAM;
        return (u16Err);
    }
    // Loop for the two longwords (8 bytes) programming
    for(i = 0; i < u16w2LongWordCount; i++)
    {
        u32DwData0 = *pDwData++;
        u32DwData1 = *pDwData++;
        u16Err = FLASH_Program2LongWords(u32WrTargetAddress, u32DwData0, u32DwData1);
        if(u16Err)
        {
            return (u16Err);
        }
        u32WrTargetAddress += 8;
    }
    // Loop for the single longword (4 bytes) programming
    for(i = 0; i < u16WrLeftLongWords; i++)
    {
        u32DwData0 = *pDwData++;
        u16Err = FLASH_Program1LongWord(u32WrTargetAddress, u32DwData0);
        if(u16Err)
        {           
            return (u16Err);
        }
        u32WrTargetAddress += 4;
    }
    u8WrLeftBytes = (u8WrLeftBytes-(u16WrLeftLongWords<<2));    // calculate the # of bytes that are not programmed
    if(!u8WrLeftBytes){
        return (u16Err);
    }

    /* little_ENDIAN */        
    u32DwData0 = 0xFFFFFFFFL;        
    pData = (uint8_t*)pDwData+u8WrLeftBytes-1;  // pointer to the left bytes
    for(i = u8WrLeftBytes; i >0; i--)
    {
        u32DwData0 <<= 8;
        u32DwData0 |= *pData--; // MSB byte first
    }

    // Now program the last longword
    u16Err = FLASH_Program1LongWord(u32WrTargetAddress, u32DwData0);    

    return (u16Err);
}

/***********************************************************************************************
*
* @brief    set the level of the I2C interrupt pin
* @param    0: notice the host to read data, other: no data need to read
* @return   none
*
************************************************************************************************/
void Boot_Data_Interrupt_Set(uint8_t pin_level)
{
    if(0 == pin_level)
        OUTPUT_CLEAR(PTH, PTH0);
    else
        OUTPUT_SET(PTH, PTH0);
}

/***********************************************************************************************
*
* @brief      if the data to be sent, the interrupt pin set low
* @param    none
* @return   flase: the interrupt pin don't set low
                 true: the interrupt pin need to be set low
*
************************************************************************************************/
uint8_t Boot_Data_Interrupt_Low_Check(void)
{
    uint8_t result = FALSE;

    if(((Boot_Send_Buf.read_pos + 1) % BOOT_SEND_BUF_NUMBER) != Boot_Send_Buf.write_pos)
    {
        result = TRUE;
    }
    else
        result = FALSE;

    return result;
}

/***********************************************************************************************
*
* @brief    send the data to the EEPROM(BR24A16F) on the same page or different page
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   none
*
************************************************************************************************/
uint8_t Boot_Data_Eeprom_Random_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size)
{
    uint32_t write_start_addr = 0; /* start address in every write cycle */
    uint32_t write_end_addr = 0;    /*  start address in every write cycle */
    uint32_t addr_int = 0;
    uint32_t data_number = 0;
    uint8_t u8ErrorStatus = 0;

    /* the int of date_addr/EEPROM_PAGE_LENGTH_MAX */
    addr_int = data_addr/EEPROM_PAGE_LENGTH_MAX;
    /* start address in every write cycle */
//    write_end_addr = ((addr_int+1)*EEPROM_PAGE_LENGTH_MAX < (data_addr+data_size))? (addr_int+1)*EEPROM_PAGE_LENGTH_MAX : (data_addr+data_size);

    for(write_start_addr = data_addr; write_start_addr < (data_addr+data_size); addr_int++)
    {
        write_end_addr = ((addr_int+1)*EEPROM_PAGE_LENGTH_MAX < (data_addr+data_size))? (addr_int+1)*EEPROM_PAGE_LENGTH_MAX : (data_addr+data_size);
        u8ErrorStatus = Boot_Data_Eeprom_Master_Write(write_start_addr, &(send_data[data_number]), write_end_addr-write_start_addr);
        if(u8ErrorStatus)
            break;
        /* after writed, the start address offset */
        write_start_addr =write_end_addr ;
        /* the position of send data offset */
        data_number = write_end_addr - data_addr;
        
        Timer_If_Delay(TIMER_DELAY_6MS);
    }

    return u8ErrorStatus;
}

/***********************************************************************************************
*
* @brief    send the data to the EEPROM(BR24A16F) on the same page or different page
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   none
*
************************************************************************************************/
uint8_t Boot_Data_Eeprom_Random_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size)
{
    uint32_t read_start_addr = 0; /* start address in every read cycle */
    uint32_t read_end_addr = 0;    /*  start address in every read cycle */
    uint32_t addr_int = 0;
    uint32_t data_number = 0;
    uint8_t u8ErrorStatus = 0;

    /* the int of date_addr/EEPROM_PAGE_LENGTH_MAX */
    addr_int = data_addr/EEPROM_PAGE_LENGTH_MAX;
    /* start address in every read cycle */
//    read_end_addr = ((addr_int+1)*EEPROM_PAGE_LENGTH_MAX < (data_addr+data_size))? (addr_int+1)*EEPROM_PAGE_LENGTH_MAX : (data_addr+data_size);

    for(read_start_addr = data_addr; read_start_addr < (data_addr+data_size); addr_int++)
    {
        read_end_addr = ((addr_int+1)*EEPROM_PAGE_LENGTH_MAX < (data_addr+data_size))? (addr_int+1)*EEPROM_PAGE_LENGTH_MAX : (data_addr+data_size);
        u8ErrorStatus = Boot_Data_Eeprom_Master_Read(read_start_addr, &(read_data[data_number]), read_end_addr-read_start_addr);
        if(u8ErrorStatus)
            break;
        /* after read, the start address offset */
        read_start_addr = read_end_addr;
        /* the position of read data offset */
        data_number = read_end_addr - data_addr;
    }

    return u8ErrorStatus;
}

/***********************************************************************************************
*
* @brief    read the data from the same page or different page
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   none
*
************************************************************************************************/
static uint8_t Boot_Data_Eeprom_Master_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size)
{
    uint8_t slave_addr = 0;
    uint8_t data_addr_low = 0;
    uint32_t i;
    uint8_t u8ErrorStatus = 0;
    
    if((NULL==send_data) || (0 == data_size))
        return u8ErrorStatus;

    /*  the address has 11 bits, the high 3 bits is in the slave_addr(bit3, bit2, bit1) */
    slave_addr = ((uint8_t)(data_addr>>7))&0x0E;
    slave_addr |= EEPROM_ADDRESS;
    /* the end of 8 bits in the data_addr */
    data_addr_low = (uint8_t)data_addr;

    /* send start signals to bus */
    u8ErrorStatus = I2C_Start(EEPROM_I2C);

    /* send device address to slave */
    u8ErrorStatus = I2C_WriteOneByte(EEPROM_I2C,slave_addr| I2C_WRITE);

	if( u8ErrorStatus == I2C_ERROR_NULL )
    {
        /* send data address */
        u8ErrorStatus = I2C_WriteOneByte(EEPROM_I2C,data_addr_low);
        
        if( u8ErrorStatus == I2C_ERROR_NULL )
        {
            /* if no error occur, received the correct ack from slave continue to send data to slave */
            if( u8ErrorStatus == I2C_ERROR_NULL )
            {
                for(i=0;i<data_size;i++)
                {
                    u8ErrorStatus = I2C_WriteOneByte(EEPROM_I2C,send_data[i]);
                    if( u8ErrorStatus != I2C_ERROR_NULL )
                    {
                        I2C_Stop(EEPROM_I2C);
                        return u8ErrorStatus;
                    }
                }
             }
        }
    }   
	 /* send stop signals to bus */
	 u8ErrorStatus = I2C_Stop(EEPROM_I2C);

	 return u8ErrorStatus;
}

/***********************************************************************************************
*
* @brief    read the data from the same page
* @param    reg_addr : the address of the register
                read_data : the pointer of the read data
                data_size : the size of the read data
* @return   0: read OK
                other: read error
*
************************************************************************************************/
static uint8_t Boot_Data_Eeprom_Master_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size)
{
    uint8_t slave_addr = 0;
    uint8_t data_addr_low = 0;
    uint32_t i;
    uint8_t u8ErrorStatus=0;;
    
    if((NULL==read_data) || (0 == data_size))
        return u8ErrorStatus;
    
    /*  the address has 11 bits, the high 3 bits is in the slave_addr(bit3, bit2, bit1) */
    slave_addr = ((uint8_t)(data_addr>>7))&0x0E;
    slave_addr |= EEPROM_ADDRESS;
    /* the end of 8 bits in the data_addr */
    data_addr_low = (uint8_t)data_addr;

    /* send start signals to bus */
    u8ErrorStatus = I2C_Start(EEPROM_I2C);

    /* send device address to slave */
    u8ErrorStatus = I2C_WriteOneByte(EEPROM_I2C,slave_addr| I2C_WRITE);

	if( u8ErrorStatus == I2C_ERROR_NULL )
    {
        /* send data address */
        u8ErrorStatus = I2C_WriteOneByte(EEPROM_I2C,data_addr_low);

    	if( u8ErrorStatus == I2C_ERROR_NULL )
        {
            /* repeart start */
            u8ErrorStatus = I2C_RepeatStart(EEPROM_I2C);

            if( u8ErrorStatus == I2C_ERROR_NULL )
            {
                /* send device address to slave */
                u8ErrorStatus = I2C_WriteOneByte(EEPROM_I2C,slave_addr | I2C_READ);
                /* dummy read one byte to switch to Rx mode */
                I2C_ReadOneByte(EEPROM_I2C, &(read_data[0]), I2C_SEND_ACK);
                
                if( u8ErrorStatus == I2C_ERROR_NULL )
                {
                    for(i=0; i<data_size-1; i++)
                    {
                        /* read data */
                        u8ErrorStatus = I2C_ReadOneByte(EEPROM_I2C, &(read_data[i]), I2C_SEND_ACK);
                        
                        if( u8ErrorStatus != I2C_ERROR_NULL )
                            break;
                    }
                    u8ErrorStatus = I2C_ReadOneByte(EEPROM_I2C, &(read_data[i]), I2C_SEND_NACK);
                }
            }
        }   
    }
    
    /* send stop signals to bus */
    u8ErrorStatus = I2C_Stop(EEPROM_I2C);

    return u8ErrorStatus;
}


/***********************************************************************************************
*
* @brief      initialize the GPIO
* @param    none
* @return   none
*
************************************************************************************************/
static void Boot_Data_GPIO_Init(void)
{
    /* power enable */
    CONFIG_PIN_AS_GPIO(PTH, PTH6, OUTPUT);	 // LCM/BL/DES_POWER_EN
    OUTPUT_SET(PTH, PTH6);
    CONFIG_PIN_AS_GPIO(PTE, PTE6, OUTPUT);	 // 1V2_EN
    OUTPUT_SET(PTE, PTE6);
    /* wait for normal power supply(3.3V)  */
    Timer_If_Delay(TIMER_DELAY_2MS);
    /* PDB */
    CONFIG_PIN_AS_GPIO(PTE, PTE5, OUTPUT);	//PDB
    OUTPUT_SET(PTE, PTE5);					//PDB
    Timer_If_Delay(TIMER_DELAY_10MS);
    /* the I2C interrupt pin */
    CONFIG_PIN_AS_GPIO(PTH, PTH0, OUTPUT);	/* BISTC */
    OUTPUT_SET(PTH, PTH0);					/* BISTC */
}

/***********************************************************************************************
*
* @brief    initialize the CTP module
* @param    none
* @return   none
*
************************************************************************************************/
static void Boot_Data_ISR(void)
{
    static uint8_t Receive_Number = 0; // used by receiving data
    static uint32_t Receive_Cmd_Size = 0;
    static uint8_t Receive_Cmd_Size_flag = 0;
    static uint8_t start_receive = 0; //  1: start to save, 0: not save
    uint32_t send_position = 0;  // used by sending data
    uint8_t  send_buf[BOOT_SEND_BUF_SIZE] = {0};
    uint8_t  read_position=0;    // used by sending data
    uint8_t  receive_data=0;    // the buf is save the receive buf
    uint8_t  u8ErrorStatus=0;
    
    /* clear the interrupt falg */
    I2C_ClearStatus(BOOT_DESER_I2C, I2C_S_IICIF_MASK);

    /* IASS=1, mcu is addressed as a slave, the register data is address of the MCU */
    if(I2C_GetStatus(BOOT_DESER_I2C)&I2C_S_IAAS_MASK)
    {
        /* slave send acknowledge */
        I2C_SendAck(BOOT_DESER_I2C);

        /* slave writes to Master */
        if(I2C_GetStatus(BOOT_DESER_I2C)& I2C_S_SRW_MASK)
        {
            /* after the slave address matched, set interrupt pin high */
            Boot_Data_Interrupt_Set(1);
            /* clear the timer */
            Timer_If_Clear(EN_UPGRADE_DESER_SEND_DATA);
            /*slave send data*/
            I2C_TxEnable(BOOT_DESER_I2C);
            read_position = (Boot_Send_Buf.read_pos + 1) % BOOT_SEND_BUF_NUMBER;
            if(read_position != Boot_Send_Buf.write_pos)
            {
                /* copy the command size */
                send_buf[0] = Boot_Send_Buf.buf[read_position][0];
                /* copy the command */
                memcpy(&(send_buf[1]), &(Boot_Send_Buf.buf[read_position][1]), Boot_Send_Buf.buf[read_position][0]);
                /* send the command */
                for(send_position = 0; send_position<send_buf[0]; send_position++)
                {
                    u8ErrorStatus = I2C_WriteOneByte(BOOT_DESER_I2C, send_buf[send_position+1]);
                    if(I2C_ERROR_NULL != u8ErrorStatus)
                        break;
                }
                /* after sending data, we will reaccept data */
                start_receive = 0;
                
                /* clear buf */
                memset(Boot_Send_Buf.buf[read_position], 0x00, BOOT_SEND_BUF_SIZE);
                Boot_Send_Buf.read_pos = read_position;
                /* clear status */
                Boot_Ctrl_Cmd_Send_Status_Clear();
            }
            else
            {
                I2C_WriteOneByte(BOOT_DESER_I2C, 0xFF);
            }
        }
        /* slave read from Master*/
        else
        {
            /* Dummy read */
            I2C_RxEnable(BOOT_DESER_I2C);
            I2C_ReadDataReg(BOOT_DESER_I2C);/* read the address of the MCU, do nothing*/
        }
    }
    /* IASS=0, mcu receive other data after receiving the address of mcu, or receive ACK/NACK  */
    else
    {
        /* slave writes to master */
        if(I2C_GetStatus(BOOT_DESER_I2C)&I2C_S_SRW_MASK)
        {
            /* No get ACK from master */
            if(I2C_IsReceivedAck(BOOT_DESER_I2C))
            {
                /* No receive the ACK, switch to RX*/
                I2C_RxEnable(BOOT_DESER_I2C);

                /* Dummy read*/
                I2C_ReadDataReg(BOOT_DESER_I2C);
            }
            /* get  ACK from master */
            else
            {
                /* Here do nothing, clock  streching or send a 0xff to master.*/
                I2C_WriteOneByte(BOOT_DESER_I2C, 0xFF);
            }
        }
        /*slave receive data from master*/
        else
        {
            receive_data = I2C_ReadDataReg(BOOT_DESER_I2C);

            /* Determine whether the data begins to be accepted*/
            if((0 == start_receive) && 
                ((Boot_Receive_Buf.write_pos + 1)%BOOT_REC_BUF_NUMBER!=Boot_Receive_Buf.read_pos))
            {
                /* start to receive the data and the next */
                start_receive = 1;
                Receive_Number = BOOT_BUF_LENGTH_POS_SIZE;
                Receive_Cmd_Size = BOOT_REC_BUF_SIZE; /* temporarily set, if received separator data, update it */
                Receive_Cmd_Size_flag = 0;
                memset(Boot_Receive_Buf.buf[Boot_Receive_Buf.write_pos], 0x00, BOOT_SEND_BUF_SIZE);
            }
            else
            {
                /* ignore this byte or receive the next data */
            }

            /* judge the data is saved or not */
            if(1 == start_receive)
            {
                Boot_Receive_Buf.buf[Boot_Receive_Buf.write_pos][Receive_Number] = receive_data;
                Boot_Receive_Buf.buf[Boot_Receive_Buf.write_pos][BOOT_BUF_LENGTH_POS]++;
                if((receive_data == BOOR_DESER_RECEIVE_SEPARATOR) && (0 == Receive_Cmd_Size_flag))
                {
                    /* after received separator, it has "EXT_len" and "checksum"*/
                    Receive_Cmd_Size = Boot_Receive_Buf.buf[Boot_Receive_Buf.write_pos][BOOT_BUF_LENGTH_POS] + 2;
                    Receive_Cmd_Size_flag = 1;
                }
                else if(Receive_Number == Receive_Cmd_Size)
                {
                    Boot_Receive_Buf.write_pos=(Boot_Receive_Buf.write_pos+1)%BOOT_REC_BUF_NUMBER;
                    Receive_Number=0;
                    start_receive =0;
                    Receive_Cmd_Size_flag = 0;
                }
                else
                {
                    /* do nothing */
                }
                Receive_Number++;
            }
            else
            {
                /* ignore this byte */
            }
        }
    }
}

/***********************************************************************************************
*
* @brief    send data to register of deserializer, if not success, try again, not more than 3 times
* @param   none
* @return   0:OK; 1:error;
*
************************************************************************************************/
static uint8_t Boot_Data_Deser_Comm_Enable(void)
{
#define TI_DESER_I2C_DEV_ADDRESS    0x58
//#define SEND_DATA_TIMES         3
#define SEND_DATA_SIZE_MAX      2
//    #define SEND_DATA_SIZE_MAX      2

    TI_SERIALIZE_I2C_SET    send_data[SEND_DATA_SIZE_MAX]={{TI_DESER_I2C_DEV_ADDRESS, 0x03, 0x78},
                                                            {TI_DESER_I2C_DEV_ADDRESS, 0x05, 0x9E}};
    uint8_t read_data=0;
    uint8_t result=0;
    uint8_t init_result=1;/* 0: OK, 1: error*/
    uint8_t number=0;
//    uint8_t init_time=0;

//    for(init_time=0; (init_time<SEND_DATA_TIMES)&&(1 == init_result); init_time++)
    {
        /* send data to deserializer */
        for(number=0; number<SEND_DATA_SIZE_MAX; number++)
        {
            result = Boot_Data_Deser_I2C_Write((uint8_t *)&send_data[number], sizeof(TI_SERIALIZE_I2C_SET));
            if(I2C_ERROR_NULL != result)
                break;
        }
        
        /* read data from deserializer */
        for(number=0; number<SEND_DATA_SIZE_MAX; number++)
        {
            result = Boot_Data_Deser_I2C_Read(send_data[number].device_address, send_data[number].register_address, &read_data);
            if((I2C_ERROR_NULL != result) || (send_data[number].byte!=read_data))
            {
                init_result = 1;
                break;
            }
            else
            {
                init_result = 0;
            }
        }
    }

    return init_result;
}
/***********************************************************************************************
*
* @brief    write data to deserizlize by I2C
* @param    send_buf: the pointer of send data
                  buf_size: the size of send data
* @return   0 : no error
*
************************************************************************************************/
static uint8_t Boot_Data_Deser_I2C_Write(uint8_t *send_buf, uint8_t buf_size)
{
    uint32_t i;
    uint8_t u8ErrorStatus;

    if( (NULL==send_buf) || (1>buf_size))
        return I2C_ERROR_NULL;

    /* set write bit */
    send_buf[0]	 = (send_buf[0]&0xFE)|I2C_WRITE;
    
    I2C_IntDisable(BOOT_DESER_I2C);

	/* send start signals to bus */
    u8ErrorStatus = I2C_Start(BOOT_DESER_I2C);

	/* if no error occur, received the correct ack from slave
			continue to send data to slave
		*/
    if( u8ErrorStatus == I2C_ERROR_NULL )
    {
        for(i=0;i<buf_size;i++)
        {
            u8ErrorStatus = I2C_WriteOneByte(BOOT_DESER_I2C,send_buf[i]);
            if( u8ErrorStatus != I2C_ERROR_NULL )
            {
                break;
            }
        }
    }

	 /* send stop signals to bus */
    u8ErrorStatus = I2C_Stop(BOOT_DESER_I2C);

    I2C_IntEnable(BOOT_DESER_I2C);

    return u8ErrorStatus;
}

/***********************************************************************************************
*
* @brief    read data from  deserialize by I2C
* @param    dev_add: device address
                 reg_add: register address
                 read_data: the pointer of data which used to store
* @return   0: no error
*
************************************************************************************************/
static uint8_t Boot_Data_Deser_I2C_Read(uint8_t dev_add, uint8_t reg_add, uint8_t *read_data)
{
    uint8_t u8ErrorStatus;

    I2C_IntDisable(BOOT_DESER_I2C);

	/* send start signals to bus */
    u8ErrorStatus = I2C_Start(BOOT_DESER_I2C);
    
    /* Adress + w */
    u8ErrorStatus = I2C_WriteOneByte(BOOT_DESER_I2C, dev_add);
    if( u8ErrorStatus == I2C_ERROR_NULL )
    {
        /* Register address */
        u8ErrorStatus = I2C_WriteOneByte(BOOT_DESER_I2C,reg_add);

        if( u8ErrorStatus == I2C_ERROR_NULL )
        {
            /* Restart */
            u8ErrorStatus =I2C_RepeatStart(BOOT_DESER_I2C);
            if( u8ErrorStatus == I2C_ERROR_NULL )
            {
                /* Adress + r*/
                u8ErrorStatus = I2C_WriteOneByte(BOOT_DESER_I2C,(dev_add&0xFE)|I2C_READ);
                
                /* dummy read one byte to switch to Rx mode */
                u8ErrorStatus = I2C_ReadOneByte(BOOT_DESER_I2C,read_data,I2C_SEND_ACK);
                
                u8ErrorStatus = I2C_ReadOneByte(BOOT_DESER_I2C,read_data,I2C_SEND_NACK);
            }
            
        }
    }
     /* send stop signals to bus */
    u8ErrorStatus = I2C_Stop(BOOT_DESER_I2C);

    I2C_IntEnable(BOOT_DESER_I2C);

    return u8ErrorStatus;
}

#endif /* BOOTLOADER */
