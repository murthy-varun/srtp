/* 
 * File:   SRTP_PeerTable.h
 * Author: Varun & Shruthi
 *
 * Created on November 9, 2008, 2:34 AM
 */

#ifndef _SRTP_PEERTABLE_H
#define	_SRTP_PEERTABLE_H

#include "SRTP_CommonTypes.h"
#include "SRTP_PeerInfo.h"
#include <iostream>
#include <map>

class SRTP_PeerTable{
private:
    std::map<unsigned int, SRTP_PeerInfo> m_peer_info_map;
    static SRTP_PeerTable m_peer_table_obj;
    
    SRTP_PeerTable();
    
public:
    static SRTP_PeerTable& get_peer_table_instance();
    
    void add_peer_table_entry(unsigned int connection_id, SRTP_PeerInfo peer_info);
    
    void modify_peer_table_entry(unsigned int connection_id, SRTP_PeerInfo peer_info);
    
    void delete_peer_table_entry(unsigned int connection_id);
    
    SRTP_PeerInfo get_peer_table_entry(unsigned int connection_id);
    
    void clear_peer_table();
	
	int get_map_size();

	int get_con_num(int position);
    
};

#endif	/* _SRTP_PEERTABLE_H */

