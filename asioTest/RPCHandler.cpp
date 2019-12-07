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


void RPCHandler::rqt(boost::function<void(RPCCall*)> onRqtBack)
{
    RPCCall rpc;
    rpc.set_name("rqt");
    rpc.set_str("0248b8e92765f4ca24246014a1cb3087");
    
    deviceChannel_->sendRPC(rpc, [onRqtBack](RPCCall* rpcback){
        onRqtBack(rpcback);
    });
    
}
