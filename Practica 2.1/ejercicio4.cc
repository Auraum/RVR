#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>  
#include <sys/types.h>
#include <string.h>

#include <iostream>

int main (int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cerr << "[getaddrinfo]" << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if(sd == -1){
        std::cerr << "[socket] creación socket" << std::endl;
        return -1;
    }

    rc = bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
    
    if(rc != 0){
        std::cerr << "[bind] Fallido" << std::endl;
        return -1;
    }

    if(listen(sd, 16) != 0){
        std::cerr << "[listen] Fallido" << std::endl;
    }

    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);

    int cs = accept(sd, &cliente, &clientelen);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

    std::cout << "Conexion desde " <<  host << ":" << serv << std::endl;

    while(true){
        char buffer[80];

        int bytes = recv(cs, buffer, 79, 0);

        if(bytes <= 0){
            std::cout << "Conexion terminada" << std::endl;
            break;
        }

        buffer[bytes] = '\0';

        send(cs, buffer, bytes, 0);
    }

    close(cs);
    close(sd);

    return 0;
}
