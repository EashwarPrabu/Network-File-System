#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>

#define MAX 1000
#define DATAMAX 5000
#define PORT 5000
#define BL 10

//READ STRUCT
struct readargs { 
		char fileName[MAX];
		int count;
};

//WRITE STRUCT
struct writeargs { 
   char fileName[MAX];
   char data[DATAMAX];
};

//RENAME AND REMOVE STRUCT
struct rargs {  
   char from[MAX];
   char to[MAX];
};

void nfsServer(int clientfd) {
	char pathName[MAX];
	int choice;	
	do {
		choice =0;
		read(clientfd, &choice, sizeof(choice));
		switch(choice) {
			case 1: {
				// READ 
				char resBuffer[MAX];
				struct readargs fileRead;
				read(clientfd, &fileRead, sizeof(fileRead));
				FILE* fd = fopen(fileRead.fileName, "r");
				int res = fread(resBuffer, sizeof(char), fileRead.count, fd);
				printf("\nData read from file successfully...");
				printf("\nWritting data's to client!!");
				write(clientfd, resBuffer, sizeof(resBuffer));
				//write(sockfd, resStatus, sizeof(resStatus));
				bzero(resBuffer,sizeof(resBuffer));
				printf("\nRead operation closing!!");
				fclose(fd);
				break;
			}
			case 2: {
				//WRITE
				struct writeargs fileWrite;
				read(clientfd, &fileWrite, sizeof(fileWrite));
				printf("\nThe filename is:%s", fileWrite.fileName);
				printf("\nThe data to be written is:%s", fileWrite.data);
				FILE* fd2 = fopen(fileWrite.fileName, "a");
				fputs(fileWrite.data, fd2);
				printf("\nSuccessfully written!!\n");
				//printf("\nNEW ADDED DATA:%s", dataBuffer);
				fclose(fd2);
				break;
			}
			case 3: {	
				//RENAME
				struct rargs renameFile;
				read(clientfd, &renameFile, sizeof(renameFile));
				int ret = rename(renameFile.from, renameFile.to);
				if(ret == 0) {	
					printf("\nSuccessfully renamed!!");
				} else {	
					fprintf(stderr, "Error renaming %s.\n", renameFile.from);
				}
				break;
			}
			case 4: {
				struct rargs removeFile;
				read(clientfd, &removeFile, sizeof(removeFile));
				int ret = remove(removeFile.from);
				if(ret == 0) {	
					printf("\nSuccessfully removed!!");
				} else {	
					fprintf(stderr, "Error removing %s.\n", removeFile.from);
				}
				break;
			}
			case 5: {
	        	//ATTRIBUTES OF A FILE
	            read(clientfd, pathName, sizeof(pathName));
	            printf("\nThe filename is:%s", pathName);
	            struct stat buff;
	            stat(pathName, &buff);
	            write(clientfd, &buff, sizeof(buff));
	            printf("\nAttributes of the File were successfully written!");
	            bzero(pathName,sizeof(pathName));
	            break;
	        }
		}
	}while(choice!=0);
}

int main() {
	int sockfd, clientfd, len;
	struct sockaddr_in server, client;
	int num, result[MAX];

	// SOCKET CREATION
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printf("Error in creating socket");
		exit(0);
	} else 
		printf("Socket created successfully!");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	// BINDING
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) != 0) {
		printf("\n Error in binding");
		exit(0);
	} else {
		printf("\nBind successful");
	}

	// LISTENING
	if(listen(sockfd, BL) != 0) {
		printf("\n Error in listening");
		exit(0);
	} else {
		printf("\nServer listening......");
	}

	// ACCEPTING CLIENT
	len = sizeof(client);
	clientfd = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&len);
	if(clientfd < 0) {
		printf("\nError in accepting client");
		exit(0);
	} else {
		printf("\nConnection established successfully");
	}

	nfsServer(clientfd);

	close(sockfd);
	return 0;
}