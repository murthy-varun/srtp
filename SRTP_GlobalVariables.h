/* 
 * File:   SRTP_GlobalVariables.h
 * Author: Varun & Shruthi
 *
 * Created on November 6, 2008, 8:52 AM
 */

#ifndef _SRTP_GLOBALVARIABLES_H
#define	_SRTP_GLOBALVARIABLES_H

#include "SRTP_CommonTypes.h"

class SRTP_GlobalVariables{
private:    
    static SRTP_GlobalVariables m_global_var_obj;
    int m_global_connection_id;
    int m_global_sequence_num;
    int m_MSS_size;
    int m_window_size;
    
    SRTP_GlobalVariables();
    
public:
    static SRTP_GlobalVariables& get_global_var_obj_instance();
    void increment_connection_id();
    int get_connection_id();
    void clear_connection_id();
    void increment_sequence_num();
    int get_sequence_num();
    void clear_sequence_num();
    int get_MSS_size();
    void set_MSS_size(int MSS);
    int get_window_size();
    void set_window_size(int size);
};


#endif	/* _SRTP_GLOBALVARIABLES_H */

