//Written by Ryan Burgert 2020
//Some utility functions I'm using.

#include "../include/ryan.h"
String userInputPrompt(const String &prompt)
{
    //Give a prompt over serial, where the user is probably on my laptop typing commands.
    //Right now, that user is probably Ryan Burgert.
    //This function lets you type out a prompt and then hit enter, resulting in a single-line string returned by this function.
    Serial.print(prompt);
    String output="";
    while(true)
    {
        yield();//Don't block this thread. Yield on every loop cycle.
        int input=Serial.read();
        if(input!=-1)//If input==-1, it's Arduino's way of telling us we have no waiting serial messages
        {
            auto inputChar=char(input);//If the input isn't -1, it's supposed to be an int from 0 to 255 inclusive
            if(inputChar=='\b')
            {
                //Handle the backspace key
                size_t outputLength=output.length();
                if(outputLength>0)
                {
                    output=output.substring(0,outputLength-1);//Delete the last character from the output string
                    Serial.print("\b \b");//Delete the last character on the console: \b moves cursor to left, ' ' overwrites it
                }
            }
            else
            {
                //Handle any character we display
                Serial.print(inputChar);//Show what we type as we type it
                if(inputChar=='\n')
                {
                    return output;
                }
                else
                {
                    output+={inputChar};
                }
            }
        }
    }
}
