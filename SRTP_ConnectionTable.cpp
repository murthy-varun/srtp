#include "SRTP_ConnectionTable.h"
#include "SRTP_CommonTypes.h"
#include <iostream>

SRTP_ConnectionTable SRTP_ConnectionTable::m_connection_table_obj;

SRTP_ConnectionTable::SRTP_ConnectionTable(){
//    std::clog<<"ENTER:\t SRTP_ConnectionTable::SRTP_ConnectionTable\n";
    
//    std::clog<<"EXIT:\t SRTP_ConnectionTable::SRTP_ConnectionTable\n";
}

SRTP_ConnectionTable& SRTP_ConnectionTable::get_connection_table_instance(){
    std::clog<<"ENTER:\t SRTP_ConnectionTable::get_connection_table_instance\n";
    std::clog<<"EXIT:\t SRTP_ConnectionTable::get_connection_table_instance\n";
    return m_connection_table_obj;    
}

void SRTP_ConnectionTable::add_connection_table_entry(int connection_num, SRTP_ConnectionInfo con_info){
    std::clog<<"ENTER:\t SRTP_ConnectionTable::add_connection_table_entry\n";
    m_connection_map.insert(std::map<int, SRTP_ConnectionInfo>::value_type(connection_num, con_info));
    std::clog<<"EXIT:\t SRTP_ConnectionTable::add_connection_table_entry\n";
}

void SRTP_ConnectionTable::delete_connection_table_entry(int connection_num){
    std::clog<<"ENTER:\t SRTP_ConnectionTable::delete_connection_table_entry\n";
    m_connection_map.erase(connection_num);
    std::clog<<"EXIT:\t SRTP_ConnectionTable::delete_connection_table_entry\n";
}

SRTP_ConnectionInfo SRTP_ConnectionTable::get_connection_table_entry(int connection_num){
    std::clog<<"ENTER:\t SRTP_ConnectionTable::get_connection_table_entry\n";
    std::clog<<"EXIT:\t SRTP_ConnectionTable::get_connection_table_entry\n";
    return m_connection_map[connection_num];    
}

void SRTP_ConnectionTable::modify_connection_table_entry(int connection_id, SRTP_ConnectionInfo con_info){
    std::clog<<"ENTER:\t SRTP_ConnectionTable::modify_connection_table_entry\n";
    m_connection_map[connection_id] = con_info;
    std::clog<<"EXIT:\t SRTP_ConnectionTable::modify_connection_table_entry\n";
}

int SRTP_ConnectionTable::get_first_connection(){
	return (m_connection_map.begin())->first;
}

void SRTP_ConnectionTable::clear_connection_table(){
    std::clog<<"ENTER:\t SRTP_ConnectionTable::clear_connection_table\n";
    m_connection_map.clear();
    std::clog<<"EXIT:\t SRTP_ConnectionTable::clear_connection_table\n";
}
int SRTP_ConnectionTable::get_map_size(){
	return m_connection_map.size();
}

