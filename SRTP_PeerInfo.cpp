#include "SRTP_PeerInfo.h"
#include "SRTP_CommonTypes.h"
#include <iostream>

SRTP_PeerInfo::SRTP_PeerInfo(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::SRTP_PeerInfo\n";
    m_connection_id = 0;
    m_peer_status = SRTP_CLOSED;
    m_remote_addr = 0;
    m_remote_port = 0;
    m_window_size = 0;
	m_expected_packet = 0;
	m_cur_seq_num = 0;
    std::clog<<"EXIT:\t SRTP_PeerInfo::SRTP_PeerInfo\n";
}

SRTP_PeerInfo::SRTP_PeerInfo(unsigned int connection_id, SRTP_PeerStatus peer_status, SRTP_Address remote_addr, SRTP_Port remote_port, unsigned int window_size){
    std::clog<<"ENTER:\t SRTP_PeerInfo::SRTP_PeerInfo\n";
    m_connection_id = connection_id;
    m_peer_status = peer_status;
    m_remote_addr = remote_addr;
    m_remote_port = remote_port;
    m_window_size = window_size;
	m_expected_packet = 0;
	m_cur_seq_num = 0;
    std::clog<<"EXIT:\t SRTP_PeerInfo::SRTP_PeerInfo\n";
}

SRTP_PeerInfo::~SRTP_PeerInfo(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::~SRTP_PeerInfo\n";
    
    std::clog<<"EXIT:\t SRTP_PeerInfo::~SRTP_PeerInfo\n";
}

void SRTP_PeerInfo::set_connection_id(unsigned int connection_id){
    std::clog<<"ENTER:\t SRTP_PeerInfo::set_connection_id\n";
    m_connection_id = connection_id;
    std::clog<<"EXIT:\t SRTP_PeerInfo::set_connection_id\n";
}

unsigned int SRTP_PeerInfo::get_connection_id(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::get_connection_id\n";    
    std::clog<<"EXIT:\t SRTP_PeerInfo::get_connection_id\n";
    return m_connection_id;
}

void SRTP_PeerInfo::clear_connection_id(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_connection_id\n";
    m_connection_id = 0;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_connection_id\n";
}

void SRTP_PeerInfo::set_peer_status(SRTP_PeerStatus peer_status){
    std::clog<<"ENTER:\t SRTP_PeerInfo::set_peer_status\n";
    m_peer_status = peer_status;
    std::clog<<"EXIT:\t SRTP_PeerInfo::set_peer_status\n";
}

SRTP_PeerStatus SRTP_PeerInfo::get_peer_status(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::get_peer_status\n";    
    std::clog<<"EXIT:\t SRTP_PeerInfo::get_peer_status\n";
    return m_peer_status;
}

void SRTP_PeerInfo::clear_peer_status(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";
    m_peer_status = SRTP_CLOSED;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
}

void SRTP_PeerInfo::set_remote_addr(SRTP_Address remote_addr){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";
    m_remote_addr = remote_addr;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
}

SRTP_Address SRTP_PeerInfo::SRTP_PeerInfo::get_remote_addr(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";    
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
    return m_remote_addr;
}

void SRTP_PeerInfo::clear_remote_addr(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";
    m_remote_addr = 0;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
}

void SRTP_PeerInfo::set_remote_port(SRTP_Port remote_port){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";
    m_remote_port = remote_port;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
}

SRTP_Port SRTP_PeerInfo::get_remote_port(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";    
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
    return m_remote_port;
}

void SRTP_PeerInfo::clear_remote_port(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_peer_status\n";
    m_remote_port = 0;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_peer_status\n";
}

void SRTP_PeerInfo::set_window_size(unsigned int window_size){
    std::clog<<"ENTER:\t SRTP_PeerInfo::set_window_size\n";
    m_window_size = window_size;
    std::clog<<"EXIT:\t SRTP_PeerInfo::set_window_size\n";
}

unsigned int SRTP_PeerInfo::get_window_size(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::get_window_size\n";    
    std::clog<<"EXIT:\t SRTP_PeerInfo::get_window_size\n";
    return m_window_size;
}

void SRTP_PeerInfo::clear_window_size(){
    std::clog<<"ENTER:\t SRTP_PeerInfo::clear_window_size\n";
    m_window_size = 0;
    std::clog<<"EXIT:\t SRTP_PeerInfo::clear_window_size\n";
}
void SRTP_PeerInfo::set_expected_packet(int expected_packet){
	std::clog<<"PARAM:\t setting expected packet: "<<expected_packet<<std::endl;
	m_expected_packet = expected_packet;
}

int SRTP_PeerInfo::get_expected_packet(){
	return m_expected_packet;
}

void SRTP_PeerInfo::clear_expected_packet(){
	m_expected_packet = 0;
}

void SRTP_PeerInfo::set_cur_seq_num(int cur_seq_num){
	std::clog<<"PARAM:\t setting current sequence number: "<<cur_seq_num<<std::endl;
	m_cur_seq_num = cur_seq_num;
}

int SRTP_PeerInfo::get_cur_seq_num(){
	return m_cur_seq_num;
}

void SRTP_PeerInfo::clear_cur_seq_num(){
	m_cur_seq_num = 0;
}

