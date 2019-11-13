/* 
 * File:   SRTP_ConnectionTable.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 8:39 AM
 */

#ifndef _SRTP_CONNECTIONTABLE_H
#define	_SRTP_CONNECTIONTABLE_H

#include <map>
#include "SRTP_CommonTypes.h"
#include "SRTP_ConnectionInfo.h"

class SRTP_ConnectionTable{
private:
    std::map<int,SRTP_ConnectionInfo> m_connection_map;
    static SRTP_ConnectionTable m_connection_table_obj;
    
    SRTP_ConnectionTable();
    
public:
    static SRTP_ConnectionTable& get_connection_table_instance();
    
    void add_connection_table_entry(int connection_num, SRTP_ConnectionInfo con_info);

	void modify_connection_table_entry(int connection_id, SRTP_ConnectionInfo con_info);
    void delete_connection_table_entry(int connection_num);
    
    SRTP_ConnectionInfo get_connection_table_entry(int connection_num);

	int get_first_connection();

	int get_map_size();
            
    void clear_connection_table();
};

#endif	/* _SRTP_CONNECTIONTABLE_H */

