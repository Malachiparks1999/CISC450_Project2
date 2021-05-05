/*
Due Date:       May 7th, 2021
Creators:       Casey Rock & Malachi Parks
Section:        CISC450-010
Assignment:     Programming Assignment 2

File Description: UDP server 
*/

#include "../UDP.h"

//Simulates packet loss where packet never reaches reciever
int simulate_loss(double PKTLossRate);

int simulate_loss(double PKTLossRate){
	/*
	Configured using the parameter PKT Loss Ratio. Causes 
	the client not to transmit an PKT when a loss is indicated.
		Generate a uniformly distribute random number between 0 and 1
		PotentioanLoss <  PKT LOSS RATIO return 1, else return 0
	*/
	double potentialLoss = rand()/(RAND_MAX + 1);	//Creates a random number between 0-1
	if(potentialLoss < PKTLossRate){
		return 1;
	else{
		return 0;
}//simulate_loss

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
    const char ackStr[] = "ACK";
    int totalBytes = 0;

    //used for packet lost
    int lostPkts = 0;
    double packet_loss_rate;

    // User inputs the packet lost rate
    printf("Please enter the Packet Loss Rate. This number must between 0 and 1. Ex: .73\n");
    scanf("%lf",&packet_loss_rate);
    
    /* cant get this to work correctly to ensure it's a float between the two values
    while(packet_loss_rate >= 1 || packet_loss_rate =< 0){
	printf("Entry invalid: Number was not between 0 and 1. Try Again\n");
    }
    */

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

	// if file doesn't exist create else write only and append it
        fd = open(outputFile, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU) ;
        write(fd, recvPacket.data, strlen(recvPacket.data));

	// Track number  of packet and amount of data sent with each
        printf("Packet: %d received with %d data bytes.\n",recvPacket.pktSequenceNumber, recvPacket.count);
        totalBytes += recvPacket.count ;

        //wait for server to send whole file 
        //sendTO
        if(sendto(clientSocketID , filename , len , 0 ,
                (struct sockaddr * )  &serverAddr, serverAddrLen) < 0){
            perror("Client SendTo ACK Failed");
            exit(EXIT_FAILURE);
        };

	// If the count of data is empty then break out of loop
        if(recvPacket.count == 0 ) {
            endConnections = 0;
            break;
        }

        
   } 

//    // Recieve end of file, break immediatly
//    //RecvFrom
//    if(recv(clientSocketID,&endFile, MAX_FILE_NAME, 0 ) < 0){
//             perror("Server Accept Failed");
//             exit(EXIT_FAILURE);
//     }

//     printf("End of Transmission Packet with sequence number %d received with %d data bytes\n", recvPacket.pktSequenceNumber, recvPacket.count);

//     printf("\nNumber of data packets received: %d \n", recvPacket.pktSequenceNumber);
//     printf("Total number of data bytes received: %d \n", totalBytes);
   

    //close connection between server and host
    close(fd);
    close(clientSocketID);
    exit(0);

}
