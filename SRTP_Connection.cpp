#include "SRTP_Connection.h"
#include "SRTP_CommonTypes.h"
#include "SRTP_ConnectorThread.h"
#include "SRTP_ListenerThread.h"
#include "SRTP_ConnectionStatusTable.h"
#include "SRTP_ConnectionTable.h"
#include "SRTP_PeerTable.h"
#include "SRTP_GlobalVariables.h"

#include <iostream>
#include <stdint.h>
#include <errno.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

SRTP_Connection::SRTP_Connection(){
    std::clog<<"ENTER:\t SRTP_Connection::SRTP_Connection\n";
    m_local_addr = " ";
    m_local_port = 0;
    m_incoming_buffer_ptr;
    m_outgoing_buffer_ptr;
    m_remote_addr = " ";
    m_remote_port = 0;
    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Connection\n";
}

SRTP_Connection::SRTP_Connection(std::string localaddr, SRTP_Port localport, SRTP_IncomingBuffer* in_buffer_ptr, SRTP_OutgoingBuffer* out_buffer_ptr,std::string remoteaddr,SRTP_Port remoteport)
{
    std::clog<<"ENTER:\t SRTP_Connection::SRTP_Connection\n";
    m_local_addr = localaddr;
    m_local_port = localport;
    m_incoming_buffer_ptr = in_buffer_ptr;
    m_outgoing_buffer_ptr = out_buffer_ptr;
    m_remote_addr = remoteaddr;
    m_remote_port = remoteport;  
    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Connection\n";
}

SRTP_Connection::~SRTP_Connection(){
    std::clog<<"ENTER:\t SRTP_Connection::~SRTP_Connection\n";
    
    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Connection\n";
}

int SRTP_Connection::SRTP_Listen(){
    std::clog<<"ENTER:\t SRTP_Connection::SRTP_Listen\n";
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_local_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    SRTP_ListenerThread* listener_thread = new SRTP_ListenerThread(server_addr, m_incoming_buffer_ptr, m_outgoing_buffer_ptr);

    listener_thread->start_thread();

    return(1);
 
    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Listen\n";
}

int SRTP_Connection::SRTP_Open()
{
    std::clog<<"ENTER:\t SRTP_Connection::SRTP_Open\n";
    
    struct sockaddr_in server_addr, client_addr;
    uint32_t local_host, remote_host;    
    std::clog<<m_local_addr<<" "<<m_local_port<<" "<<m_remote_addr<<" "<<m_remote_port<<"\n";
    
	if(1 != inet_pton(AF_INET, m_local_addr.c_str(), &local_host))
	{
		std::clog<<"ERROR:\t Error while converting the IP address. Error number: "<<errno<<std::endl;
		exit(-1);
	}
	if(1 != inet_pton(AF_INET, m_remote_addr.c_str(), &remote_host))
	{
		std::clog<<"ERROR:\t Error while converting the IP address. Error number: "<<errno<<std::endl;
		exit(-1);
	}

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_remote_port);
    server_addr.sin_addr.s_addr = remote_host;

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(m_local_port);
    client_addr.sin_addr.s_addr = local_host;

    m_connector_thread = new SRTP_ConnectorThread(server_addr, client_addr, m_incoming_buffer_ptr,m_outgoing_buffer_ptr);
    m_connector_thread->start_thread();
    
    pthread_mutex_lock( &condition_mutex );
    
    int connection_number = SRTP_ConnectionTable::get_connection_table_instance().get_first_connection();

    while( SRTP_ConnectionStatusTable::get_con_status_table_instance().get_status_table_entry(connection_number) != SRTP_ACTIVE_OPEN )
    {
	std::clog<<"INFO:\t Waiting for Connection to establish \n";			
	pthread_cond_wait( &condition_cond, &condition_mutex );
    }
    pthread_mutex_unlock( &condition_mutex );
   
    pthread_mutex_lock( &sender_thread_cv_mutex );
	std::clog<<"INFO:\t Waiting for Sender Thread to Start \n";			
	pthread_cond_wait( &sender_thread_cond, &sender_thread_cv_mutex );
    pthread_mutex_unlock( &sender_thread_cv_mutex );

    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Open\n";
    return(connection_number);
}

int SRTP_Connection::SRTP_Close(int connection_num){
    std::clog<<"ENTER:\t SRTP_Connection::SRTP_Close\n";
    SRTP_Packet packet;
	int local_seq_num = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_num).get_cur_seq_num();
	SRTP_PeerInfo peer_info = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_num);
	peer_info.set_cur_seq_num(local_seq_num+1);
	SRTP_PeerTable::get_peer_table_instance().modify_peer_table_entry(connection_num, peer_info);

    packet.set_connection_id(connection_num);
    packet.set_packet_type(SRTP_FIN);
    SRTP_ConnectionInfo objconnectinfo = SRTP_ConnectionTable::get_connection_table_instance().get_connection_table_entry(connection_num);
  
    packet.set_source_addr(objconnectinfo.get_local_addr());
    packet.set_dest_addr(objconnectinfo.get_remote_addr());
    packet.set_source_port(objconnectinfo.get_local_port());
    packet.set_dest_port(objconnectinfo.get_remote_port());
    packet.set_seq_num(local_seq_num);
    packet.set_ack_num(0);

    packet.set_checksum(0);
    m_outgoing_buffer_ptr->add_packet(packet);
    std::clog<<"INFO:\t Notifying Sender Thread\n";
    if(m_outgoing_buffer_ptr->get_buffer_size() > 0)
    {
	pthread_cond_signal( &out_buf_cond );
    }
    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Close\n";
}

int SRTP_Connection::SRTP_Status(int connection_num){
    std::clog<<"ENTER:\t SRTP_Connection::SRTP_Status\n";
    
    std::clog<<"EXIT:\t SRTP_Connection::SRTP_Status\n";
}
void SRTP_Connection::join_connector_thread(){
	m_connector_thread->join_thread();
}

void SRTP_Connection::wait_for_end_connection(int connection_num){
    std::clog<<"ENTER:\t SRTP_Connection::wait_for_end_connection\n";

    pthread_mutex_lock( &end_con_cv_mutex );
	//while(SRTP_LAST_ACK != SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_num).get_peer_status())
	{
		std::clog<<"INFO:\t Waiting for connection with connection number to close: "<<connection_num<<" \n";
		std::cout<<"INFO:\t Waiting for connection with connection number to close: "<<connection_num<<" \n";
		pthread_cond_wait( &end_con_cond, &end_con_cv_mutex);
		
    std::clog<<"EXIT:\t SRTP_Connection::wait_for_end_connection\n";
	}
    pthread_mutex_unlock( &end_con_cv_mutex );
	std::clog<<"INFO:\t Connection with connection number closed"<<connection_num<<" \n";
}

