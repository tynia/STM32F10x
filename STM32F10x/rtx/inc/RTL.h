/*----------------------------------------------------------------------------
 *      RL-ARM - A P I 
 *----------------------------------------------------------------------------
 *      Name:    RTL.H 
 *      Purpose: Application Programming Interface. 
 *      Rev.:    V4.23
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __RTL_H__
#define __RTL_H__

/* RL-ARM version number. */
#define __RL_ARM_VER    423
 
#define __task          __declspec(noreturn)
#define __used          __attribute__((used))

#ifndef NULL
 #ifdef __cplusplus              // EC++
  #define NULL          0
 #else
  #define NULL          ((void *) 0)
 #endif
#endif

#ifndef EOF
 #define EOF            -1
#endif

#ifndef __size_t
 #define __size_t       1
 typedef unsigned int   size_t;
#endif

typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;

#ifndef __TRUE
 #define __TRUE         1
#endif
#ifndef __FALSE
 #define __FALSE        0
#endif

#ifdef __BIG_ENDIAN
 #define U32_LE(v)      (U32)(__rev(v))
 #define U16_LE(v)      (U16)(__rev(v) >> 16)
 #define U32_BE(v)      (U32)(v)
 #define U16_BE(v)      (U16)(v)
#else
 #define U32_BE(v)      (U32)(__rev(v))
 #define U16_BE(v)      (U16)(__rev(v) >> 16)
 #define U32_LE(v)      (U32)(v)
 #define U16_LE(v)      (U16)(v)
#endif
#define ntohs(v)        U16_BE(v)
#define ntohl(v)        U32_BE(v)
#define htons(v)        ntohs(v)
#define htonl(v)        ntohl(v)

/*----------------------------------------------------------------------------
 *                             RTX Kernel API
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus               // EC++
extern "C"  {
#endif

/* Definition of Semaphore type */
typedef U32 OS_SEM[2];

/* Definition of Mailbox type */
#define os_mbx_declare(name,cnt)    U32 name [4 + cnt]
typedef U32 OS_MBX[];

/* Definition of Mutex type */
typedef U32 OS_MUT[3];

/* Task Identification number. */
typedef U32 OS_TID;

/* Function return of system calls returning an object identification */
typedef void *OS_ID;

/* Function return of system calls indicating an event or completion state */
typedef U32 OS_RESULT;

/* Return codes */
#define OS_R_TMO        0x01
#define OS_R_EVT        0x02
#define OS_R_SEM        0x03
#define OS_R_MBX        0x04
#define OS_R_MUT        0x05

#define OS_R_OK         0x00
#define OS_R_NOK        0xff

#define OS_TCB_SIZE     48
#define OS_TMR_SIZE     8

/* Error Codes */
#define OS_ERR_STK_OVF  1
#define OS_ERR_FIFO_OVF 2
#define OS_ERR_MBX_OVF  3

#if !(__TARGET_ARCH_6S_M || __TARGET_ARCH_7_M || __TARGET_ARCH_7E_M)

/*----------------------------------------------------------------------------
 *      Functions ARM
 *---------------------------------------------------------------------------*/

/* Task Management */
#define os_sys_init(tsk)              os_sys_init0(tsk,0,NULL)
#define os_sys_init_prio(tsk,prio)    os_sys_init0(tsk,prio,NULL)
#define os_sys_init_user(tsk,prio,stk,size)                                   \
                                      os_sys_init0(tsk,prio|(size<<8),stk)
#define os_tsk_create(tsk,prio)       os_tsk_create0(tsk,prio,NULL,NULL)
#define os_tsk_create_user(tsk,prio,stk,size)                                 \
                                      os_tsk_create0(tsk,prio|(size<<8),stk,NULL)
#define os_tsk_create_ex(tsk,prio,argv) os_tsk_create_ex0(tsk,prio,NULL,argv)
#define os_tsk_create_user_ex(tsk,prio,stk,size,argv)                         \
                                      os_tsk_create_ex0(tsk,prio|(size<<8),stk,argv)
#define os_tsk_delete_self()          os_tsk_delete(0)
#define os_tsk_prio_self(prio)        os_tsk_prio(0,prio)
#define isr_tsk_get()                 os_tsk_self()

extern void      os_sys_init0 (void (*task)(void), U32 prio_stksz, void *stk);
extern OS_TID    os_tsk_create0 (void (*task)(void), U32 prio_stksz, 
                                 void *stk, void *argv);
extern OS_TID    os_tsk_create_ex0 (void (*task)(void *), U32 prio_stksz, 
                                    void *stk, void *argv);
extern OS_TID    os_tsk_self (void);
extern void      os_tsk_pass (void);
extern OS_RESULT os_tsk_prio (OS_TID task_id, U8 new_prio);
extern OS_RESULT os_tsk_delete (OS_TID task_id);

/* Event flag Management */
#define os_evt_wait_or(wflags,tmo)      os_evt_wait(wflags,tmo,__FALSE)
#define os_evt_wait_and(wflags,tmo)     os_evt_wait(wflags,tmo,__TRUE)

extern OS_RESULT os_evt_wait (U16 wait_flags, U16 timeout, BOOL and_wait);
extern void      os_evt_set (U16 event_flags, OS_TID task_id);
extern void      os_evt_clr (U16 clear_flags, OS_TID task_id);
extern void      isr_evt_set (U16 event_flags, OS_TID task_id);
extern U16       os_evt_get (void);

/* Semaphore Management */
extern void      os_sem_init (OS_ID semaphore, U16 token_count);
extern OS_RESULT os_sem_send (OS_ID semaphore);
extern OS_RESULT os_sem_wait (OS_ID semaphore, U16 timeout);
extern void      isr_sem_send (OS_ID semaphore);

/* Mailbox Management */
#define isr_mbx_check(mbx)              os_mbx_check(mbx)

extern void      os_mbx_init (OS_ID mailbox, U16 mbx_size);
extern OS_RESULT os_mbx_send (OS_ID mailbox, void *message_ptr, U16 timeout);
extern OS_RESULT os_mbx_wait (OS_ID mailbox, void  **message, U16 timeout);
extern OS_RESULT os_mbx_check (OS_ID mailbox);
extern void      isr_mbx_send (OS_ID mailbox, void *message_ptr);
extern OS_RESULT isr_mbx_receive (OS_ID mailbox, void **message);

/* Mutex Management */
extern void      os_mut_init (OS_ID mutex);
extern OS_RESULT os_mut_release (OS_ID mutex);
extern OS_RESULT os_mut_wait (OS_ID mutex, U16 timeout);

/* Time Management */
extern void      os_dly_wait (U16 delay_time);
extern void      os_itv_set (U16 interval_time);
extern void      os_itv_wait (void);

/* User Timer Management */
extern OS_ID     os_tmr_create (U16 tcnt, U16 info);
extern OS_ID     os_tmr_kill (OS_ID timer);

/* System Functions */
extern void      tsk_lock (void) __swi (5);
extern void      tsk_unlock (void);

/* Fixed Memory Block Management Functions */
extern int       _init_box (void *box_mem, U32 box_size, U32 blk_size);
extern void     *_alloc_box (void *box_mem) __swi (1);
extern void     *_calloc_box (void *box_mem);
extern int       _free_box (void *box_mem, void *box) __swi (2);

#else

/*----------------------------------------------------------------------------
 *      Functions Cortex-M
 *---------------------------------------------------------------------------*/

#define __SVC_0         __svc_indirect(0)

/* Task Management */
extern void      os_set_env    (void);
extern void      rt_sys_init   (void (*task)(void), U8 priority, void *stk);
extern void      rt_tsk_pass   (void);
extern OS_TID    rt_tsk_self   (void);
extern OS_RESULT rt_tsk_prio   (OS_TID task_id, U8 new_prio);
extern OS_TID    rt_tsk_create (void (*task)(void), U8 priority, void *stk, void *argv);
extern OS_RESULT rt_tsk_delete (OS_TID task_id);

#define os_sys_init(tsk)              os_set_env();                           \
                                      _os_sys_init((U32)rt_sys_init,tsk,0,NULL)
#define os_sys_init_user(tsk,prio,stk,size)                                   \
                                      os_set_env();                           \
                                      _os_sys_init((U32)rt_sys_init,tsk,prio|(size<<8),stk)
#define os_sys_init_prio(tsk,prio)    os_set_env();                           \
                                      _os_sys_init((U32)rt_sys_init,tsk,prio,NULL)
#define os_tsk_create(tsk,prio)       _os_tsk_create((U32)rt_tsk_create,tsk,prio,NULL,NULL)
#define os_tsk_create_user(tsk,prio,stk,size)                                 \
                                      _os_tsk_create((U32)rt_tsk_create,tsk,prio|(size<<8),stk,NULL)
#define os_tsk_create_ex(tsk,prio,argv) _os_tsk_create_ex((U32)rt_tsk_create,tsk,prio,NULL,argv)
#define os_tsk_create_user_ex(tsk,prio,stk,size,argv)                         \
                                      _os_tsk_create_ex((U32)rt_tsk_create,tsk,prio|(size<<8),stk,argv)
#define os_tsk_self()                 _os_tsk_self((U32)rt_tsk_self)
#define os_tsk_pass()                 _os_tsk_pass((U32)rt_tsk_pass)
#define os_tsk_prio(task_id,new_prio) _os_tsk_prio((U32)rt_tsk_prio,task_id,new_prio)
#define os_tsk_prio_self(prio)        _os_tsk_prio((U32)rt_tsk_prio,0,prio)
#define os_tsk_delete(task_id)        _os_tsk_delete((U32)rt_tsk_delete,task_id)
#define os_tsk_delete_self()          _os_tsk_delete((U32)rt_tsk_delete, 0)
#define isr_tsk_get()                 rt_tsk_self()

extern void      _os_sys_init(U32 p, void (*task)(void), U32 prio_stksz,
                                     void *stk)                        __SVC_0;
extern OS_TID    _os_tsk_create (U32 p, void (*task)(void), U32 prio_stksz,
                                        void *stk, void *argv)         __SVC_0;
extern OS_TID    _os_tsk_create_ex (U32 p, void (*task)(void *), U32 prio_stksz,
                                           void *stk, void *argv)      __SVC_0;
extern OS_TID    _os_tsk_self (U32 p)                                  __SVC_0;
extern void      _os_tsk_pass (U32 p)                                  __SVC_0;
extern OS_RESULT _os_tsk_prio (U32 p, OS_TID task_id, U8 new_prio)     __SVC_0;
extern OS_RESULT _os_tsk_delete (U32 p, OS_TID task_id)                __SVC_0;

/* Event flag Management */
extern OS_RESULT rt_evt_wait (U16 wait_flags,  U16 timeout, BOOL and_wait);
extern void      rt_evt_set  (U16 event_flags, OS_TID task_id);
extern void      rt_evt_clr  (U16 clear_flags, OS_TID task_id);
extern U16       rt_evt_get  (void);

#define os_evt_wait_or(wflags,tmo)    _os_evt_wait((U32)rt_evt_wait,wflags,tmo,__FALSE)
#define os_evt_wait_and(wflags,tmo)   _os_evt_wait((U32)rt_evt_wait,wflags,tmo,__TRUE)
#define os_evt_set(evt_flags,task_id) _os_evt_set((U32)rt_evt_set,evt_flags,task_id)
#define os_evt_clr(clr_flags,task_id) _os_evt_clr((U32)rt_evt_clr,clr_flags,task_id)
#define os_evt_get()                  _os_evt_get((U32)rt_evt_get)

extern OS_RESULT _os_evt_wait(U32 p, U16 wait_flags, U16 timeout,
                                     BOOL and_wait)                    __SVC_0;
extern void      _os_evt_set (U32 p, U16 event_flags, OS_TID task_id)  __SVC_0;
extern void      _os_evt_clr (U32 p, U16 clear_flags, OS_TID task_id)  __SVC_0;
extern U16       _os_evt_get (U32 p)                                   __SVC_0;
extern void      isr_evt_set (U16 event_flags, OS_TID task_id);

/* Semaphore Management */
extern void      rt_sem_init (OS_ID semaphore, U16 token_count);
extern OS_RESULT rt_sem_send (OS_ID semaphore);
extern OS_RESULT rt_sem_wait (OS_ID semaphore, U16 timeout);

#define os_sem_init(sem,t_count)     _os_sem_init((U32)rt_sem_init,sem,t_count)
#define os_sem_send(sem)             _os_sem_send((U32)rt_sem_send,sem)
#define os_sem_wait(sem,tmo)         _os_sem_wait((U32)rt_sem_wait,sem,tmo)

extern void      _os_sem_init (U32 p, OS_ID semaphore, 
                                      U16 token_count)                 __SVC_0;
extern OS_RESULT _os_sem_send (U32 p, OS_ID semaphore)                 __SVC_0;
extern OS_RESULT _os_sem_wait (U32 p, OS_ID semaphore, U16 timeout)    __SVC_0;
extern void      isr_sem_send (OS_ID semaphore);

/* Mailbox Management */
extern void      rt_mbx_init  (OS_ID mailbox, U16 mbx_size);
extern OS_RESULT rt_mbx_send  (OS_ID mailbox, void *p_msg,    U16 timeout);
extern OS_RESULT rt_mbx_wait  (OS_ID mailbox, void **message, U16 timeout);
extern OS_RESULT rt_mbx_check (OS_ID mailbox);

#define os_mbx_init(mbx,mbx_size)     _os_mbx_init((U32)rt_mbx_init,mbx,mbx_size)
#define os_mbx_send(mbx,p_msg,tmo)    _os_mbx_send((U32)rt_mbx_send,mbx,p_msg,tmo)
#define os_mbx_wait(mbx,message,tmo)  _os_mbx_wait((U32)rt_mbx_wait,mbx,message,tmo)
#define os_mbx_check(mbx)             _os_mbx_check((U32)rt_mbx_check,mbx)
#define isr_mbx_check(mbx)            rt_mbx_check(mbx)

extern void      _os_mbx_init (U32 p, OS_ID mailbox, U16 mbx_size)     __SVC_0;
extern OS_RESULT _os_mbx_send (U32 p, OS_ID mailbox, void *message_ptr,
                                      U16 timeout)                     __SVC_0;
extern OS_RESULT _os_mbx_wait (U32 p, OS_ID mailbox, void  **message,
                                      U16 timeout)                     __SVC_0;
extern OS_RESULT _os_mbx_check (U32 p, OS_ID mailbox)                  __SVC_0;
extern void      isr_mbx_send (OS_ID mailbox, void *message_ptr);
extern OS_RESULT isr_mbx_receive (OS_ID mailbox, void **message);

/* Mutex Management */
extern void      rt_mut_init    (OS_ID mutex);
extern OS_RESULT rt_mut_release (OS_ID mutex);
extern OS_RESULT rt_mut_wait    (OS_ID mutex, U16 timeout);

#define os_mut_init(mutex)         _os_mut_init((U32)rt_mut_init,mutex)
#define os_mut_release(mutex)      _os_mut_release((U32)rt_mut_release,mutex)
#define os_mut_wait(mutex,timeout) _os_mut_wait((U32)rt_mut_wait,mutex,timeout)

extern void      _os_mut_init (U32 p, OS_ID mutex)                     __SVC_0;
extern OS_RESULT _os_mut_release (U32 p, OS_ID mutex)                  __SVC_0;
extern OS_RESULT _os_mut_wait (U32 p, OS_ID mutex, U16 timeout)        __SVC_0;

/* Time Management */
extern void      rt_dly_wait (U16 delay_time);
extern void      rt_itv_set  (U16 interval_time);
extern void      rt_itv_wait (void);

#define os_dly_wait(delay_time)    _os_dly_wait((U32)rt_dly_wait,delay_time)
#define os_itv_set(interval_time)  _os_itv_set((U32)rt_itv_set,interval_time)
#define os_itv_wait()              _os_itv_wait((U32)rt_itv_wait)

extern void      _os_dly_wait (U32 p, U16 delay_time)                  __SVC_0;
extern void      _os_itv_set (U32 p, U16 interval_time)                __SVC_0;
extern void      _os_itv_wait (U32 p)                                  __SVC_0;

/* User Timer Management */
extern OS_ID     rt_tmr_create (U16 tcnt, U16 info);
extern OS_ID     rt_tmr_kill   (OS_ID timer);

#define os_tmr_create(tcnt,info)   _os_tmr_create((U32)rt_tmr_create,tcnt,info)
#define os_tmr_kill(timer)         _os_tmr_kill((U32)rt_tmr_kill,timer)

extern OS_ID     _os_tmr_create (U32 p, U16 tcnt, U16 info)            __SVC_0;
extern OS_ID     _os_tmr_kill (U32 p, OS_ID timer)                     __SVC_0;

/* System Functions */
extern void      rt_tsk_lock   (void);
extern void      rt_tsk_unlock (void);

#define tsk_lock()                 _os_tsk_lock((U32)rt_tsk_lock)
#define tsk_unlock()               _os_tsk_unlock((U32)rt_tsk_unlock)

extern void      _os_tsk_lock (U32 p)                                  __SVC_0;
extern void      _os_tsk_unlock (U32 p)                                __SVC_0;

/* Fixed Memory Block Management Functions */
extern int       _init_box (void *box_mem, U32 box_size, U32 blk_size);
extern void     *_alloc_box (void *box_mem);
extern void     *_calloc_box (void *box_mem);
extern int       _free_box (void *box_mem, void *box);

#endif

#define BOX_ALIGN_8     0x80000000
#define _declare_box(pool,size,cnt)   U32 pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt)  U64 pool[(((size)+7)/8)*(cnt) + 2]
#define _init_box8(pool,size,bsize)   _init_box (pool,size,bsize | BOX_ALIGN_8)

/* For compatibility with older configurations.*/
#define os_stk_overflow         os_error

/*----------------------------------------------------------------------------
 *                        Flash File System API
 *---------------------------------------------------------------------------*/

typedef struct {                        /* RL Time format (FFS, TCPnet)      */
  U8  hr;                               /* Hours    [0..23]                  */
  U8  min;                              /* Minutes  [0..59]                  */
  U8  sec;                              /* Seconds  [0..59]                  */
  U8  day;                              /* Day      [1..31]                  */
  U8  mon;                              /* Month    [1..12]                  */
  U16 year;                             /* Year     [1980..2107]             */
} RL_TIME;

typedef struct {                        /* Search info record                */
  S8  name[256];                        /* Name                              */
  U32 size;                             /* File size in bytes                */
  U16 fileID;                           /* System Identification             */
  U8  attrib;                           /* Attributes                        */
  RL_TIME time;                         /* Create/Modify Time                */
} FINFO;

extern int finit (const char *drive);
extern int funinit (const char *drive);
extern int fdelete (const char *filename);
extern int frename (const char *oldname, const char *newname);
extern int ffind (const char *pattern, FINFO *info);
extern U64 ffree (const char *drive);
extern int fformat (const char *drive);
extern int fanalyse (const char *drive);
extern int fcheck (const char *drive);
extern int fdefrag (const char *drive);

/* The following macros provide for common functions */
#define unlink(fn)      fdelete(fn);

/*----------------------------------------------------------------------------
 *                             TCPnet API
 *---------------------------------------------------------------------------*/

/* UDP Options */
#define UDP_OPT_SEND_CS    0x01   /* Calculate Checksum for UDP send frames  */
#define UDP_OPT_CHK_CS     0x02   /* Verify Checksum for received UDP frames */

/* TCP Socket Types */
#define TCP_TYPE_SERVER    0x01   /* Socket Type Server (open for listening) */
#define TCP_TYPE_CLIENT    0x02   /* Socket Type Client (initiate connect)   */
#define TCP_TYPE_DELAY_ACK 0x04   /* Socket Type Delayed Acknowledge         */
#define TCP_TYPE_FLOW_CTRL 0x08   /* Socket Type Flow Control                */
#define TCP_TYPE_KEEP_ALIVE 0x10  /* Socket Type Keep Alive                  */
#define TCP_TYPE_CLIENT_SERVER (TCP_TYPE_SERVER | TCP_TYPE_CLIENT)

/* TCP Callback Events */
#define TCP_EVT_CONREQ     0      /* Connect request received event          */
#define TCP_EVT_CONNECT    1      /* Connection established event            */
#define TCP_EVT_CLOSE      2      /* Connection was properly closed          */
#define TCP_EVT_ABORT      3      /* Connection is for some reason aborted   */
#define TCP_EVT_ACK        4      /* Previously send data acknowledged       */
#define TCP_EVT_DATA       5      /* Data received event                     */

/* TCP States */
#define TCP_STATE_FREE     0      /* Entry is free and unused                */
#define TCP_STATE_CLOSED   1      /* Entry allocated, socket still closed    */
#define TCP_STATE_LISTEN   2      /* Socket waiting for incoming connection  */
#define TCP_STATE_SYN_REC  3      /* SYN frame received                      */
#define TCP_STATE_SYN_SENT 4      /* SYN packet sent to establish a connect. */
#define TCP_STATE_FINW1    5      /* Tcp_close started FIN packet was sent   */
#define TCP_STATE_FINW2    6      /* Our FIN ack-ed, waiting for remote FIN  */
#define TCP_STATE_CLOSING  7      /* Received FIN independently of our FIN   */
#define TCP_STATE_LAST_ACK 8      /* Waiting for last ACK for our FIN        */
#define TCP_STATE_TWAIT    9      /* Timed waiting for 2MSL                  */
#define TCP_STATE_CONNECT  10     /* TCP Connection established              */

/* BSD Socket Address Family */
#define AF_UNSPEC          0      /* Unspecified                             */
#define AF_INET            1      /* Internet Address Family (UDP, TCP)      */
#define AF_NETBIOS         2      /* NetBios-style addresses                 */

/* BSD Protocol families, same as address families */
#define PF_UNSPEC          AF_UNSPEC
#define PF_INET            AF_INET
#define PF_NETBIOS         AF_NETBIOS

/* BSD Socket Type */
#define SOCK_STREAM        1      /* Stream Socket (Connection oriented)     */
#define SOCK_DGRAM         2      /* Datagram Socket (Connectionless)        */

/* BSD Socket Protocol */
#define IPPROTO_TCP        1      /* TCP Protocol                            */
#define IPPROTO_UDP        2      /* UDP Protocol                            */

/* BSD Internet Addresses */
#define INADDR_ANY     0x00000000 /* All IP addresses accepted               */
#define INADDR_NONE    0xffffffff /* No IP address accepted                  */

/* BSD Socket Return values */
#define SCK_SUCCESS         0     /* Success                                 */
#define SCK_ERROR         (-1)    /* General Error                           */
#define SCK_EINVALID      (-2)    /* Invalid socket descriptor               */
#define SCK_EINVALIDPARA  (-3)    /* Invalid parameter                       */
#define SCK_EWOULDBLOCK   (-4)    /* It would have blocked.                  */
#define SCK_EMEMNOTAVAIL  (-5)    /* Not enough memory in memory pool        */
#define SCK_ECLOSED       (-6)    /* Connection is closed or aborted         */
#define SCK_ELOCKED       (-7)    /* Socket is locked in RTX environment     */
#define SCK_ETIMEOUT      (-8)    /* Socket, Host Resolver timeout           */
#define SCK_EINPROGRESS   (-9)    /* Host Name resolving in progress         */
#define SCK_ENONAME       (-10)   /* Host Name not existing                  */

/* BSD Socket flags parameter */
#define MSG_DONTWAIT       0x01   /* Enables non-blocking operation          */
#define MSG_PEEK           0x02   /* Peeks at the incoming data              */

/* BSD Socket ioctl commands */
#define FIONBIO            0x01   /* Set mode (blocking/non-blocking)        */

/* ICMP (ping) Callback Events */
#define ICMP_EVT_SUCCESS   0      /* Pinged Host responded                   */
#define ICMP_EVT_TIMEOUT   1      /* Timeout, no ping response received      */

/* DNS Callback Events */
#define DNS_EVT_SUCCESS    0      /* Host name successfully resolved         */
#define DNS_EVT_NONAME     1      /* DNS Error, no such name                 */
#define DNS_EVT_TIMEOUT    2      /* Timeout resolving host                  */
#define DNS_EVT_ERROR      3      /* Erroneous response packet               */

/* DNS 'get_host_by_name()' result codes */
#define DNS_RES_OK         0      /* Function finished OK                    */
#define DNS_ERROR_BUSY     1      /* DNS Client busy, can't process request  */
#define DNS_ERROR_LABEL    2      /* Host name Label too long                */
#define DNS_ERROR_NAME     3      /* Host name loo long                      */
#define DNS_ERROR_NOSRV    4      /* Prim. DNS server not specified (0.0.0.0)*/
#define DNS_ERROR_UDPSEND  5      /* UDP Send frame error                    */

/* SMTP Callback Events */
#define SMTP_EVT_SUCCESS   0      /* Email successfully sent                 */
#define SMTP_EVT_TIMEOUT   1      /* Timeout sending email                   */
#define SMTP_EVT_ERROR     2      /* Error when sending email                */

/* ARP Cache Entry types */
#define ARP_FIXED_IP       0      /* Fixed IP adrs is refreshed after tout   */
#define ARP_TEMP_IP        1      /* Temp adrs is removed after timeout      */

/* BSD Socket typedef's */
typedef struct sockaddr {         /* << Generic Socket Address structure >>  */
  U16  sa_family;                 /* Address family                          */
  char sa_data[14];               /* Direct address (up to 14 bytes)         */
} SOCKADDR;

#pragma push
#pragma anon_unions

typedef struct in_addr {          /* << Generic IPv4 Address structure >>    */
  union {
    struct {
      U8 s_b1,s_b2,s_b3,s_b4;     /* IP address, byte access                 */
    };
    struct {
      U16 s_w1,s_w2;              /* IP address, short int access            */
    };
    U32 s_addr;                   /* IP address in network byte order        */
  };
} IN_ADDR;
#pragma pop

typedef struct sockaddr_in {      /* << IPv4 Socket Address structure >>     */
  S16 sin_family;                 /* Socket domain                           */
  U16 sin_port;                   /* Port                                    */
  IN_ADDR sin_addr;               /* IP address                              */
  S8  sin_zero[8];                /* reserved                                */
} SOCKADDR_IN;

typedef struct hostent {          /* << BSD Host Entry structure >>          */
  char *h_name;                   /* Official name of host                   */
  char **h_aliases;               /* Pointer to an array of alias names      */
  S16  h_addrtype;                /* Address Type: AF_INET, AF_NETBIOS       */
  S16  h_length;                  /* Length of address in bytes              */
  char **h_addr_list;             /* Pointer to an array of IPv4 addresses   */
} HOSTENT;

extern void init_TcpNet (void);
extern void main_TcpNet (void);
extern void timer_tick (void);
extern U8   udp_get_socket (U8 tos, U8 opt, 
                            U16 (*listener)(U8 socket, U8 *remip, U16 port, U8 *buf, U16 len));
extern BOOL udp_release_socket (U8 socket);
extern BOOL udp_open (U8 socket, U16 locport);
extern BOOL udp_close (U8 socket);
extern BOOL udp_mcast_ttl (U8 socket, U8 ttl);
extern U8  *udp_get_buf (U16 size);
extern BOOL udp_send (U8 socket, U8 *remip, U16 remport, U8 *buf, U16 dlen);
extern U8   tcp_get_socket (U8 type, U8 tos, U16 tout,
                            U16 (*listener)(U8 socket, U8 event, U8 *buf, U16 len));
extern BOOL tcp_release_socket (U8 socket);
extern BOOL tcp_listen (U8 socket, U16 locport);
extern BOOL tcp_connect (U8 socket, U8 *remip, U16 remport, U16 locport);
extern U8  *tcp_get_buf (U16 size);
extern U16  tcp_max_dsize (U8 socket);
extern BOOL tcp_check_send (U8 socket);
extern U8   tcp_get_state (U8 socket);
extern BOOL tcp_send (U8 socket, U8 *buf, U16 dlen);
extern BOOL tcp_close (U8 socket);
extern BOOL tcp_abort (U8 socket);
extern void tcp_reset_window (U8 socket);
extern BOOL arp_cache_ip (U8 *ipadr, U8 type);
extern void ppp_listen (char const *user, char const *passw);
extern void ppp_connect (char const *dialnum, char const *user, char const *passw);
extern void ppp_close (void);
extern BOOL ppp_is_up (void);
extern void slip_listen (void);
extern void slip_connect (char const *dialnum);
extern void slip_close (void);
extern BOOL slip_is_up (void);
extern U8   get_host_by_name (U8 *hostn, void (*cbfunc)(U8 event, U8 *host_ip));
extern BOOL smtp_connect (U8 *ipadr, U16 port, void (*cbfunc)(U8 event));
extern void dhcp_disable (void);
extern BOOL igmp_join (U8 *group_ip);
extern BOOL igmp_leave (U8 *group_ip);
extern BOOL snmp_trap (U8 *manager_ip, U8 gen_trap, U8 spec_trap, U16 *obj_list);
extern BOOL snmp_set_community (const char *community);
extern BOOL icmp_ping (U8 *remip, void (*cbfunc)(U8 event));

/* BSD Socket API */
extern int  socket (int family, int type, int protocol);
extern int  bind (int sock, const SOCKADDR *addr, int addrlen);
extern int  listen (int sock, int backlog);
extern int  accept (int sock, SOCKADDR *addr, int *addrlen);
extern int  connect (int sock, SOCKADDR *addr, int addrlen);
extern int  send (int sock, const char *buf, int len, int flags);
extern int  sendto (int sock, const char *buf, int len, int flags, SOCKADDR *to, int tolen);
extern int  recv (int sock, char *buf, int len, int flags);
extern int  recvfrom (int sock, char *buf, int len, int flags, SOCKADDR *from, int *fromlen);
extern int  closesocket (int sock);
extern int  getpeername (int sock, SOCKADDR *name, int *namelen);
extern int  getsockname (int sock, SOCKADDR *name, int *namelen);
extern int  ioctlsocket (int sock, long cmd, unsigned long *argp);
extern HOSTENT *gethostbyname (char *name, int *err);

#ifdef __cplusplus               // EC++
}
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

#endif
 
