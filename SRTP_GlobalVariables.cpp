#include "SRTP_GlobalVariables.h"
#include "SRTP_CommonTypes.h"
#include <iostream>

SRTP_GlobalVariables SRTP_GlobalVariables::m_global_var_obj;

SRTP_GlobalVariables::SRTP_GlobalVariables(){
    //std::clog<<"ENTER:\t SRTP_GlobalVariables::SRTP_GlobalVariables\n";
    m_global_connection_id = 2;
    m_global_sequence_num = 0;
    //std::clog<<"EXIT:\t SRTP_GlobalVariables::SRTP_GlobalVariables\n";
}

SRTP_GlobalVariables& SRTP_GlobalVariables::get_global_var_obj_instance(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::get_global_var_obj_instance\n";
    std::clog<<"EXIT:\t SRTP_GlobalVariables::get_global_var_obj_instance\n";
    return m_global_var_obj;
}

void SRTP_GlobalVariables::increment_connection_id(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::increment_connection_id\n";
    m_global_connection_id++;
    std::clog<<"EXIT:\t SRTP_GlobalVariables::increment_connection_id\n";
}

int SRTP_GlobalVariables::get_connection_id(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::get_connection_id\n";
    std::clog<<"EXIT:\t SRTP_GlobalVariables::get_connection_id\n";
    return m_global_connection_id;
}

void SRTP_GlobalVariables::clear_connection_id(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::clear_connection_id\n";
    m_global_connection_id = 0;
    std::clog<<"EXIT:\t SRTP_GlobalVariables::clear_connection_id\n";
}

void SRTP_GlobalVariables::increment_sequence_num(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::increment_sequence_num\n";
    m_global_sequence_num++;
    std::clog<<"EXIT:\t SRTP_GlobalVariables::increment_sequence_num\n";
}

int SRTP_GlobalVariables::get_sequence_num(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::get_sequence_num\n";
    std::clog<<"EXIT:\t SRTP_GlobalVariables::get_sequence_num\n";
    return m_global_sequence_num;
}

void SRTP_GlobalVariables::clear_sequence_num(){
    std::clog<<"ENTER:\t SRTP_GlobalVariables::clear_sequence_num\n";
    m_global_sequence_num = 0;
    std::clog<<"EXIT:\t SRTP_GlobalVariables::clear_sequence_num\n";
}

int SRTP_GlobalVariables::get_MSS_size()
{
    return m_MSS_size;
}

void SRTP_GlobalVariables::set_MSS_size(int MSS)
{
   m_MSS_size = MSS; 
}

void SRTP_GlobalVariables::set_window_size(int size)
{
   m_window_size = size;
}

int SRTP_GlobalVariables::get_window_size()
{
   return m_window_size;
}




