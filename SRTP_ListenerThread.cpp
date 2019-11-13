#include "SRTP_ListenerThread.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_Packet.h"
#include "SRTP_ConnectionTable.h"
#include "SRTP_ConnectionInfo.h"
#include "SRTP_ConnectionStatusTable.h"
#include "SRTP_PeerTable.h"
#include "SRTP_PeerInfo.h"
#include "SRTP_CommonFunctions.h"
#include "SRTP_ReceiverThread.h"
#include "SRTP_SenderThread.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

SRTP_ListenerThread::SRTP_ListenerThread(){
    std::clog<<"ENTER:\t SRTP_ListenerThread::SRTP_ListenerThread\n";
    
    std::clog<<"EXIT:\t SRTP_ListenerThread::SRTP_ListenerThread\n";
}

SRTP_ListenerThread::SRTP_ListenerThread(struct sockaddr_in local_socket_addr, SRTP_IncomingBuffer* in_buf, SRTP_OutgoingBuffer* out_buf){
    std::clog<<"ENTER:\t SRTP_ListenerThread::SRTP_ListenerThread\n";
    m_local_socket_addr = local_socket_addr;
	m_in_buf = in_buf;
	m_out_buf = out_buf;
    std::clog<<"EXIT:\t SRTP_ListenerThread::SRTP_ListenerThread\n";
}

SRTP_ListenerThread::~SRTP_ListenerThread(){
    std::clog<<"ENTER:\t SRTP_ListenerThread::~SRTP_ListenerThread\n";
    stop_listening();
    std::clog<<"EXIT:\t SRTP_ListenerThread::~SRTP_ListenerThread\n";
}

void SRTP_ListenerThread::run(){
    std::clog<<"ENTER:\t SRTP_ListenerThread::run\n";
    start_listening();
    std::clog<<"EXIT:\t SRTP_ListenerThread::run\n";
}

void SRTP_ListenerThread::start_listening(){
    std::clog<<"ENTER:\t SRTP_ListenerThread::start_listening\n";
    
    int socket_id;
    struct sockaddr_in server_addr;
    struct in_addr in;
    
    // Create the socket
    if((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) <0){
        std::clog<<"ERROR:\t Error while creating the socket.\n";
        exit(-1);
    }

    std::clog<<"INFO:\t Socket create successfully with socket id"<<socket_id<<"\n";

    server_addr = m_local_socket_addr;    
    
    // Bind the socket to local address
    if(bind(socket_id, (struct sockaddr*)&server_addr, sizeof(sockaddr)) <0){
        std::clog<<"ERROR:\t Error while binding the socket.";
        exit(-1);
    }


    std::clog<<"INFO:\t Socket with socket id"<<socket_id<<" is successfully binded.\n";
    
 

    std::clog<<"INFO:\t Server listening on IP address "<<ntohl(server_addr.sin_addr.s_addr)<<" and port number "<<ntohs(server_addr.sin_port)<<"\n";

	std::cout<<"INFO:\t Server listening on port: "<<ntohs(server_addr.sin_port)<<"\n";
 
    // Store the connection id in the connection table, conection status table & peer table
    // TODO: SHLD be mutexed
	pthread_mutex_lock(&table_mutex);
	SRTP_ConnectionInfo con_info(1, socket_id, ntohl(server_addr.sin_addr.s_addr), ntohs(server_addr.sin_port), 0, 0);
    SRTP_ConnectionTable::get_connection_table_instance().add_connection_table_entry(1, con_info);
    SRTP_ConnectionStatusTable::get_con_status_table_instance().add_status_table_entry(1, SRTP_LISTENING);
    
    SRTP_PeerInfo local_info(1, SRTP_LISTEN, in.s_addr, server_addr.sin_port, SRTP_INITIAL_WINDOW_SIZE);
    SRTP_PeerTable::get_peer_table_instance().add_peer_table_entry(1, local_info);
	pthread_mutex_unlock(&table_mutex);
    
    SRTP_ReceiverThread* receiver_thread = new SRTP_ReceiverThread(SRTP_CONNECTION_LISTEN, socket_id, server_addr, m_in_buf, m_out_buf);

	SRTP_SenderThread* sender_thread = new SRTP_SenderThread(socket_id, m_out_buf);
    
    receiver_thread->start_thread();
	sender_thread->start_thread();
    receiver_thread->join_thread();
	sender_thread->join_thread();

    std::clog<<"EXIT:\t SRTP_ListenerThread::start_listening\n";
}

void SRTP_ListenerThread::stop_listening(){
    std::clog<<"ENTER:\t SRTP_ListenerThread::start_listening\n";
    
    std::clog<<"EXIT:\t SRTP_ListenerThread::start_listening\n";
}

