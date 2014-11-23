/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_TCP_H__
#define __LWIP_TCP_H__

#include "lwip/opt.h"

#if LWIP_TCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/ip.h"
#include "lwip/icmp.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tcp_pcb;

/** Function prototype for tcp accept callback functions. Called when a new
 * connection can be accepted on a listening pcb.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param newpcb The new connection pcb
 * @param err An error code if there has been an error accepting.
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
typedef err_t (*tcp_accept_fn)(void *arg, struct tcp_pcb *newpcb, err_t err);

/** Function prototype for tcp receive callback functions. Called when data has
 * been received.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb which received data
 * @param p The received data (or NULL when the connection has been closed!)
 * @param err An error code if there has been an error receiving
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
typedef err_t (*tcp_recv_fn)(void *arg, struct tcp_pcb *tpcb,
                             struct pbuf *p, err_t err);

/** Function prototype for tcp sent callback functions. Called when sent data has
 * been acknowledged by the remote side. Use it to free corresponding resources.
 * This also means that the pcb has now space available to send new data.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb for which data has been acknowledged
 * @param len The amount of bytes acknowledged
 * @return ERR_OK: try to send some data by calling tcp_output
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
typedef err_t (*tcp_sent_fn)(void *arg, struct tcp_pcb *tpcb,
                              uint16 len);

/** Function prototype for tcp poll callback functions. Called periodically as
 * specified by @see tcp_poll.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb tcp pcb
 * @return ERR_OK: try to send some data by calling tcp_output
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
typedef err_t (*tcp_poll_fn)(void *arg, struct tcp_pcb *tpcb);

/** Function prototype for tcp error callback functions. Called when the pcb
 * receives a RST or is unexpectedly closed for any other reason.
 *
 * @note The corresponding pcb is already freed when this callback is called!
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param err Error code to indicate why the pcb has been closed
 *            ERR_ABRT: aborted through tcp_abort or by a TCP timer
 *            ERR_RST: the connection was reset by the remote host
 */
typedef void  (*tcp_err_fn)(void *arg, err_t err);

/** Function prototype for tcp connected callback functions. Called when a pcb
 * is connected to the remote side after initiating a connection attempt by
 * calling tcp_connect().
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb which is connected
 * @param err An unused error code, always ERR_OK currently ;-) TODO!
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 *
 * @note When a connection attempt fails, the error callback is currently called!
 */
typedef err_t (*tcp_connected_fn)(void *arg, struct tcp_pcb *tpcb, err_t err);

enum tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};

#if LWIP_CALLBACK_API
  /* Function to call when a listener has been connected.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param pcb a new tcp_pcb that now is connected
   * @param err an error argument (TODO: that is current always ERR_OK?)
   * @return ERR_OK: accept the new connection,
   *                 any other err_t abortsthe new connection
   */
#define DEF_ACCEPT_CALLBACK  tcp_accept_fn accept;
#else /* LWIP_CALLBACK_API */
#define DEF_ACCEPT_CALLBACK
#endif /* LWIP_CALLBACK_API */

/**
 * members common to struct tcp_pcb and struct tcp_listen_pcb
 */
#define TCP_PCB_COMMON(type) \
  type *next; /* for the linked list */ \
  enum tcp_state state; /* TCP state */ \
  uint8_t prio; \
  void *callback_arg; \
  /* the accept callback for listen- and normal pcbs, if LWIP_CALLBACK_API */ \
  DEF_ACCEPT_CALLBACK \
  /* ports are in host byte order */ \
  uint16 local_port


/* the TCP protocol control block */
struct tcp_pcb {
/** common PCB members */
  IP_PCB;
/** protocol specific PCB members */
  TCP_PCB_COMMON(struct tcp_pcb);

  /* ports are in host byte order */
  uint16 remote_port;
  
  uint8_t flags;
#define TF_ACK_DELAY   ((uint8_t)0x01U)   /* Delayed ACK. */
#define TF_ACK_NOW     ((uint8_t)0x02U)   /* Immediate ACK. */
#define TF_INFR        ((uint8_t)0x04U)   /* In fast recovery. */
#define TF_TIMESTAMP   ((uint8_t)0x08U)   /* Timestamp option enabled */
#define TF_RXCLOSED    ((uint8_t)0x10U)   /* rx closed by tcp_shutdown */
#define TF_FIN         ((uint8_t)0x20U)   /* Connection was closed locally (FIN segment enqueued). */
#define TF_NODELAY     ((uint8_t)0x40U)   /* Disable Nagle algorithm */
#define TF_NAGLEMEMERR ((uint8_t)0x80U)   /* nagle enabled, memerr, try to output to prevent delayed ACK to happen */

  /* the rest of the fields are in host byte order
     as we have to do some math with them */
  /* receiver variables */
  uint32_t rcv_nxt;   /* next seqno expected */
  uint16 rcv_wnd;   /* receiver window available */
  uint16 rcv_ann_wnd; /* receiver window to announce */
  uint32_t rcv_ann_right_edge; /* announced right edge of window */

  /* Timers */
  uint32_t tmr;
  uint8_t polltmr, pollinterval;
  
  /* Retransmission timer. */
  int16_t rtime;
  
  uint16 mss;   /* maximum segment size */
  
  /* RTT (round trip time) estimation variables */
  uint32_t rttest; /* RTT estimate in 500ms ticks */
  uint32_t rtseq;  /* sequence number being timed */
  int16_t sa, sv; /* @todo document this */

  int16_t rto;    /* retransmission time-out */
  uint8_t nrtx;    /* number of retransmissions */

  /* fast retransmit/recovery */
  uint32_t lastack; /* Highest acknowledged seqno. */
  uint8_t dupacks;
  
  /* congestion avoidance/control variables */
  uint16 cwnd;  
  uint16 ssthresh;

  /* sender variables */
  uint32_t snd_nxt;   /* next new seqno to be sent */
  uint16 snd_wnd;   /* sender window */
  uint32_t snd_wl1, snd_wl2; /* Sequence and acknowledgement numbers of last
                             window update. */
  uint32_t snd_lbb;       /* Sequence number of next byte to be buffered. */

  uint16 acked;
  
  uint16 snd_buf;   /* Available buffer space for sending (in bytes). */
#define TCP_SNDQUEUELEN_OVERFLOW (0xffff-3)
  uint16 snd_queuelen; /* Available buffer space for sending (in tcp_segs). */

#if TCP_OVERSIZE
  /* Extra bytes available at the end of the last pbuf in unsent. */
  uint16 unsent_oversize;
#endif /* TCP_OVERSIZE */ 

  /* These are ordered by sequence number: */
  struct tcp_seg *unsent;   /* Unsent (queued) segments. */
  struct tcp_seg *unacked;  /* Sent but unacknowledged segments. */
#if TCP_QUEUE_OOSEQ  
  struct tcp_seg *ooseq;    /* Received out of sequence segments. */
#endif /* TCP_QUEUE_OOSEQ */

  struct pbuf *refused_data; /* Data previously received but not yet taken by upper layer */

#if LWIP_CALLBACK_API
  /* Function to be called when more send buffer space is available. */
  tcp_sent_fn sent;
  /* Function to be called when (in-sequence) data has arrived. */
  tcp_recv_fn recv;
  /* Function to be called when a connection has been set up. */
  tcp_connected_fn connected;
  /* Function which is called periodically. */
  tcp_poll_fn poll;
  /* Function to be called whenever a fatal error occurs. */
  tcp_err_fn errf;
#endif /* LWIP_CALLBACK_API */

#if LWIP_TCP_TIMESTAMPS
  uint32_t ts_lastacksent;
  uint32_t ts_recent;
#endif /* LWIP_TCP_TIMESTAMPS */

  /* idle time before KEEPALIVE is sent */
  uint32_t keep_idle;
#if LWIP_TCP_KEEPALIVE
  uint32_t keep_intvl;
  uint32_t keep_cnt;
#endif /* LWIP_TCP_KEEPALIVE */
  
  /* Persist timer counter */
  uint32_t persist_cnt;
  /* Persist timer back-off */
  uint8_t persist_backoff;

  /* KEEPALIVE counter */
  uint8_t keep_cnt_sent;
};

struct tcp_pcb_listen {  
/* Common members of all PCB types */
  IP_PCB;
/* Protocol specific PCB members */
  TCP_PCB_COMMON(struct tcp_pcb_listen);

#if TCP_LISTEN_BACKLOG
  uint8_t backlog;
  uint8_t accepts_pending;
#endif /* TCP_LISTEN_BACKLOG */
};

#if LWIP_EVENT_API

enum lwip_event {
  LWIP_EVENT_ACCEPT,
  LWIP_EVENT_SENT,
  LWIP_EVENT_RECV,
  LWIP_EVENT_CONNECTED,
  LWIP_EVENT_POLL,
  LWIP_EVENT_ERR
};

err_t lwip_tcp_event(void *arg, struct tcp_pcb *pcb,
         enum lwip_event,
         struct pbuf *p,
         uint16 size,
         err_t err);

#endif /* LWIP_EVENT_API */

/* Application program's interface: */
struct tcp_pcb * tcp_new     (void)ICACHE_FLASH_ATTR;

void             tcp_arg     (struct tcp_pcb *pcb, void *arg) ICACHE_FLASH_ATTR;
void             tcp_accept  (struct tcp_pcb *pcb, tcp_accept_fn accept) ICACHE_FLASH_ATTR;
void             tcp_recv    (struct tcp_pcb *pcb, tcp_recv_fn recv) ICACHE_FLASH_ATTR;
void             tcp_sent    (struct tcp_pcb *pcb, tcp_sent_fn sent)ICACHE_FLASH_ATTR;
void             tcp_poll    (struct tcp_pcb *pcb, tcp_poll_fn poll, uint8_t interval)ICACHE_FLASH_ATTR;
void             tcp_err     (struct tcp_pcb *pcb, tcp_err_fn err)ICACHE_FLASH_ATTR;

#define          tcp_mss(pcb)             (((pcb)->flags & TF_TIMESTAMP) ? ((pcb)->mss - 12)  : (pcb)->mss)
#define          tcp_sndbuf(pcb)          ((pcb)->snd_buf)
#define          tcp_sndqueuelen(pcb)     ((pcb)->snd_queuelen)
#define          tcp_nagle_disable(pcb)   ((pcb)->flags |= TF_NODELAY)
#define          tcp_nagle_enable(pcb)    ((pcb)->flags &= ~TF_NODELAY)
#define          tcp_nagle_disabled(pcb)  (((pcb)->flags & TF_NODELAY) != 0)

#if TCP_LISTEN_BACKLOG
#define          tcp_accepted(pcb) do { \
  LWIP_ASSERT("pcb->state == LISTEN (called for wrong pcb?)", pcb->state == LISTEN); \
  (((struct tcp_pcb_listen *)(pcb))->accepts_pending--); } while(0)
#else  /* TCP_LISTEN_BACKLOG */
#define          tcp_accepted(pcb) LWIP_ASSERT("pcb->state == LISTEN (called for wrong pcb?)", \
                                               pcb->state == LISTEN)
#endif /* TCP_LISTEN_BACKLOG */

void             tcp_recved  (struct tcp_pcb *pcb, uint16 len)ICACHE_FLASH_ATTR;
err_t            tcp_bind    (struct tcp_pcb *pcb, ip_addr_t *ipaddr,
                              uint16 port)ICACHE_FLASH_ATTR;
err_t            tcp_connect (struct tcp_pcb *pcb, ip_addr_t *ipaddr,
                              uint16 port, tcp_connected_fn connected)ICACHE_FLASH_ATTR;

struct tcp_pcb * tcp_listen_with_backlog(struct tcp_pcb *pcb, uint8_t backlog)ICACHE_FLASH_ATTR;
#define          tcp_listen(pcb) tcp_listen_with_backlog(pcb, TCP_DEFAULT_LISTEN_BACKLOG)

void             tcp_abort (struct tcp_pcb *pcb)ICACHE_FLASH_ATTR;
err_t            tcp_close   (struct tcp_pcb *pcb)ICACHE_FLASH_ATTR;
err_t            tcp_shutdown(struct tcp_pcb *pcb, int shut_rx, int shut_tx)ICACHE_FLASH_ATTR;

/* Flags for "apiflags" parameter in tcp_write */
#define TCP_WRITE_FLAG_COPY 0x01
#define TCP_WRITE_FLAG_MORE 0x02

err_t            tcp_write   (struct tcp_pcb *pcb, const void *dataptr, uint16 len,
                              uint8_t apiflags)ICACHE_FLASH_ATTR;

void             tcp_setprio (struct tcp_pcb *pcb, uint8_t prio)ICACHE_FLASH_ATTR;

#define TCP_PRIO_MIN    1
#define TCP_PRIO_NORMAL 64
#define TCP_PRIO_MAX    127

extern err_t            tcp_output(struct tcp_pcb *pcb);


const char* tcp_debug_state_str(enum tcp_state s)ICACHE_FLASH_ATTR;


#ifdef __cplusplus
}
#endif

#endif /* LWIP_TCP */

#endif /* __LWIP_TCP_H__ */
