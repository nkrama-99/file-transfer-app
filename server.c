/* Some code is borrowed from Section 2.4 of the Berkeley API */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define SERVER_UDP_PORT 5000 /* well-known port */
#define MAXLEN 4096          /* maximum data length */

bool are_equal(char *expected_message, char *received_message);
char *copy_stringers(char *message, int startIndex, int len);
bool write_to_file(char *received);
void delay(int number_of_seconds);

int prev_packet_transferred;
char *prev_file_name;

int main(int argc, char **argv)
{
    //variables borrowed from Berkely API 2.4
    int sd, client_len, port, n;
    char buf[MAXLEN];
    struct sockaddr_in server, client;
    char expected_message[] = "ftp";
    int reply_len;
    bool duplicate_packet = false;

    prev_packet_transferred = 0;
    prev_file_name = (char*)malloc(MAXLEN * sizeof(char));
    strcpy(prev_file_name, "");

    char reply[MAXLEN];
    reply_len = MAXLEN;
    strcpy(reply, "");
    strcpy(reply, "yes");

    switch (argc)
    {
    case 2:
        port = atoi(argv[1]);
        break;
    default:
        fprintf(stderr, "Usage: %s <UDP listen port>\n", argv[0]);
        exit(1);
    }

    //the following is borrowed from the Berkley API
    //establishes socket and binds to it

    //AF_INET family: internet communication using TCP/IP
    //SOCK_DRAM type: delivers data in blocks of bytes
    //0 protocol: default protocol for family, type pair
    //the default protocol for AF_INET,SOCK_DGRAM is UDP
    //sd is the socket descriptor returned

    //----------------------------------------------------------------
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        fprintf(stderr, "Can’t create a socket\n");
        exit(1);
    }
    /* Bind an address to the socket */
    //erase data @ address of server
    bzero((char *)&server, sizeof(server));

    server.sin_family = AF_INET;
    //convert host byte order to network byte order (Internet)
    //to use a common data format
    server.sin_port = htons(port);
    //use any ip address available for host
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    //communication will occur via the specified port number
    if (bind(sd, (struct sockaddr *)&server,
             sizeof(server)) == -1)
    {
        fprintf(stderr, "Can’t bind name to socket\n");
        exit(1);
    }

    //-----------------------------------------------------------------------
    //wait for an incoming message
    while (1)
    {
        client_len = sizeof(client);
        //receive message from client into buf
        //returns number of bytes received
        if ((n = recvfrom(sd, buf, MAXLEN, 0,
                          (struct sockaddr *)&client, &client_len)) < 0)
        {
            fprintf(stderr, "Can’t receive datagram\n");
            exit(1);
        }

        // delay(500);
        // printf("PRINTING BUF RECEIVED BY UDP FUNCTION\n");
        // printf(buf);
        // printf("\n End of buf \n");
        duplicate_packet = write_to_file(buf);

        if (duplicate_packet == true) {
            strcpy(reply, "");
            strcpy(reply, "duplicate");
        } else {
            strcpy(reply, "");
            strcpy(reply, "yes");
        }

        //define reply and reply_len
        if (sendto(sd, reply, reply_len, 0,
                   (struct sockaddr *)&client, client_len) != reply_len)
        {
            fprintf(stderr, "Can’t send datagram\n");
            exit(1);
        }

        strcpy(buf, "");
        // printf("Packet Received, Ack sent\n");
    }
    //close the socket
    close(sd);

    // free(reply);
    free(prev_file_name);

    return (0);
}

bool are_equal(char *expected_message, char *received_message)
{
    int cmp = strcmp(expected_message, received_message);
    return (cmp == 0);
}

void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
  
    // Storing start time 
    clock_t start_time = clock(); 
  
    // looping till required time is not achieved 
    while (clock() < start_time + milli_seconds) {
        // printf("waiting\n");
    };
} 

char *copy_stringers(char *message, int startIndex, int len)
{
    int index = startIndex;
    //printf("Size: %d\n",len-startIndex);
    char *copy = malloc(sizeof(char) * (len - startIndex));
    while (index < len)
    {
        copy[index - startIndex] = message[index];
        index += 1;
    }
    //printf("Copy: %s\n",copy);
    return copy;
}

bool write_to_file(char *received)
{
    // printf(">>> reading received\n");
    // printf(received);
    // printf(">>> done reading\n");
    
    // char *total_frag = (char*)malloc(MAXLEN * sizeof(char));
    // char *packet_no = (char*)malloc(MAXLEN * sizeof(char));
    // char *size = (char*)malloc(MAXLEN * sizeof(char));
    // char *file_name = (char*)malloc(MAXLEN * sizeof(char));   
    // char *content_ptr;

    char total_frag[MAXLEN];
    char packet_no[MAXLEN];
    char size[MAXLEN];
    char file_name[MAXLEN]; 

    char *content_ptr;

    // // // printf("Processing total_frag\n");
    // int index = 0;
    // int counter = 0;

    // while (*(received + index) != ':') {
    //     // printf("Testing with: %c\n", *(received + index));
    //     *(total_frag + counter) = *(received + index);
    //     counter++;
    //     index++;
    // };
    // *(total_frag + counter) = '\0';
    // index++;
    // printf("%s & %d\n", total_frag, sizeof(total_frag)/sizeof(char));

    // // printf("Processing packet_no\n");
    // counter = 0;
    // while (*(received + index) != ':') {
    //     // printf("Testing with: %c\n", *(received + index));
    //     *(packet_no + counter) = *(received + index);
    //     counter++;
    //     index++;
    // };
    // *(packet_no + counter) = '\0';
    // index++;
    // printf("%s & %d\n", packet_no, sizeof(packet_no)/sizeof(char));

    // // printf("Processing size\n");
    // counter = 0;
    // while (*(received + index) != ':') {
    //     // printf("Testing with: %c\n", *(received + index));
    //     *(size + counter) = *(received + index);
    //     counter++;
    //     index++;
    // };
    // *(size + counter) = '\0';
    // index++;
    // printf("%s & %d\n", size, sizeof(size)/sizeof(char));

    // // printf("Processing file_name\n");
    // counter = 0;
    // while (*(received + index) != ':') {
    //     // printf("Testing with: %c\n", *(received + index));
    //     *(file_name + counter) = *(received + index);
    //     counter++;
    //     index++;
    // };
    // *(file_name + counter) = '\0';
    // index++;
    // printf("%s & %d\n", file_name, sizeof(file_name)/sizeof(char));

    // content_ptr = received + index;




    int index = 0;
    int counter = 0;

    // Processing total_frag
    // printf("Index: %d\n", index);
    counter = 0;
    for (counter = index; received[counter] != ':'; counter++) {
        // printf("Testing with: %c\n", received[counter]);
        total_frag[counter - index] = received[counter];
    }
    total_frag[counter - index] = '\0';
    index = counter + 1;
    // printf("%s & %d\n", total_frag, sizeof(total_frag)/sizeof(char));

    // Processing packet_no
    // printf("Index: %d\n", index);
    counter = 0;
    for (counter = index; received[counter] != ':'; counter++) {
        // printf("Testing with: %c\n", received[counter]);
        packet_no[counter - index] = received[counter];
    }
    packet_no[counter - index] = '\0';
    index = counter + 1;
    // printf("%s & %d\n", packet_no, sizeof(packet_no)/sizeof(char));

    // Processing size
    // printf("Index: %d\n", index);
    counter = 0;
    for (counter = index; received[counter] != ':'; counter++) {
        // printf("Testing with: %c\n", received[counter]);
        size[counter - index] = received[counter];
    }
    size[counter - index] = '\0';
    index = counter + 1;
    // printf("%s & %d\n", size, sizeof(size)/sizeof(char));

    // Processing file_name
    // printf("Index: %d\n", index);
    counter = 0;
    for (counter = index; received[counter] != ':'; counter++) {
        // printf("Testing with: %c\n", received[counter]);
        file_name[counter - index] = received[counter];
    }
    file_name[counter - index] = '\0';
    index = counter + 1;
    // printf("%s & %d\n", file_name, sizeof(file_name)/sizeof(char));

    // printf("Index: %d\n", index);
    content_ptr = received + index;





    // printf("%d\n", strlen(total_frag));
    // printf("%d\n", strlen(packet_no));
    // printf("%d\n", strlen(size));
    // printf("%d\n", strlen(file_name));
    
    int inc_limit = atoi(size);
    char *content = (char*)malloc(inc_limit * sizeof(char));
    
    for (int i=0; i<inc_limit; i++) {
        content[i] = *(content_ptr + i);
    }

    // printf("Parsing the message received:\n");
    // printf(" - total_frag : %d \n", atoi(total_frag));
    // printf(" - packet_no : %d \n", atoi(packet_no));
    // printf(" - size : %d \n", atoi(size));
    // printf(" - file_name : %s \n", file_name);    
    // printf(" - content : %s \n", content);

    int total_frag_int = atoi(total_frag);
    int packet_no_int = atoi(packet_no);
    int size_int = atoi(size);


    // Logic to ignore duplicates
    // Files must be received in correct order
    /*
        >>>>>NOTE:
        Flaw in logic: if server crashes in the middle of a transfer, prev_packet and prev_file_name are resetted
        Client needs to retransfer from beginning
    */
    
    // if it's a different file name, reset prev_packet 
    if (strcmp(prev_file_name, file_name) != 0) {
        printf("\n\n");
        printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
        printf("Server is receiving a new file\n");
        printf("file name: %s\n", file_name);
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
        // printf("Prev file name: %s\n", prev_file_name);
        strcpy(prev_file_name, file_name);
        // printf("Updated Prev file name: %s\n", prev_file_name);
        prev_packet_transferred = 0;
    }

    // if packet not in order, return false to indicate duplicate received
    /*
        >>>>>work to be done,
        - When will duplicate happen? ACK is lost
        - if duplicate happens, order will be messed up
        - client will be sending the same thing over and over
        - nothing will progress after that
        - Server Solution: server replies "duplicate" and resets prev_file_name & prev_packet
        - Client Solution: client detects "duplicate" and resends from packet 0
    */
    // printf("packet_no: %d | prev_packet_transferred: %d\n", atoi(packet_no), prev_packet_transferred);
    if (packet_no_int != prev_packet_transferred + 1) {
        // packet has already been transferred just ignore
        printf("\n\n");
        printf("<<<<<<<<<<<\n");
        printf("Duplicate!!\n");
        printf(">>>>>>>>>>>\n\n");
        return true;
    } else {
        // packet order is intact, mark it as transferred
        printf("Correct Packet\n");
        prev_packet_transferred = packet_no_int;
    }





    // printf("Writing content to file | total_frag = %s | packet_no = %s | size = %s | file_name = %s \n", total_frag , packet_no, size, file_name);
    printf("Writing content to file | total_frag = %d | packet_no = %d | size = %d | file_name = %s \n", total_frag_int , packet_no_int, size_int, file_name);
    
    FILE *fptr;

    if (packet_no_int == 1)
    {
        printf(">>>> New File\n");
        fptr = fopen(file_name, "w+b"); // "w" defines "writing mode"
    }
    else
    {
        fptr = fopen(file_name, "ab"); // "w" defines "writing mode"
    }
    fflush(fptr);

    char c = *content;
    int inc = 0;

    fwrite(content, 1, atoi(size), fptr);

    fclose(fptr);
    
    // free(total_frag);
    // free(packet_no);
    // free(size);
    // free(file_name);
    // free(content_ptr);

    return false;
}