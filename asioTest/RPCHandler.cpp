//
//  CRPCDeviceChannelRider.cpp
//  libev_test
//
//  Created by apple on 2019/11/8.
//  Copyright © 2019 apple. All rights reserved.
//

#include "RPCHandler.hpp"
#include "rpccall.pb.h"
#include <functional>
#include "RpcDeviceChannel.hpp"

void RPCHandler::rqt2(boost::function<void(RPCCall*)> OnRqtBack)
{
    RPCCall rpc;
    rpc.set_name("rqt");
    rpc.set_str("md5");
 
    
    deviceChannel_->sendRPC(rpc, [](RPCCall* rpcback){
        
    });
    
}
