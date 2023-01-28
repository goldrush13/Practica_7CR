#include "lib/include.h"


int main(void)
{   
    Configurar_SSI2();      //initmain
    uint16_t value = 1000;   // valor a convertir. 12 bits es 4096  111110100000

    SPI_write(value);
    while(1)
    {
        //PI_write(value);
    }

}
