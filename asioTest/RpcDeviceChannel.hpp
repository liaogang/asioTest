//
//  RpcDeviceChannel.hpp
//  asioTest
//
//  Created by apple on 2019/12/6.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef RpcDeviceChannel_h
#define RpcDeviceChannel_h

#include <boost/asio.hpp>
#include <map>
#include <deque>
#include <boost/bind.hpp>

typedef void (*RPCCompleteHandler)(RPCCall*);

using namespace boost;
using boost::asio::ip::tcp;

class RpcDeviceChannel : public std::enable_shared_from_this<RpcDeviceChannel>
{
public:
    RpcDeviceChannel(asio::io_context &io_context, tcp::socket socket):io_context_(io_context),socket_(std::move(socket))
    {
        rpcHandler_ = new RPCHandler(this);
    }
 
    RPCHandler* getRPCHandler()
    {
        return rpcHandler_;
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
                    printf("read head ok: %lu\n", length);

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
        
        if (body_length == 0) {
            //this is beartbeat
            do_read_header();
        }else{
            
            auto self(shared_from_this());
            
            body_buf_.resize(body_length);
            
            asio::async_read(socket_, asio::buffer(body_buf_), [this, self](boost::system::error_code ec, std::size_t length) {
                
                if (ec) {
                }else{
                    
                    if (length == this->body_buf_.length()) {
                        printf("read body ok: %lu\n", length);
                        
                        this->did_receive_rpc( this->body_buf_ );
                        this->body_buf_.clear();
                        
                    }else{
                        printf("read body failed.\n");
                    }
                    
                    
                    do_read_header();
                }
                
            });
        }
        
    }
    
    void did_receive_rpc(std::string& data)
    {
        if (deviceName_.empty()) {
            
            DeviceInfo deviceInfo;
            deviceInfo.ParseFromString(data);

            printf("接收到鉴定: %s \n",  deviceInfo.DebugString().c_str());
            
            deviceName_ = deviceInfo.devicename();
            
            
            //just test
            this->getRPCHandler()->rqt2( [](RPCCall *rpc){
                printf("wow! \n");
            });
            
        }
        else{
            RPCCall rpccall;
            rpccall.ParseFromString(data);
            
            printf("接收到服务器消息: %s \n",  rpccall.DebugString().c_str() );
            
            uint32_t sequence = rpccall.sequence();
            
            if (uncompleteHandler_.count(sequence) > 0) {
                
                auto callback = uncompleteHandler_[sequence];
                
                callback(&rpccall);

            }else{
                printf("unhandlered server rpc back. \n");
            }
            
            
            
        }
        
    }

    uint32_t genSequence(void)
    {
        sequence_++;
        return sequence_;
    }
    
    void sendRPC(RPCCall &rpc, boost::function<void(RPCCall*)> OnRpcBack)
    {
        uint32_t sequence = genSequence();
        rpc.set_sequence(sequence);
        
//        auto b = boost::bind<void(RPCCall*)>(<#F f#>, <#A1 a1#>)
        
        uncompleteHandler_[sequence] = OnRpcBack;

        unsigned int body_length = (unsigned int)rpc.ByteSizeLong();

        uint32_t head;
        head = htonl(body_length);
        
        std::string data((char*)&head, 4);
        data.append(rpc.SerializeAsString());
        
        write_data(data);
    }
    
    
    void write_data(std::string &data)
    {
        boost::asio::post(io_context_,
                          [this, data]()
                          {
            bool write_in_progress = !write_deque_.empty();
            write_deque_.push_back(data);
            if (!write_in_progress)
            {
                do_write();
            }
        });
    }
    
    void do_write()
    {
        asio::async_write(socket_,
                          asio::buffer(write_deque_.front().data(),
                                       write_deque_.front().length()),
                          [this](std::error_code ec, std::size_t /*length*/)
                          {
            if (!ec)
            {
                write_deque_.pop_front();
                if (!write_deque_.empty())
                {
                    do_write();
                }
            }
            else
            {
                socket_.close();
            }
        });
    }
    
private:
    asio::io_context &io_context_;
    tcp::socket socket_;
    std::string deviceName_;
    
    uint32_t header_buf_;
    std::string body_buf_;
    
    uint32_t sequence_;
    
    RPCHandler* rpcHandler_;
    
    std::map<uint32_t, std::function<void(RPCCall*)>> uncompleteHandler_;

    std::deque<std::string> write_deque_;
};


#endif /* RpcDeviceChannel_h */
