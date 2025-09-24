#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <random>

#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

#include <mpi.h>

#include <yokan/client.h>
#include <yokan/database.h>
#include <yokan/cxx/exception.hpp>
#include <yokan/cxx/database.hpp>
#include <yokan/collection.h>

#include <bits/stdc++.h>

const char* yk_error_to_string(yk_return_t ret) {
    switch(ret) {
        case YOKAN_SUCCESS:               return "OK";
        case YOKAN_ERR_INVALID_BACKEND:   return "InvalidType";
        case YOKAN_ERR_INVALID_CONFIG:    return "InvalidConf";
        case YOKAN_ERR_INVALID_ARGS:      return "InvalidArg";
        case YOKAN_ERR_INVALID_ID:        return "InvalidID";
        case YOKAN_ERR_KEY_NOT_FOUND:     return "NotFound";
        case YOKAN_ERR_BUFFER_SIZE:       return "SizeError";
        case YOKAN_ERR_KEY_EXISTS:        return "KeyExists";
        case YOKAN_ERR_OP_UNSUPPORTED:    return "NotSupported";
        case YOKAN_ERR_CORRUPTION:        return "Corruption";
        case YOKAN_ERR_IO:                return "IOError";
        case YOKAN_ERR_INCOMPLETE:        return "Incomplete";
        case YOKAN_ERR_TIMEOUT:           return "TimedOut";
        case YOKAN_ERR_ABORTED:           return "Aborted";
        case YOKAN_ERR_BUSY:              return "Busy";
        case YOKAN_ERR_EXPIRED:           return "Expired";
        case YOKAN_ERR_TRY_AGAIN:         return "TryAgain";
        case YOKAN_ERR_SYSTEM:            return "System";
        case YOKAN_ERR_CANCELED:          return "Canceled";
        case YOKAN_ERR_PERMISSION:        return "Permission";
        case YOKAN_ERR_MODE:              return "InvalidMode";
        case YOKAN_ERR_MIGRATED:          return "Migrated";
        case YOKAN_STOP_ITERATION:        return "StopIteration";
        case YOKAN_ERR_OTHER:             return "Other";
        default:                          return "UNKNOWN";
    }
}



int main(int argc, char *argv[])
{

    MPI_Init(NULL, NULL);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return 1;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        // Check for IPv4 addresses
        if (ifa->ifa_addr->sa_family == AF_INET) {
            void *addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addr_buffer[INET_ADDRSTRLEN];

            inet_ntop(AF_INET, addr_ptr, addr_buffer, INET_ADDRSTRLEN);

            // Skip loopback interfaces (like 127.0.0.1)
            if (!(ifa->ifa_flags & IFF_LOOPBACK)) {
                std::cout << "world size: " << world_size << ", rank: " << world_rank << " / Interface: " << ifa->ifa_name
                          << ", IPv4 Address: " << addr_buffer << std::endl;
            }
        }
    }



    std::cout << "world size: " << world_size << ", rank: " << world_rank << std::endl;

    std::string protocol = "ofi+tcp";
    int providerId = 124;
    std::string serverAddr = argv[1];

    margo_instance_id mid = margo_init(protocol.c_str(), MARGO_CLIENT_MODE, 0, 0);
    assert(mid);
    std::cout << world_rank << " ~ margo_init good" << std::endl;

    hg_addr_t server_addr = HG_ADDR_NULL;
    std::string svr_addr_str = protocol + "://" + serverAddr;
    std::cout << world_rank << " ~ svr_addr_str: " << svr_addr_str << ", provider id:" <<  providerId << std::endl;

    hg_return_t hret = margo_addr_lookup(mid, svr_addr_str.c_str(), &server_addr);
    assert(hret == HG_SUCCESS);
    std::cout << world_rank << " ~ margo_addr_lookup good" << std::endl;

    yk_return_t ret;
    yk_client_t client = YOKAN_CLIENT_NULL;
    ret = yk_client_init(mid, &client);
    assert(ret == YOKAN_SUCCESS);
    std::cout << world_rank << " ~ yk_client_init good" << std::endl;

    yk_database_handle_t db_handle = YOKAN_DATABASE_HANDLE_NULL;
    ret = yk_database_handle_create(client, server_addr, providerId, true, &db_handle);

    if (ret != YOKAN_SUCCESS) {
    std::cerr << "Rank " << world_rank << ": yk_database_handle_create failed with error " 
              << ret << " (" << yk_error_to_string(ret) << ")" << std::endl;
    
    }

    assert(ret == YOKAN_SUCCESS);
    std::cout << world_rank << " - All correct in initDB" << std::endl;

    margo_finalize(mid);
    MPI_Finalize();

    freeifaddrs(ifaddr);

    return 0;
}


// -Ipusmochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/include -I/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/include
// -L/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/lib  
// -lpyokan-client -lmargo
