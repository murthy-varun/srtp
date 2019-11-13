/* 
 * File:   SRTP_OutgoingMessageHandler.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 9:55 AM
 */

#ifndef _SRTP_OUTGOINGMESSAGEHANDLER_H
#define	_SRTP_OUTGOINGMESSAGEHANDLER_H

#include "SRTP_CommonTypes.h"
#include "SRTP_OutgoingBuffer.h"

class SRTP_OutgoingMessageHandler{
private:
   SRTP_OutgoingBuffer*  m_outgoing_buffer_ptr; 
   SRTP_SIMULATION_ERROR m_error_type;
   int m_packet_number;
public:
    SRTP_OutgoingMessageHandler(SRTP_OutgoingBuffer* out_buf_ptr);
    ~SRTP_OutgoingMessageHandler();
    int SRTP_Send(int connection_num, std::string data);

	void simulate_error_case(SRTP_SIMULATION_ERROR error_type, int packet_number);
};

#endif	/* _SRTP_OUTGOINGMESSAGEHANDLER_H */

