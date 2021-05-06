# cisc450_project2

Due Date: 	May 7th, 2021
Creators:	Casey Rock & Malachi Parks
Section:	CISC450-010
Assignment:	Programming Assignment 2


**Assignment Details:**

We were tasked with implementing a client and server model which will utilize
UDP to transmit a file. This file will be moved from the server to the client.
In this assignment we are allowed to choose any port number that will not conflict
with exisiting port numbers or ports numbers used by our peers. Additionally, UDP will
be built with reliable data transfer with sequence numbers + ack numbers.


**Steps For Assignment:**

1) Server waits for UDP request from client
2) Client prompts user for name of file to be transfered
3) Client sends name of file to the UDP server
5) Server reads file and sends it back to the client
6) Client recieves file and stores it at 'out.txt'
7) One transfer is complete server and client kill the connection


**Transmission Information:**

* Server constructs packets by reading in lines one at a time from an input file
	- Each line in the input file has sequence of printable chars with 
	80 chars per line. "Newline" char is transmitted along with with other bits
	- Transmits each line in a seperate packet
* Client receives packets and puts them into distinct lines in output file


**Data Packet Information:** (Add info on UDP of ack num)

	Count - 2 bytes
	Packet Sequence Number - 2 bytes
	Data - 0-80 bytes

* Each data packet has 4-byte long header of (Count + Seq #)
	- Must convert values in fields into network byte order when transmitted
	- Convert to host byte order when recieved by client
* Count represents # of chars in packet, ranges from 0-80, 0 means no data
* Packet Sequence Number is the packet id swapping between 0
and 1 for alternating bit protocol


**Transmitting Information:**
* When server done transmitting sends EOT packet with count 0 and next seq num of 0 or 1
* When client recieves EOT from server closes output file and terminates
* Filename sent from client will only contain data chars, no new line esc char
	- When sent count is # of data chars
	- Packet seq number for filename is 0
* Sends one packet at a time via
sendto and receiveffrom


**ACK Information:**
* After sending data packet server waits for an ACK from client.
	- ACK Seq Num - 2 bytes
* Ack seq number alternates between
0 and 1


**Simulation of Loss: SimulateLoss**

* SimulateLoss: Loss in server's transmission of data pkt to client
* Uses _Packet_ _Loss_ _Ratio_ param
* Creates random number between 0-1
* rand() < _Packet_ _Loss_ _Ratio_  return 1, else 0
* Runs before server sends data to client, 1 = transmit, 0 = don't transmit


**Simulation of Loss: SimulateACKLoss**

* Simulates loss for ACK's
* Uses _ACK_ _Loss_ _Ratio_ param
* Creates random number between 0-1
* rand() < _ACK_ _Loss_ _Ratio_ return 1, else 0
* Calls before client transmits an ACK to the server, 0 = transmit normally, 1 = ACK Loss


**Client and Server Input Parameters**

_Client:_ When client starts give following config of params to user
	- Input File Name: Name of input file sent from the client to server
	- ACK Loss Ratio: A real num btwn 0-1

_Server:_ When server starts give following config of params to user
	- Timeout: User enters int value n btwn 1-10, timeout value is then stored a 10^n microseconds
	- Packet Loss Ratio: A real num btwn 0-1


**Output of Server before EOT**

* When a data packet numbered n is generated for transmission by the server for the first time:
Packet n generated for transmission with c data bytes

* When a data packet numbered n is generated for re-transmission by the server:
Packet n generated for re-transmission with c data bytes

* When a data packet numbered n is actually transmitted by the server:
Packet n successfully transmitted with c data bytes

* When a data packet numbered n is generated, but is dropped because of loss:
Packet n lost

* When an ACK is received with number n:
ACK n received

* When a timeout expires:
Timeout expired for packet numbered n

* When the “End of Transmission” packet is sent:
End of Transmission Packet with sequence number n transmitted


**Output of Client before EOT**

* When a data packet numbered n is received by the client for the first time:
Packet n received with c data bytes

* When a data packet numbered n is received by the client, but is a duplicate packet:
Duplicate packet n received with c data bytes

* When contents of data packet numbered n are delivered to the user, i.e., are stored in the output file:
Packet n delivered to user

* When an ACK with number n is generated for transmission:
ACK n generated for transmission

* When an ACK is actually transmitted with number n:
ACK n successfully transmitted

* When an ACK is generated, but is dropped because of loss:
ACK n lost

* When the “End of Transmission” packet is received:
End of Transmission Packet with sequence number n received


**Server Stats Displayed After EOT**

1. Number of data packets generated for transmission (initial transmission only)
2. Total number of data bytes generated for transmission, initial transmission only (this should be the sum of the count fields of all packets generated for transmission for the first time only)
3. Total number of data packets generated for retransmission (initial transmissions plus retransmissions)
4. Number of data packets dropped due to loss
5. Number of data packets transmitted successfully (initial transmissions plus retransmissions)
6. Number of ACKs received
7. Count of how many times timeout expired

**Client Stats Displayed After EOT**
1. Total number of data packets received successfully
2. Number of duplicate data packets received)
3. Number of data packets received successfully, not including duplicates
4. Total number of data bytes received which are delivered to user (this should be the sum of the count fields of all received packets not including duplicates)
5. Number of ACKs transmitted without loss
6. Number of ACKs generated but dropped due to loss
7. Total number of ACKs generated (with and without loss)


###############################################################################################################

**Basic Testing Cases:**
* Packet and ACK loss rates 0, Timeout value n = 5
* Packet loss rate 0.2, ACK loss rate 0, Timeout value n = 5.
* Packet loss rate 0, ACK loss rate 0.2, Timeout value n = 5.
* Packet loss rate 0.2, ACK loss rate 0, Timeout value n = 4.


**Directory Descriptions**

* _Server:_ Used to house all server files such as test.txt and the UDPServer.c file
* _Client:_ Used to house all Client files such as UDPClient.c


**File Descriptions**

* _UDP.h:_ Houses info shared between two files such as pkt struct, ack struct and necessary header files
* _UDPClient.c:_ The client side of the UDP connection.
* _UDPserver.c:_ The server side of the UDP connection.
* _test.txt:_ The test file used to to create output.txt
* _Makefile:_ Makefile used to easily compile and remove executables


**Compilation Instructions**

Since a makefile was created, only one cmd to compile code:

make all


**Running Instructions**

Two executables will be created in the main directory:
	Server.exec: Takes in no args, runs the server side of the UDP connection
	Client.exec: Takes in no args, runs the client side of the UDP connection

TWO TERMINALS MUST BE RUN AT THE SAME TIME:

0. Ensure you have two seperate cmd terminals running
1. On the first terminal enter "./server.exec"
2. User will be prompted with "Timeout:"
	 - Enter a number between 1-10
3. User will be prompted with "Packet Loss Ratio:", 
	 - Enter a number between 0-1 inclusive
4. Swap to the second terminal not in use
5. On the second terminal enter "./client.exec"
6. On the client side user will be prompted "Input File Name"
	 - Enter "test.txt" as the file name without the paraenthesis
7. On the client side user will be prompted "Ack Loss Ratio"
	 - Enter a number between 0-1 inclusive
8. Program will run the create output.txt
	 - Read output.txt and compare to test.txt located under dir Server

Note: Under extreme tests may get stuck on client side, if so try again until client statistics are printed