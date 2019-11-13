#FTP_application.make

ftp: FTP_application.o SRTP_ConnectionTable.o SRTP_ConnectionStatusTable.o \
SRTP_PeerTable.o SRTP_PeerInfo.o SRTP_Packet.o SRTP_ListenerThread.o SRTP_ConnectorThread.o \ SRTP_ReceiverThread.o SRTP_SenderThread.o SRTP_CommonFunctions.o SRTP_GlobalVariables.o \ SRTP_IncomingBuffer.o SRTP_OutgoingBuffer.o SRTP_OutgoingMessageHandler.o \ SRTP_ConnectionInfo.o  SRTP_IncomingMessageHandler.o SRTP_Connection.o 	
	g++ -Wall -lpthread FTP_application.o SRTP_ConnectionTable.o \
	SRTP_ConnectionStatusTable.o SRTP_PeerTable.o SRTP_PeerInfo.o SRTP_Packet.o \
	SRTP_ListenerThread.o SRTP_ConnectorThread.o SRTP_ReceiverThread.o SRTP_SenderThread.o \
	SRTP_CommonFunctions.o SRTP_GlobalVariables.o  SRTP_IncomingBuffer.o \
	SRTP_OutgoingBuffer.o SRTP_OutgoingMessageHandler.o SRTP_ConnectionInfo.o \
	SRTP_IncomingMessageHandler.o SRTP_Connection.o -o ftp

FTP_application.o: FTP_application.cpp SRTP_CommonTypes.h \
SRTP_GlobalVariables.h SRTP_Packet.h SRTP_OutgoingMessageHandler.h \
SRTP_IncomingMessageHandler.h SRTP_IncomingBuffer.h SRTP_OutgoingBuffer.h \
SRTP_Connection.h SRTP_ConnectionTable.h SRTP_PeerTable.h
	g++ -g -c FTP_application.cpp

SRTP_ConnectionTable.o: SRTP_ConnectionTable.cpp SRTP_ConnectionTable.h SRTP_CommonTypes.h
	g++ -g -c SRTP_ConnectionTable.cpp

SRTP_ConnectionStatusTable.o: SRTP_ConnectionStatusTable.cpp SRTP_ConnectionStatusTable.h \
SRTP_CommonTypes.h
	g++ -g -c SRTP_ConnectionStatusTable.cpp

SRTP_PeerTable.o: SRTP_PeerTable.cpp SRTP_PeerTable.h SRTP_CommonTypes.h
	g++ -g -c SRTP_PeerTable.cpp

SRTP_PeerInfo.o: SRTP_PeerInfo.cpp SRTP_PeerInfo.h SRTP_CommonTypes.h
	g++ -g -c SRTP_PeerInfo.cpp

SRTP_Packet.o: SRTP_Packet.cpp SRTP_Packet.h SRTP_CommonTypes.h
	g++ -g -c SRTP_Packet.cpp

SRTP_ListenerThread.o: SRTP_ListenerThread.cpp SRTP_ListenerThread.h SRTP_CommonTypes.h \
SRTP_GlobalVariables.h SRTP_Packet.h SRTP_ConnectionTable.h SRTP_ConnectionInfo.h \
SRTP_ConnectionStatusTable.h SRTP_PeerInfo.h SRTP_PeerTable.h SRTP_CommonFunctions.h \
SRTP_ReceiverThread.h SRTP_SenderThread.h
	g++ -g -c SRTP_ListenerThread.cpp

SRTP_ConnectorThread.o: SRTP_ConnectorThread.cpp SRTP_ConnectorThread.h SRTP_CommonTypes.h \
SRTP_CommonFunctions.h SRTP_GlobalVariables.h SRTP_ReceiverThread.h SRTP_SenderThread.h \
SRTP_ConnectionStatusTable.h SRTP_PeerTable.h SRTP_ConnectionTable.h SRTP_ConnectionInfo.h
	g++ -g -c SRTP_ConnectorThread.cpp

SRTP_ReceiverThread.o: SRTP_ReceiverThread.cpp SRTP_ReceiverThread.h SRTP_CommonTypes.h \
SRTP_CommonFunctions.h SRTP_GlobalVariables.h SRTP_ConnectionStatusTable.h SRTP_PeerTable.h \
SRTP_ConnectionTable.h SRTP_ConnectionInfo.h
	g++ -g -c SRTP_ReceiverThread.cpp

SRTP_SenderThread.o: SRTP_SenderThread.cpp SRTP_SenderThread.h SRTP_CommonTypes.h \
SRTP_CommonFunctions.h SRTP_GlobalVariables.h SRTP_ConnectionStatusTable.h SRTP_PeerTable.h \
SRTP_ConnectionTable.h
	g++ -g -c SRTP_SenderThread.cpp

SRTP_CommonFunctions.o: SRTP_CommonFunctions.cpp SRTP_CommonFunctions.h SRTP_CommonTypes.h
	g++ -g -c SRTP_CommonFunctions.cpp

SRTP_GlobalVariables.o: SRTP_GlobalVariables.cpp SRTP_GlobalVariables.h SRTP_CommonTypes.h
	g++ -g -c SRTP_GlobalVariables.cpp

SRTP_IncomingBuffer.o: SRTP_IncomingBuffer.cpp SRTP_IncomingBuffer.h SRTP_CommonTypes.h
	g++ -g -c SRTP_IncomingBuffer.cpp

SRTP_OutgoingBuffer.o: SRTP_OutgoingBuffer.cpp SRTP_OutgoingBuffer.h SRTP_CommonTypes.h \
SRTP_SenderThread.h
	g++ -g -c SRTP_OutgoingBuffer.cpp 

SRTP_OutgoingMessageHandler.o: SRTP_OutgoingMessageHandler.cpp SRTP_OutgoingMessageHandler.h \
SRTP_CommonTypes.h SRTP_GlobalVariables.h SRTP_ConnectionTable.cpp
	g++ -g -c SRTP_OutgoingMessageHandler.cpp

SRTP_ConnectionInfo.o: SRTP_ConnectionInfo.cpp SRTP_ConnectionInfo.h SRTP_CommonTypes.h
	g++ -g -c SRTP_ConnectionInfo.cpp

SRTP_IncomingMessageHandler.o: SRTP_OutgoingMessageHandler.cpp SRTP_OutgoingMessageHandler.h \
SRTP_CommonTypes.h
	g++ -g -c SRTP_IncomingMessageHandler.cpp

SRTP_Connection.o: SRTP_Connection.cpp SRTP_Connection.h SRTP_CommonTypes.h \
SRTP_ConnectorThread.h SRTP_ListenerThread.h SRTP_ConnectionStatusTable.h \
SRTP_ConnectionTable.h SRTP_PeerTable.h SRTP_GlobalVariables.h
	g++ -g -c SRTP_Connection.cpp

clean:
	rm -rf client_log.txt server_log.txt FTP_application.o SRTP_ConnectionTable.o SRTP_ConnectionStatusTable.o SRTP_PeerInfo.o SRTP_Packet.o SRTP_ListenerThread.o SRTP_ConnectorThread.o SRTP_ReceiverThread.o SRTP_SenderThread.o SRTP_CommonFunctions.o SRTP_GlobalVariables.o SRTP_IncomingBuffer.o SRTP_OutgoingBuffer.o  SRTP_OutgoingMessageHandler.o SRTP_ConnectionInfo.o SRTP_IncomingMessageHandler.o SRTP_Connection.o

