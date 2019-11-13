/* 
 * File:   SRTP_ConnectionInfo.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 8:39 AM
 */

#ifndef _SRTP_CONNECTIONINFO_H
#define	_SRTP_CONNECTIONINFO_H

#include <map>
#include "SRTP_CommonTypes.h"

class SRTP_ConnectionInfo{
private:
	int m_connection_num;
	int m_socket_id;
	int m_local_addr;
	int m_local_port;
	int m_remote_addr;
	int m_remote_port;
 
   
public:
    SRTP_ConnectionInfo();
	SRTP_ConnectionInfo(int connection_num, 
						int socket_id, 
						int local_addr,
						int local_port,
						int remote_addr,
						int remote_port);

	~SRTP_ConnectionInfo();

	void set_connection_num(int connection_num);
	void set_socket_id(int socket_id);
	void set_local_addr(int local_addr);
	void set_local_port(int local_port);
	void set_remote_addr(int remote_addr);    
	void set_remote_port(int remote_port);


	int get_connection_num();
	int get_socket_id();
	int get_local_addr();
	int get_local_port();
	int get_remote_addr();    
	int get_remote_port();


	void clear_connection_num();
	void clear_socket_id();
	void clear_local_addr();
	void clear_local_port();
	void clear_remote_addr();    
	void clear_remote_port();
};

#endif	/* _SRTP_CONNECTIONINFO_H */

