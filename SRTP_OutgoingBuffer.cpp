#include "SRTP_OutgoingBuffer.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_SenderThread.h"
#include "SRTP_GlobalVariables.h"
#include <iostream>

pthread_mutex_t buf_max_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  buf_max_cond  = PTHREAD_COND_INITIALIZER;

SRTP_OutgoingBuffer::SRTP_OutgoingBuffer(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::SRTP_OutgoingBuffer\n";
    m_head = m_packet_buffer.begin();
    m_tail = m_packet_buffer.end();
    m_win_size = 0;    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::SRTP_OutgoingBuffer\n";
}

void SRTP_OutgoingBuffer::add_packet(SRTP_Packet packet){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::add_packet\n";
	bool is_added=false;

	// Check if the size of the Buffer is already equal to the Maximum window Size
	if(m_packet_buffer.size() >= m_win_size)
	{
		std::clog<<"INFO:\t Buffer size ("<<m_packet_buffer.size()<< ") greater or equal to Max Buffer size ("<<m_win_size<<")\n";
		std::clog<<"INFO:\t Waiting to add packet\n";
		pthread_mutex_lock( &buf_max_cv_mutex );
		pthread_cond_wait( &buf_max_cond, &buf_max_cv_mutex );
		pthread_mutex_unlock( &buf_max_cv_mutex );
	}
	

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
	out_buf_end = false;
	std::clog<<"INFO:\t Packet being added: "<<packet.get_buffer()<<" with seq num: "<<packet.get_seq_num()<<" connection num: "<<packet.get_connection_id()<<"\n";
	//std::clog<<"INFO:\t Size of Outgoing Buffer after adding packet = "<<get_buffer_size()<<"\n";

    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::add_packet\n";
}

void SRTP_OutgoingBuffer::set_head_to_begin(){
	std::clog<<"INFO: setting head to begin of buffer\n";
	m_head = m_packet_buffer.begin();
}

SRTP_Packet SRTP_OutgoingBuffer::get_first_packet(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::get_first_packet\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::get_first_packet\n";
	return m_packet_buffer.front();
}

SRTP_Packet SRTP_OutgoingBuffer::get_first_packet(int connection_id){
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


SRTP_Packet SRTP_OutgoingBuffer::get_last_packet(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::get_last_packet\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::get_last_packet\n";
	return m_packet_buffer.back();
}

SRTP_Packet SRTP_OutgoingBuffer::get_packet(int pos){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::get_packet\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::get_packet\n";
	return m_packet_buffer[pos];
}

SRTP_Packet SRTP_OutgoingBuffer::get_packet(int connection_id, int seq_num){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::get_packet\n";


	std::deque<SRTP_Packet>::iterator buffer_iterator;
	for(buffer_iterator = m_packet_buffer.begin(); buffer_iterator != m_packet_buffer.end(); buffer_iterator++){
		SRTP_Packet tmp_packet = *buffer_iterator;
		if(tmp_packet.get_connection_id() == connection_id && tmp_packet.get_seq_num() == seq_num)
		{
			break;
		}
	}
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::get_packet\n";
	return *buffer_iterator;
}

SRTP_Packet SRTP_OutgoingBuffer::get_head_packet(){
	return *m_head;
}

void SRTP_OutgoingBuffer::increment_head(){
	std::deque<SRTP_Packet>::iterator head = m_head;
	SRTP_Packet head_packet = *head;
	if(head_packet.get_seq_num() != m_packet_buffer.back().get_seq_num()){
		m_head++;
	}
	else{
		//m_head = m_packet_buffer.begin();
		out_buf_end = true;
		head_not_inc = true;
	}
	std::clog<<"INFO: incrementing the head. Head pointing to sequence number"<<(*m_head).get_seq_num()<<"\n";
}

void SRTP_OutgoingBuffer::pop_front(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::pop_front\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::pop_front\n";
	m_packet_buffer.pop_front();
}

void SRTP_OutgoingBuffer::pop_back(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::pop_back\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::pop_back\n";
	m_packet_buffer.pop_back();
}

void SRTP_OutgoingBuffer::remove_packet(int connection_id, int seq_num){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::remove_packet\n";
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
	if(m_packet_buffer.size() < m_win_size)
	{
		std::clog<<"INFO:\t Buffer size ("<<m_packet_buffer.size()<< ") less than Max Buffer size ("<<m_win_size<<")\n";

		std::clog<<"INFO:\t Notify to add packet\n";
		pthread_mutex_lock( &buf_max_cv_mutex );
		pthread_cond_signal( &buf_max_cond );
		pthread_mutex_unlock( &buf_max_cv_mutex );
	}
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::remove_packet\n";
}

void SRTP_OutgoingBuffer::increase_size(unsigned int size){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::increase_size\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::increase_size\n";
}

void SRTP_OutgoingBuffer::decrease_size(unsigned int size){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::decrease_size\n";

    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::decrease_size\n";
}

unsigned int SRTP_OutgoingBuffer::get_buffer_size(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::get_buffer_size\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::get_buffer_size\n";
	return m_packet_buffer.size();
}
void SRTP_OutgoingBuffer::set_window_size(int size){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::set_window_size\n";
	std::clog<<"INFO:\t setting window size = "<<size<<"\n";
	m_win_size = size;
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::set_window_size\n";

}

int SRTP_OutgoingBuffer::get_window_size(){
    std::clog<<"ENTER:\t SRTP_OutgoingBuffer::get_window_size\n";
    std::clog<<"EXIT:\t SRTP_OutgoingBuffer::get_window_size\n";
	return m_win_size;
}

