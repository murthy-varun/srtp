#include "SRTP_Packet.h"
#include "SRTP_CommonTypes.h"
#include <iostream>

SRTP_Packet::SRTP_Packet(){
    std::clog<<"ENTER:\t SRTP_PACKET::SRTP_PACKET\n";
    m_connection_id = 0;
    m_packet_type = SRTP_DEFAULT_PACKET;
    m_source_addr = 0;
    m_dest_addr = 0;
    m_source_port = 0;
    m_dest_port = 0;
    m_seq_num = 0;
    m_ack_num = 0;
    m_checksum = 0;
    m_window_size = 0;
    
    std::clog<<"EXIT:\t SRTP_PACKET::SRTP_PACKET\n";
}

SRTP_Packet::SRTP_Packet(unsigned int connection_id,
                        SRTP_PacketType packet_type,
                        SRTP_Address source_addr,
                        SRTP_Address dest_addr,
                        SRTP_Port source_port,
                        SRTP_Port dest_port,
                        unsigned int seq_num,
                        unsigned int ack_num,
                        unsigned int checksum,
                        unsigned int window_size,
                        std::string buffer){
    std::clog<<"ENTER:\t SRTP_PACKET::SRTP_PACKET\n";
    std::clog<<"PARAM:\t "
             <<"(connection_id:"<<connection_id<<" (packet_type):"<<packet_type
             <<"(source_addr):"<<source_addr<<" (dest_addr):"<<dest_addr
             <<" (source_port):"<<source_port<<" (dest_port):"<<dest_port
             <<" (seq_num):"<<seq_num<<" (ack_num):"<<ack_num             
             <<" (checksum):"<<checksum<<" (window_size):"<<window_size
             <<" (buffer):"<<buffer<<"\n";
    
    m_connection_id = connection_id;
    m_packet_type = packet_type;
    m_source_addr = source_addr;
    m_dest_addr = dest_addr;
    m_source_port = source_port;
    m_dest_port = dest_port;
    m_seq_num = seq_num;
    m_ack_num = ack_num;
    m_checksum = checksum;
    m_window_size = window_size;
    m_buffer = buffer;
 
    std::clog<<"EXIT:\t SRTP_PACKET::SRTP_PACKET\n";
}

SRTP_Packet::~SRTP_Packet(){
    std::clog<<"ENTER:\t SRTP_PACKET::~SRTP_PACKET\n";
    
    std::clog<<"EXIT:\t SRTP_PACKET::~SRTP_PACKET\n";
}

void SRTP_Packet::set_connection_id(unsigned int connection_id){
    std::clog<<"ENTER:\t SRTP_PACKET::set_connection_id\n";
    m_connection_id = connection_id;
    std::clog<<"EXIT:\t SRTP_PACKET::set_connection_id\n";
}

unsigned int SRTP_Packet::get_connection_id(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_connection_id\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_connection_id\n";
    return m_connection_id;
}

void SRTP_Packet::clear_connection_id(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_connection_id\n";
    m_connection_id = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_connection_id\n";
}

void SRTP_Packet::set_time(unsigned long t)
{
    m_time = t;
} 

unsigned long SRTP_Packet::get_time()
{
   return m_time;
}

void SRTP_Packet::clear_time()
{
   m_time = 0;
}

void SRTP_Packet::set_packet_type(SRTP_PacketType packet_type){
    std::clog<<"ENTER:\t SRTP_PACKET::set_packet_type\n";
    m_packet_type = packet_type;
    std::clog<<"EXIT:\t SRTP_PACKET::set_packet_type\n";
}

SRTP_PacketType SRTP_Packet::get_packet_type(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_packet_type\n";    
    std::clog<<"EXIT:\t SRTP_PACKET::get_packet_type\n";
    return m_packet_type;
}

void SRTP_Packet::clear_packet_type(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_packet_type\n";
    m_packet_type = SRTP_DEFAULT_PACKET;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_packet_type\n";
}

void SRTP_Packet::set_source_addr(SRTP_Address source_addr){
    std::clog<<"ENTER:\t SRTP_PACKET::set_source_addr\n";
    m_source_addr = source_addr;
    std::clog<<"EXIT:\t SRTP_PACKET::set_source_addr\n";
}

SRTP_Address SRTP_Packet::get_source_addr(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_source_addr\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_source_addr\n";
    return m_source_addr;
}

void SRTP_Packet::clear_source_addr(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_source_addr\n";
    m_source_addr = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_source_addr\n";
}

void SRTP_Packet::set_dest_addr(SRTP_Address dest_addr){
    std::clog<<"ENTER:\t SRTP_PACKET::set_dest_addr\n";
    m_dest_addr = dest_addr;
    std::clog<<"EXIT:\t SRTP_PACKET::set_dest_addr\n";
}

SRTP_Address SRTP_Packet::get_dest_addr()
{
    std::clog<<"ENTER:\t SRTP_PACKET::get_dest_addr\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_dest_addr\n";
    return m_dest_addr;
}

void SRTP_Packet::clear_dest_addr(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_dest_addr\n";
    m_dest_addr = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_dest_addr\n";
}

void SRTP_Packet::set_source_port (SRTP_Port source_port){
    std::clog<<"ENTER:\t SRTP_PACKET::set_source_port\n";
    m_source_port = source_port;
    std::clog<<"EXIT:\t SRTP_PACKET::set_source_port\n";
}

SRTP_Port SRTP_Packet::get_source_port(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_source_port\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_source_port\n";
    return m_source_port;   
}

void SRTP_Packet::clear_source_port(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_source_port\n";
    m_source_port = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_source_port\n";
}

void SRTP_Packet::set_dest_port(SRTP_Port dest_port){
    std::clog<<"ENTER:\t SRTP_PACKET::set_dest_port\n";
    m_dest_port = dest_port;
    std::clog<<"EXIT:\t SRTP_PACKET::set_dest_port\n";
}

SRTP_Port SRTP_Packet::get_dest_port(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_dest_port\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_dest_port\n";
    return m_dest_port;
}

void SRTP_Packet::clear_dest_port(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_dest_port\n";
    m_dest_port = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_dest_port\n";
}

void SRTP_Packet::set_seq_num(unsigned int seq_num){
    std::clog<<"ENTER:\t SRTP_PACKET::set_seq_num\n";
    m_seq_num = seq_num;
    std::clog<<"EXIT:\t SRTP_PACKET::set_seq_num\n";
}

unsigned int SRTP_Packet::get_seq_num(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_seq_num\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_seq_num\n";
    return m_seq_num;
}

void SRTP_Packet::clear_seq_num(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_seq_num\n";
    m_seq_num = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_seq_num\n";
}

void SRTP_Packet::set_ack_num(unsigned int ack_num){
    std::clog<<"ENTER:\t SRTP_PACKET::set_ack_num\n";
    m_ack_num = ack_num;
    std::clog<<"EXIT:\t SRTP_PACKET::set_ack_num\n";
}

unsigned int SRTP_Packet::get_ack_num(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_ack_num\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_ack_num\n";
    return m_ack_num;
}

void SRTP_Packet::clear_ack_num(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_ack_num\n";
    m_ack_num = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_ack_num\n";
}

void SRTP_Packet::set_checksum(unsigned int checksum){
    std::clog<<"ENTER:\t SRTP_PACKET::set_checksum\n";
    m_checksum = checksum;
    std::clog<<"EXIT:\t SRTP_PACKET::set_checksum\n";
}

unsigned int SRTP_Packet::get_checksum(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_checksum\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_checksum\n";
    return m_checksum;
}

void SRTP_Packet::clear_checksum(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_checksum\n";
    m_checksum = 0;
    std::clog<<"EXIT:\t SRTP_PACKET::clear_checksum\n";
}

void SRTP_Packet::set_window_size(unsigned int window_size){
    std::clog<<"ENTER:\t SRTP_PACKET::set_window_size\n";
    m_window_size = window_size;
    std::clog<<"EXIT:\t SRTP_PACKET::set_window_size\n";
}

unsigned int SRTP_Packet::get_window_size(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_window_size\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_window_size\n";
    return m_window_size;
}

void SRTP_Packet::clear_window_size(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_window_size\n";
    m_window_size = 0;
    std::clog<<"FUNCTION    END: SRTP_PACKET::clear_window_size\n";
}

void SRTP_Packet::set_buffer(std::string buffer){
    std::clog<<"ENTER:\t SRTP_PACKET::set_buffer\n";
    m_buffer = buffer;
    std::clog<<"EXIT:\t SRTP_PACKET::set_buffer\n";
}

std::string SRTP_Packet::get_buffer(){
    std::clog<<"ENTER:\t SRTP_PACKET::get_buffer\n";
    std::clog<<"EXIT:\t SRTP_PACKET::get_buffer\n";
    return m_buffer;
}

void SRTP_Packet::clear_buffer(){
    std::clog<<"ENTER:\t SRTP_PACKET::clear_buffer\n";

    std::clog<<"EXIT:\t SRTP_PACKET::clear_buffer\n";
}


