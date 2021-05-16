#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>  
#include <sys/types.h>
#include <string.h>

#include <iostream>

int main (int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;
    bool salir = false;
    
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

    if(connect(sd, res->ai_addr, res->ai_addrlen) != 0){
        std::cerr << "[connect] Fallido" << std::endl;
        return -1;
    }

    while(!salir){
        std::cin >> buffer;

        if(buffer[0] == 'Q' && strlen(buffer) == 1){
            salir = true;
        }
        else{
            if(send(sd, buffer, strlen(buffer), 0) == -1){
                std::cerr << "[send] Fallido" << std::endl;
            }
            
            int bytes = recv(sd, buffer, 79, 0);
            
            buffer[bytes] = '\0';
            std::cout << buffer << std::endl;          
        }
    }

    freeaddrinfo(res);
    close(sd);

    return 0;
}
