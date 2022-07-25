#pragma once
// 3d party lib
#include <boost/asio.hpp>

// standard lib
#include <optional>
#include <sstream>
// #include <utility>

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

        // Setters
        void set_name(std::string new_name);
        void set_ip_address(std::string ip);
        void set_port(uint16_t port);

        // Getters
        std::string get_name() const;
        std::string get_ip_address() const;
        uint16_t get_port() const;

        std::optional<uint8_t> read_DO(uint8_t address,  uint8_t reg_count = 1);
        std::optional<uint8_t> read_DI(uint8_t address,  uint8_t reg_count = 1);
        std::optional<uint16_t> read_AO(uint8_t address);
        std::optional<uint16_t> read_AI(uint8_t address);
        bool write_DO(uint8_t address, bool value);
        bool write_AO(uint8_t address, uint8_t value);
        size_t write_DOs(uint8_t address, uint8_t count, uint8_t value);
        size_t write_AOs(uint8_t address, uint8_t count, uint8_t value);
    private:
        void init_bytes_to_send();
        void sock_send(uint8_t bytes);
        void sock_read();
        std::string get_error_msg(uint8_t err);

    private:
        // boost::asio::ip::tcp
        tcp::endpoint ep_;
        tcp::socket sock_;

        //connection
        bool is_connected = false;

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
            /**
             * @todo add loging using boost::log
             * 
             */
            is_connected = false;
        }
        else
        {
            try
            {
                sock_.connect(ep_);
                is_connected = true;
            }
            catch (...)
            {
                /**
                 * @todo add loging using boost::log
                 * 
                 */
            }
        }
        
        return is_connected;
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

        // after changing ip address reconnection is required
        is_connected = false;
    }

    /**
     * @brief set port number
     * 
     * @param port 
     */
    void ModbusTCP::set_port(uint16_t port) 
    {
        port_ = port;

        // after changing port number reconnection is required
        is_connected = false;
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
     * @brief read digital output(s) at address
     * 
     * @param address       coil address
     * @param reg_count     number of registers following the address
     * @return std::optional<uint8_t>   each bit is responsible for coil condition 0/1 - OFF/ON
     */
    std::optional<uint8_t> ModbusTCP::read_DO(uint8_t address, uint8_t reg_count)
    {
        std::optional<uint8_t> result;
        try
        {
            init_bytes_to_send();
            send_[7] = 1;
            send_[9] = address;
            send_[11] = reg_count;

            sock_send(12);           
            sock_read();
            
            result = rcv_[9];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }    
        return result;
    }

    /**
     * @brief read digital input at address
     * 
     * @param address       coil address
     * @param reg_count     number of registers following the address
     * @return std::optional<uint8_t>   each bit is responsible for coil condition 0/1 - OFF/ON 
     */
    std::optional<uint8_t> ModbusTCP::read_DI(uint8_t address, uint8_t reg_count)
    {
        std::optional<uint8_t> result;
        try
        {
            init_bytes_to_send();
            send_[7] = 2;
            send_[9] = address;
            send_[11] = reg_count;

            sock_send(12);
            sock_read();

            result = rcv_[9];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }    
        return result;
    }

    /**
     * @brief read analog output at address
     * 
     * @param address       coil address  
     * @return uint16_t     Dec value
     */
    std::optional<uint16_t> ModbusTCP::read_AO(uint8_t address) 
    {
        std::optional<uint16_t> result;
        try
        {
            init_bytes_to_send();
            send_[7] = 3;
            send_[9] = address;
            send_[11] = 1;
            
            sock_send(12);
            sock_read();
            
            // the result consists of two bytes 10th and 11th
            result = rcv_[9];

            // merging 2 bytes in uint16_t value and returning it as result
            result = (result.value() << 8) | rcv_[10];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }    
        return result;
    }

    std::optional<uint16_t> ModbusTCP::read_AI(uint8_t address)
    {
        std::optional<uint16_t> result;
        try
        {
            init_bytes_to_send();
            send_[7] = 4;
            send_[9] = address;
            send_[11] = 1;
            
            sock_send(12);
            sock_read();
            
            // the result consists of two bytes 10th and 11th
            result = rcv_[9];

            // merging 2 bytes in uint16_t value and returning it as result
            result = (result.value() << 8) | rcv_[10];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }    
        return result;
    }

    bool ModbusTCP::write_DO(uint8_t address, bool turn_on)
    {
        try
        {
            init_bytes_to_send();
            send_[7] = 5;
            send_[9] = address;
            if (turn_on)
            {
                send_[10] = '\xff';
            }
            else
            {
                send_[10] = 0;
            }

            sock_send(12);
            sock_read();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
        return true;
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
    void ModbusTCP::sock_send(uint8_t byte_count)
    {
        uint8_t bytes_sent = 0;

        if (!is_connected)
        {
            connect();
        }

        try
        {
            bytes_sent = sock_.write_some(boost::asio::buffer(send_, byte_count));  
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            is_connected = false;
        }

        if (bytes_sent != byte_count)
        {
            /**
             * @todo add loging using boost::log
             * 
             */
            std::stringstream err_msg;
            err_msg << "Was sent: " << bytes_sent << " bytes. Should be: " << byte_count;
            throw std::exception(err_msg.str().c_str());
        }
    }

    /**
     * @brief read bytes from socket
     * 
     * @return uint8_t      number of bytes read
     */
    void ModbusTCP::sock_read()
    {
        // number '9' is correspond situetion when device sending an error message
        uint8_t bytes_recieved = 9;
        if (!is_connected)
        {
            connect();
        }

        try
        {
            bytes_recieved = sock_.read_some(boost::asio::buffer(rcv_));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            is_connected = false;
        }

        if (bytes_recieved == 9)
        {
            /**
             * @todo add loging using boost::log
             * 
             */
            std::string err_msg = get_error_msg(rcv_[8]);
            throw std::exception(err_msg.c_str());
        }
    }

    std::string ModbusTCP::get_error_msg(uint8_t err)
    {
        std::stringstream err_msg;
        err_msg << "ModBus read error: ";
        switch (err)
        {
            case '\x01':
            err_msg << "Received function code could not be processed";
            break;
            case '\x02':
            err_msg << "The data address specified in the request is not available";
            break;
            case '\x03':
            err_msg << "The value contained in the request data field is an invalid value";
            break;
            case '\x04':
            err_msg << "An unrecoverable error occurred while the slave attempted to perform the requested action";
            break;
            case '\x05':
            err_msg << "The slave has received the request and is processing it, but it takes a long time. \
                        This response prevents the master from generating a timeout error";
            break;
            case '\x06':
            err_msg << "The slave device is busy processing the command. \
                        The master must repeat the message later when the slave is free.";
            break;
            case '\x07':
            err_msg << "The slave device cannot execute the program function specified in the request. \
                        This code is returned for an unsuccessful program request using function numbers 13 or 14. \
                        The master must request diagnostic or error information from the slave.";
            break;
            case '\x08':
            err_msg << "The slave device encountered a parity error while reading extended memory. \
                        The master may repeat the request, but usually in such cases repair is required";
            break;
            case '\x0a':
            err_msg << "Gateway misconfigured or overloaded with requests";
            break;
            case '\x0b':
            err_msg << "Slave device is not online or there is no response from it";
            break;
            
            default:
            err_msg << "Unknown Error";
        }
        return err_msg.str();
    }

}

