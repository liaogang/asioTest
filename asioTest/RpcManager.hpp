//
//  RpcManager.hpp
//  asioTest
//
//  Created by apple on 2019/12/6.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef RpcManager_h
#define RpcManager_h

#include "RpcDeviceChannel.hpp"

using namespace boost;
using boost::asio::ip::tcp;

class RpcManager {

public:
    RpcManager(asio::io_context &io_context, const asio::ip::tcp::endpoint &endpoint):io_context_(io_context), acceptor_(io_context, endpoint)
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

                auto deviceChannel = std::make_shared<RpcDeviceChannel>(io_context_, std::move(socket));
                deviceChannels_.push_back(deviceChannel);
                deviceChannel->do_read_header();
                
            }
            
            do_accept();
        });
        
    }
    
    asio::io_context &io_context_;
    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<RpcDeviceChannel>> deviceChannels_;
};



#endif /* RpcManager_h */
