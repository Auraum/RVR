#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    //Serializar los campos type, nick y message en el buffer _data

    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    char* buf = _data;

    memcpy(buf, &type, sizeof(uint8_t));
    buf += sizeof(uint8_t);

    //we assume fixed length of 8 bytes
    memcpy(buf, nick.c_str(), NICK_LEN*sizeof(char));
    buf += NICK_LEN * sizeof(char);
     
    //we assume fixed length of 80 bytes 
    memcpy(buf, message.c_str(), MESSAGE_LEN*sizeof(char));
    buf += MESSAGE_LEN * sizeof(char);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* buf = _data;

    memcpy(&type, buf, sizeof(uint8_t));
    buf += sizeof(uint8_t);

    nick = buf;
    buf += NICK_LEN * sizeof(char);
     
    message = buf;
    buf += MESSAGE_LEN * sizeof(char);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage cm;
        Socket* s = &socket;
        socket.recv(cm, s);

        std::unique_ptr<Socket> sockP(s);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        auto it = clients.begin();
        switch(cm.type){
            case ChatMessage::LOGIN:
                clients.push_back(std::move(sockP));
                std::cout << cm.nick << " logged in" << std::endl;
                break;
            case ChatMessage::MESSAGE:
                while(it != clients.end()){
                    if(!(**it == *sockP)) {
                        socket.send(cm, **it);
                    }
                    it++;
                }
                break;
            case ChatMessage::LOGOUT:               
                bool flag = false;
                while(!flag && it!=clients.end()){
                    if(**it == *sockP){
                        it = clients.erase(it);
                        std::cout << cm.nick << " logged out" << std::endl;
                        flag = true;
                    }
                    else it++;
                }
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline

        std::string msg;
        std::getline(std::cin, msg);
        if(msg == "LOGOUT") {
            logout();
        }
        else{
            ChatMessage em(nick, msg);
            em.type = ChatMessage::MESSAGE;

            // Enviar al servidor usando socket
            socket.send(em, socket);
        }
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage em;
        socket.recv(em);
        std::cout << em.nick << ": " << em.message << std::endl;
    }
}

