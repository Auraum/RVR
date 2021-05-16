#include <sys/socket.h>  
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#include <iostream>

int main (int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void*) &hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AF_UNSPEC;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    for(struct addrinfo *ai = res; ai != NULL; ai = ai->ai_next){

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        getnameinfo(ai->ai_addr, ai->ai_addrlen, host, NI_MAXHOST,
        serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << host << " " << res->ai_family << " " << res->ai_socktype << std::endl;
    }

    freeaddrinfo(res);

    return 0;
}
