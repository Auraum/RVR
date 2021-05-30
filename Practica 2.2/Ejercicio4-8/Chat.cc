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

        switch(cm.type){
            case ChatMessage::LOGIN:
                clients.push_back(std::move(sockP));
                std::cout << cm.nick << " logged in" << std::endl;
                break;
            case ChatMessage::MESSAGE:
                for(auto const& c :clients){
                    if(c != sockP) socket.send(cm, *c);
                }
                break;
            case ChatMessage::LOGOUT:               
                bool flag = false;
                auto it = clients.begin();
                while(!flag && it!=clients.end()){
                    if(**it == *sockP){
                        clients.erase(it);
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
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

