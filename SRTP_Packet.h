/* 
 * File:   SRTP_Packet.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 2:24 AM
 */

#ifndef _SRTP_PACKET_H
#define	_SRTP_PACKET_H

#include "SRTP_CommonTypes.h"

class SRTP_Packet{
private:
    unsigned int m_connection_id;
    SRTP_PacketType m_packet_type;
    SRTP_Address m_source_addr;
    SRTP_Address m_dest_addr;
    SRTP_Port m_source_port;
    SRTP_Port m_dest_port;
    unsigned int m_seq_num;
    unsigned int m_ack_num;
    unsigned int m_checksum;
    unsigned int m_window_size;
    std::string m_buffer;
    unsigned long m_time;
    
public:
    SRTP_Packet();
    
    SRTP_Packet(unsigned int connection_id,
                SRTP_PacketType packet_type,
                SRTP_Address source_addr,
                SRTP_Address dest_addr,
                SRTP_Port source_port,
                SRTP_Port dest_port,
                unsigned int seq_num,
                unsigned int ack_num,
                unsigned int checksum,
                unsigned int window_size,
                std::string buffer);
    
    ~SRTP_Packet();
    
    void set_connection_id(unsigned int connection_id);
    unsigned int get_connection_id();
    void clear_connection_id();
    
    void set_packet_type(SRTP_PacketType packet_type);
    SRTP_PacketType get_packet_type();
    void clear_packet_type();
    
    void set_source_addr (SRTP_Address source_addr);
    SRTP_Address get_source_addr();
    void clear_source_addr();
    
    void set_dest_addr (SRTP_Address dest_addr);
    SRTP_Address get_dest_addr();
    void clear_dest_addr();
    
    void set_source_port (SRTP_Port source_port);
    SRTP_Port get_source_port();
    void clear_source_port();
    
    void set_dest_port (SRTP_Port dest_port);
    SRTP_Port get_dest_port();
    void clear_dest_port();
    
    void set_seq_num (unsigned int seq_num);
    unsigned int get_seq_num();
    void clear_seq_num();
    
    void set_ack_num (unsigned int ack_num);
    unsigned int get_ack_num();
    void clear_ack_num();
   
    void set_checksum (unsigned int checksum);
    unsigned int get_checksum();
    void clear_checksum();
    
    void set_window_size (unsigned int window_size);
    unsigned int get_window_size();
    void clear_window_size();
    
    void set_buffer (std::string buffer);
    std::string get_buffer();
    void clear_buffer();
    
    void set_time(unsigned long t);
    unsigned long get_time();
    void clear_time();
};

#endif	/* _SRTP_PACKET_H */

