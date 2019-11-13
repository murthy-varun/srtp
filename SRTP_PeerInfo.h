/* 
 * File:   SRTP_PeerInfo.h
 * Author: Varun & Shruthi
 *
 * Created on November 9, 2008, 2:40 AM
 */

#ifndef _SRTP_PEERINFO_H
#define	_SRTP_PEERINFO_H

#include "SRTP_CommonTypes.h"
#include <iostream>

class SRTP_PeerInfo{
private:
    unsigned int m_connection_id;
    SRTP_PeerStatus m_peer_status;
    SRTP_Address m_remote_addr; 
    SRTP_Port m_remote_port;
    unsigned int m_window_size;
	int m_expected_packet;
	int m_cur_seq_num;
    
public:
    SRTP_PeerInfo();
    SRTP_PeerInfo(unsigned int connection_id, SRTP_PeerStatus peer_status, SRTP_Address remote_addr, SRTP_Port remote_port, unsigned int window_size);
    ~SRTP_PeerInfo();
    
    void set_connection_id(unsigned int connection_id);
    unsigned int get_connection_id();
    void clear_connection_id();
    
    void set_peer_status(SRTP_PeerStatus peer_status);
    SRTP_PeerStatus get_peer_status();
    void clear_peer_status();
    
    void set_remote_addr(SRTP_Address remote_addr);
    SRTP_Address get_remote_addr();
    void clear_remote_addr();
    
    void set_remote_port(SRTP_Port remote_port);
    SRTP_Port get_remote_port();
    void clear_remote_port();
    
    void set_window_size(unsigned int window_size);
    unsigned int get_window_size();
    void clear_window_size();

	void set_expected_packet(int expected_packet);
	int get_expected_packet();
	void clear_expected_packet();

	void set_cur_seq_num(int cur_seq_num);
	int get_cur_seq_num();
	void clear_cur_seq_num();

};

#endif	/* _SRTP_PEERINFO_H */

