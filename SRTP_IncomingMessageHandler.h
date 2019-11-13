/* 
 * File:   SRTP_IncomingMessageHandler.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 9:55 AM
 */

#ifndef _SRTP_INCOMINGMESSAGEHANDLER_H
#define	_SRTP_INCOMINGMESSAGEHANDLER_H

#include "SRTP_CommonTypes.h"
#include "SRTP_IncomingBuffer.h"

class SRTP_IncomingMessageHandler{
private:
    SRTP_IncomingBuffer* m_incoming_buffer_ptr;
        
public:
    SRTP_IncomingMessageHandler(SRTP_IncomingBuffer* in_buf_ptr);
    ~SRTP_IncomingMessageHandler();
    std::string SRTP_Receive(int connection_num);
	std::string SRTP_Receive(int* connection_num);
};

#endif	/* _SRTP_INCOMINGMESSAGEHANDLER_H */

