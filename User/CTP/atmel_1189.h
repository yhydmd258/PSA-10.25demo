
#ifdef  CTP_MODULE
#ifndef __QT_Controller_H__
#define __QT_Controller_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "derivative.h"
#include "CTP_ctrl.h"
//#include "Base.h"
//*****************************************************************************
//		Common Defines
//*****************************************************************************
/*
typedef signed char             int8_t ;  //!< 8-bit signed integer.
typedef unsigned char           uint8_t ;  //!< 8-bit unsigned integer.
typedef signed short int        int16_t;  //!< 16-bit signed integer.
typedef unsigned short int      uint16_t;  //!< 16-bit unsigned integer.
typedef signed long int         int32_t;  //!< 32-bit signed integer.
typedef unsigned long int       uint32_t;  //!< 32-bit unsigned integer.
typedef signed long long int    int64_t;  //!< 64-bit signed integer.
typedef unsigned long long int  uint64_t;  //!< 64-bit unsigned integer.
typedef float                   float32_t;  //!< 32-bit floating-point number.
typedef double                  float64_t;  //!< 64-bit floating-point number.
*/

#define false   0
#define true    1

//#define NULL           0

/*
 * casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to __u16, __u32, __u64. At least not in a portable way.
 */
// penghu #define BIG_ENDIAN

typedef  unsigned short __u16;
typedef  unsigned long  __u32;

#define ___constant_swab16(x) ((__u16)(				\
	(((__u16)(x) & (__u16)0x00ffU) << 8) |			\
	(((__u16)(x) & (__u16)0xff00U) >> 8)))

#define ___constant_swab32(x) ((__u32)(				\
	(((__u32)(x) & (__u32)0x000000ffUL) << 24) |		\
	(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) |		\
	(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) |		\
	(((__u32)(x) & (__u32)0xff000000UL) >> 24)))

/**
 * __swab16 - return a byteswapped 16-bit value
 * @x: value to byteswap
 */
#define __swab16(x)	___constant_swab16(x)

/**
 * __swab32 - return a byteswapped 32-bit value
 * @x: value to byteswap
 */
#define __swab32(x)	___constant_swab32(x)

#if defined(BIG_ENDIAN)
#define cpu_to_le32p(x) ___constant_swab32(x)
#define le32_to_cpup(x) ___constant_swab32(x)
#define cpu_to_le16p(x) ___constant_swab16(x)
#define le16_to_cpup(x) ___constant_swab16(x)
#define hi16_to_cpup(x) (x)
#else
#define cpu_to_le32p(x) (x)
#define le32_to_cpup(x) (x)
#define cpu_to_le16p(x) (x)
#define le16_to_cpup(x) (x)
#define hi16_to_cpup(x) ___constant_swab16(x)
#endif

/************************************************************************************************************
*       I2C Blocks
* ***************************************************************************/
/* I2c read/write flags */
#define I2C_WRITE_FLAG	0x00	/* bit 0 of slave-addres byte */
#define I2C_READ_FLAG	0x01

/* Retry times on NACK */
#define NACK_RETRY_MAX	10

typedef struct
{
    UINT8 slave_addr;
    UINT8 *txbuf;
    UINT16 txlen;
    UINT8 *rxbuf;
    UINT16 rxlen;

}i2c_package_t;

//*****************************************************************************
//		info_block_driver
//*****************************************************************************

/*! \brief Object table element struct. */
typedef struct
{
	UINT8 object_type;     // Object type ID. */
	UINT16 i2c_address;    // Start address of the obj config structure. 
	UINT8 size;            // Byte length of the obj config structure -1.
	UINT8 instances;       // Number of objects of this obj. type -1. 
    UINT8 num_report_ids;  // The max number of touches in a screen,
                              //   max number of sliders in a slider array, etc.
} object_t;

/*! \brief Info ID struct. */
typedef struct
{
	UINT8 family_id;            /* address 0 */
	UINT8 variant_id;           /* address 1 */
	
	UINT8 version;              /* address 2 */
	UINT8 build;                /* address 3 */
	
	UINT8 matrix_x_size;        /* address 4 */
	UINT8 matrix_y_size;        /* address 5 */
                                     /*! Number of entries in the object table. The actual number of objects
    * can be different if any object has more than one instance. */
	UINT8 num_declared_objects; /* address 6 */
} info_id_t;

/*! \brief Info block struct holding ID and object table data and their CRC sum.
*
* Info block struct. Similar to one in info_block.h, but since
* the size of object table is not known beforehand, it's pointer to an
* array instead of an array. This is not defined in info_block.h unless
* we are compiling with IAR AVR or AVR32 compiler (__ICCAVR__ or __ICCAVR32__
* is defined). If this driver is compiled with those compilers, the
* info_block.h needs to be edited to not include that struct definition.
*
* CRC is 24 bits, consisting of CRC and CRC_hi; CRC is the lower 16 bytes and
* CRC_hi the upper 8.
*
*/

typedef struct
{
    /*! Info ID struct. */
    info_id_t info_id;
	
    /*! Pointer to an array of objects. */
    object_t *objects;
	
    /*! CRC field, low bytes. */
    UINT16 CRC;
	
    /*! CRC field, high byte. */
    UINT8 CRC_hi;
} info_block_t;

typedef struct
{
	UINT8 object_type;     // Object type. */
	UINT8 instance;        // Instance number. */
} report_id_map_t;

//*****************************************************************************
//		std_objects_driver
//*****************************************************************************

/*! ===Header File Version Number=== */
#define OBJECT_LIST__VERSION_NUMBER     0x11

/*! \defgroup OBJECT_LIST ===Object Type Number List===
 This list contains the enumeration of each of the object types used in the
 chip information table.
 The Structure of the name used is:
 <OBJECTPURPOSE>_<OBJECTDESCRIPTION>_T<TYPENUMBER>
 Possible Object Purposes include:
 DEBUG, GEN (General), TOUCH (Touch Sensitive Objects), PROCI
 (Processor - instance), PROCT (Processor - type), PROCG (Processor - global)
 Below is the actual list, reserved entries should not be used, spare entries
 are available for use. No entries should ever be re-used once they have been
 included in the list. Once an entry is added its configuration and/or status
 structures should be completed and commented below.
 */

#define RESERVED_T0                               0u
#define RESERVED_T1                               1u
#define GEN_MESSAGEPROCESSOR_T5                   5u
#define GEN_COMMANDPROCESSOR_T6                   6u
#define TOUCH_MULTITOUCHSCREEN_T9                 9u
#define TOUCH_SINGLETOUCHSCREEN_T10               10u
#define TOUCH_KEYARRAY_T15                        15u
#define PROCI_ONETOUCHGESTUREPROCESSOR_T24        24u
#define PROCI_TWOTOUCHGESTUREPROCESSOR_T27        27u
#define MULTIPLE_TOUCHSCREEN_T100                   100u
#define MESSAGE_COUNT_T44                           44u

/*
 * All entries spare up to 255
*/
#define RESERVED_T255                             255u


/* MXT_TOUCH_MULTI_T9 status */
#define MXT_T9_UNGRIP		(1 << 0)
#define MXT_T9_SUPPRESS		(1 << 1)
#define MXT_T9_AMP		(1 << 2)
#define MXT_T9_VECTOR		(1 << 3)
#define MXT_T9_MOVE		(1 << 4)
#define MXT_T9_RELEASE		(1 << 5)
#define MXT_T9_PRESS		(1 << 6)
#define MXT_T9_DETECT		(1 << 7)


//*****************************************************************************
//		touch_driver
//*****************************************************************************

/*! \brief Touch driver version number. */
#define TOUCH_DRIVER_VERSION    0x02u

#define I2C_APPL_ADDR_0         0x4A    //0x4A,0x4B,0x24,0x25,
#define I2C_APPL_ADDR_1         0x4A
#define I2C_BOOT_ADDR_0         0x4A
#define I2C_BOOT_ADDR_1         0x4A

#define NUM_OF_I2C_ADDR         4

/* \brief Defines CHANGE line active mode. */
#define CHANGELINE_NEGATED          0u
#define CHANGELINE_ASSERTED         1u

#define CONNECT_OK                  1u
#define CONNECT_ERROR               2u

#define READ_MEM_OK                 1u
#define READ_MEM_FAILED             2u

#define MESSAGE_READ_OK             1u
#define MESSAGE_READ_FAILED         2u

#define WRITE_MEM_OK                1u
#define WRITE_MEM_FAILED            2u

#define I2C_INIT_OK                 1u
#define I2C_INIT_FAIL               2u

#define CRC_CALCULATION_OK          1u
#define CRC_CALCULATION_FAILED      2u

#define ID_MAPPING_OK               1u
#define ID_MAPPING_FAILED           2u

#define ID_DATA_OK                  1u
#define ID_DATA_NOT_AVAILABLE       2u

enum driver_setup_t {DRIVER_SETUP_OK, DRIVER_SETUP_INCOMPLETE};

/* Array of I2C addresses where we are trying to find the chip. */
//penghu extern UINT8 i2c_addresses[];

/*! \brief Returned by get_object_address() if object is not found. */
#define OBJECT_NOT_FOUND   0u

/*! Address where object table starts at touch IC memory map. */
#define OBJECT_TABLE_START_ADDRESS      7U

/*! Size of one object table element in touch IC memory map. */
#define OBJECT_TABLE_ELEMENT_SIZE       6U

/*! Offset to REPORTALL register from the beginning of command processor. */
#define REPORTATLL_OFFSET   3u

/* global variable*/
#define REPORT_ID_MAP_SIZE_MAX 100

/*----------------------------------------------------------------------------
Function prototypes.
----------------------------------------------------------------------------*/

/* Initializes the touch driver: tries to connect to given address,
* sets the message handler pointer, reads the info block and object
* table, sets the message processor address etc. */

extern UINT8 init_touch_driver(UINT8 I2C_address, void (*handler)(UINT8 *, UINT8));
extern UINT8 close_touch_driver(void);
extern UINT8 get_variant_id(UINT8 *variant);
extern UINT8 get_family_id(UINT8 *family_id);
extern UINT8 get_build_number(UINT8 *build);
extern UINT8 get_version(UINT8 *version);

extern UINT8 get_object_size(UINT8 object_type);
extern UINT8 type_to_report_id(UINT8 object_type, UINT8 instance);
extern UINT8 report_id_to_type(UINT8 report_id, UINT8 *instance);
extern UINT8 read_id_block(info_id_t *id);
extern UINT8 get_max_report_id(void);
extern UINT16 get_object_address(UINT8 object_type, UINT8 instance);
extern UINT32 get_stored_infoblock_crc(void);
//extern UINT8 calculate_infoblock_crc(UINT32 *crc_pointer);
extern UINT32 CRC_24(UINT32 crc, UINT8 byte1, UINT8 byte2);

extern info_block_t info_block;
//extern report_id_map_t report_id_map[REPORT_ID_MAP_SIZE_MAX];
//extern int max_report_id ;
//extern UINT8 max_message_length_T5;
extern UINT8 max_message_length_T44;
//extern UINT16 message_processor_address_T5;
extern UINT16 message_processor_address_T44;
extern UINT16 command_processor_address;
extern enum driver_setup_t driver_setup;
extern UINT8 msg[256];

/*! Touch device I2C Address */

extern UINT8 QT_i2c_address;
extern  UINT8 QT_i2c_boot_address;
// extern UINT8 chip_detected_flag;

//*****************************************************************************
//		main
//*****************************************************************************
extern void *malloc(unsigned int num_bytes);
extern void get_message(void);
extern void get_touch_message(void);
extern void get_touch_message_for_PSA1(void);
extern void get_touch_message_for_PSA(TOUCH_COORDINATE_BUF *data);

/*! Pointer to message handler provided by main app. */
extern void (*application_message_handler)(UINT8 *, UINT8);
extern void message_handler(UINT8 *msg, UINT8 length);

extern UINT8 init_touch_app(void);

extern UINT8 write_mem( UINT16 Address, UINT8 ByteCount, UINT8 *Data );
extern UINT8 read_mem( UINT16 Address, UINT8 ByteCount, UINT8 *Data );
extern UINT8 read_uint16_t( UINT16 Address, UINT16 *Data );

extern UINT32 ChangeLineStatus( void );

extern unsigned char tmsg[];
extern unsigned short x_pos, y_pos, s_t9sts;

//*****************************************************************************
//  I2C Functions
//*****************************************************************************
#if 0
extern void I2cStart(void);
extern void I2cStop(void);
extern UINT8 I2cTxByte( UINT8 TxData);
extern UINT8 I2cRxByte( UINT8 AckState );
#endif

extern unsigned char  TWI_Wait(void);
extern void Twi_SendStop(void);
extern void InitTwi(void);
extern UINT8 address_slave(void);

// use TWI (I2C) peripheral of mega32
#define TWI_START  0x08  //启动信号发送成功
#define TWI_REP_START  0x10  //重复启动信号发送成功
#define TWSR_STATUS_MASK  0xf8 //TWI状态寄存器
#define TWI_TWBR_200KHZ   72   //18,设置TWI的波特率200kbps，8MHz,TWPS=0,72=50hkz
//TWBR =(FOSC/SCLf -16)/2
#define TWI_ADR_BITS  1  //地址左移1位
#define TWI_READ_BIT  0  //在0位读出或者写入
#define TWI_MRX_ADR_ACK  0x40  //主机接收- SLA+R 应答
#define TWI_MTX_ADR_ACK  0x18  //主机发送- SLA+W 应答
#define TWI_MRX_DATA_ACK 0x50  //主机接收- data received 应答
#define TWI_MTX_DATA_ACK 0x28  //主机发送- data transmitted 应答
#define FOSC  8000000   //8MHz

extern UINT16 address_pointer;
extern unsigned char TwiWaitTimer;

#ifdef __cplusplus
}
#endif

#endif //__QT_Controller_H__
#endif /*CTP_MODULE*/
