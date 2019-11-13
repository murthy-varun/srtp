This code is compilable in Ubuntu Linux 2.6.27-7 generic

------------------------------------------------------------------------------
Instructions:
------------------------------------------------------------------------------
1. unzip the source code
   a) gunzip SRTP.tar.gz
   b) tar -xvf SRTP.tar
   The source code should be extracted to a directory 'SRTP'

2. compile the code using the command
   make ftp
   make chat

   The two execuables ftp and chat will be created

3. To Run FTP
   a) Open two terminals - one server and one client
   b) run the following command on the server
	./ftp -s <IP address of server> < server Port> -m <MSS> -b <Buffer Size>
   c)run the following command on the client
	 ./ftp -c <IP address of Client> <Client Port> <IP address of server> <Server Port> [-uf|-df] <filename> -m <MSS> -b <Buffer Size>

4. To run chat
   a) Open two terminals for clients

   b) On each client run the following command

	./chat <local IP address> <local Port> -m <MSS> -b <Buffer Size>

   c) On one terminal choose to listen and the other choose option 1 to connect to listening client and give its IP address and port


------------------------End of Instructions--------------------------------------