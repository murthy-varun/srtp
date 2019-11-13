#include "SRTP_IncomingBuffer.h"
#include "SRTP_CommonTypes.h"
#include <iostream>



SRTP_IncomingBuffer::SRTP_IncomingBuffer(){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::SRTP_IncomingBuffer\n";
    m_head = m_packet_buffer.begin();
    m_tail = m_packet_buffer.end();
    m_size = 0;    std::clog<<"EXIT:\t SRTP_IncomingBuffer::SRTP_IncomingBuffer\n";
}


void SRTP_IncomingBuffer::add_packet(SRTP_Packet packet){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::add_packet\n";

	bool is_added=false;

	if(get_buffer_size() == 0){
		m_packet_buffer.push_back(packet);
		is_added=true;
	}
	else{
		// Add the packet to the deque and sort it based on the sequence numbers
		std::deque<SRTP_Packet>::iterator buffer_iterator;
		for(buffer_iterator = m_packet_buffer.begin(); buffer_iterator != m_packet_buffer.end(); buffer_iterator++)
		{
			SRTP_Packet buffer_packet = *buffer_iterator;
			if(packet.get_seq_num() > buffer_packet.get_seq_num()){
				continue;
			}
			else{
				m_packet_buffer.insert(buffer_iterator, packet);
				is_added=true;
				break;
			}
		}

		if(!is_added){
			m_packet_buffer.push_back(packet);
			is_added = true;
		}
	}
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::add_packet\n";
}

SRTP_Packet SRTP_IncomingBuffer::get_first_packet(){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::get_first_packet\n";
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::get_first_packet\n";
	return m_packet_buffer.front();
}

SRTP_Packet SRTP_IncomingBuffer::get_first_packet(int connection_id){
	std::deque<SRTP_Packet>::iterator buffer_iterator;
	for(buffer_iterator = m_packet_buffer.begin(); buffer_iterator != m_packet_buffer.end(); buffer_iterator++){
		SRTP_Packet tmp_packet = *buffer_iterator;
		if(tmp_packet.get_connection_id() == connection_id)
		{
			break;
		}
	}
	return *buffer_iterator;
}

SRTP_Packet SRTP_IncomingBuffer::get_last_packet(){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::get_last_packet\n";
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::get_last_packet\n";
	return m_packet_buffer.back();
}

SRTP_Packet SRTP_IncomingBuffer::get_packet(int pos){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::get_packet\n";
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::get_packet\n";
	return m_packet_buffer[pos];
}

SRTP_Packet SRTP_IncomingBuffer::get_packet(int connection_id, int seq_num){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::get_packet\n";


	std::deque<SRTP_Packet>::iterator buffer_iterator;
	for(buffer_iterator = m_packet_buffer.begin(); buffer_iterator != m_packet_buffer.end(); buffer_iterator++){
		SRTP_Packet tmp_packet = *buffer_iterator;
		if(tmp_packet.get_connection_id() == connection_id && tmp_packet.get_seq_num() == seq_num)
		{
			break;
		}
	}
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::get_packet\n";
	return *buffer_iterator;
}

SRTP_Packet SRTP_IncomingBuffer::get_head_packet(){
	return *m_head;
}

void SRTP_IncomingBuffer::increment_head(){
	m_head++;
}

void SRTP_IncomingBuffer::pop_front(){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::pop_front\n";
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::pop_front\n";
	m_packet_buffer.pop_front();
}

void SRTP_IncomingBuffer::pop_back(){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::pop_back\n";
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::pop_back\n";
	m_packet_buffer.pop_back();
}

void SRTP_IncomingBuffer::remove_packet(int connection_id, int seq_num){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::remove_packet\n";
	std::deque<SRTP_Packet>::iterator buffer_iterator;
	bool is_present = false;

	for(buffer_iterator = m_packet_buffer.begin(); buffer_iterator != m_packet_buffer.end(); buffer_iterator++){
		SRTP_Packet tmp_packet = *buffer_iterator;
		if(tmp_packet.get_connection_id() == connection_id && tmp_packet.get_seq_num() == seq_num)
		{
			std::clog<<"INFO:\t removing packet with connection id: "<<(*buffer_iterator).get_connection_id()<<" and seq num: "<<(*buffer_iterator).get_seq_num()<<"\n";
			m_packet_buffer.erase(buffer_iterator);
			is_present = true;
			break;
		}
	}

	if(!is_present)
	{
		std::clog<<"ERROR:\n Packet not available in the Buffer\n";
	}
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::remove_packet\n";
}


void SRTP_IncomingBuffer::increase_size(unsigned int size){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::increase_size\n";

    std::clog<<"EXIT:\t SRTP_IncomingBuffer::increase_size\n";
}

void SRTP_IncomingBuffer::decrease_size(unsigned int size){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::decrease_size\n";

    std::clog<<"EXIT:\t SRTP_IncomingBuffer::decrease_size\n";
}

unsigned int SRTP_IncomingBuffer::get_buffer_size(){
    std::clog<<"ENTER:\t SRTP_IncomingBuffer::get_buffer_size\n";
    std::clog<<"EXIT:\t SRTP_IncomingBuffer::get_buffer_size\n";
	return m_packet_buffer.size();
}
