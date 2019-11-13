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
#include "SRTP_PeerTable.h"

using namespace std;	

int main(int argc , char* argv[])
{
   try  
   {
	int flag = 0;
	std::ofstream logfile;
	std::streambuf* sbuf_clog;
    if(argc<=2)
	{
          std::cout << "Usage: application -s <ipaddress> <port> -m <MSS> -b <buffer_size>"
                    << std::endl << "application -c <local ip address> <local port> <remote ip address> <remote port> -uf|df <filename> -m <MSS> -b <buffer size>"<<std::endl;
          throw "Bad Input: Less number of Args";
    }
	else
	{
		if(!(strcmp(argv[1] , "-s")))
		{	
		        if(argc != 8)
		        {     std::cout << "Usage: application -s <ipaddress> <port> -m <MSS> -b <buffer_size>" << std::endl;
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
				if(argc != 12)
		        {     std::cout << "Usage: application -c <local ip address> <local port> <remote ip address> <remote port> -uf|df <filename> -m <MSS> -b <buffer size>" << std::endl;
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
			       	 	std::clog << "Enter: Application Server started successfully" << std::endl;
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

		           	while(true)
		           	{
		               	int connectionId = SRTP_ConnectionTable::get_connection_table_instance().get_first_connection();
		               	if(connectionId)
		               	{
						   	pthread_mutex_lock( &in_buf_cv_mutex );
						   	while(in_buffer.get_buffer_size() <= 0)
						   	{
								pthread_cond_wait( &in_buf_cond, &in_buf_cv_mutex );
						   	}
						   	pthread_mutex_unlock( &in_buf_cv_mutex );

						   	int flag = 0 ;
							ofstream input_file;
							

						  	for(int i=0; i< in_buffer.get_buffer_size(); i++)
			   	          	{
								std::string buffer = ihandler.SRTP_Receive(connectionId);

				                if(!(buffer.compare(0,16,"Upload filename: ")))
				                {
				                   std::string filename = buffer.substr(17,(buffer.size()));
				                   input_file.open(filename.c_str());
				                   flag = 1;
				                }
				                else if(!(buffer.compare(0,25,"*****End of the File*****")))
				                {
				                    input_file.close();
				                }
				                else if(!(buffer.compare(0,19,"Download filename: ")))
				                {
									std::string filename = buffer.substr(20,(buffer.size()));
									ifstream output_file(filename.c_str());

									if(output_file.is_open())
									{
										std::string line, remainder;
										while(!output_file.eof())
										{
											getline(output_file, line);

											if(!remainder.empty())
											{
												line = remainder + line;  
											}

											int size = line.size() -1;
											while (size > MSS)
											{
												std::string substring = line.substr(0,MSS);
												ohandler.SRTP_Send(connectionId, substring);
												remainder  = line.substr(MSS,size-MSS);
												size = remainder.size();
												line = remainder;
											}

				        				  	remainder = line;
					               		} 
				        		       	if(!remainder.empty())
				        		       	{
					                   	  ohandler.SRTP_Send(connectionId, remainder);  
				        		       	}
					                	// End of the file-- notification
				        		       	line = "*****End of the File*****";
						               	ohandler.SRTP_Send(connectionId, line);
				        		       	output_file.close();
					   			 	} // end of if
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

				SRTP_Connection conn(local_addr , local_port ,&in_buffer ,&out_buffer, remote_addr, remote_port);

				int connection_number = conn.SRTP_Open();
				if(connection_number < 0)
				{
					throw "Application Connection Error : Client failed";
				}
				else
				{
					std::clog << "INFO:\t Client connected successfully"<< std::endl;
				}

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
								                             
				if(!strcmp(argv[6],"-uf"))
				{
					std::string filename = argv[7];
					ifstream output_file(filename.c_str());
					std::string tmp_packet("Upload filename: ");
					tmp_packet = tmp_packet + filename;
					ohandler.SRTP_Send(connection_number, tmp_packet);

					if(output_file.is_open())
					{
					   std::string line, remainder;
					while(true){
					   while(!output_file.eof())
					   {
						  getline(output_file, line);
						  if(!remainder.empty())
							  line = remainder + line;  
						  int size = line.size() -1;
						  while (size > MSS)
						  {
							  std::string substring = line.substr(0,MSS);
							  ohandler.SRTP_Send(connection_number, substring);
							  remainder  = line.substr(MSS,size-MSS);
							  size = remainder.size();
							  line = remainder;
						  }
				
						   remainder = line;
					   } 
						if(!remainder.empty())
						{
							 ohandler.SRTP_Send(connection_number, remainder);  
						}
						// End of the file-- notification
						line = "*****End of the File*****";
						ohandler.SRTP_Send(connection_number, line);
						output_file.close();
						}
					}
					else      
					{  
						std::clog << "File Open error";         
						exit(1);
					}
				}
				else if(!strcmp(argv[6],"-df"))
				{
					std::string filename = argv[7];
					ofstream input_file(filename.c_str());
					ohandler.SRTP_Send(connection_number, "Download filename: "+filename);
					while(true)
					{  			
						std::string buffer = ihandler.SRTP_Receive(connection_number);
					   	if(input_file.is_open())
					   	{
							input_file << buffer;
					   	}            
					   	else 
					   	{
							std::clog << "File Open error";         
							exit(1);
					   	}
					   	if (!strcmp(buffer.c_str(),"*****End of the File*****"))
						{
				 			break;
						}
					} // end of while
					input_file.close();
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
}

