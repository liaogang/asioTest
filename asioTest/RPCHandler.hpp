//
//  CRPCDeviceChannelRider.hpp
//  libev_test
//
//  Created by apple on 2019/11/8.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef CRPCDeviceChannelRider_hpp
#define CRPCDeviceChannelRider_hpp

#include <stdio.h>
#include <string>
#include <boost/function.hpp>
#include "rpccall.pb.h"


class RpcDeviceChannel;

class RPCHandler
{
public:

    RPCHandler(RpcDeviceChannel* deviceChannel):deviceChannel_(deviceChannel)
    {
        
    }

    void rqt(boost::function<void(RPCCall*)> onRqtBack);
    

    
private:
    RpcDeviceChannel* deviceChannel_;
    boost::function<void(RPCCall*)> OnRpcBack;
};

#endif /* CRPCDeviceChannelRider_hpp */
