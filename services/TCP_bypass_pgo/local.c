#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
static size_t total_bytes = 0;
#define PORT 1338

static uint64_t system_time()
{
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return tv.tv_sec*(uint64_t)1000000000ull+tv.tv_nsec;
}

// Function designed for chat between client and server.
static void func(int sockfd)
{
    uint64_t start_ts = system_time();
    for (;;) {
        char buffer[8192];
        // read the message from client and copy it in buffer
        int len = read(sockfd, buffer, sizeof(buffer));
        if (len < 0) {
          perror("read failed");
        }
        if (len == 0) {
          printf("Closed, read %zu bytes\n", total_bytes);
          break;
        }
        total_bytes += len;
    }
    uint64_t diff = system_time() - start_ts;
    double durs   = (double) diff / 1000000000ULL;
    double mbits  = ((double) total_bytes / (1024*1024) * 8) / durs;
    printf("%.3f\n", mbits);
}
  
// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
  
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
  
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
  
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
  
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
  
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");
  
    // Function for chatting between client and server
    func(connfd);
  
    // After chatting close the socket
    close(sockfd);
}
