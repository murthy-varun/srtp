/* 
 * File:   SRTP_ConnectorThread.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 2:01 AM
 */

#ifndef _SRTP_CONNECTORTHREAD_H
#define	_SRTP_CONNECTORTHREAD_H

#include "SRTP_Thread.h"
#include "SRTP_IncomingBuffer.h"
#include "SRTP_OutgoingBuffer.h"
#include "SRTP_CommonTypes.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class SRTP_ConnectorThread:public SRTP_Thread{
private:
	struct sockaddr_in m_remote_socket_addr;
	struct sockaddr_in m_local_socket_addr;
	SRTP_IncomingBuffer* m_in_buf;
	SRTP_OutgoingBuffer* m_out_buf;
    
public:
    SRTP_ConnectorThread();
    SRTP_ConnectorThread(struct sockaddr_in remote_socket, struct sockaddr_in local_socket_addr, SRTP_IncomingBuffer* in_buf, SRTP_OutgoingBuffer* out_buf);
    ~SRTP_ConnectorThread();
    virtual void run();
    void open_connection();
    void close_connection();
};


#endif	/* _SRTP_CONNECTORTHREAD_H */

