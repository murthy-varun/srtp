/* 
 * File:   SRTP_Connection.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 9:06 AM
 */

#ifndef _SRTP_CONNECTION_H
#define	_SRTP_CONNECTION_H
#include <string>
#include "SRTP_CommonTypes.h"
#include "SRTP_IncomingBuffer.h"
#include "SRTP_OutgoingBuffer.h"
#include "SRTP_ConnectorThread.h"

class SRTP_Connection{
private:
    std::string m_local_addr;
    SRTP_Port m_local_port;
    SRTP_Port m_remote_port;
    std::string m_remote_addr;
    SRTP_IncomingBuffer* m_incoming_buffer_ptr;
    SRTP_OutgoingBuffer* m_outgoing_buffer_ptr;
    SRTP_ConnectorThread* m_connector_thread;
    
public:
      SRTP_Connection();
   
      SRTP_Connection(std::string localaddr, SRTP_Port localport, SRTP_IncomingBuffer* in_buffer_ptr, SRTP_OutgoingBuffer* out_buffer_ptr,std::string remoteaddr = " ",SRTP_Port remoteport = 0 );
      ~SRTP_Connection();
      
      int SRTP_Listen();
      int SRTP_Open();
      int SRTP_Status(int connection_num);
      int SRTP_Close(int connection_num);
	  void join_connector_thread();
	  void wait_for_end_connection(int connection_num);
};


#endif	/* _SRTP_CONNECTION_H */

