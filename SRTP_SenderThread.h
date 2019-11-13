/* 
 * File:   SRTP_SenderThread.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 2:13 AM
 */

#ifndef _SRTP_SENDERTHREAD_H
#define	_SRTP_SENDERTHREAD_H

#include "SRTP_Thread.h"
#include "SRTP_Packet.h"
#include "SRTP_OutgoingBuffer.h"
#include "SRTP_CommonTypes.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>


class SRTP_SenderThread:public SRTP_Thread{
private:
	SRTP_OutgoingBuffer* m_outgoing_buffer;
    int m_socket_id;
        
public:
	SRTP_SenderThread();    
	SRTP_SenderThread(int socket_id, SRTP_OutgoingBuffer* out_buf);
    ~SRTP_SenderThread();
    virtual void run();
    void send();

};


#endif	/* _SRTP_SENDERTHREAD_H */

