// MODULO 2 - PUERTO D - 
// 

#include "lib/include.h"
extern void Configurar_SSI2(void)       // método configurar SPI
{
    
    SYSCTL->RCGCSSI |= (1<<2);          // Activo el reloj para el Modulo 2 del SSI2 pag 390
    SYSCTL->RCGCGPIO |= (1<<3);         // El bit 3 corresponde al puerto D pg 382
    
    GPIOD_AHB->DIR |= (0<<3) | (1<<2) | (1<<1) | (0<<0); //selector es salida = 1
    //                 SCLK     CS       MOSI      MISO
    GPIOD_AHB->AFSEL = (1<<3) | (1<<2) | (1<<1) | (1<<0);         // Le digo que estos pines van a ser controlados por otro periferico (SSI) pg 770  
    GPIOD_AHB->PCTL = (GPIOD_AHB->PCTL&0xFFFF0000) | 0x0000FFFF;  // Combinación pp 787, 1808 (segun esta pg debo poner un 15)
    //GPIOD_AHB->PCTL |= 0x0000FFFF; // tabla p.788, video santama
    
    // Entradas digitales
    GPIOD_AHB->DEN |= (1<<0)|(1<<1)|(1<<2)|(1<<3);  // Digitales porque voy a enviar una trama digital
    //                MISO    MOSI    CS    SCLK

    GPIOD_AHB->DATA |= (1<<5); //registrar CS

    SSI2->CR1 = (0<<1);          // SSE=0 DESHABILITAR modulo (ensure that the SSE bit in the SSICR1 register is clear)
    // minuto 25, pte5
    //NOTA: El CR1 me permite indicar cuantos bits voy a mandar, y conf la velocidad del reloj de transmision      
    
    
    // SELECCIONO SI EL QSSI VA A SER MAESTRO O ESCLAVO:
    SSI2->CR1 = (0<<2);          // En el bit 2 se le pone 0 para que sea maestro MS = 0 modo maestro pp 1248 
    
    SSI2->CC = (0x0<<0);         // Le digo que trabajaré con el reloj del sistema (system clock = 50MHz) pp 1386
    

    // Para el registro SSICPSR (clock prescale):
    // pp.1242 
    // Assuming the system clock is 20 MHz, the bit rate calculation would be:
    // SSInClk   = SysClk    / (CPSDVSR * (1 + SCR))   despejar SCR
    // 1 000 000 = 20 000 000/ (      2 * (1 + SCR))
    
    // SCR = (SysClk/SSInClk*CPSDVSR) - 1 = 9
    // SCR = (20 000 000/1 000 000*2) - 1 = 9
    // CPSDVSR y SSInClk son al azar
    
    SSI2->CPSR = 0x2; //2.5MHz

//  SSI2->CR0 = (0x9<<8) | 0x07; // 0x07 porque envía datos de 8 bits pp. resgistro 1369  SE CAMBIA A 12 QUE CORRESPONDEN AL DAC MCP4921
    SSI2->CR0 = (0x00<<8) | (0x0<<6) | (0x1<<4) | (0xF<<0); // datos de 12 bits,  SE CAMBIA A 'B' (12 bits DACMCP4921)
//                 SCR    cantidad de datos a enviar           0x0 es el modo (read on rising edge, changed on a falling edge)

    
    SSI2->CR1 |= (1<<1);         //SSE=1 habilitar modoulo p.961 (0x02)
}



////////////////////////////  COMUNICACIÓN SPI //////////////////////

extern void SPI_write(uint16_t value) //duda
{
    SSI2->DR = value;
    while ((SSI2->SR & (1<<0)) == 0);
    
    
}

extern void DAC_Output(unsigned int valueDAC)
{
    char temp;
    GPIOD_AHB->DATA &= ~(1<<3); // CS = 0 se niega
    
    // envío high byte (los primeros 8 bits)
    temp = (valueDAC >> 8) & 0x0F;      // [11...8] a [3...0]
    temp |= 0x30;
    SPI_write(temp);
    
    // envío low byte
    temp = valueDAC;
    SPI_write(temp);        // mando los ultimos 8 bits

    GPIOD_AHB->DATA |= (1<<3);  //CS = 1

}

/*extern void SPI_write_data(uint8_t reg, uint8_t data)
{
    GPIOD_AHB->DATA &= ~(1<<3); // CS = 0 se niega
    SPI_write(reg & ~0x80); //escribir registro + MSB igualado a cero
    SPI_write(data);
    GPIOD_AHB->DATA |= (1<<3); //CS = 1
}

extern uint8_t SPI_read(void)
{
    uint8_t data = 0;
    while ((SSI2->SR & 0x10) == 0x10); // espera por busy bit
    data = SSI2->DR;
    return data;
}

extern uint8_t SPI_read_data(uint8_t reg)
{
    uint8_t data = 0;
    GPIOD_AHB->DATA &= ~(1<<3); // CS = 0
    SPI_write(reg | 0x80); // escribe registro + MSB
    SPI_write(0x00); //escribir dato para generar señal de reloj
    data = SPI_read(); //leer dato
    GPIOD_AHB->DATA |= (1<<3); //CS = 1
    return data;
}*/
