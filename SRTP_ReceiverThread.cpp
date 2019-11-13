#include "SRTP_ReceiverThread.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_CommonFunctions.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_ConnectionStatusTable.h"
#include "SRTP_PeerTable.h"
#include "SRTP_ConnectionTable.h"
#include "SRTP_ConnectionInfo.h"

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t con_server_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  con_server_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t in_buf_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  in_buf_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t end_con_cv_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  end_con_cond  = PTHREAD_COND_INITIALIZER;

int global_threshold = 0;
int global_no_ack_recd = 0;
int global_no_ack_req = 0;

bool out_buf_end = false;
unsigned int num_packets_recvd = 0;
unsigned int num_acks_recvd = 0;
unsigned int num_acks_sent = 0;
unsigned int num_checksum_errors = 0;
unsigned long delay = 0.0;

SRTP_ReceiverThread::SRTP_ReceiverThread(){
    std::clog<<"ENTER:\t SRTP_ReceiverThreadead::SRTP_ReceiverThread\n";
    m_socket_id = 0;
    std::clog<<"EXIT:\t SRTP_ReceiverThread::SRTP_ReceiverThread\n";
}

SRTP_ReceiverThread::SRTP_ReceiverThread(SRTP_ConnectionType connection_type, int socket_id, struct sockaddr_in local_socket, SRTP_IncomingBuffer* in_buf, SRTP_OutgoingBuffer* out_buf){
    std::clog<<"ENTER:\t SRTP_ReceiverThreadead::SRTP_ReceiverThread\n";
    m_connection_type = connection_type;
    m_socket_id = socket_id;
    m_local_socket = local_socket;
	m_incoming_buffer = in_buf;
	m_outgoing_buffer = out_buf;
    std::clog<<"EXIT:\t SRTP_ReceiverThread::SRTP_ReceiverThread\n";
}

SRTP_ReceiverThread::~SRTP_ReceiverThread(){
    std::clog<<"ENTER:\t SRTP_ReceiverThread::~SRTP_ReceiverThread\n";

    std::clog<<"EXIT:\t SRTP_ReceiverThread::~SRTP_ReceiverThread\n";
}

void SRTP_ReceiverThread::run(){
    std::clog<<"ENTER:\t SRTP_ReceiverThread::run\n";
    receive();
    std::clog<<"EXIT:\t SRTP_ReceiverThread::run\n";
}

void SRTP_ReceiverThread::receive(){
    std::clog<<"ENTER:\t SRTP_ReceiverThread::receive\n";

	(void)signal(SIGALRM,SRTP_CommonFunctions::timer_signal_handler);

    switch(m_connection_type)
    {
        case SRTP_CONNECTION_LISTEN:
            handle_client_msg(); // Runs on Server-Side
            break;
        case SRTP_CONNECTION_OPEN:
            handle_server_msg(); // Runs on Client-Side
            break;
        default:
            std::clog<<"ERROR:\t Invalid connection type. Exiting.\n";
    }

    std::clog<<"EXIT:\t SRTP_ReceiverThread::receive\n";
}

void SRTP_ReceiverThread::handle_client_msg(){
    std::clog<<"ENTER:\t SRTP_ReceiverThread::handle_client_msg\n";

    int bytes_read;
    struct sockaddr_in peer_addr, client_addr;
    socklen_t addr_len;
    char recv_data[1024];

    while(m_stop_thread != true){
		std::clog<<"INFO:\t Server waiting for message\n";
        bytes_read = recvfrom(m_socket_id, recv_data, 1024, 0, (struct sockaddr*)&peer_addr, &addr_len);

	    if(bytes_read > 0)
		{
	        // Process the message.
	        SRTP_PacketStructure* srtp_packet_ptr;
	        SRTP_Packet incoming_message_packet;

	        srtp_packet_ptr = (SRTP_PacketStructure*)recv_data;
		
			std::clog<<"INFO:\t Received message of length = "<<bytes_read<<"\n";
			//std::cout<<"INFO:\t Received message of length = "<<bytes_read<<"\n";

	        // caluculate & verify the checksum
	        if(SRTP_CommonFunctions::get_instance().caluculate_checksum(*srtp_packet_ptr) == 0 )
	        {			
				std::clog<<"INFO:\t Incoming Message = \n"
				<<"connection id: "<<ntohs(srtp_packet_ptr->connection_id)<<"\n"
					<<"packet type: "<<ntohs(srtp_packet_ptr->packet_type)<<"\n"
					<<"source address: "<<ntohl(srtp_packet_ptr->source_addr)<<"\n"
					<<"destination address: "<<ntohl(srtp_packet_ptr->dest_addr)<<"\n"
					<<"source port: "<<ntohs(srtp_packet_ptr->source_port)<<"\n"
					<<"destination port: "<<ntohs(srtp_packet_ptr->dest_port)<<"\n"
					<<"seq number: "<<ntohl(srtp_packet_ptr->seq_num)<<"\n"
					<<"ack number: "<<ntohl(srtp_packet_ptr->ack_num)<<"\n"
					<<"winodow size: "<<ntohs(srtp_packet_ptr->window_size)<<"\n"
					<<"checksum: "<<ntohs(srtp_packet_ptr->checksum)<<"\n"
					<<"data: "<<srtp_packet_ptr->data<<"\n";

			    incoming_message_packet.set_connection_id(ntohs(srtp_packet_ptr->connection_id));
			    incoming_message_packet.set_packet_type((SRTP_PacketType)ntohs(srtp_packet_ptr->packet_type));
			    incoming_message_packet.set_source_addr((SRTP_Address)ntohl(srtp_packet_ptr->source_addr));
			    incoming_message_packet.set_dest_addr((SRTP_Address)ntohl(srtp_packet_ptr->dest_addr));
			    incoming_message_packet.set_source_port((SRTP_Port)ntohs(srtp_packet_ptr->source_port));
			    incoming_message_packet.set_dest_port((SRTP_Port)ntohs(srtp_packet_ptr->dest_port));
			    incoming_message_packet.set_seq_num(ntohl(srtp_packet_ptr->seq_num));
			    incoming_message_packet.set_ack_num(ntohl(srtp_packet_ptr->ack_num));
			    incoming_message_packet.set_window_size(ntohs(srtp_packet_ptr->window_size));
			    incoming_message_packet.set_checksum(ntohs(srtp_packet_ptr->checksum));

			    std::string srtp_data(srtp_packet_ptr->data, bytes_read - SRTP_HEADER_SIZE);
				incoming_message_packet.set_buffer(srtp_data);

			    std::clog<<"INFO:\t Incoming Message Type  = "<<incoming_message_packet.get_packet_type()<<"\n";

				int packet_connection_id = incoming_message_packet.get_connection_id();
				int packet_seq_num = incoming_message_packet.get_seq_num();

				int expected_seq_num;
				// For first packet
				if(packet_connection_id == 0)
				{
					expected_seq_num = 0;
				}
				else
				{
					expected_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(packet_connection_id).get_expected_packet();
				}

std::clog<<"INFO:\t expected seq num: "<<expected_seq_num<<"packet seq num: "<<packet_seq_num<<" packet_connection_id: "<<packet_connection_id<<" \n";

				//check whether data is the expected packet? (Go-back-N)
				if(packet_seq_num == expected_seq_num)
				{
					pthread_mutex_lock(&table_mutex);
					// increment expected Sequence number
					SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(packet_connection_id);
					peer_info.set_expected_packet(expected_seq_num+1);
					SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(packet_connection_id, peer_info);
					pthread_mutex_unlock(&table_mutex);

					// If the SYN bit is set, process the message & send back a response packet with SYN+ACK bit set
					if(incoming_message_packet.get_packet_type() == SRTP_SYN){

						std::cout<<"INFO:\t SYN packet received from ( "<<incoming_message_packet.get_source_addr()<<", "<<incoming_message_packet.get_source_port()<<")\n";
						// TODO: SHLD be mutexed
						
						int local_connection_id = SRTP_GlobalVariables::get_global_var_obj_instance().get_connection_id();
						int local_seq_num = 0;
						int local_window_size = SRTP_GlobalVariables::get_global_var_obj_instance().get_MSS_size();

						// Store the Peer information in the peer table
						pthread_mutex_lock(&table_mutex);
						SRTP_PeerInfo peer_info(local_connection_id, SRTP_SYN_RCVD, incoming_message_packet.get_source_addr(), incoming_message_packet.get_source_port(), incoming_message_packet.get_window_size());
						peer_info.set_expected_packet(1);
						peer_info.set_cur_seq_num(1);
						SRTP_PeerTable::get_peer_table_instance().add_peer_table_entry(local_connection_id, peer_info);

						SRTP_ConnectionStatusTable::get_con_status_table_instance().add_status_table_entry(local_connection_id, SRTP_ATTEMPT_OPEN);
						pthread_mutex_unlock(&table_mutex);

						// create the SYN+ACK packet to send
						SRTP_PacketStructure outgoing_packet;
						outgoing_packet.connection_id = htons(local_connection_id);
						outgoing_packet.packet_type = htons(SRTP_SYN_ACK);
						outgoing_packet.source_addr = htonl(incoming_message_packet.get_dest_addr());
						outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						outgoing_packet.source_port = htons(incoming_message_packet.get_dest_port());
						outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						outgoing_packet.seq_num = htonl(local_seq_num);
						outgoing_packet.ack_num = htonl(incoming_message_packet.get_seq_num());
						outgoing_packet.window_size = htons(local_window_size);
						outgoing_packet.checksum = htons(0);
						outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
						//outgoing_packet.data = (char*)temp_string.c_str();


				client_addr.sin_family = AF_INET;
				client_addr.sin_addr.s_addr = (srtp_packet_ptr->source_addr);
				client_addr.sin_port = (srtp_packet_ptr->source_port);

				std::clog<<"INFO:\t Sending SYN+ACK message to the peer: "<<ntohl(client_addr.sin_addr.s_addr)<<" "<<ntohs(client_addr.sin_port)<<" with socket = "<<m_socket_id<<"\n";


						// send the SYN_ACK packet to the peer
						int bytes_sent = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(sockaddr));
						std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";


						SRTP_GlobalVariables::get_global_var_obj_instance().increment_connection_id();
					}

					// Else if ACK bit is set
					else if(incoming_message_packet.get_packet_type() == SRTP_ACK){
						unsigned int local_connection_id = incoming_message_packet.get_connection_id();

						pthread_mutex_lock(&table_mutex);

						// check if the ACK was for a previously received SYN request
						if(SRTP_SYN_RCVD == SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){
						    //TODO: SHLD be mutexed
						    // Set  the connection status table and the peer table with appropriate value
						std::cout<<"INFO:\t ACK received for SYN+ACK packet"<<"\n";
						    std::clog<<"INFO:\t New Connection Established with conection id: "<<local_connection_id<<"\n";
							std::cout<<"INFO:\t New Connection Established with conection id: "<<local_connection_id<<"\n";

							
						SRTP_ConnectionInfo con_info(local_connection_id, m_socket_id, ntohl(srtp_packet_ptr->dest_addr), ntohs(srtp_packet_ptr->dest_port), ntohl(srtp_packet_ptr->source_addr), ntohs(srtp_packet_ptr->source_port));
						SRTP_ConnectionTable::get_connection_table_instance().add_connection_table_entry(local_connection_id, con_info);
							SRTP_ConnectionStatusTable::get_con_status_table_instance().modify_status_table_entry(local_connection_id, SRTP_ACTIVE_OPEN);
							int temp_exp_packet = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_expected_packet();
							int temp_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
						    SRTP_PeerInfo peer_info(local_connection_id, SRTP_ESTBLSHD, incoming_message_packet.get_source_addr(), incoming_message_packet.get_source_port(), incoming_message_packet.get_window_size());
							peer_info.set_expected_packet(temp_exp_packet);
							peer_info.set_cur_seq_num(temp_seq_num);
						    SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);

							pthread_mutex_unlock(&table_mutex);

							// Notify that connection has been established
							pthread_mutex_lock(&con_server_cv_mutex);
							std::clog<<"INFO: Notifying the listener that a connection has been established"<<SRTP_PeerTable::get_peer_table_instance().get_map_size()<<"\n";
							pthread_cond_signal(&con_server_cond);
							pthread_mutex_unlock(&con_server_cv_mutex);

							
							// start slow-start phase
							m_outgoing_buffer->set_window_size(1);

						}


						// check if the ACK was for a previously sent DATA request
						else if(SRTP_ESTBLSHD == SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){
						    // TODO: Handle ACK for data
							num_acks_recvd++;
							// Resetting the Retransmission timer
							std::clog<<"Starting the Retransmission Timer: "<<alarm(SRTP_RETRANSMISSION_TIME)<<"\n";

							// Check the Ack number and delete it from out buffer
							int local_ack_num = incoming_message_packet.get_ack_num();
							int local_con_num = incoming_message_packet.get_connection_id();

							SRTP_Packet packet = m_outgoing_buffer->get_packet(local_con_num, local_ack_num);
                                                        unsigned long time = packet.get_time();
                                                         timeval tim;
                                                        gettimeofday(&tim, NULL);
                                                        unsigned long time_received = (tim.tv_sec * 1000000)+tim.tv_usec;
                                                        delay = delay + time_received - time;
                                                        std::clog<<"INFO:\t RTT for the packet is :"<< time_received - time << std::endl;

	

							m_outgoing_buffer->remove_packet(local_con_num, local_ack_num);
	
							pthread_mutex_unlock(&table_mutex);
						}
						else if(SRTP_LAST_ACK == SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){
							std::clog<<"INFO:\t Connection closed for connection id: "<<incoming_message_packet.get_connection_id()<<std::endl;

							std::clog<<"INFO:\t Timer has been reset"<<alarm(0)<<" \n";
							//TODO: Clear the tables
						    SRTP_ConnectionTable::get_connection_table_instance().delete_connection_table_entry(local_connection_id);
						    SRTP_ConnectionStatusTable::get_con_status_table_instance().delete_status_table_entry(local_connection_id);

						    SRTP_PeerTable::get_peer_table_instance().delete_peer_table_entry(local_connection_id);
							pthread_mutex_unlock(&table_mutex);

							// Notify that connection has been closed
							pthread_mutex_lock(&end_con_cv_mutex);
							std::clog<<"INFO:\t Notifying the connection close: "<<incoming_message_packet.get_connection_id()<<std::endl;
							pthread_cond_signal( &end_con_cond);
							pthread_mutex_unlock(&end_con_cv_mutex);


						}

						else{
						    std::clog<<"ERROR:\t Invalid ACk received for connection id:"<<local_connection_id<<"\n";
							pthread_mutex_unlock(&table_mutex);
						}
					}
					// Else if FIN bit is set,
					else if(incoming_message_packet.get_packet_type() == SRTP_FIN){
						unsigned int local_connection_id = incoming_message_packet.get_connection_id();
						// check if the connection has been previously established
						if(SRTP_ESTBLSHD != SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){
						    std::clog<<"WAR:\t No previous connection was established from the peer. Hence ignoring\n";
						}
						else
						{
							std::clog<<"INFO:\t FIN message arrived. Initiating close of the Connection\n";
							std::cout<<"INFO:\t FIN message arrived. Initiating close of the Connection\n";
							int local_connection_id = ntohs(srtp_packet_ptr->connection_id);
						    int local_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
							SRTP_PeerInfo peer_information = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id);
							peer_information.set_cur_seq_num(local_seq_num+1);
							SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_information);

						    // Set  the connection status table and the peer table with appropriate value
							pthread_mutex_lock(&table_mutex);
						    SRTP_ConnectionStatusTable::get_con_status_table_instance().modify_status_table_entry(local_connection_id, SRTP_CLOSE_REQ_ACT);

							int temp_exp_packet = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_expected_packet();
							int temp_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
						    SRTP_PeerInfo peer_info(local_connection_id, SRTP_CLOSE_WAIT, incoming_message_packet.get_source_addr(), incoming_message_packet.get_source_port(), incoming_message_packet.get_window_size());
							peer_info.set_expected_packet(temp_exp_packet);
							peer_info.set_cur_seq_num(temp_seq_num);

						    SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);
							pthread_mutex_unlock(&table_mutex);

						    // create the ACK packet to send
						    SRTP_PacketStructure outgoing_packet;
						    outgoing_packet.connection_id = htons(local_connection_id);
						    outgoing_packet.packet_type = htons(SRTP_ACK);
						    outgoing_packet.source_addr = htonl(incoming_message_packet.get_dest_addr());
						    outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						    outgoing_packet.source_port = htons(incoming_message_packet.get_dest_port());
						    outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						    outgoing_packet.seq_num = htonl(local_seq_num);
						    outgoing_packet.ack_num = htonl(incoming_message_packet.get_seq_num());
						    outgoing_packet.window_size = htons(SRTP_INITIAL_WINDOW_SIZE);
						    outgoing_packet.checksum = htons(0);
							outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
						   //outgoing_packet.data = (char*)temp_string.c_str();

						    // send the ACK packet to the peer
							std::clog<<"INFO:\t Sending the ACK for the FIN. Initiating half-close with seq num: "<< local_seq_num<<"\n";
							std::cout<<"INFO:\t Sending the ACK for the FIN. Initiating half-close with seq num: "<< local_seq_num<<"\n";
						    int bytes_sent = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&peer_addr, sizeof(sockaddr));
							std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";



							//sleep(3); // To send all the messages from server
							pthread_mutex_lock(&table_mutex);
						    SRTP_ConnectionStatusTable::get_con_status_table_instance().modify_status_table_entry(local_connection_id, SRTP_CLOSE_REQ_PAS);

							temp_exp_packet = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_expected_packet();
							temp_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_expected_packet();

						    SRTP_PeerInfo peer_inf(local_connection_id, SRTP_LAST_ACK, incoming_message_packet.get_source_addr(), incoming_message_packet.get_source_port(), incoming_message_packet.get_window_size());
							peer_inf.set_expected_packet(temp_exp_packet);
							peer_inf.set_cur_seq_num(temp_seq_num-1);
						    SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_inf);
							pthread_mutex_unlock(&table_mutex);


						    int local_seq_number = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();

							peer_information = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id);
							peer_information.set_cur_seq_num(local_seq_number+1);
							SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_information);
						    // create the FIN packet to send
						    outgoing_packet.connection_id = htons(local_connection_id);
						    outgoing_packet.packet_type = htons(SRTP_FIN);
						    outgoing_packet.source_addr = htonl(incoming_message_packet.get_dest_addr());
						    outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						    outgoing_packet.source_port = htons(incoming_message_packet.get_dest_port());
						    outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						    outgoing_packet.seq_num = htonl(local_seq_number);
						    outgoing_packet.ack_num = htonl(0);
						    outgoing_packet.window_size = htons(SRTP_INITIAL_WINDOW_SIZE);
						    outgoing_packet.checksum = htons(0);
							outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);

						    // send the FIN packet to the peer
							std::clog<<"INFO:\t Sending the FIN packet to the client with seq num seq: "<< local_seq_num<<"\n";
							std::cout<<"INFO:\t Sending the FIN packet to the client seq num seq: "<< local_seq_num<<"\n";
						    bytes_sent = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&peer_addr, sizeof(sockaddr));
							std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";

							std::cout<<"INFO:\t Connection Terminated for connection number:  "<<local_connection_id<<" \n";

						    SRTP_GlobalVariables::get_global_var_obj_instance().increment_sequence_num();
						}
						//TODO: The transition from CLOSE_WAIT to LAST_ACK
					}

					else if(incoming_message_packet.get_packet_type() == SRTP_DATA){
						// TODO: Handle Incoming data

						num_packets_recvd++;

				
					    // Put the incoming packet into the buffer
						m_incoming_buffer->add_packet(incoming_message_packet);

						// Notify that data is being written in Incoming buffer
						if(m_incoming_buffer->get_buffer_size() > 0){
							std::clog<<"INFO:\t Notifying that Incoming buffer has packets\n";
							pthread_cond_signal( &in_buf_cond );
						}

						// Send an Acknowledgement packet back

						int local_connection_id = ntohs(srtp_packet_ptr->connection_id);
						int local_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
						SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id);
						peer_info.set_cur_seq_num(local_seq_num+1);
						SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);

						// create the ACK packet to send
						SRTP_PacketStructure outgoing_packet;
						outgoing_packet.connection_id = htons(local_connection_id);
						outgoing_packet.packet_type = htons(SRTP_ACK);
						outgoing_packet.source_addr = htonl(incoming_message_packet.get_dest_addr());
						outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						outgoing_packet.source_port = htons(incoming_message_packet.get_dest_port());
						outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						outgoing_packet.seq_num = htonl(local_seq_num);
						outgoing_packet.ack_num = htonl(incoming_message_packet.get_seq_num());
						outgoing_packet.window_size = htons(SRTP_INITIAL_WINDOW_SIZE);
						outgoing_packet.checksum = htons(0);
						outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
						//outgoing_packet.data = (char*)temp_string.c_str();


						client_addr.sin_family = AF_INET;
						client_addr.sin_addr.s_addr = (srtp_packet_ptr->source_addr);
						client_addr.sin_port = (srtp_packet_ptr->source_port);

						std::clog<<"INFO:\t Sending ACK message to the peer: "<<ntohl(client_addr.sin_addr.s_addr)<<" "<<ntohs(client_addr.sin_port)<<" with seq num: "<<local_seq_num<<"\n";


						// send the ACK packet to the peer
						int bytes_sent = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(sockaddr));
						std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";
						num_acks_sent++;


						SRTP_GlobalVariables::get_global_var_obj_instance().increment_sequence_num();
					}
					// else ignore
					else {
						std::clog<<"WAR:\t Invalid Packet type received, Ignoring the message.\n";
						std::cout<<"WAR:\t Invalid Packet type received, Ignoring the message.\n";
					}

				}
				else
				{
					std::clog<<"ERROR:\t Packet with unexpected sequence number. Packet dropped: expected: "<<expected_seq_num<<" recvd: "<<packet_seq_num<<"\n";
					std::cout<<"ERROR:\t Packet with unexpected sequence number. Packet dropped: expected: "<<expected_seq_num<<" recvd: "<<packet_seq_num<<" con id: "<<packet_connection_id<<" \n";
				}
			}// end of if
			else
			{
				num_checksum_errors++;
			    std::clog<<"ERROR:\t Checksum failed. Discarding message\n";
				std::cout<<"ERROR:\t Checksum failed. Discarding message\n";
			}
	    }// end of if
    }// end of while
    std::clog<<"EXIT:\t SRTP_ReceiverThread::handle_client_msg\n";
}

void SRTP_ReceiverThread::handle_server_msg(){
    std::clog<<"ENTER:\t SRTP_ReceiverThread::handle_server_msg\n";


    int bytes_read;
    struct sockaddr_in peer_addr, client_addr;
    socklen_t addr_len;
    char recv_data[1024];

    while(m_stop_thread != true){
		std::clog<<"INFO:\t Client waiting for message\n";
		bytes_read = recvfrom(m_socket_id, recv_data, 1024, 0, (struct sockaddr*)&peer_addr, &addr_len);

		if(bytes_read > 0)
		{

			// Process the message.
			SRTP_PacketStructure* srtp_packet_ptr;
			SRTP_Packet incoming_message_packet;

			srtp_packet_ptr = (SRTP_PacketStructure*)recv_data;

			std::clog<<"INFO:\t Received message of length = "<<bytes_read<<"\n";
			//std::cout<<"INFO:\t Received message of length = "<<bytes_read<<"\n";

	        // caluculate & verify the checksum
	        if(SRTP_CommonFunctions::get_instance().caluculate_checksum(*srtp_packet_ptr) == 0 )
	        {
				std::clog<<"INFO:\t Incoming Message = \n"
				<<"connection id: "<<ntohs(srtp_packet_ptr->connection_id)<<"\n"
					<<"packet type: "<<ntohs(srtp_packet_ptr->packet_type)<<"\n"
					<<"source address: "<<ntohl(srtp_packet_ptr->source_addr)<<"\n"
					<<"destination address: "<<ntohl(srtp_packet_ptr->dest_addr)<<"\n"
					<<"source port: "<<ntohs(srtp_packet_ptr->source_port)<<"\n"
					<<"destination port: "<<ntohs(srtp_packet_ptr->dest_port)<<"\n"
					<<"seq number: "<<ntohl(srtp_packet_ptr->seq_num)<<"\n"
					<<"ack number: "<<ntohl(srtp_packet_ptr->ack_num)<<"\n"
					<<"winodow size: "<<ntohs(srtp_packet_ptr->window_size)<<"\n"
					<<"checksum: "<<ntohs(srtp_packet_ptr->checksum)<<"\n"
					<<"data: "<<srtp_packet_ptr->data<<"\n";


				incoming_message_packet.set_connection_id(ntohs(srtp_packet_ptr->connection_id));
				incoming_message_packet.set_packet_type((SRTP_PacketType)ntohs(srtp_packet_ptr->packet_type));
				incoming_message_packet.set_source_addr((SRTP_Address)ntohl(srtp_packet_ptr->source_addr));
				incoming_message_packet.set_dest_addr((SRTP_Address)ntohl(srtp_packet_ptr->dest_addr));
				incoming_message_packet.set_source_port((SRTP_Port)ntohs(srtp_packet_ptr->source_port));
				incoming_message_packet.set_dest_port((SRTP_Port)ntohs(srtp_packet_ptr->dest_port));
				incoming_message_packet.set_seq_num(ntohl(srtp_packet_ptr->seq_num));
				incoming_message_packet.set_ack_num(ntohl(srtp_packet_ptr->ack_num));
				incoming_message_packet.set_window_size(ntohs(srtp_packet_ptr->window_size));
				incoming_message_packet.set_checksum(ntohs(srtp_packet_ptr->checksum));

			    std::string srtp_data(srtp_packet_ptr->data, bytes_read - SRTP_HEADER_SIZE);
				incoming_message_packet.set_buffer(srtp_data);

				std::clog<<"INFO:\t Incoming Message Type  = "<<incoming_message_packet.get_packet_type()<<"\n";

				int packet_connection_id = incoming_message_packet.get_connection_id();
				int packet_seq_num = incoming_message_packet.get_seq_num();
				int expected_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(packet_connection_id).get_expected_packet();

std::clog<<"INFO:\t expected seq num: "<<expected_seq_num<<"packet seq num: "<<packet_seq_num<<" packet_connection_id: "<<packet_connection_id<<" \n";

				//check whether data is the expected packet? (Go-back-N)
				if(packet_seq_num == expected_seq_num)
				{
					// increment expected Sequence number
					SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(packet_connection_id);
					peer_info.set_expected_packet(expected_seq_num+1);
					SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(packet_connection_id, peer_info);

					// if SYN+ACK bit is set
					if(incoming_message_packet.get_packet_type() == SRTP_SYN_ACK){

						std::cout<<"INFO:\t Received SYN+ACK message from the server\n";
						unsigned int local_connection_id = incoming_message_packet.get_connection_id();
						int local_seq_num = 1;

						// Agree on window size (minimum of the advertised windows)
						int packet_window_size = incoming_message_packet.get_window_size();
						int adv_buffer_size = SRTP_GlobalVariables::get_global_var_obj_instance().get_window_size();
						int local_window_size;
						if(packet_window_size < adv_buffer_size){
							local_window_size = packet_window_size;
						}else{
							local_window_size = adv_buffer_size;
						}
						global_threshold = local_window_size/2;

						// start slow-start phase
						m_outgoing_buffer->set_window_size(1);
						std::cout<<"INFO:\t In Slow-start phase\n";

						// create and send ACK packet
						SRTP_PacketStructure outgoing_packet;
						outgoing_packet.connection_id = htons(local_connection_id);
						outgoing_packet.packet_type = htons(SRTP_ACK);
						outgoing_packet.source_addr = (m_local_socket.sin_addr.s_addr);
						outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						outgoing_packet.source_port = (m_local_socket.sin_port);
						outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						outgoing_packet.seq_num = htonl(local_seq_num);
						outgoing_packet.ack_num = htonl(incoming_message_packet.get_seq_num());
						outgoing_packet.window_size = htons(local_window_size);
						outgoing_packet.checksum = htons(0);
						outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
						//outgoing_packet.data = (char*)temp_string.c_str();

						client_addr.sin_family = AF_INET;
						client_addr.sin_addr.s_addr = (srtp_packet_ptr->source_addr);
						client_addr.sin_port = (srtp_packet_ptr->source_port);

						std::clog<<"INFO:\t Sending ACK message to the peer: "<<ntohl(client_addr.sin_addr.s_addr)<<" "<<ntohs(client_addr.sin_port)<<" from "<<ntohl(m_local_socket.sin_addr.s_addr)<<" "<<ntohs(m_local_socket.sin_port)<<" with socket = "<<m_socket_id<<"\n";
						std::cout<<"INFO:\t Sending ACK message to the server\n";


						// send the ACK packet to the peer
						int bytes_sent = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(sockaddr));
						std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";

						SRTP_GlobalVariables::get_global_var_obj_instance().increment_sequence_num();

						// replace the connection id in connection table, status table and peer table
						pthread_mutex_lock(&table_mutex);
						unsigned int temp_sock_id = SRTP_ConnectionTable::get_connection_table_instance().get_connection_table_entry(1).get_socket_id();
						SRTP_ConnectionTable::get_connection_table_instance().delete_connection_table_entry(1);

						SRTP_ConnectionInfo con_info(local_connection_id, temp_sock_id, ntohl(m_local_socket.sin_addr.s_addr), ntohs(m_local_socket.sin_port), ntohl(client_addr.sin_addr.s_addr), ntohs(client_addr.sin_port));
						SRTP_ConnectionTable::get_connection_table_instance().add_connection_table_entry(local_connection_id, con_info);

						SRTP_ConnectionStatusTable::get_con_status_table_instance().delete_status_table_entry(1);
						SRTP_ConnectionStatusTable::get_con_status_table_instance().add_status_table_entry(local_connection_id, SRTP_ACTIVE_OPEN);

						SRTP_PeerTable::get_peer_table_instance().delete_peer_table_entry(1);
						SRTP_PeerInfo local_info(local_connection_id, SRTP_ESTBLSHD, m_local_socket.sin_addr.s_addr, m_local_socket.sin_port, local_window_size);
						local_info.set_expected_packet(1);
						local_info.set_cur_seq_num(2);

						SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, local_info);

						SRTP_ConnectionStatus status = SRTP_ConnectionStatusTable::get_con_status_table_instance().get_status_table_entry(local_connection_id);
						pthread_mutex_unlock(&table_mutex);

						// Notify that connection has been established					
						if(status == SRTP_ACTIVE_OPEN){
							std::clog<<"INFO:\t Notify that conection has been established\n";
							pthread_cond_signal( &condition_cond );
							std::clog<<"INFO:\t After Notification\n";
						}
						std::cout<<"INFO:\t Connection has been established with connection number: "<<local_connection_id<<"\n";

					}
					// Else if ACK bit is set
					else if(incoming_message_packet.get_packet_type() == SRTP_ACK){

						unsigned int local_connection_id = incoming_message_packet.get_connection_id();
						pthread_mutex_lock(&table_mutex);
						// check if the ACK was for a previously received FIN request
						if(SRTP_FIN_WAIT_1 == SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){

							std::clog<<"INFO:\t Half close. No more packets will be sent\n";
							std::cout<<"INFO:\t ACK received for previous FIN. Half close. No more packets will be sent\n";

							// Change the connection status tables
							int temp_exp_packet = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_expected_packet();
							int temp_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
							SRTP_PeerInfo peer_info(local_connection_id, SRTP_FIN_WAIT_2, incoming_message_packet.get_source_addr(), incoming_message_packet.get_source_port(), incoming_message_packet.get_window_size());
							peer_info.set_expected_packet(temp_exp_packet);
							peer_info.set_cur_seq_num(temp_seq_num);
							SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);

							// remove the packet
							int local_ack_num = incoming_message_packet.get_ack_num();
							int local_con_num = incoming_message_packet.get_connection_id();

							std::clog<<"INFO:\t FIN Packet being removed from Outgoing stack: connection id: "<<local_con_num<<"Sequence number: "<<local_ack_num<<"\n";
							m_outgoing_buffer->remove_packet(local_con_num, local_ack_num);

							// reset alarm
							std::clog<<"INFO:\t Timer has been reset"<<alarm(0)<<" \n";

							pthread_mutex_unlock(&table_mutex);

						}

						else if(SRTP_FIN_WAIT_2 == SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){

				
							//TODO: SHLD be mutexed
							// Set  the connection status table and the peer table with appropriate value
							SRTP_ConnectionStatusTable::get_con_status_table_instance().modify_status_table_entry(local_connection_id, SRTP_NO_CONNECTION);

							int temp_exp_packet = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_expected_packet();
							int temp_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();

							SRTP_PeerInfo peer_info(local_connection_id, SRTP_CLOSED, incoming_message_packet.get_source_addr(), incoming_message_packet.get_source_port(), incoming_message_packet.get_window_size());
							peer_info.set_expected_packet(temp_exp_packet);
							peer_info.set_cur_seq_num(temp_seq_num);
							SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);

							std::clog<<"INFO:\t Connection closed for conection id:"<<local_connection_id<<"\n";
							std::cout<<"INFO:\t Connection closed for conection id:"<<local_connection_id<<"\n";
							pthread_mutex_unlock(&table_mutex);
						}

						// check if the ACK was for a previously sent DATA request
						else if(SRTP_ESTBLSHD == SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()){
							// TODO: Handle ACK for data

							num_acks_recvd++;
							global_no_ack_recd++;

							// congestion control mechanism
							int cwnd = m_outgoing_buffer->get_window_size();
							int adv_buffer_size = SRTP_GlobalVariables::get_global_var_obj_instance().get_window_size();
							global_no_ack_req = cwnd;

							if(global_threshold >= cwnd)
							{
								std::clog<<"INFO: In Slow-start phase\n";
								cwnd+=1;
								if(cwnd <= adv_buffer_size){
									m_outgoing_buffer->set_window_size(cwnd);
								}
								else{
									m_outgoing_buffer->set_window_size(adv_buffer_size);
								}
							}
							else
							{
								std::clog<<"INFO: In Congestion avoidance phase\n";
								if(global_no_ack_req == global_no_ack_recd){
									global_no_ack_recd = 0;
									cwnd+=1;

									if(cwnd <= adv_buffer_size){
										m_outgoing_buffer->set_window_size(cwnd);
									}
									else{
										m_outgoing_buffer->set_window_size(adv_buffer_size);
									}
								}
							}

							// Resetting the Retransmission timer
							std::clog<<"Starting the Retransmission Timer: "<<alarm(SRTP_RETRANSMISSION_TIME)<<"\n";

							int local_ack_num = incoming_message_packet.get_ack_num();
							int local_con_num = incoming_message_packet.get_connection_id();

SRTP_Packet packet = m_outgoing_buffer->get_packet(local_con_num, local_ack_num);
                                                        unsigned long time = packet.get_time();
                                                         timeval tim;
                                                        gettimeofday(&tim, NULL);
                                                        unsigned long time_received = (tim.tv_sec * 1000000)+tim.tv_usec;
                                                        delay = delay + time_received - time;

                                                        std::clog<<"INFO:\t RTT for the packet is :"<< time_received - time << std::endl;
							std::clog<<"INFO:\t Packet being removed from Outgoing stack: connection id: "<<local_con_num<<"Sequence number: "<<local_ack_num<<"\n";
							m_outgoing_buffer->remove_packet(local_con_num, local_ack_num);


							pthread_mutex_unlock(&table_mutex);
						}

						else{
							std::clog<<"INFO:\t SRTP_ReceiverThread: Invalid ACk received for connection id:"<<local_connection_id<<"\n";
							std::clog<<"INFO:\t :Status "<<SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status()<<"\n";
							pthread_mutex_unlock(&table_mutex);
						}
					}

					else if(incoming_message_packet.get_packet_type() == SRTP_DATA){
						// TODO: Handle Incoming data

						num_packets_recvd++;

							
				        // Put the incoming packet into the buffer
						m_incoming_buffer->add_packet(incoming_message_packet);

						// Notify that data is being written in Incoming buffer
						if(m_incoming_buffer->get_buffer_size() > 0){
							std::clog<<"INFO:\t Notifying that Incoming buffer has packets\n";
							pthread_cond_signal( &in_buf_cond );
						}

						// Send an Acknowledgement packet back
						int local_connection_id = ntohs(srtp_packet_ptr->connection_id);
						int local_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
						SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id);
						peer_info.set_cur_seq_num(local_seq_num+1);
						SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);

						// create the ACK packet to send
						SRTP_PacketStructure outgoing_packet;
						outgoing_packet.connection_id = htons(local_connection_id);
						outgoing_packet.packet_type = htons(SRTP_ACK);
						outgoing_packet.source_addr = htonl(incoming_message_packet.get_dest_addr());
						outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						outgoing_packet.source_port = htons(incoming_message_packet.get_dest_port());
						outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						outgoing_packet.seq_num = htonl(local_seq_num);
						outgoing_packet.ack_num = htonl(incoming_message_packet.get_seq_num());
						outgoing_packet.window_size = htons(SRTP_INITIAL_WINDOW_SIZE);
						outgoing_packet.checksum = htons(0);
						outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
						//outgoing_packet.data = (char*)temp_string.c_str();


						client_addr.sin_family = AF_INET;
						client_addr.sin_addr.s_addr = (srtp_packet_ptr->source_addr);
						client_addr.sin_port = (srtp_packet_ptr->source_port);

						std::clog<<"INFO:\t Sending ACK message to the peer: "<<ntohl(client_addr.sin_addr.s_addr)<<" "<<ntohs(client_addr.sin_port)<<" with seq num: "<<local_seq_num<<"\n";


						// send the ACK packet to the peer
						int bytes_sent  = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(sockaddr));
						num_acks_sent++;
						std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";

						SRTP_GlobalVariables::get_global_var_obj_instance().increment_sequence_num();

					}
					// If FIN packet arrives from the server
					else if(incoming_message_packet.get_packet_type() == SRTP_FIN){
						unsigned int local_connection_id = incoming_message_packet.get_connection_id();

						// check if the previous state was FIN_WAIT_2
						if( SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_peer_status() == SRTP_FIN_WAIT_2){
							// Send an ACK back to the server
						std::cout<<"INFO:\t FIN received from server.\n";
						unsigned int local_connection_id = incoming_message_packet.get_connection_id();
						int local_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id).get_cur_seq_num();
						SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(local_connection_id);
						peer_info.set_cur_seq_num(local_seq_num+1);
						SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(local_connection_id, peer_info);

						// create and send ACK packet
						SRTP_PacketStructure outgoing_packet;
						outgoing_packet.connection_id = htons(local_connection_id);
						outgoing_packet.packet_type = htons(SRTP_ACK);
						outgoing_packet.source_addr = (m_local_socket.sin_addr.s_addr);
						outgoing_packet.dest_addr = htonl(incoming_message_packet.get_source_addr());
						outgoing_packet.source_port = (m_local_socket.sin_port);
						outgoing_packet.dest_port = htons(incoming_message_packet.get_source_port());
						outgoing_packet.seq_num = htonl(local_seq_num);
						outgoing_packet.ack_num = htonl(incoming_message_packet.get_seq_num());
						outgoing_packet.window_size = htons(SRTP_INITIAL_WINDOW_SIZE);
						outgoing_packet.checksum = htons(0);
						outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
						//outgoing_packet.data = (char*)temp_string.c_str();

						client_addr.sin_family = AF_INET;
						client_addr.sin_addr.s_addr = (srtp_packet_ptr->source_addr);
						client_addr.sin_port = (srtp_packet_ptr->source_port);

					std::clog<<"INFO:\t Sending ACK message to the Server: "<<ntohl(client_addr.sin_addr.s_addr)<<" "<<ntohs(client_addr.sin_port)<<" from "<<ntohl(m_local_socket.sin_addr.s_addr)<<" "<<ntohs(m_local_socket.sin_port)<<" with socket = "<<m_socket_id<<"\n";

					std::cout<<"INFO:\t Sending ACK message to the Server: "<<local_seq_num<<"\n";


						// send the ACK packet to the peer
						int bytes_sent = sendto(m_socket_id, (char*)(&outgoing_packet), SRTP_HEADER_SIZE, 0, (struct sockaddr*)&client_addr, sizeof(sockaddr));
						std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";

						SRTP_GlobalVariables::get_global_var_obj_instance().increment_sequence_num();

						// Close the socket
						std::clog<<"INFO: \t Connection terminated\n";
						std::cout<<"INFO: \t Connection terminated\n";

						// Run-time statistics
						std::cout<<"\n--------------------------------------\n";						
						std::cout<<"Run Time Statistics\n";
						std::cout<<"--------------------------------------\n";
						std::cout<<"Number of packets sent: "<<num_packets_sent<<"\n";
						std::cout<<"Number of packets received: "<<num_packets_recvd<<"\n";
						std::cout<<"Number of acknowledgements sent: "<<num_acks_sent<<"\n";
						std::cout<<"Number of acknowledgements received: "<<num_acks_recvd<<"\n";
						std::cout<<"Number of packets retransmitted: "<<num_packets_retrans<<"\n";
						std::cout<<"Number of checksum errors: "<<num_checksum_errors<<"\n";

						std::cout<<"Average delay of packets: "<<((float)delay/num_acks_recvd)<<" microseconds\n";

						std::cout<<"Data rate = "<<(SRTP_GlobalVariables::get_global_var_obj_instance().get_MSS_size()) /((float)delay/num_acks_recvd) * 1000 * 8 << "Kbps" <<std::endl;
						std::cout<<"--------------------------------------\n\n";

						exit(-1);
						//close(m_socket_id);


						}
					}
					// else ignore
					else {
						std::clog<<"INFO:\t SRTP_ReceiverThread: Invalid Packet type received, Ignoring the message.\n";
					}
				}
				else{
					std::clog<<"ERROR:\t Packet with unexpected sequence number. Packet dropped: expected: "<<expected_seq_num<<" recvd: "<<packet_seq_num<<"\n";
					std::cout<<"ERROR:\t Packet with unexpected sequence number. Packet dropped: expected: "<<expected_seq_num<<" recvd: "<<packet_seq_num<<"\n";
				}
			}// end of if
			else
			{
				num_checksum_errors++;
			    std::clog<<"ERROR:\t Checksum failed. Discarding message\n";
			}

		}//end of if
    }//end of while
    std::clog<<"EXIT:\t SRTP_ReceiverThread::handle_server_msg\n";
}


