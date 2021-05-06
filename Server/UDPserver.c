/*
Due Date:       May 7th, 2021
Creators:       Casey Rock & Malachi Parks
Section:        CISC450-010
Assignment:     Programming Assignment 2

File Description: UDP server 
*/

#include "../UDP.h"

//Used to simulate lost ack
int simulate_loss(double packetLossRate);

int simulate_loss(double packetLossRate){
	/* Configured using the paramater ACK loss ratio. Causes the 
	   Server not to transmit ack when loss is indicated. Client
	   moves on as if nothing happens.
		Generate a uniformly distributed random num between 0 and 1
		packetSuccessRate < packetLossRate return 1, else return 0
	*/
     
	double packetSuccessRate = ((double) rand()  / (RAND_MAX));
	if(packetSuccessRate > packetLossRate){
		return 1; //transmit 
    }
	else{
		return 0; // don't transmit
	}
}//simulate_loss

int main(int argc, char const * argv[]){
    struct sockaddr_in serverAddr, clientAddr ;
    struct ack_packet ack;
    struct timeval read_timeout;
    int ackSize = sizeof(ack);
    ack.sequenceNum = 0;
    socklen_t clientAddrLen = sizeof(clientAddr); 
    int serverSocketID ;
    srand(time(NULL));
    char recvBuff[MAX_FILE_NAME];
    char filepath[MAX_FILE_NAME]= "./Server/" ;
    FILE *fp ;
    struct packet sendPacket;
    struct packet final ;
    int sequenceNumberCount = 0;
    int totalBytes = 0;
    int timeoutSec;
    double packet_loss_ratio;
    int numRetransPackets = 0;
    int numDroppedPackets = 0;
    int totalNumPackets = 0;
    int numACK = 0;
    int numTimeoutExpires = 0;
    
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
            
            if(simulate_loss(packet_loss_ratio)){
                printf("Packet %d generated for transmission with %d data bytes\n", sendPacket.pktSequenceNumber, sendPacket.count);
                if(sendto(serverSocketID, &sendPacket,sendPacket.count , 0,
                            (struct sockaddr * ) &clientAddr, clientAddrLen) < 0 ){
                    perror("Server Send Failed");
                    exit(EXIT_FAILURE);
                };
                totalNumPackets++;

                
                
            } else{
                printf("Packet %d lost \n",sequenceNumberCount );
                 numDroppedPackets++;
                int temptime = timeoutSec ;
                while(temptime--){
                    wait(NULL);
                    // printf("waiting \n");
                }
                printf("Timeout expired for packet numbered %d\n", sendPacket.pktSequenceNumber);
                numTimeoutExpires++;
                printf("Packet %d generated for re-transmission with %d data bytes\n", sendPacket.pktSequenceNumber, sendPacket.count);
                if(sendto(serverSocketID, &sendPacket,sendPacket.count , 0,
                            (struct sockaddr * ) &clientAddr, clientAddrLen) < 0 ){
                    perror("Server Send Failed");
                    exit(EXIT_FAILURE);
                };
            }
             printf("Packet %d successfully  for re-transmission with %d data bytes\n", sendPacket.pktSequenceNumber, sendPacket.count);
             numRetransPackets++;
             totalNumPackets++;
            //TIME START
            read_timeout.tv_sec = timeoutSec;    
            read_timeout.tv_usec = pow(10.0,timeoutSec);
            setsockopt(serverSocketID, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
            

            while(recvfrom(serverSocketID,&ack, ackSize, 0,
                            (struct sockaddr * ) &clientAddr, &clientAddrLen) < 0){
                
                printf("ACK timeout\n");
                numTimeoutExpires++;
                if(sendto(serverSocketID, &sendPacket,sendPacket.count , 0,
                            (struct sockaddr * ) &clientAddr, clientAddrLen) < 0 ){
                    perror("Server Send Failed");
                    exit(EXIT_FAILURE);
                };        
            }

	        // Ensuring packet recieved, ack same information to Server console
            // recvFrom 
             

            printf("ACK %d received\n",ack.sequenceNum);
            numACK++;
            // Clear buffer in case
            memset(sendPacket.data, 0, sizeof sendPacket.data);
        }
    }else{
        printf("File does not exist\n");
    }

    // // Final send of information in pkt
    int finalSeqNum = sequenceNumberCount ; 
    int finalBytes = totalBytes;
    sequenceNumberCount++;
    sendPacket.pktSequenceNumber = sequenceNumberCount; // this needs to be a VAR 
    sendPacket.count = 0; 
    totalBytes += sendPacket.count ;
    
    //sendT0
   if(sendto(serverSocketID, &sendPacket,sizeof(sendPacket) , 0,
                            (struct sockaddr * ) &clientAddr, clientAddrLen) < 0 ){
                    perror("Server Send Failed");
                    exit(EXIT_FAILURE);
    };

    printf("End of Transmission Packet with sequence number %d transmitted with %d data bytes \n\n", sequenceNumberCount, sendPacket.count);
    
    printf("Number of data packets generated for transmission: %d \n", finalSeqNum);
    printf("Total number of data bytes generated for transmission, initial transmission only: %d \n", finalBytes );
    printf("Total number of data packets generated for retransmission: %d \n", numRetransPackets );
    printf("Number of data packets dropped due to loss: %d \n", numDroppedPackets );
    printf("Number of data packets transmitted successfully: %d \n", totalNumPackets);
    printf("Number of ACKs received: %d \n", numACK);
    printf("Count of how many times timeout expired: %d \n", numTimeoutExpires);

    fclose(fp); // close file
    close(serverSocketID); // close Server connection


}
