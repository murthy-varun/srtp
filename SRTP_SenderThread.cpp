#include "SRTP_SenderThread.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_CommonFunctions.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_ConnectionStatusTable.h"
#include "SRTP_PeerTable.h"
#include "SRTP_ConnectionTable.h"

#include <iostream>
#include <stdio.h>

pthread_mutex_t out_buf_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  out_buf_cond  = PTHREAD_COND_INITIALIZER;

pthread_mutex_t sender_thread_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  sender_thread_cond  = PTHREAD_COND_INITIALIZER;

bool timer_expired = false;
bool head_not_inc = false;
unsigned int num_packets_sent = 0;
unsigned int num_packets_retrans = 0;
unsigned long time_sent;

SRTP_SenderThread::SRTP_SenderThread(){
    std::clog<<"ENTER:\t SRTP_SenderThread::SRTP_SenderThread\n";    
    m_socket_id = 0;    
    std::clog<<"EXIT:\t SRTP_SenderThread::SRTP_SenderThread\n";
}

SRTP_SenderThread::SRTP_SenderThread(int socket_id, SRTP_OutgoingBuffer* out_buf){
    std::clog<<"ENTER:\t SRTP_SenderThread::SRTP_SenderThread\n";    
    m_socket_id = socket_id;
	m_outgoing_buffer = out_buf;
    std::clog<<"EXIT:\t SRTP_SenderThread::SRTP_SenderThread\n";
}

SRTP_SenderThread::~SRTP_SenderThread(){
    std::clog<<"ENTER:\t SRTP_SenderThread::~SRTP_SenderThread\n";
    std::clog<<"EXIT:\t SRTP_SenderThread::~SRTP_SenderThread\n";
}

void SRTP_SenderThread::run(){
    std::clog<<"ENTER:\t SRTP_SenderThread::run\n";
    send();
    std::clog<<"EXIT:\t SRTP_SenderThread::run\n";
}

void SRTP_SenderThread::send(){
    std::clog<<"ENTER:\t SRTP_SenderThread::send\n";

	pthread_mutex_lock( &sender_thread_cv_mutex );
	pthread_cond_signal( &sender_thread_cond);
	pthread_mutex_unlock( &sender_thread_cv_mutex);


    int socket_id;
    struct sockaddr_in peer_addr, local_addr;
    socklen_t addr_len;

	peer_addr.sin_family = AF_INET;
	local_addr.sin_family = AF_INET;

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
	while(m_stop_thread != true){

		std::clog<<"INFO:\t Sender thread. Waiting to send data\n";
		pthread_mutex_lock( &out_buf_cv_mutex );
		while((m_outgoing_buffer->get_buffer_size() <= 0) || (out_buf_end == true))
		{
			pthread_cond_wait( &out_buf_cond, &out_buf_cv_mutex );
		}
		pthread_mutex_unlock( &out_buf_cv_mutex );
	
		if(timer_expired == true)
		{
			m_outgoing_buffer->set_head_to_begin();
			num_packets_retrans++;
			timer_expired = false;
			//m_outgoing_buffer->set_window_size(1);
		}
		/*else if(head_not_inc == true)
		{
			std::clog<<"INFO:\t Head not incremented previously. hence incrementing\n";
			m_outgoing_buffer->increment_head();
			head_not_inc = false;
		}*/

		std::clog<<"INFO:\t Sender thread active. Data available in Outgoing buffer\n";
		if(m_outgoing_buffer->get_buffer_size() > 0)
		{

			// get the packet to be sent from the buffer
			SRTP_Packet outgoing_msg_packet = m_outgoing_buffer->get_head_packet();
			m_outgoing_buffer->increment_head();

			// do not pop-out the message until ack is received
			//m_outgoing_buffer->pop_front();

			local_addr.sin_addr.s_addr = htonl(outgoing_msg_packet.get_source_addr());
			local_addr.sin_port = htons(outgoing_msg_packet.get_source_port());
			peer_addr.sin_addr.s_addr = htonl(outgoing_msg_packet.get_dest_addr());
			peer_addr.sin_port = htons(outgoing_msg_packet.get_dest_port());

			std::clog<<"INFO:\t data message being framed.\n";


		 	int local_connection_num = outgoing_msg_packet.get_connection_id();
			int local_seq_num = outgoing_msg_packet.get_seq_num();
			SRTP_PacketType packet_type = SRTP_DATA;

			// check if the packet is a FIN packet, if yes then connection status tables have to be updated
			if(outgoing_msg_packet.get_packet_type() == SRTP_FIN){
				std::clog<<"INFO:\t FIN message. Initiating the close of the connection\n";
				std::cout<<"\nINFO:\t Sending FIN message. Initiating the close of the connection\n";
				
				pthread_mutex_lock(&table_mutex);
				int temp_exp_packet = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_num).get_expected_packet();
				int temp_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_num).get_cur_seq_num();
		        SRTP_PeerInfo peer_info(local_connection_num, SRTP_FIN_WAIT_1, outgoing_msg_packet.get_dest_addr(), outgoing_msg_packet.get_dest_port(), outgoing_msg_packet.get_window_size());
				peer_info.set_expected_packet(temp_exp_packet);
				peer_info.set_cur_seq_num(temp_seq_num);
		        SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_num, peer_info);

		        SRTP_ConnectionStatusTable::get_con_status_table_instance().modify_status_table_entry(local_connection_num, SRTP_CLOSE_REQ_ACT);
				pthread_mutex_unlock(&table_mutex);

				//change the packet type
				packet_type = SRTP_FIN;
			}


			// Create & send data packet to the peer
			SRTP_PacketStructure outgoing_packet;
			outgoing_packet.connection_id = htons(local_connection_num);
			outgoing_packet.packet_type = htons(packet_type);
			outgoing_packet.source_addr = local_addr.sin_addr.s_addr;
			outgoing_packet.dest_addr = peer_addr.sin_addr.s_addr;
			outgoing_packet.source_port = local_addr.sin_port;
			outgoing_packet.dest_port = peer_addr.sin_port;
			outgoing_packet.seq_num = htonl(local_seq_num);
			outgoing_packet.ack_num = htonl(0);
			outgoing_packet.window_size = htons(outgoing_msg_packet.get_window_size());
			//TODO: checksum
			outgoing_packet.checksum = htons(0);
			outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);

			std::string temp_string = outgoing_msg_packet.get_buffer();
			for(int i=0; i< temp_string.length(); i++){
				outgoing_packet.data[i] = temp_string[i];
			}

			std::clog<<"INFO:\t Sending message to the peer: "<<ntohl(peer_addr.sin_addr.s_addr)<<" "<<ntohs(peer_addr.sin_port)<<" from "<<ntohl(local_addr.sin_addr.s_addr)<<" "<<ntohs(local_addr.sin_port)<<" with socket = "<<m_socket_id<<"\n";

			int bytes_sent = sendto(m_socket_id, (char*)&outgoing_packet, SRTP_HEADER_SIZE+temp_string.length(), 0, (struct sockaddr*)&peer_addr, sizeof(sockaddr));
			if(bytes_sent == -1)
			{
				perror("ERROR: :");
			}
			else
			{
				std::clog<<"Number of bytes sent = "<<bytes_sent<<"\n";
				num_packets_sent++;
			}

		
			std::clog<<"INFO:\t Message sent.\n";
		}
	}
	std::clog<<"EXIT:\t SRTP_SenderThread::send\n";
}
