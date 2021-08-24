#include <fcntl.h>  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/stat.h>  
#include <sys/types.h>  
#include <string.h>
 
typedef struct 
{
    int id;
    int logIn[10];
    char password[100];
    float balance;
}   account;

void print_account_details() 
{
    int fd = open("bankdb.txt", O_RDONLY); 
    if (fd == -1) { 
        perror("Database Open Error : ");
        exit(1);
    }
    int size = sizeof(account);
    account* admin = (account*)calloc(1, sizeof(account));
    if(read(fd, admin, size) != size) 
    {
        perror("Read Database Error : "); 
        exit(1);
    }
    int N = admin->balance;
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
            if(pointer->logIn[j] > 0) {
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

int main() {
    int fd;
    account admin;
    admin.id = 0;
    admin.logIn[0] = 1;
    for(int i = 1; i < 10; i ++) admin.logIn[i] = 0;
    strcpy(admin.password, "123456\0");
    admin.balance = 20;


    fd = open("bankdb.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666); 
    if (fd == -1) { 
        perror("Database Open Error : ");
        exit(1);
    }

    int size = sizeof(account);
    if(write(fd, &admin, size) != size) 
    {
        perror("Write to Database Error : "); 
        exit(1);
    }

    account* accounts = (account*)calloc(1, sizeof(account));
    int uid = 1;
    for(int i = 1; i <= 10; i++)
    {   
        accounts->id = uid;
	uid += 1;
        strcpy(accounts->password,"123456");
        accounts->balance = 0.0;
        accounts->logIn[0] = i+1;
        for(int j = 1; j < 10; j ++)
            accounts->logIn[j] = 0;

        if(write(fd, accounts, size) != size) 
        {
            perror("Write to Database Error : "); 
            free(accounts);
            exit(1);
        }
    }

    for(int i = 11; i < 30; i = i + 2)
    {
        accounts->id = uid;
	uid += 1;
        accounts->logIn[0] = i+1;
        accounts->logIn[1] = i+2;
        strcpy(accounts->password,"123456");
        accounts->balance = 0.0;
        for(int j = 2; j < 10; j++)
            accounts->logIn[j] = 0;

        if(write(fd, accounts, size) != size) 
        {
            perror("Write to Database Error : "); 
            free(accounts);
            exit(1);
        }
    }
    close(fd);
    printf("Successfully created database.\n");
    print_account_details();
    return 0;
}
