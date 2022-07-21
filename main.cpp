#include <iostream>
// #include <boost/asio.hpp>
#include "modbusTCP.h"
#include <array>


using boost::asio::ip::tcp;

int main()
{
    boost::asio::io_service service;
    modbus::ModbusTCP adam5000(service, "192.168.1.10", 502, "adam5000");
    adam5000.connect();
    // int a;
    // std::cin >> a;
    // std::cout << std::boolalpha << adam5000.is_connected();
    std::cout << adam5000.read_AO(48);
//    tcp::endpoint adam5000(boost::asio::ip::address::from_string("192.168.1.10"), 502);
//    tcp::socket sock(service);
//    sock.connect(adam5000);
    // отправка
//    uint8_t send[16];
//    std::array<uint8_t,16> send{1};
//    send[0] = 0;
//    send[1] = 0;
//    send[2] = 0;
//    send[3] = 0;
//    send[4] = 0;
//    send[5] = 6;
//    send[6] = 1;
//    send[7] = 3;
//    send[8] = 0;
//    send[9] = 48;
//    send[10] = 0;
//    send[11] = 1;
//    auto a = sock.write_some(boost::asio::buffer(send, 12));

//    modbus::ModbusTCP modbusTCP(service)
//    //прием
//    uint8_t rcv[16];
//    sock.read_some(boost::asio::buffer(rcv));
//    uint16_t b = rcv[9];
//    b = (b << 8) | rcv[10];
//    std::cout << b;
    return 0;
}