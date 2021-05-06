/*
Due Date:       May 7th, 2021
Creators:       Casey Rock & Malachi Parks
Section:        CISC450-010
Assignment:     Programming Assignment 2

File Description: UDP server 
*/

#include "../UDP.h"

//Simulates packet loss where packet never reaches reciever
int simulate_ACK_loss(double ACKLossRate);

int simulate_ACK_loss(double ACKLossRate){
	/*
	Configured using the parameter PKT Loss Ratio. Causes 
	the client not to transmit an PKT when a loss is indicated.
		Generate a uniformly distribute random number between 0 and 1
		PotentioanLoss <  PKT LOSS RATIO return 1, else return 0
	*/
	double SuccessfulAck = ((double) rand() / (RAND_MAX));	//Creates a random number between 0-1
	if(SuccessfulAck > ACKLossRate){
		return 1; // transmit Ack
    }
	else{
		return 0; // dron't transmit Ack
    }
}//simulate_ACK_loss


int main(int argc, char const * argv[]){
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    int clientSocketID ;
    char filename[MAX_FILE_NAME];
    struct packet recvPacket;
    struct packet endFile ;
    const char outputFile[] = "output.txt";
    int len;
    int fd ;
    int endConnections = 1;
    struct ack_packet ack;
    int ackSize = sizeof(ack);
    int totalBytes = 0;

    // Seeding random num generator for AckLoss
    srand(time(NULL));

    //used for packet lost
    int lostPkts = 0;
    double ack_loss_rate;

    
    //Name of file client wants server to transmit back
    printf( "Input File Name:  ");
    fgets(filename, MAX_FILE_NAME, stdin);
    len = strlen(filename);
    filename[len - 1] = '\0';
    fflush(stdin);
    // User inputs the packet lost rate
    printf("ACK Loss Ratio:");
    fscanf(stdin, "%lf", &ack_loss_rate);
    
  

    


    //Establishes connection
    if((clientSocketID = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))< 0){
        perror("Client Socket Failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET ;  // Obtaining IP Address, ensuring in IPv4 family
    serverAddr.sin_port = htons(PORT); //Changing to host byte order to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY ;
    

   // If client cannot send to server kick back error
   //sendT0
   if(sendto(clientSocketID , filename , len , 0 ,
                (struct sockaddr * )  &serverAddr, serverAddrLen) < 0){
        perror("Client Send Failed");
        exit(EXIT_FAILURE);
   };

   // While end connection is not 0, recieve info from server
   while(endConnections){
       //recvFrom
        if(recvfrom(clientSocketID,&recvPacket, MAX_FILE_NAME, 0,
                    (struct sockaddr *) &serverAddr, &serverAddrLen) < 0){
            perror("Client RecvFrom Failed");
            exit(EXIT_FAILURE);
        }
        printf("Packet: %d received with %d data bytes.\n",recvPacket.pktSequenceNumber, recvPacket.count);

	// if file doesn't exist create else write only and append it
        fd = open(outputFile, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU) ;
        write(fd, recvPacket.data, strlen(recvPacket.data));
        printf("Packet %d delivered to user\n",recvPacket.pktSequenceNumber);

	// Track number  of packet and amount of data sent with each
        
        totalBytes += recvPacket.count ;

        //wait for server to send whole file 
        //sendTO
        printf("ACK %d generated for transmission\n", ack.sequenceNum);
        if(simulate_ACK_loss(ack_loss_rate)){
                
                if(sendto(clientSocketID , &ack , ackSize , 0 ,
                    (struct sockaddr * )  &serverAddr, serverAddrLen) < 0){
                    perror("Client SendTo ACK Failed");
                    exit(EXIT_FAILURE);
                };

            } else{
                printf("ACK %d lost\n", ack.sequenceNum);
                
               if(recvfrom(clientSocketID,&recvPacket, MAX_FILE_NAME, 0,
                        (struct sockaddr *) &serverAddr, &serverAddrLen) < 0){
                perror("Client RecvFrom Failed");
                exit(EXIT_FAILURE);
                }
                printf("Duplicate packet %d received with c data bytes\n",recvPacket.pktSequenceNumber);

                if(sendto(clientSocketID , &ack , ackSize , 0 ,
                    (struct sockaddr * )  &serverAddr, serverAddrLen) < 0){
                    perror("Client SendTo ACK Failed");
                    exit(EXIT_FAILURE);
                };
            }
            printf("ACK %d successfully transmitted\n", ack.sequenceNum);
        



        if(ack.sequenceNum == 0)
            ack.sequenceNum = 1;
        else
            ack.sequenceNum = 0;
        

	// If the count of data is empty then break out of loop
        if(recvPacket.count == 0 ) {
            endConnections = 0;
            break;
        }

        
   } 


    printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", recvPacket.pktSequenceNumber, recvPacket.count);

    //close connection between server and host
    close(fd);
    close(clientSocketID);
    exit(0);

}
