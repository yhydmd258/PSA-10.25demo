#include "project_cfg.h" 
#ifdef  DESERIALIZE_MODULE
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
/*******************************************************************************
* Includes
********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "derivative.h"
#include "deserialize_uart.h"
#include "deserialize_data.h"
#include "uart.h"


/*******************************************************************************
* Constants
********************************************************************************/


/*******************************************************************************
* Macros
********************************************************************************/
                
/*******************************************************************************
* Types
********************************************************************************/


/*******************************************************************************
* Variables
********************************************************************************/
static DES_UART_RECEIVE_DATA receive_buf={0};

/*******************************************************************************
* Global Variables
********************************************************************************/


/*******************************************************************************
* static Functions
********************************************************************************/
static void Uart0_SendChar(UINT8 send);
//static void Uart1_SendChar(UINT8 send);
static UINT8 Uart0_GetChar(void);

/***********************************************************************************************
*
* @brief    Deserialize_UART_Init - Initalizes the Uart 0 to run at 500K bauds assuming bus clock of 24Mhz
* @param    none
* @return   none
*
************************************************************************************************/  
void Deserialize_UART_Init(void)
{
    SIM_PINSEL0 |= SIM_PINSEL_UART0PS_MASK;
#if 0
    SIM_SCGC |=  SIM_SCGC_UART0_MASK;		/* Enable bus clock in UART0*/
    UART0_BDH = 0;							/* One stop bit*/
    UART0_BDL = 3;						    /* Baud rate at 500K, bus clock/(16*BR)=24M/(16*3)=500K*/
    UART0_C1  = 0;							/* No parity enable,8 bit format*/
    UART0_C2 |= UART_C2_TE_MASK;			/* Enable Transmitter*/
    UART0_C2 |= UART_C2_RE_MASK;			/* Enable Receiver*/
    UART0_C2 |= UART_C2_RIE_MASK;			/* Enable Receiver interrupts*/
#endif

#if 0
	SIM_SCGC |=  SIM_SCGC_UART0_MASK;		/* Enable bus clock in UART0*/
	UART0_BDH = 0;							/* One stop bit*/
	UART0_BDL = 11;						    /* Baud rate at 115200, bus clock/(16*BR)=20M/(16*BR)=113636*/
//    UART0_BDL = 5;                         /* Baud rate at 256000, bus clock/(16*BR)=20M/(16*BR)=250000*/
	UART0_C1  = 0;							/* No parity enable,8 bit format*/
    UART0_C1 |= UART_C1_M_MASK;             /* No parity enable,9 bit format */
    UART0_C1 |= UART_C1_PE_MASK;            /* parity enable */
	UART0_C2 |= UART_C2_TE_MASK;			/* Enable Transmitter*/
	UART0_C2 |= UART_C2_RE_MASK;			/* Enable Receiver*/
	UART0_C2 |= UART_C2_RIE_MASK;			/* Enable Receiver interrupts*/	
#endif
#if 1
			UART_ConfigType Uart_Config={{0}};
		
			Uart_Config.sctrl1settings.bits.bM=1;	/* 9 bit Mode */
			Uart_Config.sctrl1settings.bits.bPe=1;	/* Parity Enable */
			Uart_Config.bSbns=0;					/* One stop bit*/
			Uart_Config.sctrl2settings.bits.bRe=1;	/* Receiver enable*/
			Uart_Config.sctrl2settings.bits.bTe=1;	/* Transmitter enable*/
			Uart_Config.sctrl2settings.bits.bRie=1; /* Receiver interrupt enable*/
			Uart_Config.u32SysClkHz = 20000000; 	/* Bus clock in Hz*/
		//	  Uart_Config.u32Baudrate = 256000; 	  /* Baud rate*/
			 Uart_Config.u32Baudrate = 9600;	   /* Baud rate*/
		//	Uart_Config.u32Baudrate = 115200;	  /* Baud rate*/
			/*Initialization of UART module*/
		//	UART_SetCallback(MAX_SERIALIZE_UART, Max_Ser_Data_IRQHander);
			UART_Init(UART0,&Uart_Config);
#endif
 //   NVIC_EnableIRQ(UART0_IRQn);
#if 0
    /* variables init */
    memset(receive_buf.data_buf, 0x00, sizeof(receive_buf.data_buf));
    receive_buf.read_pos = 0x00;
    receive_buf.write_pos = 0x00;
#endif
}

/***********************************************************************************************
*
* @brief    Deserialize_UART_Init - Initalizes the Uart 0 to run at 500K bauds assuming bus clock of 24Mhz
* @param    none
* @return   none
*
************************************************************************************************/  
DES_UART_RECEIVE_DATA* Deserialize_UART_Get_Data(void)
{
    return &receive_buf;
}

/***********************************************************************************************
*
* @brief    Uart_SetCallback - Set a callback function to execute when a byte is received on the Uart2
* @param    pointer to function with an UINT8 argument
* @return   none
*
************************************************************************************************/  
void Deserialize_UART_Send_Data(UINT8* data, UINT8 data_size)
{
    UINT8 send_size=0;

    if((NULL == data) || (0 == data_size))
        return;
    
    while(send_size < data_size)
    {
        Uart0_SendChar(data[send_size]);    /* send data */
        send_size++;
    }
}

/***********************************************************************************************
*
* @brief    UART2_IRQHandler - Uart2 reception interrupt, calls the user defined callback function 
* @param    none
* @return   none
*
************************************************************************************************/
void UART0_IRQHandler(void)
{
	(void)UART0_S1;		/* Clear reception flag mechanism*/
#if 0
//    DISABLE_INTERRUPTS;
    
    if(((0 == receive_buf.read_pos)&&(0 == receive_buf.write_pos))  /* buf is empty */
        || (receive_buf.read_pos != receive_buf.write_pos))         /* buf is not full */
    {
        receive_buf.data_buf[receive_buf.write_pos] = Uart0_GetChar(); /* receive data and save it */
        
        receive_buf.write_pos++;                                    /* write position offset */
        receive_buf.write_pos = receive_buf.write_pos%RECEIVE_MAX_SIZE; /* prevent more than the max length */
    }

//    ENABLE_INTERRUPTS;
#endif
    receive_buf.data_buf[receive_buf.write_pos] = Uart0_GetChar(); /* receive data and save it */
    if((0==receive_buf.write_pos) && (0x79==receive_buf.data_buf[0]))
        receive_buf.write_pos++;
    else if((1==receive_buf.write_pos) && (0x66==receive_buf.data_buf[1]))
        receive_buf.write_pos++;
    else if((2==receive_buf.write_pos) && (0x01 == receive_buf.data_buf[2]))
        receive_buf.write_pos++;
    else if(3==receive_buf.write_pos)
    {
        if(3 != receive_buf.data_buf[3])
        {
            memset(&receive_buf.data_buf[0],0,3);
            receive_buf.write_pos = 0;
        }
        else
        {
            receive_buf.write_pos++;
        }
    }
    else 
    {
        if(receive_buf.write_pos>3)
        {
            if(receive_buf.write_pos < 6)
                receive_buf.write_pos++;
            else
            {
            if(Deserialize_Data_Backlight_sta.update_flag==0)
            	{
               	 Deserialize_Data_Set_Backlight(receive_buf.data_buf[4],1);
               	 memset(&receive_buf.data_buf[0],0,7);
               	 receive_buf.write_pos = 0;
            	}
            }
        }
        else
            receive_buf.write_pos = 0;
    }
    
}

/***********************************************************************************************
*
* @brief    Uart_SendChar - Send a single byte on Uart2 
* @param    byte to send
* @return   none
*
************************************************************************************************/  
static void Uart0_SendChar(UINT8 send)
{
	while((UART0_S1&UART_S1_TDRE_MASK)==0);		/* Wait for transmit buffer to be empty*/
	(void)UART0_S1;								/* Read UART0_S1 register*/
	UART0_D=send;								/* Send data*/
}
/*static void Uart1_SendChar(UINT8 send)
{
	while((UART1_S1&UART_S1_TDRE_MASK)==0);		
	(void)UART1_S1;								
	UART1_D=send;								
}*/

/***********************************************************************************************
*
* @brief    Uart_GetChar - Retrieve the received char from the Uart2 buffer (should be called from Interrupt) 
* @param    none
* @return   received byte
*
************************************************************************************************/  
static UINT8 Uart0_GetChar(void)
{
	
  UINT8 recieve;
  while(( UART0_S1 & UART_S1_RDRF_MASK)==0);	/* Wait for received buffer to be full*/
  (void) UART0_S1;								/* Read UART0_S1 register*/
  recieve= UART0_D;								/* Read received data*/
  return recieve;

} 
#endif /* DESERIALIZE_MODULE */

