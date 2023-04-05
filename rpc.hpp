#pragma once

#include <string>
#include <map>

#include "utils.hpp"

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>

namespace tl = thallium;


struct CollectedData{
    size_t n;
    std::string float;
    std::vector<char> data;
};


class RPC
{
    std::unique_ptr<tl::engine> myEngine;

    // Server
    CollectedData recvedData;

    // Client
    tl::remote_procedure remote_do_rdma;
    tl::endpoint server_endpoint;

  public:
    RPC(std::string rpcMode, std::string ip, int port);
    ~RPC(){};

    int clientSend(std::string msg1, std::string msg2);

    CollectedData serverRecv();
    void recvData(const tl::request& req, tl::bulk& b);
};


inline RPC::RPC(std::string rpcMode, std::string ip="127.0.0.1", int port=1234)
{
    std::string address = "tcp://" + ip + ":" + std::to_string(port);

    if (rpcMode == "client")
    {
        myEngine = std::make_unique<tl::engine>("tcp", THALLIUM_CLIENT_MODE);
        remote_do_rdma = myEngine->define("do_rdma");
        server_endpoint = myEngine->lookup(address);
    }
    else // (rpcMode == "server")
    {
        myEngine = std::make_unique<tl::engine>(address, THALLIUM_SERVER_MODE);
    }   
};


inline int RPC::clientSend(std::string msg1, std::string msg2)
{
    std::vector<std::pair<void*,std::size_t>> segments(2);
    segments[0].first  = (void*)(&msg1[0]);
    segments[0].second = msg1.size()+1;

    segments[1].first  = (void*)(&msg2[0]);
    segments[1].second = msg2.size()+1;


    tl::bulk myBulk = myEngine.expose(segments, tl::bulk_mode::read_only);
    remote_do_rdma.on(server_endpoint)(myBulk);

    return 1;
}


inline CollectedData RPC::serverRecv()
{
    myEngine->define("do_rdma", recvData);
    return CollectedData;
}


inline void RPC::recvData(const tl::request& req, tl::bulk& b)
{
    tl::endpoint ep = req.get_endpoint();
    std::vector<std::pair<void*,std::size_t>> segments(1);
    
    std::vector<char> buffer(info);
    segments[0].first  = (void*)(&info[0]);
    segments[0].second = info.size()+1;

    tl::bulk bulk_a = myEngine->expose(segments, tl::bulk_mode::write_only);
    b.on(ep) >> bulk_a;

    // Data format is:
    // - number of elements
    // - data type


    recvedData.data.resize(dataSize);
    
    segments[0].first  = (void*)(&buffer[0]);
    segments[0].second = buffer.size()+1;

    tl::bulk bulk_b = myEngine.expose(segments, tl::bulk_mode::read_only);
    b.on(ep) >> bulk_b;


    req.respond();
}