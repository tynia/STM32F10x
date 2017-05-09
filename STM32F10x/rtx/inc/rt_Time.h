/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_TIME.H
 *      Purpose: Delay and interval wait functions definitions
 *      Rev.:    V4.20
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

/* Variables */
extern U16 os_time;

/* Functions */
extern void rt_dly_wait (U16 delay_time);
extern void rt_itv_set  (U16 interval_time);
extern void rt_itv_wait (void);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

