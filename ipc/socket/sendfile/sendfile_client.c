#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>

int FileSize(char *pFileName)
{
    struct stat sb;
    if (stat(pFileName, &sb) == -1)
    {
        return -1;
    }
    return sb.st_size;
}

int main(int argc, char *argv[])
{
    int                sockfd;
    int                len;
    struct sockaddr_in address;
    if (argc < 2)
    {
        printf("Usage: %s <file_to_send>\n", argv[0]);
        return 0;
    }

    int filefd = open(argv[1], O_RDONLY);
    if (filefd == 0)
    {
        printf("Failed to open %s\n", argv[1]);
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("sock");
        exit(1);
    }
    memset(&address, 0x00, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port   = htons(9734);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    len = sizeof(address);

    if ((connect(sockfd, (struct sockaddr *) &address, len)) == -1)
    {
        perror("connect");
        close(filefd);
        exit(EXIT_FAILURE);
    }

    int size = FileSize(argv[1]);
    if (sendfile(sockfd, filefd, 0, size) == -1)
    {
        perror("sendfile");
    }

    close(filefd);
    close(sockfd);
}
