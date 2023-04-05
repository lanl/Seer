#include <iostream>
#include <memory>
#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>

namespace tl = thallium;
std::unique_ptr<tl::engine> myEngine;


void fn(const tl::request& req, tl::bulk& b)
{
    tl::endpoint ep = req.get_endpoint();

    // std::vector<char> v(6);
    // std::vector<std::pair<void*,std::size_t>> segments(1);
    // segments[0].first  = (void*)(&v[0]);
    // segments[0].second = v.size();

    std::vector<char> v(8);
    //std::vector<char> data(5);

    std::vector<std::pair<void*,std::size_t>> segments(1);
    segments[0].first  = (void*)(&v[0]);
    segments[0].second = v.size();

    // segments[1].first  = (void*)(&data[0]);
    // segments[1].second = data.size();



    // std::vector<std::pair<void*,std::size_t>> segments(2);
    // segments[0].first  = (void*)(&v[0]);
    // segments[0].second = v.size();

    // segments[1].first  = (void*)(&data[0]);
    // segments[1].second = data.size();

    
    
    tl::bulk local = myEngine->expose(segments, tl::bulk_mode::write_only);
    b.on(ep) >> local;



    std::cout << "Server received bulk: ";
    for(auto c : v) 
        std::cout << c;
    // for (auto i : data)
    //     std::cout << i;
    std::cout << std::endl;
    
        
    req.respond();
    
}


int main(int argc, char** argv) 
{
    // std::string ipaddress = "192.168.81.45";
    // std::string port = "1234";

    // DataServer rpc(ipaddress, port);
    // std::cout << "Server running at: tcp://" << ipaddress << ":" << port << std::endl;

    //tl::engine myEngine("tcp://127.0.0.1:1234", THALLIUM_SERVER_MODE);
    myEngine = std::make_unique<tl::engine>("tcp://127.0.0.1:1234", THALLIUM_SERVER_MODE);

    // std::function<void(const tl::request&, tl::bulk&)> f = [&myEngine](const tl::request& req, tl::bulk& b) 
    // {
    //     tl::endpoint ep = req.get_endpoint();
    //     std::vector<char> v(6);
    //     std::vector<std::pair<void*,std::size_t>> segments(1);
    //     segments[0].first  = (void*)(&v[0]);
    //     segments[0].second = v.size();
    //     tl::bulk local = myEngine.expose(segments, tl::bulk_mode::write_only);
    //     b.on(ep) >> local;
    //     std::cout << "Server received bulk: ";
    //     for(auto c : v) std::cout << c;
    //     std::cout << std::endl;
    //     req.respond();
    // };

    myEngine->define("do_rdma",fn);
}

// ip is infiniband address
