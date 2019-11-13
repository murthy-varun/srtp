/* 
 * File:   SRTP_ListenerThread.h
 * Author: Varun & Shruthi
 *
 * Created on November 5, 2008, 9:07 PM
 */

#ifndef _SRTP_LISTENERTHREAD_H
#define	_SRTP_LISTENERTHREAD_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "SRTP_Thread.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_IncomingBuffer.h"
#include "SRTP_OutgoingBuffer.h"

class SRTP_ListenerThread:public SRTP_Thread{
private:
    struct sockaddr_in m_local_socket_addr;
	SRTP_IncomingBuffer* m_in_buf;
	SRTP_OutgoingBuffer* m_out_buf;
    
public:
    SRTP_ListenerThread();
    SRTP_ListenerThread(struct sockaddr_in local_socket_addr, SRTP_IncomingBuffer* in_buf, SRTP_OutgoingBuffer* out_buf);
    ~SRTP_ListenerThread();
    virtual void run();
    void start_listening();
    void stop_listening();
};


#endif	/* _SRTP_LISTENERTHREAD_H */

