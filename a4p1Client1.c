#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 10035 /*port*/

int
main(int argc, char **argv) 
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];

 // alarm(300);  // to terminate after 300 seconds
  
 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=2) {
  perror("Usage: TCPClient <Server IP> <Server Port>"); 
  exit(1);
 }
  
 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }
  
 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 // servaddr.sin_port =  htons(argv[2]); 
 servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
  
 //Connection of the client to the socket 
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }
  
 while (fgets(sendline, MAXLINE, stdin) != NULL) {
  
    FILE * fp;
   /* open the file for writing*/
   fp = fopen ("a4p1Client1Log.txt","w");
 
   /* write 10 lines of text into the file stream*/
       fprintf (fp, "Client Access PID: 3875 Thread: 0 \n");
   
 
   /* close the file*/  
   fclose (fp);  
  send(sockfd, sendline, strlen(sendline), 0);
    
  if (recv(sockfd, recvline, MAXLINE,0) == 0){
   //error: server terminated prematurely
   perror("The server terminated prematurely"); 
   exit(4);
  }
  printf("%s", "String received from the server: ");
  sleep(3);
  fputs(recvline, stdout);
 }

 exit(0);
}