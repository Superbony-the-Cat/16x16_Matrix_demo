#include <Arduino.h>

//  
// orginal YT:  Kevin Darrah  
// https://youtu.be/A_S3LAUQHwU
//
// modified : YT Superbony the Cat
// https://youtu.be/f-Uvjt281DE

//VARIABLES AND DEFINES HERE - NEEDED BY THE WS2812 DRIVER CODE
#define WS2812_pin 8// only digital pin 8 works right now
#define numberOfLEDs 256// total number of RGB LEDs

#define kMatrixWidth 16
byte RGB[768];//take your number of LEDs and multiply by 3    3x256 =  768

// FUNCTIONS HERE
void RGB_update(int LED, byte RED, byte GREEN, byte BLUE);//function to drive LEDs

void mapLEDXY(int y, int x, byte RED, byte GREEN, byte BLUE) {
        int RGBlocation = 0;

        if (y % 2 == 0) { //even column
                RGBlocation = x + y * 16;

        } else { //odd column
                RGBlocation = 15 - x + y * 16;
        }

        RGB[RGBlocation * 3] = BLUE;
        RGB[RGBlocation * 3 + 1] = RED;
        RGB[RGBlocation * 3 + 2] = GREEN;
}
void clearLEDs() {
        memset(RGB, 0, sizeof(RGB));
}

//WS2812 Driver Function
void RGB_update(int LED, byte RED, byte GREEN, byte BLUE) {
        // LED is the LED number starting with 0
        // RED, GREEN, BLUE is the brightness 0..255 setpoint for that LED
        byte ExistingPort, WS2812pinHIGH;//local variables here to speed up pinWrites

        if(LED>=0) {//map the REG GREEN BLUE Values into the RGB[] array
                RGB[LED * 3] = BLUE;
                RGB[LED * 3 + 1] = RED;
                RGB[LED * 3 + 2] = GREEN;
        }

        noInterrupts();//kill the interrupts while we send the bit stream out...
        ExistingPort = PORTB; // save the status of the entire PORT B - let's us write to the entire port without messing up the other pins on that port
        WS2812pinHIGH = PORTB | 1; //this gives us a byte we can use to set the whole PORTB with the WS2812 pin HIGH
        int bitStream = numberOfLEDs * 3;//total bytes in the LED string

//This for loop runs through all of the bits (8 at a time) to set the WS2812 pin ON/OFF times
        for (int i = bitStream - 1; i >= 0; i-- ) {

                PORTB = WS2812pinHIGH; //bit 7  first, set the pin HIGH - it always goes high regardless of a 0/1

                //here's the tricky part, check if the bit in the byte is high/low then right that status to the pin
                // (RGB[i] & B10000000) will strip away the other bits in RGB[i], so here we'll be left with B10000000 or B00000000
                // then it's easy to check if the bit is high or low by AND'ing that with the bit mask ""&& B10000000)"" this gives 1 or 0
                // if it's a 1, we'll OR that with the Existing port, thus keeping the pin HIGH, if 0 the pin is written LOW


                PORTB = ((RGB[i] & B10000000) && B10000000) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t");//these are NOPS - these let us delay clock cycles for more precise timing
                PORTB = ExistingPort;//okay, here we know we have to be LOW regardless of the 0/1 bit state
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t");//minimum LOW time for pin regardless of 0/1 bit state

                // then do it again for the next bit and so on... see the last bit though for a slight change

                PORTB = WS2812pinHIGH;//bit 6
                PORTB = ((RGB[i] & B01000000) && B01000000) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t");
                PORTB = ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t" );

                PORTB = WS2812pinHIGH;//bit 5
                PORTB = ((RGB[i] & B00100000) && B00100000) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" );
                PORTB = ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t" );

                PORTB = WS2812pinHIGH;//bit 4
                PORTB = ((RGB[i] & B00010000) && B00010000) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" );
                PORTB = ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t" );

                PORTB = WS2812pinHIGH;//bit 3
                PORTB = ((RGB[i] & B00001000) && B00001000) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" );
                PORTB = ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t");

                PORTB = WS2812pinHIGH;//bit 2
                PORTB = ((RGB[i] & B00000100) && B00000100) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" );
                PORTB = ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t");

                PORTB = WS2812pinHIGH;//bit 1
                PORTB = ((RGB[i] & B00000010) && B00000010) | ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t");
                PORTB = ExistingPort;
                __asm__ ("nop\n\t" "nop\n\t" "nop\n\t" );

                PORTB = WS2812pinHIGH;//bit 0
                __asm__ ("nop\n\t" "nop\n\t" );//on this last bit, the check is much faster, so had to add a NOP here
                PORTB = ((RGB[i] & B00000001) && B00000001) | ExistingPort;
                __asm__ ( "nop\n\t" "nop\n\t" "nop\n\t");
                PORTB = ExistingPort;//note there are no NOPs after writing the pin LOW, this is because the FOR Loop uses clock cycles that we can use instead of the NOPS


        }
        interrupts();//enable the interrupts
        delayMicroseconds(80); // min delay
// all done!
}//void RGB_update

void setup() {
        pinMode(WS2812_pin, OUTPUT);
        clearLEDs();
        RGB_update(-1, 0, 0, 0);

}//setup


void loop() {
int B = 255;
        mapLEDXY(0, 0, 0, 0, B); mapLEDXY(0, 15, 0, 0, B); mapLEDXY(1, 1, 0, 0, B);   // x,y , R ,G , B  // B=255
        mapLEDXY(1, 14, 0, 0, B); mapLEDXY(2, 2, 0, 0, B); mapLEDXY(2, 13, 0, 0, B);
        mapLEDXY(3, 3, 0, 0, B); mapLEDXY(3, 12, 0, 0, B); mapLEDXY(4, 4, 0, 0, B);
        mapLEDXY(4, 11, 0, 0, B); mapLEDXY(5, 5, 0, 0, B); mapLEDXY(5, 10, 0, 0, B);
        mapLEDXY(6, 6, 0, 0, B); mapLEDXY(6, 9, 0, 0, B); mapLEDXY(7, 7, 0, 0, B);
        mapLEDXY(7, 8, 0, 0, B); mapLEDXY(8, 7, 0, 0, B); mapLEDXY(8, 8, 0, 0, B);
        mapLEDXY(9, 6, 0, 0, B); mapLEDXY(9, 9, 0, 0, B); mapLEDXY(10, 5, 0, 0, B);
        mapLEDXY(10, 10, 0, 0, B); mapLEDXY(11, 4, 0, 0, B); mapLEDXY(11, 11, 0, 0, B);
        mapLEDXY(12, 3, 0, 0, B); mapLEDXY(12, 12, 0, 0, B); mapLEDXY(13, 2, 0, 0, B);
        mapLEDXY(13, 13, 0, 0, B); mapLEDXY(14, 1, 0, 0, B); mapLEDXY(14, 14, 0, 0, B);
        mapLEDXY(15, 0, 0, 0, B); mapLEDXY(15, 15, 0, 0, B);


        RGB_update(-1, 0, 0, 0);
        delay(1000);
       // clearLEDs();
       // RGB_update(-1, 0, 0, 0);
       // delay(1000);

        for (int i = 0; i < 256; i++) {
                mapLEDXY(0, 0, i, 0, 0); mapLEDXY(0, 14, i, 0, 0); mapLEDXY(1, 1, i, 0, 0); // x,y , R ,G , B  // R= i
                mapLEDXY(1, 14, i, 0, 0); mapLEDXY(2, 2, i, 0, 0); mapLEDXY(2, 13, i, 0, 0);
                mapLEDXY(3, 3, i, 0, 0); mapLEDXY(3, 12, i, 0, 0); mapLEDXY(4, 4, i, 0, 0);
                mapLEDXY(4, 11, i, 0, 0); mapLEDXY(5, 5, i, 0, 0); mapLEDXY(5, 10, i, 0, 0);
                mapLEDXY(6, 6, i, 0, 0); mapLEDXY(6, 9, i, 0, 0); mapLEDXY(7, 7, i, 0, 0);
                mapLEDXY(7, 8, i, 0, 0); mapLEDXY(8, 7, i, 0, 0); mapLEDXY(8, 8, i, 0, 0);
                mapLEDXY(9, 6, i, 0, 0); mapLEDXY(9, 9, i, 0, 0); mapLEDXY(10, 5, i, 0, 0);
                mapLEDXY(10, 10, i, 0, 0); mapLEDXY(11, 4, i, 0, 0); mapLEDXY(11, 11, i, 0, 0);
                mapLEDXY(12, 3, i, 0, 0); mapLEDXY(12, 12, i, 0, 0); mapLEDXY(13, 2, i, 0, 0);
                mapLEDXY(13, 13, i, 0, 0); mapLEDXY(14, 1, i, 0, 0); mapLEDXY(14, 14, i, 0, 0);
                mapLEDXY(15, 0, i, 0, 0); mapLEDXY(15, 15, i, 0, 0);
                RGB_update(-1, 0, 0, 0);
                delay(2);
        }
        delay(1000);
        for (int i = 255; i >= 0; i--) {
                mapLEDXY(0, 0, i, 0, 0); mapLEDXY(0, 14, i, 0, 0); mapLEDXY(1, 1, i, 0, 0); // x,y , R ,G , B  // R = i
                mapLEDXY(1, 14, i, 0, 0); mapLEDXY(2, 2, i, 0, 0); mapLEDXY(2, 13, i, 0, 0);
                mapLEDXY(3, 3, i, 0, 0); mapLEDXY(3, 12, i, 0, 0); mapLEDXY(4, 4, i, 0, 0);
                mapLEDXY(4, 11, i, 0, 0); mapLEDXY(5, 5, i, 0, 0); mapLEDXY(5, 10, i, 0, 0);
                mapLEDXY(6, 6, i, 0, 0); mapLEDXY(6, 9, i, 0, 0); mapLEDXY(7, 7, i, 0, 0);
                mapLEDXY(7, 8, i, 0, 0); mapLEDXY(8, 7, i, 0, 0); mapLEDXY(8, 8, i, 0, 0);
                mapLEDXY(9, 6, i, 0, 0); mapLEDXY(9, 9, i, 0, 0); mapLEDXY(10, 5, i, 0, 0);
                mapLEDXY(10, 10, i, 0, 0); mapLEDXY(11, 4, i, 0, 0); mapLEDXY(11, 11, i, 0, 0);
                mapLEDXY(12, 3, i, 0, 0); mapLEDXY(12, 12, i, 0, 0); mapLEDXY(13, 2, i, 0, 0);
                mapLEDXY(13, 13, i, 0, 0); mapLEDXY(14, 1, i, 0, 0); mapLEDXY(14, 14, i, 0, 0);
                mapLEDXY(15, 0, i, 0, 0); mapLEDXY(15, 15, i, 0, 0);
                RGB_update(-1, 0, 0, 0);
                delay(2);
        }
        delay(1000);
      //  clearLEDs();
      //  RGB_update(-1, 0, 0, 0);



}//loop






