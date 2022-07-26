#include <iostream>
#include <string>
#include "modbusTCP.h"
#include <array>


#include <optional>
#include <sstream>

struct Valve_2_2{
    Valve_2_2(modbus::ModbusTCP* adam, uint8_t address) : adam_(adam), address_(address){}
    void open()
    {
        adam_->write_DO(address_, true);
    }
    void close()
    {
        adam_->write_DO(address_, false);
    }
    bool is_opened()
    {
        return adam_->read_DO(address_).value_or(false);
    }
    private:
        uint8_t address_;
        modbus::ModbusTCP* adam_;
};

using boost::asio::ip::tcp;


int main()
{
    // boost::asio::io_service service;
    // modbus::ModbusTCP* adam5000 = new modbus::ModbusTCP(service, "192.168.1.10", 502, "adam5000");
    // adam5000->connect();
    // adam5000->write_AOs(56, 2, 0, 0);
    return 0;
}