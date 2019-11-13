/* 
 * File:   SRTP_CommonFunctions.h
 * Author: Varun & Shruthi
 *
 * Created on November 9, 2008, 5:13 AM
 */

#ifndef _SRTP_COMMONFUNCTIONS_H
#define	_SRTP_COMMONFUNCTIONS_H

#include "SRTP_CommonTypes.h"

class SRTP_CommonFunctions{
private:
    static SRTP_CommonFunctions m_common_func_obj;
    SRTP_CommonFunctions();
public:
    static SRTP_CommonFunctions& get_instance();
    
    uint16_t caluculate_checksum(SRTP_PacketStructure& packet);

	static void timer_signal_handler(int sig);
};

#endif	/* _SRTP_COMMONFUNCTIONS_H */


