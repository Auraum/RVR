#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>  
#include <sys/types.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <thread>

#define MAX_T 4

class mThread{
    private:
        char buffer[80];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        int _sd;
    public:
        mThread(int sd) : _sd(sd) {}
        void message(){

            while(true){
                int bytesSent = 0;
                struct sockaddr cliente;
                socklen_t clientelen = sizeof(struct sockaddr);

                int bytes = recvfrom(_sd, (void*) buffer, 79, 0, &cliente, &clientelen);

                buffer[-1] = '\0';

                if (bytes == -1){
                    std::cerr << "[recvfrom] fallido" << std::endl;
                }
                else {
                    getnameinfo(&cliente, clientelen, host, NI_MAXHOST,
                    serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

                    std::cout << bytes << " bytes de " << host << ":" << serv <<
                    " procesado por " << std::this_thread::get_id() << std::endl;

                    sleep(3);

                    if(bytes == 2){
                        
                        time_t t;
                        time(&t);
                        struct tm* info;
                        int len;

                        info = localtime(&t);

                        switch(buffer[0]){
                            case 't':
                                len = strftime(buffer, 79, "%r", info);
                                bytesSent = sendto(_sd, buffer, len, 0, &cliente, clientelen);
                                break;
                            case 'd':
                                len = strftime(buffer, 79, "%F", info);
                                bytesSent = sendto(_sd, buffer, len, 0, &cliente, clientelen);
                                break;
                            case 'q':
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
                
        }
};

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

    if(rc != 0){
        std::cerr << "[bind] Fallido" << std::endl;
        return -1;
    }

    std::thread threads[MAX_T];

    for(int i= 0; i < MAX_T; i++){
        mThread* mt = new mThread(sd);

        std::thread([&mt](){
            mt->message();

            delete mt;
        }).detach();
    }

    char input = ' ';
    while(!salir){
        std::cin >> input;

        if(input == 'q'){
            salir = true;
        }
    }

    close(sd);

    std::cout << "Saliendo..." << std::endl;

    return 0;
}
