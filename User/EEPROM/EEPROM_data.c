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
#ifdef EEPROM_MODULE
#include <string.h>
#include "I2C.h"
#include "../Timer/timer_if.h"
#include "../Command/command.h"

#include "EEPROM_data.h"
#include "EEPROM_ctrl.h"

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
static uint8_t EEPROM_Data_Master_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size);
static uint8_t EEPROM_Data_Master_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size);

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief    init the EEPROM module, this mudule is initialized after CTP init
* @param    none
* @return   none
*
************************************************************************************************/
void EEPROM_Data_Init(void)
{

}

/***********************************************************************************************
*
* @brief    send the data to the EEPROM(BR24A16F) on the same page or different page
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   none
*
************************************************************************************************/
uint8_t EEPROM_Data_Random_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size)
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
        u8ErrorStatus = EEPROM_Data_Master_Write(write_start_addr, &(send_data[data_number]), write_end_addr-write_start_addr);
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
uint8_t EEPROM_Data_Random_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size)
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
        u8ErrorStatus = EEPROM_Data_Master_Read(read_start_addr, &(read_data[data_number]), read_end_addr-read_start_addr);
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
* @brief    read the data from the same page
* @param    send_data : the pointer of the send data
                 data_size : ths size of the send data
* @return   none
*
************************************************************************************************/
static uint8_t EEPROM_Data_Master_Write(uint32_t data_addr,uint8_t *send_data, uint32_t data_size)
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
static uint8_t EEPROM_Data_Master_Read(uint8_t data_addr, uint8_t *read_data, uint32_t data_size)
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

#endif	/* EEPROM_MODULE */
