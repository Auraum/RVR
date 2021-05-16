#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>  
#include <sys/types.h>
#include <string.h>

#include <iostream>

int main (int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;
    
    if(argc != 4 || strlen(argv[3]) != 1) {
        std::cerr << "Uso: <ip> <puerto> <comando>(1 caracter)" << std::endl;
        return -1;
    }

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    char buffer[80];

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

    int bytes = sendto(sd, (void *)argv[3], 2,0, res->ai_addr, res->ai_addrlen);

    if(bytes != 2){
        std::cerr << "[sendto] No se pudo mandar el comando" << std::endl;
    }

    if(argv[3][0] != 'q'){
        bytes = recvfrom(sd, buffer, 79, 0, res->ai_addr, &res->ai_addrlen);

        if(bytes == -1){
            std::cerr << "[recvfrom] No se recibio respuesta" << std::endl;
        }

        buffer[-1] = '\0';
        buffer[bytes] = '\0';

        std::cout << buffer << std::endl;
    }

    freeaddrinfo(res);
    close(sd);

    return 0;
}
