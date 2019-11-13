/* 
 * File:   SRTP_ReceiverThread.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 2:08 AM
 */

#ifndef _SRTP_RECEIVERTHREAD_H
#define	_SRTP_RECEIVERTHREAD_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#include "SRTP_Thread.h"
#include "SRTP_Packet.h"
#include "SRTP_IncomingBuffer.h"
#include "SRTP_OutgoingBuffer.h"
#include "SRTP_CommonTypes.h"

class SRTP_ReceiverThread:public SRTP_Thread{
private:
    SRTP_ConnectionType m_connection_type;
    SRTP_IncomingBuffer* m_incoming_buffer;
	SRTP_OutgoingBuffer* m_outgoing_buffer;
    int m_socket_id;
    struct sockaddr_in m_local_socket;
   
    void handle_server_msg();
    void handle_client_msg(); 
        
public:
    SRTP_ReceiverThread();
    SRTP_ReceiverThread(SRTP_ConnectionType connection_type, int socket_id, struct sockaddr_in local_socket, SRTP_IncomingBuffer* in_buf, SRTP_OutgoingBuffer* out_buf);
    ~SRTP_ReceiverThread();
    virtual void run();
    void receive(); 

   
};


#endif	/* _SRTP_RECEIVERTHREAD_H */

