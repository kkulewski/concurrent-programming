#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "153.19.1.202"
#define SERVER_PORT 5000

int main()
{
    struct sockaddr_in recipient;
    int size = sizeof(struct sockaddr_in);
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(SERVER_PORT);
    inet_pton(PF_INET, SERVER_IP, &recipient.sin_addr);

    int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socket_descriptor, (struct sockaddr*) &recipient, size);

    int value = 0;
    printf("Provide value:\n");
    scanf("%d", &value);
    value = htonl(value);
    sendto(socket_descriptor, (char*) &value, sizeof(int), 0, (struct sockaddr*) &recipient, size);

    int result = 0;
    recvfrom(socket_descriptor, (char*) &result, sizeof(int), 0, (struct sockaddr*) &recipient, &size);
    result = ntohl(result);
    printf("Received result: %d\n", result);
}
