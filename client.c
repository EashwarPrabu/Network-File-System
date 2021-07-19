#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <time.h>
#include <netinet/in.h>
#define DATAMAX 5000
#define MAX 1000
#define PORT 5000

// READ STRUCT
struct readargs {
   char fileName[MAX];
   int count;
};

// WRITE STRUCT
struct writeargs {
   char fileName[MAX];
   char data[DATAMAX];
};

//RENAME AND REMOVE STRUCT
struct rargs {
   char from[MAX];
   char to[MAX];
};

int main() {
	int clientfd;
	struct sockaddr_in server;
	char pathName[MAX];
	int choice;
	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd == -1) {
		printf("Error in creating socket");
		exit(0);
	} else
		printf("Socket created successfully");

	// SPECIFYING SERVER CONFIG
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);

	// CONNECTING TO SERVER
	if(connect(clientfd, (struct sockaddr *)&server, sizeof(server)) != 0) {
		printf("\nError in connecting");
		exit(0);
	} else {
		printf("\nSuccessfully connected");
	}
	printf("\nWelcome to NFS....");
	do{
		choice =0;
		printf("\nChoose the file operations you want to perform..");
		printf("\nThe choices are:\n1.Read\n2.Write\n3.Rename\n4.Remove\n5.Get attributes of the file\nEnter '0' to close the connection");
		printf("\nEnter the Choice:");
		scanf("%d",&choice);
		
		write(clientfd, &choice, sizeof(choice));
		switch(choice) {
			case 1: {
				// READ
				struct readargs fileRead;
				printf("Enter the file name:");
				scanf("%s", fileRead.fileName);
				printf("Enter Count:");
				scanf("%d", &fileRead.count);
				write(clientfd, &fileRead, sizeof(fileRead));
				char data[MAX];
				printf("Successfully received...");
				read(clientfd, data, sizeof(data));
				printf("\nThe data obtained from the file is:%s", data);
				bzero(data,sizeof(data));
				break;
			}
			case 2: {
				// WRITE
				struct writeargs fileWrite;
				printf("Enter file name:");
				scanf("%s", fileWrite.fileName);
				printf("Enter the data:(Use ~ to terminate)");
				scanf("%[^~]s", fileWrite.data);
				getc(stdin);
				printf("\nThe data to be written is:%s", fileWrite.data);
				write(clientfd, &fileWrite, sizeof(fileWrite));
				printf("\nSuccessfully written...");
				break;
			}
			case 3: {
				// RENAME
				struct rargs renameFile;
				printf("Enter the old-name of the file:");
				scanf("%s", renameFile.from);
				printf("Enter the new-name of the file:");
				scanf("%s", renameFile.to);
				write(clientfd, &renameFile, sizeof(renameFile));
				printf("Successfully renamed...");
				break;
			}
			case 4: {
				// REMOVE
				struct rargs removeFile;
				printf("Enter the filename to remove:");
				scanf("%s", removeFile.from);
				write(clientfd, &removeFile, sizeof(removeFile));	
				printf("Successfully removed...");		
				break;
			}
			case 5: {
	     		// ATTRIBUTES OF A FILE
	         printf("Enter the filename:");
	         scanf("%s", pathName);
	         struct stat buff;
	         write(clientfd, pathName, sizeof(pathName));
	         read(clientfd, &buff, sizeof(buff));
	         printf("Successfully obtained attributes of the file...");
	         printf("\nMode of the file: %o", buff.st_mode);
	         printf("\nRecently Accessed time: %s", ctime(&buff.st_atim));
	         printf("Recently Modified time: %s", ctime(&buff.st_mtim));
	         printf("User ID: %d", buff.st_uid);
	         printf("\nInode number: %lo\n", buff.st_ino);
	         printf("Size of the File: %ld bytes\n", buff.st_size);
	         bzero(pathName,sizeof(pathName));
	         break;
	      }
			default: {
				if(choice!=0) {
					printf("\nInvalid case!!\n");	
				}
			}
		}
	} while(choice!=0);
	close(clientfd); 
	return 0;
}