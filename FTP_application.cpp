#include<iostream>
#include<string>
#include<string.h>
#include<fstream>
#include<exception>

#include "SRTP_CommonTypes.h"
#include "SRTP_GlobalVariables.h"
#include "SRTP_Packet.h"
#include "SRTP_IncomingBuffer.h"
#include "SRTP_OutgoingBuffer.h"
#include "SRTP_IncomingMessageHandler.h"
#include "SRTP_OutgoingMessageHandler.h"
#include "SRTP_Connection.h"
#include "SRTP_ConnectionTable.h"
#include "SRTP_ConnectionStatusTable.h"
#include "SRTP_PeerTable.h"

using namespace std;

std::string encrypt(std::string value,std::string key);
std::string key("jwhdghdhk");

int main(int argc , char* argv[])
{
   try
   {
    int flag = 0;
    std::ofstream logfile;
    std::streambuf* sbuf_clog;
    if(argc<=2)
    {
          std::cout << "Usage: application -s <ipaddress> <port> -m <MSS> -b <buffer_size> [-l|-o|-d] <packet number>"
                    << std::endl << "application -c <local ip address> <local port> <remote ip address> <remote port> -uf|df <filename> -m <MSS> -b <buffer size> [-l|-o|-d] <packet number>"<<std::endl;
          throw "Bad Input: Less number of Args";
    }
    else
    {
        if(!(strcmp(argv[1] , "-s")))
        {
                if(argc < 8)
                {     std::cout << "Usage: application -s <ipaddress> <port> -m <MSS> -b <buffer_size> [-l|-o|-d] <packet number>" << std::endl;
                      throw "Bad Input: Less number of Args in case of Server";
                }

                logfile.open("server_log.txt");
                sbuf_clog = std::clog.rdbuf();
                if(logfile.is_open())
                std::clog.rdbuf(logfile.rdbuf());
                flag = 1;
            }
            if(!(strcmp(argv[1] , "-c")))
            {
                if(argc < 12)
                {     std::cout << "Usage: application -c <local ip address> <local port> <remote ip address> <remote port> -uf|df <filename> -m <MSS> -b <buffer size> [-l|-o|-d] <packet number>" << std::endl;
                      throw "Bad Input: Less number of Args in case of Client";
                }

                logfile.open("client_log.txt");
                sbuf_clog = std::clog.rdbuf();
                if(logfile.is_open())
                std::clog.rdbuf(logfile.rdbuf());
                flag = 2;
            }
        }// end else

        switch(flag)
        {
          case 1:
                {
					int flag = 0 ;
                    ofstream input_file;
	                std::string server_address = argv[2];
                    int server_port = atoi(argv[3]);

                    SRTP_IncomingBuffer in_buffer;
                    SRTP_OutgoingBuffer out_buffer;

                    SRTP_IncomingMessageHandler ihandler(&in_buffer);
                    SRTP_OutgoingMessageHandler ohandler(&out_buffer);

                    SRTP_Connection conn(server_address , server_port ,&in_buffer ,&out_buffer);

                    // Start Listener
                    int i = conn.SRTP_Listen();

                    if (i!=1)
                    {
                        throw "Application Error : Server didnt start";
                    }
                    else
                    {
                        std::clog << "INFO:\t Application Server started successfully" << std::endl;
						std::cout << "INFO:\t Application Server started successfully" << std::endl;
                    }

                    int MSS;
                    if(!strcmp(argv[4],"-m"))
                    {
                          MSS = atoi(argv[5]);
                          SRTP_GlobalVariables::get_global_var_obj_instance().set_MSS_size(MSS);
                    }
                    else
                    {
                        throw "Application : Error Wrong input";
                    }

                    int buffer_size;
                    if(!strcmp(argv[6],"-b"))
                    {
                           buffer_size = atoi(argv[7]);
                           SRTP_GlobalVariables::get_global_var_obj_instance().set_window_size(buffer_size);
                    }
                    else
                    {
                        throw "Application : Error Wrong input";
                    }

					if(argc > 8)
					{
						SRTP_SIMULATION_ERROR err;
						if(!strcmp(argv[8],"-l"))
							err = PACKET_LOSS;
						else if(!strcmp(argv[8],"-o"))
							err = OUT_OF_ORDER;
						else if(!strcmp(argv[8],"-d"))
							err = DUPLICATE;
						else
							throw "Application : Error Wrong input";

						ohandler.simulate_error_case(err, atoi(argv[9]));
					}

                    while(true)
                    {
						pthread_mutex_lock( &con_server_cv_mutex );
						while(SRTP_PeerTable::get_peer_table_instance().get_map_size() <= 2)
						{
							std::clog<<"INFO:\t Listener waiting for a connection\n";
							std::cout<<"INFO:\t Listener waiting for a connection\n";
							pthread_cond_wait( &con_server_cond, &con_server_cv_mutex );
						}
						pthread_mutex_unlock( &con_server_cv_mutex );
						std::clog<<"INFO: \tconnection present\n";

						int no_peers = SRTP_PeerTable::get_peer_table_instance().get_map_size();

						for(int i=2; i< no_peers; i++)
						{
		                    int connectionId = SRTP_PeerTable::get_peer_table_instance().get_con_num(i);
							int agreed_win_size = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connectionId).get_window_size();

		                    if(connectionId)
		                    {
		                    pthread_mutex_lock( &in_buf_cv_mutex );
		                    while(in_buffer.get_buffer_size() <= 0)
		                    {				
		                        pthread_cond_wait( &in_buf_cond, &in_buf_cv_mutex );
		                    }
		                    pthread_mutex_unlock( &in_buf_cv_mutex );
	                    
		                    for(int i=0; i< in_buffer.get_buffer_size(); i++)
		                    {
		                        std::string buffer = ihandler.SRTP_Receive(connectionId);
								buffer = encrypt(buffer, key);

		                        if(!(buffer.compare(0,17,"Upload filename: ")))
		                        {									
		                            std::string filename = buffer.substr(17,(buffer.size()));
									std::clog<<"INFO:\t Opening File: "<<filename<<std::endl;
		                            input_file.open(filename.c_str());
		                            flag = 1;
		                        }
		                        else if(!(buffer.compare(0,25,"*****End of the File*****")))
		                        {
									std::cout<<"INFO:\t Upload complete\n";
		                            input_file.close();
									conn.wait_for_end_connection(connectionId);
		                            break;
		                        }
		                        else if(!(buffer.compare(0,19,"Download filename: ")))
		                        {
		                            std::string filename = buffer.substr(19,(buffer.size()));
		                            ifstream output_file(filename.c_str());
									std:clog<<output_file.is_open()<<std::endl;
		                            std::string line;
		                            char input[MSS];
		                            if(output_file.is_open())
		                            {
		                                while(!output_file.eof())
		                                {
		                                    output_file.read(input,MSS);
											std::string tmp_string(input, output_file.gcount());
		                                    line = tmp_string;
											line = encrypt(line, key);
		                                    ohandler.SRTP_Send(connectionId,line);
		                                }

		                                 // End of the file-- notification
		                                line = "*****End of the File*****";
										line = encrypt(line, key);
		                                ohandler.SRTP_Send(connectionId, line);
		                                output_file.close();
										conn.wait_for_end_connection(connectionId);
		                            }
		                            else
		                            {
		                               std::clog << "File Open error";
		                               exit(1);
		                            }
		                            break;
		                        }
		                        else
		                        {
		                            if(input_file.is_open() && flag)
		                            {
		                                input_file << buffer;
		                            }
		                        }
		                    }// end of for
						}// end of for
                    }// end of if
                }// end of while
                break;
            }

          case 2:
                {
                std::string local_addr, remote_addr;
                int local_port, remote_port;

                SRTP_IncomingBuffer in_buffer;
                SRTP_OutgoingBuffer out_buffer;

                SRTP_IncomingMessageHandler ihandler(&in_buffer);
                SRTP_OutgoingMessageHandler ohandler(&out_buffer);

                local_addr = argv[2];
                local_port = atoi(argv[3]);
                remote_addr = argv[4];
                remote_port = atoi(argv[5]);

                int MSS;
                if(!strcmp(argv[8],"-m"))
                {
                    MSS = atoi(argv[9]);
                    SRTP_GlobalVariables::get_global_var_obj_instance().set_MSS_size(MSS);
                }
                else
                {
                    throw "Application : Error Wrong input";
                }

                int buffer_size;
                if(!strcmp(argv[10],"-b"))
                {
                    buffer_size = atoi(argv[11]);
                    SRTP_GlobalVariables::get_global_var_obj_instance().set_window_size(buffer_size);
                }
                else
                {
                    throw "Application : Error Wrong input";
                }

				if(argc > 12)
				{
					SRTP_SIMULATION_ERROR err;
					if(!strcmp(argv[12],"-l"))
						err = PACKET_LOSS;
					else if(!strcmp(argv[12],"-o"))
						err = OUT_OF_ORDER;
					else if(!strcmp(argv[12],"-d"))
						err = DUPLICATE;
					else
						throw "Application : Error Wrong input";

					ohandler.simulate_error_case(err, atoi(argv[13]));
				}

				// Start the connection
                SRTP_Connection conn(local_addr , local_port ,&in_buffer ,&out_buffer, remote_addr, remote_port);

                int connection_number = conn.SRTP_Open();
                if(connection_number < 0)
                {
                    throw "Application Connection Error : Client failed";
                }
                else
                {
                    std::clog << "INFO:\t Client connected successfully with Connection number: "<<connection_number<< std::endl;
                }


                if(!strcmp(argv[6],"-uf"))
                {

					pthread_mutex_lock( &condition_mutex );
					while( SRTP_ConnectionStatusTable::get_con_status_table_instance().get_status_table_entry(connection_number) != SRTP_ACTIVE_OPEN )
					{
						std::clog<<"INFO: Waiting for Connection to establish \n";			
						pthread_cond_wait( &condition_cond, &condition_mutex );
					}
					pthread_mutex_unlock( &condition_mutex );

					int agreed_win_size = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_number).get_window_size();

					std::clog<<"INFO:\t Agreed window size = "<<agreed_win_size<<std::endl;
					std::cout<<"INFO:\t Agreed window size = "<<agreed_win_size<<std::endl;

                    std::string filename = argv[7];
                    ifstream output_file(filename.c_str());
					std::string first_packet = "Upload filename: "+filename;
					first_packet = encrypt(first_packet, key);
                    ohandler.SRTP_Send(connection_number, first_packet);
					std::cout<<"Uploading file";

                    if(output_file.is_open())
                    {
                       std::string line;
                       char input[MSS];

                       while(!output_file.eof())
                       {
                          output_file.read(input,MSS);
						  std::string temp_str(input,output_file.gcount()); 
                          line = temp_str;
						  line = encrypt(line, key);
                          ohandler.SRTP_Send(connection_number, line);
						  std::cout<<".";
                       }
                       // End of the file-- notification
                       line = "*****End of the File*****";
					   line = encrypt(line, key);
                       ohandler.SRTP_Send(connection_number, line);
                       output_file.close();
					   conn.SRTP_Close(connection_number);
					   conn.join_connector_thread();

                    }
                    else
                    {
                        std::clog << "File Open error";
                        exit(1);
                    }
                }
                else if(!strcmp(argv[6],"-df"))
                {

					pthread_mutex_lock( &condition_mutex );
					while( SRTP_ConnectionStatusTable::get_con_status_table_instance().get_status_table_entry(connection_number) != SRTP_ACTIVE_OPEN )
					{
						std::clog<<"INFO: Waiting for Connection to establish \n";			
						pthread_cond_wait( &condition_cond, &condition_mutex );
					}
					pthread_mutex_unlock( &condition_mutex );

					int agreed_win_size = SRTP_PeerTable::get_peer_table_instance().get_peer_table_entry(connection_number).get_window_size();
					std::clog<<"INFO:\t Agreed window size = "<<agreed_win_size<<std::endl;
					std::cout<<"INFO:\t Agreed window size = "<<agreed_win_size<<std::endl;

                    std::string filename = argv[7];
                    ofstream input_file(filename.c_str());
					std::string first_packet = "Download filename: "+filename;
				    first_packet = encrypt(first_packet, key);
                    ohandler.SRTP_Send(connection_number, first_packet);
					std::cout<<"Downloading file";
                    while(true)
                    {
	                    pthread_mutex_lock( &in_buf_cv_mutex );
	                    while(in_buffer.get_buffer_size() <= 0)
	                    {
	                        pthread_cond_wait( &in_buf_cond, &in_buf_cv_mutex );
	                    }
	                    pthread_mutex_unlock( &in_buf_cv_mutex );

	                    for(int i=0; i< in_buffer.get_buffer_size(); i++){
		                    std::string buffer = ihandler.SRTP_Receive(connection_number);
							buffer = encrypt(buffer, key);

		                    if (!strcmp(buffer.c_str(),"*****End of the File*****"))
		                    {
								input_file.close();
								std::cout<<"\nINFO:\t Download complete\n";
							   	conn.SRTP_Close(connection_number);
							   	conn.join_connector_thread();
		                    }							
		                    else if(input_file.is_open())
		                    {
								std::cout<<".";
								std::clog<<buffer<<std::endl;
		                        input_file << buffer;
		                    }
		                    else
		                    {
		                        std::clog << "File Open error";
		                        exit(1);
		                    }
						}
                    } // end of while
                    
                }
                else
                {
                    throw "Application : Error : Wrong input";
                }
                break;
                }
          }

       if(logfile.is_open())
       std::clog.rdbuf(sbuf_clog);

        logfile.close();
    }
    catch(std::string& e)
    {
         std::cout << "Application Error"<<e<< std::endl;
    }
    catch(exception& e)
    {
         std::cout << "Some Error"<<std::endl;
    }

}

std::string encrypt(std::string value,std::string key)
{
    std::string result(value);

    short unsigned int klen=key.length();
    short unsigned int vlen=value.length();

    short unsigned int i=0;
    short unsigned int j=0;

    for(i;i<vlen;i++)
    {
        result[i]=value[i]^key[j];
        j=(++j<klen?j:0);
    }
   
    return result;
}


