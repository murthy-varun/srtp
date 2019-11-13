/* 
 * File:   SRTP_OutgoingBuffer.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 9:44 AM
 */

#ifndef _SRTP_OUTGOINGBUFFER_H
#define	_SRTP_OUTGOINGBUFFER_H

#include "SRTP_CommonTypes.h"
#include "SRTP_Packet.h"
#include <deque>


class SRTP_OutgoingBuffer{
private:
    std::deque<SRTP_Packet> m_packet_buffer;
    std::deque<SRTP_Packet>::iterator m_head;
    std::deque<SRTP_Packet>::iterator m_tail;
    unsigned int m_win_size;
    

public:    
	SRTP_OutgoingBuffer();

    void add_packet(SRTP_Packet packet);
    SRTP_Packet get_first_packet();
    SRTP_Packet get_first_packet(int connection_id);
    SRTP_Packet get_last_packet();
	SRTP_Packet get_packet(int pos);
	SRTP_Packet get_packet(int connection_id, int seq_num);
	void set_head_to_begin();
	SRTP_Packet get_head_packet();
	void increment_head();
    void pop_front();
	void pop_back();
	void remove_packet(int connection_id, int seq_num);
    void increase_size(unsigned int size);
    void decrease_size(unsigned int size);
    unsigned int get_buffer_size();
	void set_window_size(int size);
	int get_window_size();
};

#endif	/* _SRTP_OUTGOINGBUFFER_H */

