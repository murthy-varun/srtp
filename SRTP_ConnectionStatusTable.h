/* 
 * File:   SRTP_ConnectionStatusTable.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 8:52 AM
 */

#ifndef _SRTP_CONNECTIONSTATUSTABLE_H
#define	_SRTP_CONNECTIONSTATUSTABLE_H

#include <map>
#include "SRTP_CommonTypes.h"

class SRTP_ConnectionStatusTable{
private:
    std::map<int,SRTP_ConnectionStatus> m_connection_map;
    static SRTP_ConnectionStatusTable m_con_status_table_obj;
    
    SRTP_ConnectionStatusTable();
    
public:
    static SRTP_ConnectionStatusTable& get_con_status_table_instance();
    
    void add_status_table_entry(int connection_num, SRTP_ConnectionStatus status);
    
    void modify_status_table_entry(int connection_num, SRTP_ConnectionStatus status);
    
    void delete_status_table_entry(int connection_num);
    
    SRTP_ConnectionStatus get_status_table_entry(int connection_num);
	
	int get_num_connections();
    
    void clear_status_table();
            
};


#endif	/* _SRTP_CONNECTIONSTATUSTABLE_H */

