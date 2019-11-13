#include "SRTP_PeerTable.h"
#include "SRTP_CommonTypes.h"

SRTP_PeerTable SRTP_PeerTable::m_peer_table_obj;

SRTP_PeerTable::SRTP_PeerTable(){
//    std::clog<<"ENTER:\t SRTP_PeerTable::SRTP_PeerTable\n";
    
//    std::clog<<"EXIT:\t SRTP_PeerTable::SRTP_PeerTable\n";
}

SRTP_PeerTable& SRTP_PeerTable::get_peer_table_instance(){
    std::clog<<"ENTER:\t SRTP_PeerTable::get_peer_table_instance\n";    
    std::clog<<"EXIT:\t SRTP_PeerTable::get_peer_table_instance\n";
    return m_peer_table_obj;
}

void SRTP_PeerTable::add_peer_table_entry(unsigned int connection_id, SRTP_PeerInfo peer_info){
    std::clog<<"ENTER:\t SRTP_PeerTable::add_peer_table_entry\n";
    m_peer_info_map.insert(std::map<unsigned int, SRTP_PeerInfo>::value_type(connection_id, peer_info));
    std::clog<<"EXIT:\t SRTP_PeerTable::add_peer_table_entry\n";
}

void SRTP_PeerTable::modify_peer_table_entry(unsigned int connection_id, SRTP_PeerInfo peer_info){
    std::clog<<"ENTER:\t SRTP_PeerTable::modify_peer_table_entry\n";
    m_peer_info_map[connection_id] = peer_info;
    std::clog<<"EXIT:\t SRTP_PeerTable::modify_peer_table_entry\n";
}

void SRTP_PeerTable::delete_peer_table_entry(unsigned int connection_id){
    std::clog<<"ENTER:\t SRTP_PeerTable::delete_peer_table_entry\n";
    m_peer_info_map.erase(connection_id);
    std::clog<<"EXIT:\t SRTP_PeerTable::delete_peer_table_entry\n";
}

SRTP_PeerInfo SRTP_PeerTable::get_peer_table_entry(unsigned int connection_id){
    std::clog<<"ENTER:\t SRTP_PeerTable::get_peer_table_entry\n";    
    std::clog<<"EXIT:\t SRTP_PeerTable::get_peer_table_entry\n";
    return m_peer_info_map[connection_id];
}

void SRTP_PeerTable::clear_peer_table(){
    std::clog<<"ENTER:\t SRTP_PeerTable::clear_peer_table\n";
    m_peer_info_map.clear();
    std::clog<<"EXIT:\t SRTP_PeerTable::clear_peer_table\n";
}
int SRTP_PeerTable::get_map_size(){
	return m_peer_info_map.size();
}

int SRTP_PeerTable::get_con_num(int position){
	std::map<unsigned int, SRTP_PeerInfo>::iterator map_iterator = m_peer_info_map.begin();
	
	for(int i=0; i< position; i++){
		map_iterator++;
	}
	return map_iterator->first;
}
