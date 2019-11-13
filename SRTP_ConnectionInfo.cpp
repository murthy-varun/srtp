#include "SRTP_CommonTypes.h"
#include "SRTP_ConnectionInfo.h"

SRTP_ConnectionInfo::SRTP_ConnectionInfo(){
	m_connection_num = 0;
	m_socket_id = 0;
	m_local_addr = 0;
	m_local_port = 0;
	m_remote_addr = 0;
	m_remote_port = 0;
}

SRTP_ConnectionInfo::SRTP_ConnectionInfo(int connection_num, 
					int socket_id, 
					int local_addr,
					int local_port,
					int remote_addr,
					int remote_port)
{
	m_connection_num = connection_num;
	m_socket_id = socket_id;
	m_local_addr = local_addr;
	m_local_port = local_port;
	m_remote_addr = remote_addr;
	m_remote_port = remote_port;
}

SRTP_ConnectionInfo::~SRTP_ConnectionInfo(){}

void SRTP_ConnectionInfo::set_connection_num(int connection_num){
	m_connection_num = connection_num;
}

void SRTP_ConnectionInfo::set_socket_id(int socket_id){
	m_socket_id = socket_id;
}

void SRTP_ConnectionInfo::set_local_addr(int local_addr){
	m_local_addr = local_addr;
}

void SRTP_ConnectionInfo::set_local_port(int local_port){
	m_local_port = local_port;
}

void SRTP_ConnectionInfo::set_remote_addr(int remote_addr){
	m_remote_addr = remote_addr;
}

void SRTP_ConnectionInfo::set_remote_port(int remote_port){
	m_remote_port = remote_port;
}

int SRTP_ConnectionInfo::get_connection_num(){
	return m_connection_num;
}

int SRTP_ConnectionInfo::get_socket_id(){
	return m_socket_id;
}

int SRTP_ConnectionInfo::get_local_addr(){
	return m_local_addr;
}

int SRTP_ConnectionInfo::get_local_port(){
	return m_local_port;
}

int SRTP_ConnectionInfo::get_remote_addr(){
	return m_remote_addr;
}

int SRTP_ConnectionInfo::get_remote_port(){
	return m_remote_port;
}

void SRTP_ConnectionInfo::clear_connection_num(){
	m_connection_num = 0;
}

void SRTP_ConnectionInfo::clear_socket_id(){
	m_socket_id = 0;
}

void SRTP_ConnectionInfo::clear_local_addr(){
	m_local_addr = 0;
}

void SRTP_ConnectionInfo::clear_local_port(){
	m_local_port = 0;
}

void SRTP_ConnectionInfo::clear_remote_addr(){
	m_remote_addr = 0;
}

void SRTP_ConnectionInfo::clear_remote_port(){
	m_remote_port = 0;
}

