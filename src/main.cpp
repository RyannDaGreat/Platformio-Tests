#ifndef CLION
//Clion doesn't know this, but actually CLION is defined because of platformio.ini . These lines are just for helping in my IDE. Don't actually compile with them.
#include "../../../../../.platformio/packages/framework-arduinoespressif8266/cores/esp8266/HardwareSerial.h"
#else
#include "Arduino.h"
#endif

#include "../include/escaper.h"
#include "../include/ryan.h"
#include "../lib/ArduinoJson/ArduinoJson.h"

Unescaper u;
void handler(char*message,unsigned int length)
{
    Serial.print("CHUNK: ");
    while(length--)
    {
        Serial.print(*(message++));
    }
    Serial.println();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("HELLO WORLD!");
}

void loop()
{
    String s=userInputPrompt();

    Serial.print("GOT MESSAGE:");
    Serial.print(s);
    Serial.println();

    char array[s.length()];

    s.toCharArray(array, s.length());


    u.feed(array,s.length(),handler);
}


