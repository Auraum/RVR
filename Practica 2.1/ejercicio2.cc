#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>  
#include <sys/types.h>
#include <time.h>
#include <string.h>

#include <iostream>

int main (int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;
    bool salir = false;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

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

    int bytesSent = 0;

    while(!salir){
        
        char buffer[80];

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t clientelen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void*) buffer, 79, 0, &cliente, &clientelen);

        buffer[-1] = '\0';

        if (bytes == -1){
            std::cerr << "[recvfrom] fallido" << std::endl;
            return -1;
        }
        else {
            getnameinfo(&cliente, clientelen, host, NI_MAXHOST,
            serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << bytes << " bytes de " << host << ":" << serv << std::endl;

            if(bytes == 2){
                
                time_t t;
                time(&t);
                struct tm* info;
                int len;

                info = localtime(&t);

                switch(buffer[0]){
                    case 't':
                        len = strftime(buffer, 79, "%r", info);
                        bytesSent = sendto(sd, buffer, len, 0, &cliente, clientelen);
                        break;
                    case 'd':
                        len = strftime(buffer, 79, "%F", info);
                        bytesSent = sendto(sd, buffer, len, 0, &cliente, clientelen);
                        break;
                    case 'q':
                        salir = true;
                        break;
                    default:
                        std::cout << "Comando no soportado: " << buffer[0] << std::endl;
                        break;
                }
            }
            else std::cout << "Demasiados caracteres" << std::endl;
        }

        if(bytesSent == -1){
            std::cerr << "[sendto] fallido" << std::endl;
        }
    }

    close(sd);

    std::cout << "Saliendo..." << std::endl;

    return 0;
}
