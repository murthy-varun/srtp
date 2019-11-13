#include "SRTP_OutgoingMessageHandler.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_ConnectionTable.h"
#include "SRTP_PeerTable.h"

#include <iostream>

SRTP_OutgoingMessageHandler::SRTP_OutgoingMessageHandler(SRTP_OutgoingBuffer *out_buf_ptr)
{
    std::clog<<"ENTER:\t SRTP_OutgoingMessageHandler::SRTP_OutgoingMessageHandler\n";
    m_outgoing_buffer_ptr = out_buf_ptr;
	m_error_type = NO_ERROR;
	m_packet_number = 0;
    std::clog<<"EXIT:\t SRTP_OutgoingMessageHandler::SRTP_OutgoingMessageHandler\n";
}

SRTP_OutgoingMessageHandler::~SRTP_OutgoingMessageHandler(){
    std::clog<<"ENTER:\t SRTP_OutgoingMessageHandler::~SRTP_OutgoingMessageHandler\n";
	m_error_type = NO_ERROR;
	m_packet_number = 0;
    std::clog<<"EXIT:\t SRTP_OutgoingMessageHandler::~SRTP_OutgoingMessageHandler\n";
}

int SRTP_OutgoingMessageHandler::SRTP_Send(int connection_num, std::string data)
{
    std::clog<<"ENTER:\t SRTP_OutgoingMessageHandler::SRTP_Send\n";
    
    try
    {
        SRTP_Packet packet;
		int local_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_num).get_cur_seq_num();

		SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_num);
		peer_info.set_cur_seq_num(local_seq_num+1);
        SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(connection_num, peer_info);



        SRTP_ConnectionInfo objconnectinfo = SRTP_ConnectionTable::get_connection_table_instance().get_connection_table_entry(connection_num);
		int local_win_size = SRTP_GlobalVariables::get_global_var_obj_instance().get_window_size();

        packet.set_connection_id(connection_num);
        packet.set_packet_type(SRTP_DATA);
        packet.set_source_addr(objconnectinfo.get_local_addr());
        packet.set_dest_addr(objconnectinfo.get_remote_addr());
        packet.set_source_port(objconnectinfo.get_local_port());
        packet.set_dest_port(objconnectinfo.get_remote_port());
        packet.set_seq_num(local_seq_num);
		packet.set_ack_num(0);
        packet.set_window_size(local_win_size);
		packet.set_checksum(0);
        packet.set_buffer(data);
	timeval tim;
        gettimeofday(&tim, NULL);
        unsigned long time_sent = (tim.tv_sec * 1000000)+tim.tv_usec;
        packet.set_time(time_sent);
        
		// Check if we have to simulate errors
		if(m_packet_number == local_seq_num){
			switch(m_error_type)
			{
				case PACKET_LOSS:
				{
					std::clog<<"WAR:\t Packet has not been added to simulate Packet loss\n";
					break;
				}
				case OUT_OF_ORDER:
				{
					std::clog<<"WAR:\t Packet has been added out of order to simulate out of order\n";
					packet.set_seq_num(local_seq_num+1);
					m_outgoing_buffer_ptr->add_packet(packet);
					break;
				}
				case DUPLICATE:
				{
					std::clog<<"WAR:\t Packet has been added twice out of order to simulate duplicate packet\n";
					m_outgoing_buffer_ptr->add_packet(packet);
					m_outgoing_buffer_ptr->add_packet(packet);
					break;
				}
				case NO_ERROR:
				{
					m_outgoing_buffer_ptr->add_packet(packet);
					break;
				}
			}
		}
		else{

			std::clog<<"INFO:\t adding packet to the buffer: "<<data<<" with seq num: "<<local_seq_num<<" connection id"<<connection_num<<" packet type: "<<packet.get_packet_type()<<"\n";
			m_outgoing_buffer_ptr->add_packet(packet);
		}


		pthread_mutex_lock( &out_buf_cv_mutex );
 		if(m_outgoing_buffer_ptr->get_buffer_size() > 0)
        {
	        std::clog<<"INFO:\t Notifying Sender Thread\n";
			pthread_cond_signal( &out_buf_cond );
		}
		pthread_mutex_unlock( &out_buf_cv_mutex );
	}
   catch(...)
   {
	exit(0);
   }
   std::clog<<"EXIT:\t SRTP_OutgoingMessageHandler::SRTP_Send\n";
}
void SRTP_OutgoingMessageHandler::simulate_error_case(SRTP_SIMULATION_ERROR error_type, int packet_number){
	std::clog<<"ENTER:\t SRTP_OutgoingMessageHandler::simulate_error_case\n";
	std::clog<<"PARAM:\t Error type: "<<error_type<<" packet number: "<<packet_number<<std::endl;
	m_error_type = error_type;
	m_packet_number = packet_number;	
	std::clog<<"EXIT:\t SRTP_OutgoingMessageHandler::simulate_error_case\n";
}
