#ifndef RAMICARTRIDGE_H
#define RAMICARTRIDGE_H
#include <QDebug>
#include <iostream>
#include <regex>
#include <bitset>

/* This class allows you to communicate using the RAMI brand cartridge.
 * To use encryption, create a Params structure containing your parameters,
 * then pass it to the encrypt() static method
 * You'll get a Data structure as an answer
 * You should be able to print the result using the print(Data) or print(Params) static methods
*/

class RamiProtocol
{

typedef char unsigned uchar;
public:
    typedef struct{
        uchar adressColum;
        uchar lineState;
        uchar ack;
    }Data;

    typedef struct Params{
        static const uchar adress=0x8;
        static const uchar ack='!';
        uchar row;
        uchar column;
        bool state;
    }Params;

    static RamiProtocol& instance();
    static void print(const Data&);
    static void print(const Params&);
    static Data encrypt(const Params& params);
    static Params decrypt(const Data&);
    static Params decrypt(const std::string&);

private:

    static Data encrypt(uchar adress, uchar line, uchar column, bool state, uchar ack);
    static RamiProtocol m_instance;
    RamiProtocol();
    ~RamiProtocol();
};

#endif // RAMICARTRIDGE_H
