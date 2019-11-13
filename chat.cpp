#include<iostream>
#include<iomanip>
#include<string>
#include<string.h>
#include<fstream>
#include<exception>
#include<time.h>
#include<pthread.h>

#include "SRTP_CommonTypes.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_Packet.h"
#include "SRTP_IncomingBuffer.h"
#include "SRTP_OutgoingBuffer.h"
#include "SRTP_IncomingMessageHandler.h"
#include "SRTP_OutgoingMessageHandler.h"
#include "SRTP_Connection.h"
#include "SRTP_ConnectionTable.h"
#include "SRTP_PeerTable.h"

void* handle_recv_msgs(void* in_buf);
void* handle_send_msgs(void* out_buf);

pthread_cond_t sender_wait_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sender_wait_cv_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Incoming_Pointers{
	int type; //0-server 1-client
	SRTP_IncomingMessageHandler* ihandler;
	SRTP_IncomingBuffer* in_buf;
};

struct Outgoing_Pointers{
	int type; //0-server 1-client
	SRTP_OutgoingMessageHandler* ohandler;
	SRTP_OutgoingBuffer* out_buf;
};

int main(int argc , char* argv[])
{
    std::ofstream logfile;
    std::streambuf* sbuf_clog;
    logfile.open("log.txt");
    sbuf_clog = std::clog.rdbuf();
    if(logfile.is_open())
    std::clog.rdbuf(logfile.rdbuf());


    try
    {
        if (argc < 7)
        {
            std::cout << "Wrong usage: "<< "talk <local ipaddress> <local port> -m <MSS> -b <buffer_size>"<< std::endl;
            throw "Bad Input: Less number of Args";
        }
        else
        {
            int MSS;
            if(!strcmp(argv[3],"-m"))
            {
                MSS = atoi(argv[4]);
                SRTP_GlobalVariables::get_global_var_obj_instance().set_MSS_size(MSS);
            }
            else
            {
                throw "Application : Error Wrong input";
            }

            int buffer_size;
            if(!strcmp(argv[5],"-b"))
            {
                buffer_size = atoi(argv[6]);
                SRTP_GlobalVariables::get_global_var_obj_instance().set_window_size(buffer_size);
            }
            else
            {
                throw "Application : Error Wrong input";
            }

            std::string local_address = argv[1];
            int local_port = atoi(argv[2]);

            std::string remote_addr;
            int remote_port;

            SRTP_IncomingBuffer in_buffer;
            SRTP_OutgoingBuffer out_buffer;
            SRTP_IncomingMessageHandler ihandler(&in_buffer);
            SRTP_OutgoingMessageHandler ohandler(&out_buffer);

			if(argc > 7)
			{
				SRTP_SIMULATION_ERROR err;
				if(!strcmp(argv[7],"-l"))
					err = PACKET_LOSS;
				else if(!strcmp(argv[7],"-o"))
					err = OUT_OF_ORDER;
				else if(!strcmp(argv[7],"-d"))
					err = DUPLICATE;
				else
					throw "Application : Error Wrong input";

				ohandler.simulate_error_case(err, atoi(argv[8]));
			}

			// Start listener
            SRTP_Connection conn(local_address , local_port ,&in_buffer ,&out_buffer);
            int i = conn.SRTP_Listen();
            if(i!=1)
            {
                 std::clog << "Listen failed: " << std::endl;
				 std::cout << "INFO:\t Application Server start failed" << std::endl;
            }
            else{
                 std::clog << "Listening on port: " << local_port << std::endl;
				 std::cout << "INFO:\t Application Server started successfully" << std::endl;
            }

            bool stop_chat = false;
            int option = 0;
			int type = 0;

			std::cout<<"Press return to get the options\n";
			stop_chat = getchar();
            std::cout<<" Options: \n1. Open a Connection \n2. Continue Listening \n3. Quit\nEnter the option:";
            std::cin>>option;
            switch(option)
            {
                case 1:
                    {
                        std::cout<<"Enter the remote IP address that you want to connect:";
                        std::cin>>remote_addr;
                        std::cout<<"Enter the remote port that you want to connect:";
                        std::cin>>remote_port;

						// generating local port by random
						srand(time(NULL));
						int local_port_num = (rand()%1000)+20000;

                        SRTP_Connection connection(local_address ,local_port_num, &in_buffer ,&out_buffer, remote_addr, remote_port);
                        int connection_number = connection.SRTP_Open();
                        if(connection_number < 2)
                        {
                            throw "Application Connection Error : Client failed";
                        }
                        else
                        {
                            std::clog << "INFO:\t connected successfully with Connection number: "<<connection_number<< std::endl;
                            std::cout<<"Start chat:\n";
                        }
						type =1;
					
						fflush(stdin);
						fflush(stdout);

                        break;
                    }
                case 2:
                    {
                        pthread_mutex_lock( &con_server_cv_mutex );
                        if(SRTP_PeerTable::get_peer_table_instance().get_map_size() <= 2)
                        {
                            std::clog<<"INFO: \tWaiting a connection\n";
                            pthread_cond_wait( &con_server_cond, &con_server_cv_mutex );
                        }
                        pthread_mutex_unlock( &con_server_cv_mutex );
                        std::clog<<"INFO: \tconnection present\n";
						std::cout<<"Connection started \n";

						type =0;
						fflush(stdin);
						fflush(stdout);
						break;

                    }
                case 3:
                    {
                        std::cout<<"Stopping the talk application\n";
                        stop_chat = true;
                        break;
                    }
            } //end of switch

			// start a receive thread
			Incoming_Pointers in_ptrs;
			in_ptrs.ihandler = &ihandler;
			in_ptrs.in_buf = &in_buffer;
			in_ptrs.type = type;
			pthread_t recv_thread;
			int ret1 = pthread_create(&recv_thread, NULL, handle_recv_msgs, (void *)(&in_ptrs) );

			// start a sender thread
			Outgoing_Pointers out_ptrs;
			out_ptrs.ohandler = &ohandler;
			out_ptrs.out_buf = &out_buffer;
			out_ptrs.type = type;
			pthread_t send_thread;
			int ret2 = pthread_create(&send_thread, NULL, handle_send_msgs, (void *)(&out_ptrs) );


			pthread_join(recv_thread, NULL);			
			pthread_join(send_thread, NULL);

        }// end of else
    }// end of try
    catch(std::string& e)
    {
         std::cout << "Application Error"<<e<< std::endl;
    }

   if(logfile.is_open())
   std::clog.rdbuf(sbuf_clog);

    logfile.close();

}


void* handle_recv_msgs(void* incoming_ptr)
{
	Incoming_Pointers* in_ptrs = (Incoming_Pointers*)incoming_ptr;
	SRTP_IncomingBuffer* in_buf_ptr = in_ptrs->in_buf;
	SRTP_IncomingMessageHandler* ihandler = in_ptrs->ihandler;
	int type = in_ptrs->type;

	int start_num;
	if(type == 0)
		start_num = 2;
	else if(type == 1)
		start_num = 0;

	while(true){

		    int connection_number;
		    std::string out_msg = ihandler->SRTP_Receive(&connection_number);
		    std::cout<<"\nFrom "<<connection_number<<": ";
			std::cout<<out_msg<<"\n";
		
	}
}

void* handle_send_msgs(void* outgoing_ptr){
	Outgoing_Pointers* out_ptrs = (Outgoing_Pointers*)outgoing_ptr;
	SRTP_OutgoingBuffer* out_buf_ptr = out_ptrs->out_buf;
	SRTP_OutgoingMessageHandler* ohandler = out_ptrs->ohandler;
	int type = out_ptrs->type;

	int start_num;
	if(type == 0)
		start_num = 2;
	else if(type == 1)
		start_num = 0;

	char input_message[1024];
	std::string out_msg;

    int MSS = SRTP_GlobalVariables::get_global_var_obj_instance().get_MSS_size();

	while(true){

			int no_peers = SRTP_PeerTable::get_peer_table_instance().get_map_size();
			for(int i=start_num; i< no_peers; i++)
			{
				int connection_number = SRTP_PeerTable::get_peer_table_instance().get_con_num(i);

				std::cout<<"To "<<connection_number<<": ";
				std::cin.getline(input_message, sizeof(input_message), '\n');

				int size = std::cin.gcount();
				std::string remainder(input_message, size);
				int flag = 0;

				pthread_mutex_lock(&sender_wait_cv_mutex);
				while(size <= 0){
					pthread_cond_wait(&sender_wait_cond, &sender_wait_cv_mutex);
				}
				pthread_mutex_unlock(&sender_wait_cv_mutex);
				
				if(size > MSS){
					while(remainder.size()<=MSS){
						ohandler->SRTP_Send(connection_number, remainder.substr(0, MSS));
						remainder = remainder.substr(MSS, remainder.size());
					}
					ohandler->SRTP_Send(connection_number, remainder);
				}
				else{
					ohandler->SRTP_Send(connection_number, input_message);
				}
			}
	
	}
}

