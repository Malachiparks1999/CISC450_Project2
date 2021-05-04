/*
Due Date:       April 5th, 2021
Creators:       Casey Rock & Malachi Parks
Section:        CISC450-010
Assignment:     Programming Assignment 1

File Description: UDP server 
*/

#include "../UDP.h"



int main(int argc, char const * argv[]){
    struct sockaddr_in serverAddr;
    int clientSocketID ;
    char filename[MAX_FILE_NAME];
    struct packet recvPacket;
    struct packet endFile ;
    const char outputFile[] = "output.txt";
    int len;
    int fd ;
    int endConnections = 1;
    const char echoStr[] = "Echo";
    int totalBytes = 0;

    //Name of file client wants server to transmit back
    printf( "Enter a file name : ");
    fgets(filename, MAX_FILE_NAME, stdin);
    len = strlen(filename);
    filename[len - 1] = '\0';


    //Establishes connection
    if((clientSocketID = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))< 0){
        perror("Client Socket Failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET ;  // Obtaining IP Address, ensuring in IPv4 family
    serverAddr.sin_port = htons(PORT); //Changing to host byte order to network byte order

    //Ensure the IP address is IPV4
    if(inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    

   // If client cannot send to server kick back error
   //sendT0
   if(send(clientSocketID , filename , len , 0 ) < 0){
        perror("Client Send Failed");
        exit(EXIT_FAILURE);
   };

   // While end connection is not 0, recieve info from server
   while(endConnections){
       //recvFrom
        if(recv(clientSocketID,&recvPacket, MAX_FILE_NAME, 0 ) < 0){
            perror("Server Accept Failed");
            exit(EXIT_FAILURE);
        }

	// if file doesn't exist create else write only and append it
        fd = open(outputFile, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU) ;
        write(fd, recvPacket.data, strlen(recvPacket.data));

	// Track number  of packet and amount of data sent with each
        printf("Packet: %d received with %d data bytes.\n",recvPacket.pktSequenceNumber, recvPacket.count);
        totalBytes += recvPacket.count ;

        //wait for server to send whole file 
        //sendTO
        if(send(clientSocketID , echoStr , strlen(echoStr) , 0 ) < 0){
            perror("Client Send Failed");
            exit(EXIT_FAILURE);
        };

	// If the count of data is empty then break out of loop
        if(recvPacket.count == 0 ) {
            endConnections = 0;
            break;
        }
   } 

   // Recieve end of file, break immediatly
   //RecvFrom
   if(recv(clientSocketID,&endFile, MAX_FILE_NAME, 0 ) < 0){
            perror("Server Accept Failed");
            exit(EXIT_FAILURE);
    }

    printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", recvPacket.pktSequenceNumber, recvPacket.count);

    printf("\nNumber of data packets received: %d \n", recvPacket.pktSequenceNumber);
    printf("Total number of data bytes received: %d \n", totalBytes);
   

    //close connection between server and host
    close(fd);
    close(clientSocketID);
    exit(0);

}
