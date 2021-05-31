//
// Created by akif on 5/31/21.
//

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
void serverMain();
int main(){

}
void serverMain(){
    int sockfd, clientfd;
    struct sockaddr_in host_addr, client_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1)
        xerror(__func__, "socket");

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
        xerror(__func__, "setsockopt");

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(args.port);
    host_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(host_addr.sin_zero), '\0', 8); // zero the rest.

    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
        xerror(__func__, "bind");

    if (listen(sockfd, 20) == -1)
        xerror(__func__, "listen");

    while (TRUE)
    {
        socklen_t len = sizeof(struct sockaddr_in);
        if ((clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &len)) == -1)
            xerror(__func__, "accept");

        xsem_wait(conr->graph_mutex);
        enqueue(&conr->client_queue, clientfd);
        xsem_post(conr->graph_mutex);

        /* forward connection */
        xsem_post(conr->handler_sem);
    }
    exit(EXIT_SUCCESS);
}