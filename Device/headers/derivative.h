#ifndef _DERIVATIVE_H_
#define _DERIVATIVE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Note: This file is recreated by the project wizard whenever the MCU is
 *       changed and should not be edited by hand
 */

/* Include the derivative-specific header file */
#include <SKEAZ1284.h>
#include "typedefs.h"

#define FALSE 0
#define TRUE 1

#define DISABLEINTERRUPTS __asm(" CPSID i");
#define ENABLEINTERRUPTS __asm(" CPSIE i");

#ifdef __cplusplus
}
#endif

#endif
