#include "ramiProtocol.h"

RamiProtocol RamiProtocol::m_instance = RamiProtocol();
RamiProtocol::RamiProtocol(){
}

RamiProtocol::~RamiProtocol(){

}

RamiProtocol& RamiProtocol::instance(){
   return m_instance;
}

RamiProtocol::Data RamiProtocol::encrypt(uchar adress, uchar line, uchar column, bool state, uchar ack){
    RamiProtocol::Data result;
    //we make a binary mask '1111'
    char mask = (1 << 5)-1;
    result.adressColum=(adress & mask) << 4;
    result.adressColum+=(column & mask);
    result.lineState=(line & mask) << 4;
    result.lineState+=state;
    result.ack=ack;
    return result;
}

RamiProtocol::Data RamiProtocol::encrypt(const Params& params){
    return RamiProtocol::encrypt(params.adress,params.row,params.column,params.state,params.ack);
}

void RamiProtocol::print(const Data& data){
    std::bitset<8> adressColumn(data.adressColum), lineState(data.lineState), ack(data.ack);
    std::cout << "adressColumn:" << adressColumn << std::endl
                 << "lineState:" << lineState << std::endl
                 << "ack:" << ack << std::endl;
}

void RamiProtocol::print(const Params& params){
    std::cout << "adress:" << std::bitset<8>(params.adress) << std::endl
              << "column:" << std::bitset<8>(params.column) << std::endl
              << "line:" << std::bitset<8>(params.row) << std::endl
              << "state:" << params.state << std::endl
              << "ack:" << std::bitset<8>(params.ack) << std::endl;
}

RamiProtocol::Params RamiProtocol::decrypt(const Data& data){
    std::string str;
    str.append(1,data.adressColum);
    str.append(1,data.lineState);
    str.append(1,data.ack);
    return RamiProtocol::decrypt(str);
}

RamiProtocol::Params RamiProtocol::decrypt(const std::string& str){
    RamiProtocol::Params result;
    std::smatch sm;
    //TODO: create regex in case we increase number of columns possible
    bool matches=std::regex_match(str,sm,std::regex("[\x81\x82\x83\x84].!"));
    qDebug() << "rami protocol matches:" << matches;
    //TODO: add exception in case it doesn't match the protocol
    if(matches){
        std::string matchedStr = sm[0];
        result.column=matchedStr[0]-0x80;
        result.row=matchedStr[1]/16;
        result.state=matchedStr[1]&1;
        return result;
    }
}
