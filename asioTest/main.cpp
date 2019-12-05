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
#include "rpccall.pb.h"

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

        
        auto buf = asio::buffer((void*)&header_buf_, 4);
        
        asio::async_read(socket_, buf, [this, self](std::error_code ec, std::size_t length){

            printf("read header: %lu \n", length);

            if (ec) {
            }
            else{
                
                if (length == 4) {
                    printf("read head ok: %d\n", length);
                    
                    
                    //decode header
                    uint32_t body_length = ntohl(this->header_buf_);
                    do_read_body(body_length);
                    
                }else{
                    printf("read head failed.\n");
                }

            }
        });
        
    }
    
    void do_read_body(uint32_t body_length)
    {
        printf("to read body of length: %d \n", body_length);
        
        auto self(shared_from_this());
        
        body_buf_.resize(body_length);
        
        asio::async_read(socket_, asio::buffer(body_buf_), [this, self](boost::system::error_code ec, std::size_t length) {
            
            if (ec) {
            }else{
                
                if (length == this->body_buf_.length()) {
                    printf("read body ok: %d\n", length);
                    
                    
                    this->do_read_rpc( this->body_buf_ );
                    this->body_buf_.clear();
                    
                }else{
                    printf("read body failed.\n");
                }

                
            }
            
        });
        
    }
    
    void do_read_rpc(std::string& data)
    {
        if (deviceName_.empty()) {
            
            DeviceInfo deviceInfo;
            deviceInfo.ParseFromString(data);

            printf("接收到鉴定: %s \n",  deviceInfo.DebugString().c_str());
            
            deviceName_ = deviceInfo.devicename();
        }
        else{
            RPCCall rpccall;
            rpccall.ParseFromString(data);
            
            printf("接收到服务器消息: %s \n",  rpccall.DebugString().c_str() );
        }
        
    }
    
    
private:
    tcp::socket socket_;
    std::string deviceName_;
    
    uint32_t header_buf_;
    std::string body_buf_;
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
