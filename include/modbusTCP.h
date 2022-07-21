#pragma once

#include <boost/asio.hpp>
#include <utility>

using boost::asio::ip::tcp;

/**
 * @brief Lib for using modbusTCP protocol with Advantech Adam devices
 * 
 */

namespace modbus
{
    class ModbusTCP
    {
    public:
        /**
         * @brief Construct a new ModbusTCP object
         * 
         * @param[in] service   boost::asio::io_service to construct socket connection
         * 
         */
        explicit ModbusTCP(boost::asio::io_service& service)
                :   sock_(service) {};

        /**
         * @brief Construct a new ModbusTCP object
         * 
         * @param[in] service   boost::asio::io_service to construct socket connection
         * @param[in] ip        ip address to connect to  
         * 
         */
        ModbusTCP(boost::asio::io_service& service, std::string ip)
                :   sock_(service),
                    ip_address_(std::move(ip)) {};

        /**
         * @brief Construct a new ModbusTCP object
         * 
         * @param[in] service   boost::asio::io_service to construct socket connection
         * @param[in] ip        ip address to connect to  
         * @param[in] port      determines port number 
         * 
         */
        ModbusTCP(boost::asio::io_service& service, std::string ip, uint16_t port)
                :   sock_(service),
                    ip_address_(std::move(ip)),
                    port_(port) {};

        /**
         * @brief Construct a new ModbusTCP object
         * 
         * @param[in] service   boost::asio::io_service to construct socket connection
         * @param[in] ip        ip address to connect to  
         * @param[in] port      determines port number 
         * @param[in] name      Name of the object, if nessesary
         * 
         */
        ModbusTCP(boost::asio::io_service& service, std::string ip, uint16_t port, std::string name)
                :   sock_(service),
                    ip_address_(std::move(ip)),
                    port_(port),
                    name_(std::move(name)) {};
        /**
         * @brief Destroy the ModbusTCP object
         * 
         */
        ~ModbusTCP() = default;
        
        // connection
        bool connect();
        bool is_connected();

        // Setters
        void set_name(std::string new_name);
        void set_ip_address(std::string ip);
        void set_port(uint16_t port);

        // Getters
        std::string get_name() const;
        std::string get_ip_address() const;
        uint16_t get_port() const;

        bool read_DO(uint8_t address);
        bool read_DI(uint8_t address);
        uint16_t read_AO(uint8_t address);
        size_t read_AI();
        size_t write_DO(uint8_t adress, uint8_t value);
        size_t write_AO(uint8_t adress, uint8_t value);
        size_t write_DOs(uint8_t adress, uint8_t count, uint8_t value);
        size_t write_AOs(uint8_t adress, uint8_t count, uint8_t value);
    private:
        void init_bytes_to_send();
        uint8_t sock_send(uint8_t bytes);
        uint8_t sock_read();

    private:
        // boost::asio::ip::tcp
        tcp::endpoint ep_;
        tcp::socket sock_;

        std::array<uint8_t,17> send_;
        std::array<uint8_t,13> rcv_;

        std::string name_ = "unknown";
        std::string ip_address_;
        uint16_t port_ = 502;
    };

    /**
     * @brief connects to endpoint ip address
     * 
     * @return true     connection confermed
     * @return false    connection failed
     * 
     * @todo add loging in case failed connection
     */
    bool ModbusTCP::connect() 
    {
        ep_.address(boost::asio::ip::address::from_string(ip_address_));
        ep_.port(port_);

        if (ep_.address().is_unspecified())
        {
            // No address is specified, connection cannot be completed
            //loging
            return false;
        }
        
        try
        {
            sock_.connect(ep_);
        }
        catch (...)
        {
            //loging
        }
        return true;
    }

    bool ModbusTCP::is_connected()
    {
        return sock_.is_open();
    }


    /**
     * @brief set name
     * 
     * @param name 
     */
    void ModbusTCP::set_name(std::string name) 
    {
        name_ = std::move(name);
    }

    /**
     * @brief set ip address
     * 
     * @param ip 
     */
    void ModbusTCP::set_ip_address(std::string ip) 
    {
        ip_address_ = std::move(ip);
    }

    /**
     * @brief set port number
     * 
     * @param port 
     */
    void ModbusTCP::set_port(uint16_t port) 
    {
        port_ = port;
    }

    /**
     * @brief get name of the object
     * 
     * @return std::string 
     */
    std::string ModbusTCP::get_name() const 
    {
        return name_;
    }

    /**
     * @brief get ip address
     * 
     * @return std::string 
     */
    std::string ModbusTCP::get_ip_address() const 
    {
        return ip_address_;
    }

    /**
     * @brief get port number
     * 
     * @return uint16_t 
     */
    uint16_t ModbusTCP::get_port() const 
    {
        return port_;
    }

    /**
     * @brief read digital output at address
     * 
     * @param address       coil address
     * @return true 
     * @return false 
     */
    bool read_DO(uint8_t address)
    {
        return true;
    }

    /**
     * @brief read digital input at address
     * 
     * @param address       coil address
     * @return true    
     * @return false 
     */
    bool read_DI(uint8_t address)
    {
        return true;

    }

    /**
     * @brief read analog output at address
     * 
     * @param address       coil address  
     * @return uint16_t     Dec value
     */
    uint16_t ModbusTCP::read_AO(uint8_t address) 
    {
        init_bytes_to_send();
        send_[7] = 3;
        send_[9] = address;
        send_[11] = 1;
        auto response = sock_send(12);
        if (response != 12)
        {
            //return fail
        }
        
        response = sock_read();
        if (response == 7)
        {
            return 6666;
        }

        // the result consists of two bytes 10th and 11th
        uint16_t value = rcv_[9];
        // merging 2 bytes in uint16_t value and returning it as result
        value = (value << 8) | rcv_[10];
        return value;
    }

    /**
     * @brief initiates default values of bytes for sending
     * 
     */
    void ModbusTCP::init_bytes_to_send()
    {
        send_ = {0, 0, 0, 0, 0, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    }

    /**
     * @brief write byte_count through socket
     * 
     * @param byte_count    number of bytes to write
     * @return uint8_t      number of byte written
     */
    uint8_t ModbusTCP::sock_send(uint8_t byte_count)
    {
        try
        {
            return sock_.write_some(boost::asio::buffer(send_, byte_count));  
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return 0;
        }
        
    }

    /**
     * @brief read bytes from socket
     * 
     * @return uint8_t      number of bytes read
     */
    uint8_t ModbusTCP::sock_read()
    {
        return sock_.read_some(boost::asio::buffer(rcv_));
    }
}