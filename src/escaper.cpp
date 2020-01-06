#include "../include/escaper.h"

bool Unescaper::feed(char c)
{
    if(start)
    {
        length=0;
        start=false;
    }
    if(c==DELIMITER)
    {
        if(valid and not escape)
        {
            reset();
            return true;
        }
        else
        {
            reset();
            return false;
        }
    }
    else
    {
        if(valid)
        {
            if(length>=UNESCAPER_BUFFER_SIZE)
            {
                //You're overflowing the buffer (the message is too big)...
                valid=false;
            }
            else if(escape)
            {
                escape=false;
                switch(c)
                {
                    case DELIMITER_HELPER: buffer[length++]=DELIMITER; break;
                    case ESCAPE_HELPER   : buffer[length++]=ESCAPE   ; break;
                    default              : valid           =false    ; break;
                }
            }
            else if(c==ESCAPE)
            {
                escape=true;
            }
            else
            {
                buffer[length++]=c;
            }
        }
        return false;
    }
}

void Unescaper::feed(char *characters,
          unsigned int size,
          void handler(char *message,unsigned int length))
{
    //The handler might be called multiple times given a string of characters
    //This is meant for feeding a serial fragment
    while(size--)
    {
        char c=*(characters++);
        if(feed(c))
        {
            handler(buffer,length);
        }
    }
}


void escapeAndSendMessage(void send(char),
                          char*message,
                          unsigned int length)
{
    //Send should be something like Serial.write(char)
    //Given an unescaped message, send a whole escaped message
    //(this function runs with constant memory, as opposed to writing a new string and sending that)
    while(length--)
    {
        char c=*(message++);
        switch(c)
        {
            case DELIMITER: send(ESCAPE); send(DELIMITER_HELPER); break;
            case ESCAPE   : send(ESCAPE); send(ESCAPE_HELPER   ); break;
            default       : send(c     );
        }
    }
}