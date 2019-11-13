#include "SRTP_CommonFunctions.h"
#include "SRTP_CommonTypes.h"
#include <iostream>

SRTP_CommonFunctions SRTP_CommonFunctions::m_common_func_obj;

SRTP_CommonFunctions::SRTP_CommonFunctions(){
//    std::clog<<"ENTER:\t SRTP_CommonFunctions::SRTP_CommonFunctions\n";
    
//    std::clog<<"EXIT:\t SRTP_CommonFunctions::SRTP_CommonFunctions\n";
}

SRTP_CommonFunctions& SRTP_CommonFunctions::get_instance(){
    std::clog<<"ENTER:\t SRTP_CommonFunctions::get_instance\n";
    std::clog<<"EXIT:\t SRTP_CommonFunctions::get_instance\n";
    return m_common_func_obj;
}

uint16_t SRTP_CommonFunctions::caluculate_checksum(SRTP_PacketStructure& packet){
        std::clog<<"ENTER:\t SRTP_CommonFunctions::caluculate_checksum\n";

        unsigned short word16;
        unsigned long sum;

        //initialize sum to zero
        sum=0;

        word16 = packet.connection_id;
        sum = sum + word16;
        word16 = packet.packet_type;
        sum = sum + word16;
        word16 = packet.source_port;
        sum = sum + word16;
        word16 = packet.dest_port;
        sum = sum + word16;

        uint32_t temp = packet.source_addr;
        word16 = ((temp & 0xFF00)>>16 ) + (packet.source_addr & 0x00FF);
        sum = sum + word16;

        temp = packet.seq_num ;
        word16 = ((temp & 0xFF00)>>16) + (packet.seq_num & 0x00FF);
		sum = sum + word16;

        temp = packet.ack_num;
        word16 = ((temp & 0xFF00)>>16) + (packet.ack_num & 0x00FF);
        sum = sum + word16;

        temp = packet.dest_addr ;
        word16 = ((temp &0xFF00)>> 16) + (packet.dest_addr & 0x00FF);
        sum = sum + word16;

        word16 = packet.window_size;
        sum = sum + word16;

        word16 = packet.checksum;
        sum = sum + word16;

        while (sum>>16)
        sum = (sum & 0xFFFF)+(sum >> 16);

        // Take the one's complement of sum
        sum = ~sum;

        std::clog<<"INFO:\t calculated_checksum: " << (uint16_t)sum<<"\n";

        std::clog<<"EXIT:\t SRTP_CommonFunctions::caluculate_checksum\n";
        return ((uint16_t) sum);
}    

void SRTP_CommonFunctions::timer_signal_handler(int sig){
	std::clog<<"****************************** IN SIGALARM *******************\n";
	//std::cout<<"****************************** IN SIGALARM *******************\n";
	
	timer_expired = true;
	out_buf_end = false;	
	// Notify sender thread
	std::clog<<"INFO:\t Notifying Sender Thread\n";
	if(out_buf_end == false){
		pthread_cond_signal( &out_buf_cond );
	}
}

