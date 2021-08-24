

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>    
#include <netinet/in.h> 
#include <arpa/inet.h>      
#include <sys/socket.h>

char input[1024] = {0};

void setNull(char write_buffer[1024], char read_buffer[1024]) {
    for(int i = 0; i < 1024; i ++) {
        write_buffer[i] = '\0';
        read_buffer[i] = '\0';
        input[i] = '\0';
    }
}

int performOperation(int uid, int sock) {
    char write_buffer[1024] = {0};
    char read_buffer[1024] = {0};
    int option;
    recv(sock, read_buffer, sizeof(read_buffer), 0);
    write(1, read_buffer, sizeof(read_buffer));
    scanf("%[^\n]%*c", input);
    sprintf(write_buffer, "%s", input);
    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
    option = atoi(write_buffer);
    if(uid == 1) {
        if(option == 1) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", write_buffer);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            int N = atoi(write_buffer);
            if(N > 10) {
                setNull(write_buffer, read_buffer);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                write(1, read_buffer, sizeof(read_buffer));
                return 1;
            }
            for(int i = 0; i < N; i ++) {
                setNull(write_buffer, read_buffer);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                write(1, read_buffer, sizeof(read_buffer));
                scanf("%[^\n]%*c", write_buffer);                
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
            // Password
            char password[100] = {0};
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", password);
            send(sock, password, sizeof(password), MSG_CONFIRM);
            // Balance
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", write_buffer);
            send(sock, write_buffer, sizeof(write_buffer), 0);
            // Confirmation
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
        else if(option == 2) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", input);
            sprintf(write_buffer, "%s", input);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
        else if(option == 3) {
            setNull(write_buffer, read_buffer);
            // UserId
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", input);
            sprintf(write_buffer, "%s", input);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            // New Number of Users.
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", write_buffer);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            int N = atoi(write_buffer);

            for(int i = 0; i < N; i ++) {
                setNull(write_buffer, read_buffer);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                write(1, read_buffer, sizeof(read_buffer));
                scanf("%[^\n]%*c", write_buffer);
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
            // Password
            char password[100] = {0};
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", password);
            send(sock, password, sizeof(password), MSG_CONFIRM);

            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", input);
            sprintf(write_buffer, "%s", input);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
        else if(option == 4) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", write_buffer);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            printf("\n");
            return 1;
        }
        else if(option == 5) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            return 0;
        }
        else {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
    }
    else {
        if(option == 1 || option == 2 || option == 3) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            if(option == 3) return 1;
            scanf("%[^\n]%*c", write_buffer);
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
        else if(option == 4) {
            char password[100] = {0};
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            scanf("%[^\n]%*c", password);
            send(sock, password, sizeof(password), MSG_CONFIRM);

            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
        else if(option == 5) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
        else if(option == 6) {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
            return 0;
        }
        else {
            setNull(write_buffer, read_buffer);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            write(1, read_buffer, sizeof(read_buffer));
        }
    }
    return 1;
}


void start(int sock) {
    char write_buffer[1024] = {0};
    char read_buffer[1024] = {0};
    char smallInp[100];
    // LogIn Procedure.
    // UserID
    int uid;
    setNull(write_buffer, read_buffer);
    recv(sock, read_buffer, sizeof(read_buffer), 0);
    write(1, read_buffer, sizeof(read_buffer));
    scanf("%[^\n]%*c", write_buffer);
    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
    uid = atoi(write_buffer);
    // UserName
    setNull(write_buffer, read_buffer);
    recv(sock, read_buffer, sizeof(read_buffer), 0);
    write(1, read_buffer, sizeof(read_buffer));
    scanf("%[^\n]%*c", smallInp);
    send(sock, smallInp, sizeof(smallInp), MSG_CONFIRM);
    uid = atoi(smallInp);
    // Password
    setNull(write_buffer, read_buffer);
    recv(sock, read_buffer, sizeof(read_buffer), 0);
    write(1, read_buffer, sizeof(read_buffer));
    scanf("%[^\n]%*c", smallInp);
    send(sock, smallInp, sizeof(smallInp), MSG_CONFIRM);
    // Confirmation
    recv(sock, read_buffer, sizeof(read_buffer), 0);
    write(1, read_buffer, sizeof(read_buffer));
    if(strcmp(read_buffer, "Success") != 0) {
        return ;
    }
    int running = 1;
    while(running) {
        running = performOperation(uid, sock);
    }
    
}

int main() {
    int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons( 8080 ); 

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 

    start(sock);
	return 0; 
}