//
//  main.cpp
//  asioTest
//
//  Created by apple on 2019/12/3.
//  Copyright © 2019 apple. All rights reserved.
//

#include <iostream>
#include <memory>
#include <vector>
#include "rpccall.pb.h"
#include "RPCHandler.hpp"
#include "RpcManager.hpp"

using namespace boost;
using boost::asio::ip::tcp;

int main(int argc, const char * argv[]) {

    asio::io_context io_context;

    tcp::endpoint endpoint(tcp::v4(), 9098);
    
    RpcManager manager(io_context, endpoint);

    io_context.run();

    return 0;
}
