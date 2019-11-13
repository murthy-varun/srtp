#include "SRTP_ConnectorThread.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_CommonFunctions.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_ReceiverThread.h"
#include "SRTP_SenderThread.h"
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

SRTP_ConnectorThread::SRTP_ConnectorThread(){
    std::clog<<"ENTER:\t SRTP_ConnectorThread::SRTP_ConnectorThread\n";
    std::clog<<"EXIT:\t SRTP_ConnectorThread::SRTP_ConnectorThread\n";
}

SRTP_ConnectorThread::SRTP_ConnectorThread(struct sockaddr_in remote_socket_addr, struct sockaddr_in local_socket_addr, SRTP_IncomingBuffer* in_buf, SRTP_OutgoingBuffer* out_buf){
    std::clog<<"ENTER:\t SRTP_ConnectorThread::SRTP_ConnectorThread\n";
    m_remote_socket_addr = remote_socket_addr;
    m_local_socket_addr = local_socket_addr;
	m_in_buf = in_buf;
	m_out_buf = out_buf;
    std::clog<<"EXIT:\t SRTP_ConnectorThread::SRTP_ConnectorThread\n";
}

SRTP_ConnectorThread::~SRTP_ConnectorThread(){
    std::clog<<"ENTER:\t SRTP_ConnectorThread::~SRTP_ConnectorThread\n";
    close_connection();
    std::clog<<"EXIT:\t SRTP_ConnectorThread::~SRTP_ConnectorThread\n";
}

void SRTP_ConnectorThread::run(){
    std::clog<<"ENTER:\t SRTP_ConnectorThread::run\n";
    open_connection();
    std::clog<<"EXIT:\t SRTP_ConnectorThread::run\n";
}

void SRTP_ConnectorThread::open_connection(){
    std::clog<<"ENTER:\t SRTP_ConnectorThread::open_connection\n";
    
    int socket_id;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    
    server_addr = m_remote_socket_addr;
    client_addr	= m_local_socket_addr;

    // Create the socket
    if((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        std::clog<<"ERROR:\t Error while creating the socket.\n";
        exit(-1);
    }

    // Bind the socket to local address
    if(bind(socket_id, (struct sockaddr*)&client_addr, sizeof(sockaddr)) == -1){
        std::clog<<"ERROR:\t Error while binding the socket. Error number ="<<errno<<"\n";
        exit(-1);
    }
   
	int local_window_size = SRTP_GlobalVariables::get_global_var_obj_instance().get_MSS_size();
 
    std::clog<<"INFO:\t SYN message being framed.\n";
 
    // Create & send SYN packet to the server
    SRTP_PacketStructure outgoing_packet;
    outgoing_packet.connection_id = 0;
    outgoing_packet.packet_type = htons(SRTP_SYN);
    outgoing_packet.source_addr = (client_addr.sin_addr.s_addr);
    outgoing_packet.dest_addr = (server_addr.sin_addr.s_addr);
    outgoing_packet.source_port = (client_addr.sin_port);
    outgoing_packet.dest_port = (server_addr.sin_port);
    outgoing_packet.seq_num = htonl(0);
    outgoing_packet.ack_num = htonl(0);
    outgoing_packet.window_size = htons(local_window_size);
    //TODO: checksum
    outgoing_packet.checksum = htons(0);
	outgoing_packet.checksum = SRTP_CommonFunctions::get_instance().caluculate_checksum(outgoing_packet);
    //outgoing_packet.data = (char*)temp_data.c_str();
   
    std::clog<<"INFO:\t Sending SYN message to the peer: "<<ntohl(server_addr.sin_addr.s_addr)<<" "<<ntohs(server_addr.sin_port)<<" from "<<ntohl(m_local_socket_addr.sin_addr.s_addr)<<" "<<ntohs(m_local_socket_addr.sin_port)<<" with socket = "<<socket_id<<"\n";

	std::cout<<"INFO:\t Sending SYN message to the peer: "<<ntohl(server_addr.sin_addr.s_addr)<<" "<<ntohs(server_addr.sin_port)<<" from "<<ntohl(m_local_socket_addr.sin_addr.s_addr)<<" "<<ntohs(m_local_socket_addr.sin_port)<<" with socket = "<<socket_id<<"\n";

    int bytes_sent = sendto(socket_id, (char*)&outgoing_packet, SRTP_HEADER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(sockaddr));
	std::clog<<"INFO:\t Number of bytes sent = "<<bytes_sent<<"\n";

    
    std::clog<<"INFO:\t SYN message sent.\n";
      
    // Store the connection id in the connection table, conection status table & peer table
    // TODO: SHLD be mutexed
	pthread_mutex_lock(&table_mutex);
	SRTP_ConnectionInfo con_info(1, socket_id, ntohl(m_local_socket_addr.sin_addr.s_addr), ntohs(m_local_socket_addr.sin_port), ntohl(server_addr.sin_addr.s_addr), ntohs(server_addr.sin_port));
    SRTP_ConnectionTable::get_connection_table_instance().add_connection_table_entry(1, con_info);
    SRTP_ConnectionStatusTable::get_con_status_table_instance().add_status_table_entry(1, SRTP_ATTEMPT_OPEN);
    
    SRTP_PeerInfo local_info(1, SRTP_SYN_SENT, client_addr.sin_addr.s_addr, client_addr.sin_port, local_window_size);
	local_info.set_expected_packet(0);
	local_info.set_cur_seq_num(1);
    SRTP_PeerTable::get_peer_table_instance().add_peer_table_entry(1, local_info);
	pthread_mutex_unlock(&table_mutex);

    std::clog<<"INFO:\t Creating Receiver Thread.\n";
    
    SRTP_ReceiverThread* receiver_thread = new SRTP_ReceiverThread(SRTP_CONNECTION_OPEN, socket_id, client_addr, m_in_buf, m_out_buf);

	SRTP_SenderThread* sender_thread = new SRTP_SenderThread(socket_id, m_out_buf);

    receiver_thread->start_thread();
    sender_thread->start_thread();
    receiver_thread->join_thread();
    sender_thread->join_thread();

    std::clog<<"EXIT:\t SRTP_ConnectorThread::open_connection\n";
}

void SRTP_ConnectorThread::close_connection(){
    std::clog<<"ENTER:\t SRTP_ConnectorThread::close_connection\n";
    
    std::clog<<"EXIT:\t SRTP_ConnectorThread::close_connection\n";
}
