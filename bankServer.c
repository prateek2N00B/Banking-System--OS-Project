#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <sys/select.h>

typedef struct 
{
    int id;
    int logIn[10];
    char password[100];
    float balance;
}   account;
int totalUsers = 21;
char details[1024];
#define SIZE sizeof(account)

void init() {
    int fd = open("bankdb.txt", O_RDONLY); 
    if (fd == -1) { 
        perror("Database Open Error : ");
        exit(1);
    }
    int size = sizeof(account);
    account* admin = (account*)calloc(1, SIZE);
    if(read(fd, admin, size) != size) 
    {
        perror("Read Database Error : "); 
        exit(1);
    }
    int N = admin->balance;
    printf("Admin Balance = %d\n", N);
    totalUsers = N + 1;
    printf("Total Users = %d\n", totalUsers);
    close(fd);
}

void print_account_details() 
{
    int fd = open("bankdb.txt", O_RDONLY); 
    if (fd == -1) { 
        perror("Database Open Error : ");
        exit(1);
    }
    int size = sizeof(account);
    account* admin = (account*)calloc(1, SIZE);
    if(read(fd, admin, size) != size) 
    {
        perror("Read Database Error : "); 
        exit(1);
    }
    int N = admin->balance;
    printf("Admin Balance = %d\n", N);
    for(int i = 0; i < N; i ++)
    {   
        account* pointer = (account*)calloc(1, sizeof(account));
        int sz = read(fd, pointer, size);
        if(sz != size) 
        {
            perror("Read Database Error : "); 
            exit(1);
        }
        if(pointer->logIn[0] == 1)
            continue ;
        printf("Account Number %d\n", pointer->id);
        printf("Password: %s\n",pointer->password);
        printf("Balance: %f\n", pointer->balance);
        int counter = 0;
        printf("Account Holders Id: ");
        for(int j = 0; j < 10; j ++)
        {
            if(pointer->logIn[j] != 0) {
                printf("%d ", pointer->logIn[j]);
                counter ++;
            }
        }
        if(counter > 1)
            printf("(Joint account)\n");
        else
            printf("(Single account)\n");
        printf("\n");
        free(pointer);
    }
    free(admin);
    close(fd);
}

// User Functions Begin

int deposit(int uid, float amt){
    printf("[+] Deposit amt = %f in UID = %d\n", amt, uid);
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();
    int success = 1;
    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return -1;
    }
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        return 0;
    }

    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    acc->balance += amt;
    lseek(fd, uid * SIZE, SEEK_SET);
    if(write(fd, acc, SIZE) == -1) {
        perror("Write Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return success;
}

int withdraw(int uid, float amt){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();
    
    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        return 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    if(acc->balance >= amt) {
        acc->balance -= amt;
        success = 1;
    }
    else {
        success = 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(write(fd, acc, SIZE) == -1) {
        perror("Write Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return success;
}

float balance(int uid){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();

    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        return 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return acc->balance;
}

int change_password(int uid, char password[100]){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();

    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        return 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    strcpy(acc->password,password);
    lseek(fd, uid * SIZE, SEEK_SET);
    if(write(fd, acc, SIZE) == -1) {
        perror("Write Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return success;
}

int get_details(int uid){
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();

    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        success = 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    if(success == 0) {
        success = 0;
    }
    for(int i = 0; i < 1024; i ++)  details[i] = '\0';
    sprintf(details,"UID : %d \nPassword : %s \nBalance : %f\nAccount Holders Id : ",
    acc->id, acc->password, acc->balance);
    for(int j = 0; j < 10; j ++) {
        if(acc->logIn[j] != 0) {
            char num[10] = {'\0'};
            sprintf(num, "%d ", acc->logIn[j]);
            int c = 0;
            for(int k = 0;k < 10; k ++) if(num[k] != '\0') c ++;
            strncat(details, num, c);
        }
    }
    return success;
}
// Admin Function Begin
int signup(int uid, int logIn[10], char password[100], float balance){
    printf("[+] Add account with password = %s , balance = %f\n", password, balance);
    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    lseek(fd, totalUsers*SIZE, SEEK_SET);

    strcpy(acc->password, password);
    acc->balance = balance;
    for(int i = 0; i < 10; i ++) {
        acc->logIn[i] = logIn[i];
    }
    acc->id = totalUsers;
    totalUsers ++;
    if(write(fd, acc, SIZE) != SIZE) {
        
        success = 0; 
    } 
    close(fd);
    return success;
}

int delete_user(int uid){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();

    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        success = 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    printf("Current Owners\n");
    for(int i = 0; i < 10; i ++) printf("%d ", acc->logIn[i]);
    for(int i = 0; i < 10; i ++) acc->logIn[i] = 0;
    acc->logIn[0] = -1;
    lseek(fd, uid * SIZE, SEEK_SET);
    if(write(fd, acc, SIZE) == -1) {
        perror("Write Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return success;
}

int modify_user(int uid, int logIn[10], char password[100], float balance){
    if(uid > totalUsers) {
        return 0;
    }
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();

    account* acc = (account*)calloc(1, SIZE);
    int fd = open("bankdb.txt",O_RDWR);
    if(fd == -1){
        perror("File Error : "); 
        return 0;
    }
    int success = 1;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {
        perror("fcntl") ; 
        success = 0;
    }
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = 0;
    }
    for(int i = 0; i < 10; i ++) acc->logIn[i] = logIn[i];
    acc->balance = balance;
    strcpy(acc->password, password);
    lseek(fd, uid * SIZE, SEEK_SET);
    if(write(fd, acc, SIZE) == -1) {
        perror("Write Error : "); 
        success = 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return success;
}


int validateUser(int uid, char userId[100], char password[100]) {
    int id = atoi(userId);
    printf("[+] UId = %d\n", uid);
    printf("[+] Id = %d\n", id);
    printf("[+] Pass = %s\n", password);
    int fd = open("bankdb.txt", O_RDONLY);
    static struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = uid * SIZE;
    lock.l_whence = SEEK_SET;
    lock.l_len = SIZE;
    lock.l_pid = getpid();

    if(fcntl (fd, F_SETLKW, &lock) == -1) {
        perror("Read Lock : ");
        return -1;
    }
    int success = -1;
    account* acc   = (account*)calloc(1, SIZE);
    lseek(fd, uid * SIZE, SEEK_SET);
    if(read(fd, acc, SIZE) == -1) {
        perror("Read Error : "); 
        success = -1;
    }
    int passCheck = strcmp(acc->password, password);
    if(passCheck == 0) {
        for(int j = 0; j < 10; j ++) {
            int checkId = acc->logIn[j] == id;
            if(checkId == 1) {
                uid = acc->id;
                success = 1;
                break;
            } 
        }
    }
    else {
        success = -1;
    }
    free(acc);
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
    return success;
}

void filter(char buffer[1024]) {
    for(int i = 0; i < 1024; i ++) {
        if(buffer[i] == '\0') return ;
        if(buffer[i] == '\n') {buffer[i] = '\0'; return ; }
    }
}

void filter2(char buffer[100]) {
    printf("Buf = %s\n", buffer);
    for(int i = 0; i < 100; i ++) {
        if(buffer[i] == '\0') return ;
        if(buffer[i] == '\n') {buffer[i] = '\0'; return ; }
    }
}

void setNull(char write_buffer[1024], char read_buffer[1024]) {
    for(int i = 0; i < 1024; i ++) {
        write_buffer[i] = '\0';
        read_buffer[i] = '\0';
    }
}

void *connection_handler(void *socket_desc)
{
	int sock = *(int*)socket_desc , option, deposit_amt, withdraw_amt, ret, balance_amt;
	char password[100], userId[100];
    char write_buffer[1024] = {0};
    char read_buffer[1024]  = {0};
    int uid;
    // Get UserId.
    
    strcpy(write_buffer, "Enter Account Number: \n\0");
    send(sock, write_buffer, sizeof(write_buffer), 0);
    recv(sock, read_buffer, sizeof(read_buffer), 0);
    uid = atoi(read_buffer);
    // Get UserId.
    setNull(write_buffer, read_buffer);
    strcpy(write_buffer, "Login id: \n\0");
    send(sock, write_buffer, sizeof(write_buffer), 0);
    recv(sock, userId, sizeof(userId), 0);
    filter2(userId);
    // Get Password.
    strcpy(write_buffer, "Password: \n\0");
    send(sock, write_buffer, sizeof(write_buffer), 0);
    recv(sock, password, sizeof(password), 0);
    filter2(password);
    int res = validateUser(uid, userId, password);
    printf("%d\n", uid);
    if(res != -1) {
        strcpy(write_buffer, "Success");
        send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
    }
    else {
        strcpy(write_buffer, "Login NOT Successfull. Try again later. ");
        send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
        close(sock);
        return 0;
    }
	while(1){
        if(uid == 0) {
            setNull(write_buffer, read_buffer);
            strcpy(write_buffer, "[+]Press\n1 -> Add\n2 -> Delete\n3 -> Modify\n4 -> Search\n5 -> Exit\n\0");
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            int option = atoi(read_buffer);
            if(option == 1) { // SignUp 
                char password[100] = {0};
                int logIn[10] = {0};
                float balance;
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter Number of users : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                int N = atoi(read_buffer);
                // printf("N = %d\n", N);
                if(N > 10) {
                    strcpy(write_buffer, "Maximum Limit = 10. \n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    continue;
                }
                for(int i = 0; i < N; i ++) {
                    setNull(write_buffer, read_buffer);
                    strcpy(write_buffer, "Enter User Id : \n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    recv(sock, read_buffer, sizeof(read_buffer), 0);
                    int id = atoi(read_buffer);
                    logIn[i] = id;
                }
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter Password : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, password, sizeof(password), 0);
                // printf("Password = %s\n", password);
                
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter Balance : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                // printf("Balance = %s\n", read_buffer);
                balance = atof(read_buffer);
                int success = signup(1, logIn, password, balance);
                if(success) {
                    strcpy(write_buffer, "Account Successfully Added.\n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    deposit(0, 1.0);
                }
                else {
                    strcpy(write_buffer, "Error Occurred while adding account.\n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                }
                print_account_details();
            }
            else if(option == 2) { // Delete
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter UserID : ");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                int uid = atoi(read_buffer);
                setNull(write_buffer, read_buffer);
                if(uid > totalUsers) {
                    strcpy(write_buffer, "Invalid User ID. \n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                }
                printf("[+] Delete Id := %d\n", uid);
                int success = delete_user(uid);
                if(success){
                    strcpy(write_buffer, "User Account has been deleted.\n");
                }
                else
                    strcpy(write_buffer, "Error deleting account. \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                print_account_details();
            }
            else if(option == 3) { // Modify
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter UserID : ");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                int uid = atoi(read_buffer);
                printf("[+] Modify UID = %d\n", uid);

                char password[100];
                int logIn[10] = {0};
                float balance;

                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter New Number of users : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                int N = atoi(read_buffer);
                printf("[+] New Number of Holders = %d\n", N);
                for(int i = 0; i < N; i ++) {
                    setNull(write_buffer, read_buffer);
                    strcpy(write_buffer, "Enter User Id : \n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    recv(sock, read_buffer, sizeof(read_buffer), 0);
                    int id = atoi(read_buffer);
                    logIn[i] = id;
                }
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter New Password : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, password, sizeof(password), 0);
                printf("[+] New Password := %s\n", password);
                
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter Balance : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                balance = atof(read_buffer);
                printf("[+] New Balance = %f\n", balance);
                int success = modify_user(uid, logIn, password, balance);
                if(success) {
                    setNull(write_buffer, read_buffer);
                    strcpy(write_buffer, "Account Successfully Modified.\n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                }
                else {
                    setNull(write_buffer, read_buffer);
                    strcpy(write_buffer, "Error Occurred while modifying account.\n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                }
                print_account_details();
            }
            else if(option == 4) { // Search
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter UserID : ");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                int uid = atoi(read_buffer);
                printf("[+] Get Information UID := %d\n", uid);
                if(uid > totalUsers) {
                    setNull(write_buffer, read_buffer);
                    strcpy(write_buffer, "Invalid User ID. \n");
                    send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    continue;
                }
                int success = get_details(uid);
                setNull(write_buffer, read_buffer);
                if(success)
                    strcpy(write_buffer, details);
                else
                    strcpy(write_buffer, "Error fetching details. \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
            else if(option == 5) {
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Goodbye!!\n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(sock);
                break;
            }
            else {
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Invalid Choice!!\n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
        }
        else {
            strcpy(write_buffer, "\n[+]Press\n1 -> Deposit\n2 -> Withdraw\n3 -> Balance Enquiry\n4 -> Password Change\n5 -> View details\n6 -> Exit\n");
            send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            recv(sock, read_buffer, sizeof(read_buffer), 0);
            int option = atoi(read_buffer);
            if (option == 1) { // deposit
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter Amount : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);
                float amt = atof(read_buffer);
                printf("[+]Deposit Amount = %f in UID = %d\n",amt, uid);
                int success = deposit(uid, amt);
                if(success) {
                    strcpy(write_buffer, "Amount Deposited Successfully. \n");
                }
                else {
                    strcpy(write_buffer, "Error : Cannot Deposit. \n");
                }
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                print_account_details();
            }
            else if (option == 2) { // withdraw
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Enter Amount : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, read_buffer, sizeof(read_buffer), 0);

                float amt = atof(read_buffer);
                printf("[+]Withdraw Amount = %f in UID = %d\n",amt, uid);
                int success = withdraw(uid, amt);
                setNull(write_buffer, read_buffer);
                if(success) {
                    strcpy(write_buffer, "Amount withdrawn successfully. \n");
                }
                else {
                    strcpy(write_buffer, "Error : Cannot withdraw this amount. \n");
                }
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                print_account_details();
            }
            else if (option == 3) { // check balance
                setNull(write_buffer, read_buffer);
                float amt = balance(uid);
                sprintf(write_buffer,"You account balance = %f\n", amt);
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
            else if(option == 4) { // change password
                setNull(write_buffer, read_buffer);
                char new_password[100];
                strcpy(write_buffer, "Enter new Password : \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(sock, new_password, sizeof(new_password), 0);
                int success = change_password(uid, new_password);
                setNull(write_buffer, read_buffer);
                if(success) {
                    strcpy(write_buffer, "Password Successfully changed. \n");
                }
                else {
                    strcpy(write_buffer, "Error while changing password. \n");
                }
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
            else if (option == 5) { // get details
                setNull(write_buffer, read_buffer);
                int success = get_details(uid);
                if(success)
                    strcpy(write_buffer, details);
                else
                    strcpy(write_buffer, "Error fetching details. \n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
            else if(option == 6) {
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Goodbye!!\n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(sock);
                break;
            }
            else {
                setNull(write_buffer, read_buffer);
                strcpy(write_buffer, "Invalid Choice!!\n");
                send(sock, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            }
        }
	}
    return 0;
} 

int main(int argc, char const *argv[]) 
{ 
    init();
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 

	int addrlen = sizeof(address); 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
		perror("Socket Error : "); 
		exit(1); 
	} 

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("Set Scoket Options Error : "); 
		exit(1); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( 8080 ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
		perror("Bind Error : "); 
		exit(1); 
	} 
	if (listen(server_fd, 10) < 0) { 
		perror("Listen : "); 
		exit(1); 
	} 
    printf("[+] Listening ...\n");
    while(1){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
            perror("Accept Error "); 
            exit(1); 
        } 
        pthread_t thread_id;
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &new_socket) < 0)
        {
            perror("Thread Creation Error : ");
            exit(1);
        }
        printf("[+] Connection Accepted.\n");
    }
	return 0; 
} 
