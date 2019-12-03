//
//  main.cpp
//  asioTest
//
//  Created by apple on 2019/12/3.
//  Copyright © 2019 apple. All rights reserved.
//

#include <iostream>
#include <boost/asio.hpp>


using namespace boost;
using boost::asio::ip::tcp;

class RpcManager {
    
    
public:
    RpcManager(asio::io_context &io_context, const asio::ip::tcp::endpoint &endpoint): acceptor_(io_context, endpoint)
    {
        do_accept();
    }
private:
    void do_accept()
    {
        
        
        
    }
    
    tcp::acceptor acceptor_;
    
};




int main(int argc, const char * argv[]) {
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}
