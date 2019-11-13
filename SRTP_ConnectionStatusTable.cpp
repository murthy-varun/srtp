#include "SRTP_ConnectionStatusTable.h"
#include "SRTP_CommonTypes.h"
#include <iostream>
#include <pthread.h>

pthread_mutex_t table_mutex = PTHREAD_MUTEX_INITIALIZER;

SRTP_ConnectionStatusTable SRTP_ConnectionStatusTable::m_con_status_table_obj;

SRTP_ConnectionStatusTable::SRTP_ConnectionStatusTable(){
    //std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::SRTP_ConnectionStatusTable\n";    
    //std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::SRTP_ConnectionStatusTable\n";
}

SRTP_ConnectionStatusTable& SRTP_ConnectionStatusTable::get_con_status_table_instance(){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::get_con_status_table_instance\n";
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::get_con_status_table_instance\n";
    return m_con_status_table_obj;    
}

void SRTP_ConnectionStatusTable::add_status_table_entry(int connection_num, SRTP_ConnectionStatus status){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::add_status_table_entry\n";
    m_connection_map.insert(std::map<int,SRTP_ConnectionStatus>::value_type(connection_num, status));
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::add_status_table_entry\n";
}

void SRTP_ConnectionStatusTable::modify_status_table_entry(int connection_num, SRTP_ConnectionStatus status){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::modify_status_table_entry\n";
    m_connection_map[connection_num] = status;
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::modify_status_table_entry\n";
}

void SRTP_ConnectionStatusTable::delete_status_table_entry(int connection_num){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::delete_status_table_entry\n";
    m_connection_map.erase(connection_num);
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::delete_status_table_entry\n";
}

SRTP_ConnectionStatus SRTP_ConnectionStatusTable::get_status_table_entry(int connection_num){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::get_status_table_entry\n";
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::get_status_table_entry\n";
    return m_connection_map[connection_num];    
}

void SRTP_ConnectionStatusTable::clear_status_table(){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::clear_status_table\n";
    m_connection_map.clear();
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::clear_status_table\n";
}
int SRTP_ConnectionStatusTable::get_num_connections(){
    std::clog<<"ENTER:\t SRTP_ConnectionStatusTable::get_num_connections\n";
    std::clog<<"EXIT:\t SRTP_ConnectionStatusTable::get_num_connections\n";
    return m_connection_map.size();    
}

