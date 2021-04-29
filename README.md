# cisc450_project1

Due Date: 	April 5th, 2021
Creators:	Casey Rock & Malachi Parks
Section:	CISC450-010
Assignment:	Programming Assignment 1


**Assignment Details:**

We were tasked with implementing a client and server model which will utilize
TCP to transmit a file. This file will be moved from the server to the client.
In this assignment we are allowed to choose any port number that will not conflict
with exisiting port numbers or ports numbers used by our peers.

**Steps For Assignment:**

1) Server waits for TCP request from client
2) Client prompts user for name of file to be transfered
3) Client establishes TCP connection to server
4) Client sends filename to server
5) Server reads file and sends it back to the client
6) Client recieves file and stores it at 'out.txt'
7) One transfer is complete server and client kill the connection


**Transmission Information:**

* Server constructs packets by reading in lines one at a time from an input file
	- Each line in the input file has sequence of printable chars with 
	80 chars per line. "Newline" char is transmitted along with with other bits
	- Transmits each line in a seperate packet
* Client receives packets and puts them into distinct lines in output file

**Data Packet Information:**

	Count - 2 bytes
	Packet Sequence Number - 2 bytes
	Data - 0-80 bytes

* Each data packet has 4-byte long header of (Count + Seq #)
	- Must convert values in fields into network byte order when transmitted
	- Convert to host byte order when recieved by client
* Count represents # of chars in packet, ranges from 0-80, 0 means no data
* Packet Sequence Number is the packet id, incremented by one for each packet
* When server done transmitting sends EOT packet with count 0 and next seq num
* When client recieves EOT from server closes output file and terminates
* Filename sent from client will only contain data chars, no new line esc char
	- When sent count is # of data chars
	- Packet seq number for filename is 0

**TCP Connection Information:**

* Two send operations for client, one for header then one for data for each transsmission
* Same for server two recieve operations, one for header and another for data
* Can use two buffers for this reason


**Output Information:**

* When a new data packet numbered n is sent by server:
	- _Packet_ _n_ _transmitted_ _with_ _c_ _data bytes_

* When a data packet numbered n is recieved by the client
	- _Packet_ _n_ _received_ _with_ _c_ _data_ _bytes_

* When the "End of Transmission" packet is sent:
	- _End_ _of_ _Transmission_ _Packet_ _with_ _sequence_ _number_ _n_ _transmitted_ _with_ _c_ _data bytes_

* When the "End of Transmission" packet is received:
	- _End_ _of_ _Transmission_ _Packet_ _with_ _sequence_ _number_ _n_ _received_ _with_ _c_ _data_ _bytes_

Before terminating execution - following stats should be printed
* For Server:
	-_Number_ _of_ _data_ _packets_ _transmitted_
	-_Total_ _number_ _of_ _data_ _bytes_ _transmitted_ (_this_ _should_ _be_ _the_
	-_sum_ _of_ _the_ _count_ _fields_ _of_ _all_ _transmitted_ _packets_. 

* For Client:
	-_Number_ _of_ _data_ _packets_ _transmitted_
	-_Total_ _number_ _of_ _data_ _bytes_ _received_ (_this_ _should_ _be_ _the_ _sum_
	_of_ _the_ _count_ _fields_ _of_ _all_ _received_ _packets_.

#########################################################################################################################

**Directory Descriptions**

* _Server:_ Used to house all server files such as test.txt and the TCPServer.c file
* _Client:_ Used to house all Client files such as TCPClient.c


**File Descriptions**

* _TCP.h:_ Used to house information shared between two files such as pkt struct and necessary header files
* _TCPClient.c:_ The client side of the TCP connection.
* _TCPserver.c:_ The server side of the TCP connection.
* _test.txt:_ The test file used to to create output.txt
* _Makefile:_ Makefile used to easily compile and remove executables


**Compilation Instructions**

Since a makefile was created, only one cmd to compile code:

make all


**Running Instructions**

Two executables will be created in the main directory:
	Server.exec: Takes in no args, runs the server side of the TCP connection
	Client.exec: Takes in no args, runs the client side of the TCP connection

TWO TERMINALS MUST BE RUN AT THE SAME TIME:
	1) On the first terminal enter "./Server.exec"
	2) On the second terminal enter "./Client.exec"
	3) On the client side user will be prompted "Enter a Filename"
		a) Enter "test.txt" as the file name
	4) Program will run the create output.txt
	5) Read output.txt and compare to test.txt located under dir Server
