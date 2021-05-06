/*
Due Date:       May 7th, 2021
Creators:       Casey Rock & Malachi Parks
Section:        CISC450-010
Assignment:     Programming Assignment 2

File Description: UDP server 
*/

#include "../UDP.h"

//Used to simulate lost ack
int simulate_ACK_loss(double ackLossRate);

int simulate_ACK_loss(double ackLossRate){
	/* Configured using the paramater ACK loss ratio. Causes the 
	   Server not to transmit ack when loss is indicated. Client
	   moves on as if nothing happens.
		Generate a uniformly distributed random num between 0 and 1
		potentialLoss < ACKLossRate return 1, else return 0
	*/
	double potentialLoss = (rand()/RAND_MAX + 1.);
	if(potentialLoss < ackLossRate){
		return 1;
    }
	else{
		return 0;
	}
}//simulate_ACK_loss

int main(int argc, char const * argv[]){
    struct sockaddr_in serverAddr, clientAddr ;
    socklen_t clientAddrLen = sizeof(clientAddr); 
    int serverSocketID ;
    

    char recvBuff[MAX_FILE_NAME];
    char filepath[MAX_FILE_NAME]= "./Server/" ;
    FILE *fp ;
    char fileBuffer[MESSMAX];
    struct packet sendPacket;
    struct packet final ;
    int sequenceNumberCount = 0;
    int totalBytes = 0;
    char ack[MESSMAX];
    int recvMesgSize;
    int timeoutSec;
    double packet_loss_ratio;
    
     //Name of file client wants server to transmit back
    printf( "Timeout:  ");
    fscanf(stdin, "%d", &timeoutSec);
    fflush(stdin);
    
    // User inputs the packet lost rate
    printf("Packet Loss Ratio: ");
    fscanf(stdin, "%lf", &packet_loss_ratio);

    
    // Create a socket, if failed exit with failure
    if((serverSocketID = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))< 0){
        perror("Server Socket Failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));
 
    serverAddr.sin_family = AF_INET ;  // IPv4 Family
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Make sure it can take any IPv4 address
    serverAddr.sin_port = htons(PORT); // Change for network byte order to host byte order
    
  
    // Bind a socket to server
    if(bind(serverSocketID, (struct sockaddr * ) &serverAddr, sizeof(serverAddr)) < 0){
        perror("Server Bind Failed");
        exit(EXIT_FAILURE);
    }

    // File name recieved 0 then kill connection
    // recvFrom Server
    if(recvfrom(serverSocketID,recvBuff, MAX_FILE_NAME, 0,
                            (struct sockaddr * ) &clientAddr, &clientAddrLen) < 0){
        perror("Server Recvfrom Failed");
        exit(EXIT_FAILURE);
    }

    // Concat buff to filepath so file can be found in Server dir
    strcat(filepath,recvBuff) ;
    
    // If can't open then push out error
    if((fp = fopen(filepath, "r")) < 0){
        perror("Failed to open");
        exit(EXIT_FAILURE);
    };
   
    printf("%s\n", filepath);
    //clear buffer of received file
    memset(recvBuff, 0, sizeof recvBuff);

    if(fp > 0){ // make sure file exists, so it doesn't kick back seg fault
        while(fread(sendPacket.data, MESSMAX, 1, fp)){
            sequenceNumberCount++;
            sendPacket.pktSequenceNumber = sequenceNumberCount; // this needs to be a VAR 
            sendPacket.count = sizeof(sendPacket.data) + 4; 
            totalBytes += sendPacket.count ;

	        // Send packet to client
            printf("Packet: %d transmitted with %d data bytes \n", sendPacket.pktSequenceNumber, sendPacket.count);
            if(sendto(serverSocketID, &sendPacket,sendPacket.count , 0,
                        (struct sockaddr * ) &clientAddr, clientAddrLen) < 0 ){
                perror("Server Send Failed");
                exit(EXIT_FAILURE);
            };

	        // Ensuring packet recieved, ack same information to Server console
            // recvFrom 
             if(recvfrom(serverSocketID,recvBuff, MAX_FILE_NAME, 0,
                            (struct sockaddr * ) &clientAddr, &clientAddrLen) < 0){
                perror("Server Recvfrom ACK Failed");
                exit(EXIT_FAILURE);
            }

            printf("%s\n", recvBuff);
            // Clear buffer in case
            memset(sendPacket.data, 0, sizeof sendPacket.data);
        }
    }else{
        printf("File does not exist");
    }

    // // Final send of information in pkt
    // sequenceNumberCount++;
    // sendPacket.pktSequenceNumber = sequenceNumberCount; // this needs to be a VAR 
    // sendPacket.count = 0; 
    // totalBytes += sendPacket.count ;
    
    // //sendT0
    // if(send(serverSocketID,&sendPacket, sizeof sendPacket.data + 4, 0) < 0 ){
    //         perror("Server end of transission Send Failed");
    //         exit(EXIT_FAILURE);
    // };

    // printf("End of Transmission Packet with sequence number %d transmitted with %d data bytes \n\n", sequenceNumberCount, sendPacket.count);

    // printf("Number of data packets transmitted: %d \n", sequenceNumberCount);
    // printf("Total number of data bytes transmitted: %d\n", totalBytes) ;
    
    fclose(fp); // close file
    close(serverSocketID); // close Server connection


}
