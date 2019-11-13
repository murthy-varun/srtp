/* 
 * File:   SRTP_CommonTypes.h
 * Author: Varun & Shruthi
 *
 * Created on November 3, 2008, 10:37 PM
 */

#ifndef _SRTP_COMMONTYPES_H
#define	_SRTP_COMMONTYPES_H

#include<stdint.h>
#include<unistd.h>
#include<pthread.h>
#include<string>
#include <stdlib.h>
#include <sys/time.h>

typedef int SRTP_ThreadID;
typedef unsigned long SRTP_Address;
typedef unsigned short SRTP_Port;

// constants
const int SRTP_HEADER_SIZE = 32;
const int SRTP_INITIAL_WINDOW_SIZE = 1024;
const int SRTP_RETRANSMISSION_TIME = 5;

// variables for congestion control
extern int global_threshold;
extern int global_no_ack_recd;
extern int global_no_ack_req;

// Mutexes and condition variable for thread synchronisation
extern pthread_mutex_t out_buf_cv_mutex;
extern pthread_cond_t  out_buf_cond;
extern pthread_mutex_t in_buf_cv_mutex;
extern pthread_cond_t  in_buf_cond;
extern pthread_mutex_t condition_mutex;
extern pthread_cond_t  condition_cond;
extern pthread_mutex_t table_mutex;
extern pthread_mutex_t sender_thread_cv_mutex;
extern pthread_cond_t  sender_thread_cond;
extern pthread_mutex_t con_server_cv_mutex;
extern pthread_cond_t  con_server_cond;
extern pthread_mutex_t end_con_cv_mutex;
extern pthread_cond_t  end_con_cond;

// Variables for re-transmission
extern bool out_buf_end;
extern bool timer_expired;
extern bool head_not_inc;

// Statistics counters
extern unsigned int num_packets_sent;
extern unsigned int num_packets_recvd;
extern unsigned int num_packets_retrans;
extern unsigned int num_checksum_errors;
extern unsigned int num_acks_sent;
extern unsigned int num_acks_recvd;
extern unsigned long time_sent;
extern unsigned long time_recvd;

enum SRTP_ConnectionType{
    SRTP_NONE = 0,
    SRTP_CONNECTION_LISTEN = 1,
    SRTP_CONNECTION_OPEN =2,
};

enum SRTP_CounterType{
    SRTP_NUM_CONNECTIONS_COUNTER = 0,
    SRTP_NUM_PKTS_TX_COUNTER =1,
    SRTP_NUM_PKTS_RX_COUNTER =2,
    SRTP_NUM_PKTS_RETX_COUNTER =3,
    SRTP_NUM_CHECKSUM_ERRORS_COUNTER =4
};

enum SRTP_EventType{
    SRTP_DEFAULT_EVENT = -1,
    SRTP_TIMER_EVENT = 0
};

enum SRTP_PacketType{    
    SRTP_DEFAULT_PACKET = -1,
    SRTP_SYN            = 0,
    SRTP_SYN_ACK        = 1,
    SRTP_ACK            = 2,
    SRTP_FIN            = 3,
    SRTP_RST            = 4,
    SRTP_DATA           = 5
};

enum SRTP_PeerStatus{
    SRTP_CLOSED     = 0,
    SRTP_LISTEN     = 1,    
    SRTP_SYN_RCVD   = 2,
    SRTP_SYN_SENT   = 3,
    SRTP_ESTBLSHD   = 4,
    SRTP_CLOSE_WAIT = 5,
    SRTP_CLOSING    = 6,
    SRTP_FIN_WAIT_1 = 7,
    SRTP_FIN_WAIT_2 = 8,
    SRTP_LAST_ACK   = 9,
    SRTP_TIME_WAIT  = 10
};

enum SRTP_ConnectionStatus{
    SRTP_ATTEMPT_OPEN   = 1,
    SRTP_LISTENING      = 2,
    SRTP_ACTIVE_OPEN    = 3,
    SRTP_PASSIVE_OPEN   = 4,
    SRTP_CLOSE_REQ_ACT  = 5,
    SRTP_CLOSE_REQ_PAS  = 6,
    SRTP_NO_CONNECTION  = -1
};

enum SRTP_CongestionPhase{
	SLOW_START = 0,
	COGESTION_AVOIDANCE = 1
};

enum SRTP_SIMULATION_ERROR{
	NO_ERROR	 = 0,
	PACKET_LOSS  = 1,
	OUT_OF_ORDER = 2,
	DUPLICATE	 = 3
};

struct SRTP_PacketStructure{
  uint16_t connection_id;
  uint16_t packet_type;
  uint32_t source_addr;
  uint32_t dest_addr;
  uint16_t source_port;
  uint16_t dest_port;
  uint32_t seq_num;
  uint32_t ack_num;
  uint16_t window_size;
  uint16_t checksum;
  char data[1024];
  
};



#endif	/* _SRTP_COMMONTYPES_H */

