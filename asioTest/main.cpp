//
//  main.cpp
//  asioTest
//
//  Created by apple on 2019/12/3.
//  Copyright © 2019 apple. All rights reserved.
//

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <vector>

using namespace boost;
using boost::asio::ip::tcp;


class RpcDeviceChannel : public std::enable_shared_from_this<RpcDeviceChannel>
{
public:

    RpcDeviceChannel(tcp::socket socket):socket_(std::move(socket))
    {
    }
    
    void do_read_header()
    {
        auto self(shared_from_this());

        uint32_t raw_header;
        auto buf = asio::buffer((void*)&raw_header, 4);
        
        asio::async_read(socket_, buf, [this, self, raw_header](std::error_code ec, std::size_t length){

            printf("read header: %lu \n", length);

            if (ec) {
            }
            else{
                //decode header
                uint32_t body_length = ntohl(raw_header);
                do_read_body(body_length);
            }
        });
        
    }
    
    void do_read_body(uint32_t body_length)
    {
        auto self(shared_from_this());
        
        std::string str;
        str.resize(body_length);
        
        asio::async_read(socket_, asio::buffer(str), [this, self, str](boost::system::error_code ec, std::size_t length) {
            
            if (ec) {
            }else{
                this->do_read_rpc( str );
            }
            
        });
        
    }
    
    void do_read_rpc(std::string rpc)
    {
     
        
        
        
    }
    
    
private:
    tcp::socket socket_;

};




class RpcManager {

public:
    RpcManager(asio::io_context &io_context, const asio::ip::tcp::endpoint &endpoint): acceptor_(io_context, endpoint)
    {
        do_accept();
    }
private:
    void do_accept()
    {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket){
            if (ec) {
                printf("accept error.\n");
            }
            else{
                printf("a client coming.\n");

                auto pDeviceChannel =  std::make_shared<RpcDeviceChannel>( std::move(socket) );
                
                deviceChannels_.push_back(pDeviceChannel);
                
                pDeviceChannel->do_read_header();
                
            }
            
            do_accept();
        });
        
    }
    
    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<RpcDeviceChannel>> deviceChannels_;
};




int main(int argc, const char * argv[]) {

    asio::io_context io_context;

    tcp::endpoint endpoint(tcp::v4(), 9098);
    
    RpcManager manager(io_context, endpoint);

    io_context.run();

    return 0;
}
