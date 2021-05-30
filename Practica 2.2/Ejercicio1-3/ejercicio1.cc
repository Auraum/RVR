#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x(_x),pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        alloc_data(MAX_SIZE);

        char* buf = _data;
        
        memcpy(buf, name, MAX_NAME * sizeof(char));
        buf += MAX_NAME*sizeof(char);

        memcpy(buf, &pos_x, sizeof(int16_t));
        buf += sizeof(int16_t);

        memcpy(buf, &pos_y, sizeof(int16_t));
        buf += sizeof(int16_t);
    }

    int from_bin(char * data)
    {
        memcpy(name, data, MAX_NAME*sizeof(char));
        data += MAX_NAME*sizeof(char);

        memcpy(&pos_x, data, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(&pos_y, data, sizeof(int16_t));
        data += sizeof(int16_t);

        return 0;
    }

    void to_file(){
        //equivalent to open with O_CREAT, O_TRUNC, O_WRONLY
        int file = creat("./playerFile", 0666);
        write(file, _data, _size);
        close(file);
    }

    void from_file(int file){
        char* buf = (char*)malloc(MAX_SIZE);
        read(file, buf, MAX_SIZE);
        from_bin(buf);
        close(file);
    };

public:
  int16_t pos_x;
  int16_t pos_y;
  
  static const size_t MAX_NAME = 20;

  char name[MAX_NAME];

  static const size_t MAX_SIZE = MAX_NAME*sizeof(char) + 2*sizeof(int16_t);
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    one_w.to_file();
    // 3. Leer el fichero
    int file = open("./playerFile", O_RDONLY);
    // 4. "Deserializar" en one_r
    one_r.from_file(file);
    // 5. Mostrar el contenido de one_r
    std::cout << "Name: " << one_r.name << std::endl;
    std::cout << "Pos x: " << one_r.pos_x << std::endl;
    std::cout << "Pos y: " << one_r.pos_y << std::endl;

    return 0;
}

