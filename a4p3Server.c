#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>      
#include <stdlib.h> 
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
/*
    CONCURRENT SERVER: THREAD EXAMPLE
    Must be linked with the "pthread" library also, e.g.:
       cc -o example example.c -lnsl -lsocket -lpthread 

    This program creates a connection socket, binds a name to it, then
    listens for connections to the sockect.  When a connection is made,
    it accepts messages from the socket until eof, and then waits for
    another connection...

    This is an example of a CONCURRENT server -- by creating threads several
    clients can be served at the same time...

    This program has to be killed to terminate, or alternately it will abort in
    120 seconds on an alarm...
*/

char result[5000];

sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";

#define PORTNUMBER 10035


struct serverParm {
           int connectionDesc;
       };

/*static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   strncat(result, argv[0], (sizeof(result) - strlen(result)));
   return 0;
}
*/
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void *serverThread(void *parmPtr) {

#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[1025];

    /* Server thread code to deal with message processing */
    printf("DEBUG: connection made, connectionDesc=%d\n",
            PARMPTR->connectionDesc);
    if (PARMPTR->connectionDesc < 0) {
        printf("Accept failed\n");
        return(0);    /* Exit thread */
    }
    

    /* Receive messages from sender... */
    while ((recievedMsgLen=
            read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0) 
    {
        recievedMsgLen[messageBuf] = '\0';
        printf("%s\n", messageBuf);
        sqlite3_open("book.db", &db);
        rc = sqlite3_exec(db, messageBuf, callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Operation done successfully\n");
   }
   sqlite3_close(db);
   FILE * fp;
   /* open the file for writing*/
   fp = fopen ("a4p3ServerLog.txt","w");
 
   /* write 10 lines of text into the file stream*/
       fprintf (fp, "Client Access PID: 3536 Thread: 0 \n");
   
 
   /* close the file*/  
   fclose (fp);                   /* And memory leaks */
        printf("Message: %s\n",result);
        if (write(PARMPTR->connectionDesc,"GOT IT\0",7) < 0) {
               perror("Server: write error");
               return(0);
           }
    }
    close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
    free(PARMPTR);
    
    return(0);                       /* Exit thread */
}



main () {
    int listenDesc;
    struct sockaddr_in myAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;

    /* For testing purposes, make sure process will terminate eventually */
    alarm(120);  /* Terminate in 120 seconds */

    /* Create socket from which to read */
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("open error on socket");
        exit(1);
    }

    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons(PORTNUMBER);
        
    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
        perror("bind error");
        exit(1);
    }

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    listen(listenDesc,5);

    while (1) /* Do forever */ {
        /* Wait for a client connection */
        connectionDesc = accept(listenDesc, NULL, NULL);

        /* Create a thread to actually handle this client */
        parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
        parmPtr->connectionDesc = connectionDesc;
        if (pthread_create(&threadID, NULL, serverThread, (void *)parmPtr) 
              != 0) {
            perror("Thread create error");
            close(connectionDesc);
            close(listenDesc);
            exit(1);
        }

        printf("Parent ready for another connection\n");
    }

}