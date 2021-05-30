#include "Socket.cc"
#include <iostream>

int main(){
    Socket s("127.0.0.1","11037");
    std::cout << s << std::endl;
    return 0;
}