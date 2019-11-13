#include "SRTP_IncomingMessageHandler.h"
#include "SRTP_CommonTypes.h"
#include <iostream>

SRTP_IncomingMessageHandler::SRTP_IncomingMessageHandler(SRTP_IncomingBuffer* in_buf_ptr){
    std::clog<<"ENTER:\t SRTP_IncomingMessageHandler::SRTP_IncomingMessageHandler\n";
    m_incoming_buffer_ptr = in_buf_ptr;
    std::clog<<"EXIT:\t SRTP_IncomingMessageHandler::SRTP_IncomingMessageHandler\n";
}

SRTP_IncomingMessageHandler::~SRTP_IncomingMessageHandler(){
    std::clog<<"ENTER:\t SRTP_IncomingMessageHandler::~SRTP_IncomingMessageHandler\n";
    
    std::clog<<"EXIT:\t SRTP_IncomingMessageHandler::~SRTP_IncomingMessageHandler\n";
}

std::string SRTP_IncomingMessageHandler::SRTP_Receive(int connection_num){
    std::clog<<"ENTER:\t SRTP_IncomingMessageHandler::SRTP_Receive\n";

    pthread_mutex_t in_buf_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock( &in_buf_cv_mutex );
    while(m_incoming_buffer_ptr->get_buffer_size() <= 0)
    {
		pthread_cond_wait( &in_buf_cond, &in_buf_cv_mutex );
    }
    pthread_mutex_unlock( &in_buf_cv_mutex );
		
    std::clog<<"INFO:\t Data available in Incoming buffer\n";
    SRTP_Packet packet = m_incoming_buffer_ptr->get_first_packet(connection_num);
    m_incoming_buffer_ptr->remove_packet(connection_num, packet.get_seq_num());
    std::clog<<"EXIT:\t SRTP_IncomingMessageHandler::SRTP_Receive\n";
    return(packet.get_buffer());
}

std::string SRTP_IncomingMessageHandler::SRTP_Receive(int* connection_num){
    std::clog<<"ENTER:\t SRTP_IncomingMessageHandler::SRTP_Receive\n";

    pthread_mutex_t in_buf_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock( &in_buf_cv_mutex );
    while(m_incoming_buffer_ptr->get_buffer_size() <= 0)
    {
		pthread_cond_wait( &in_buf_cond, &in_buf_cv_mutex );
    }
    pthread_mutex_unlock( &in_buf_cv_mutex );
		
    std::clog<<"INFO:\t Data available in Incoming buffer\n";
    SRTP_Packet packet = m_incoming_buffer_ptr->get_first_packet();
    m_incoming_buffer_ptr->remove_packet(packet.get_connection_id(), packet.get_seq_num());
	*connection_num = packet.get_connection_id();
    std::clog<<"EXIT:\t SRTP_IncomingMessageHandler::SRTP_Receive\n";
    return(packet.get_buffer());
}

