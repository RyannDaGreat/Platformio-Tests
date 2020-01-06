/*

Written by Ryan Burgert 2019

How to send separate messages over serial safely?

Goal: Send messages that can be checked, and if they're wrong, dump them.
Send arbitrary bytes (so just waiting for a '\n' doesn't cut it; I'd eventually like to use messagepack (which is a compressed version of JSON))

Escaping:
  Two Bytes:
    Delimiter byte
      This byte is never sent in any messages; when this byte is received it means 'end of message' like a null terminator.
    Escape byte
      There will be equal to or more of these in the final message than in the original
    Escape    Helper byte
      When this byte follows an escape byte, it's treated as the original escape character
      There will be equal to or more of these in the final message than in the orgiginal
    Delimiter Helper byte
      When this byte follows an escape byte, it's treated as the delimiter
      There will be equal to or more of these in the final message than in the orgiginal

    No two of these four bytes can be the same; that would break the algorithm
    The escape and delimiter bytes should be chosen to be chosen such that they appear least frequently in messages (the penalty is at most doubling the message size)
    The choice of byte for the delimiter helper and escape helper bytes shouldn't have any impact on performance or memory whatsoever (it's completely arbitrary. They just have to be unique, that's all.)

    How it works:
      Let's call the Delimiter Byte D,
                    the Escape byte E,
              delimiter helper byte d,
         and the escape helper byte e.

      Procedures:
         * Note: A 'message' is an array of bytes
        To 'escapify'   a message, search/replace (E with Ee) and (D with Ed)
        To 'unescapify' a message, search/replace (Ee with E) and (Ed with D)

      The goal of 'escapifying' a message is to turn a string of bytes into a string of bytes that's guarenteed not to contain D, which lets us use D to split multiple messages from a stream

       * Note: Not all received serial messages are valid. For example, if a raw message ends with E, it is invalid (because E must always be followed by either e or d)
           If an invalid message is received, it should be discarded.
           Regex should fully match the given message: ([^E]|(Ee)|(Ed))*
       * Note: The unescaped message is NEVER larger than the escaped message. The escaped message is either equal or larger in size. Therefore, the escaped message can be modified in-place, saving memory on an arduino (so we don't have to make a second copy)
       * Note: The size of the unescaped message is the size of the escaped message minus the number of E's in the escaped message

  I have tested this code, but I don't know how to include those tests in this file without making it really mucky and eating up the Arduino's program memory. I'll leave unit tests for another day...
*/

#ifndef ESCAPER_H //No need for a header file; this file has no dependencies
#define ESCAPER_H

//DELIMITER, ESCAPE, DELIMITER_HELPER and ESCAPE_HELPER must all be unique
const char DELIMITER       ='D';
const char ESCAPE          ='E';
const char DELIMITER_HELPER='d';
const char ESCAPE_HELPER   ='e';
const unsigned int UNESCAPER_BUFFER_SIZE=1024;//The max unescaped message length

class Unescaper
{
  private:
    bool escape=false;
    bool valid =true ;//If at some point we overflowed, or made some regex error along the way that can't possibly be fixed before DELIMITER arrives
    bool start =true ;
    inline void reset()
    {
        escape=false;
        start =true ;
        valid =true ;
    }
  public:
    char buffer[UNESCAPER_BUFFER_SIZE];
    unsigned int length=0;

    bool feed(char c);

    void feed(char *characters,
              unsigned int size,
              void handler(char *message,unsigned int length));
};


void escapeAndSendMessage(void send(char),
                          char*message,
                          unsigned int length);

#endif