#include "seer_o.h"


 InSitu::InSitu(int rank, int worldSize, std::string inputJsonFile)
{ 
    myRank = rank; 
    numRanks = worldSize; 
    numThreads = 2;
    numDatabases = 0;
    jsonFilename = inputJsonFile;

    srand(time(0));
    simID = rand();

    std::ifstream jsonFile(inputJsonFile);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;
    simID = jsonInput["sim-id"];
    
    loadDatabases();

    if (numDatabases == 0)
        std::cout << "Error; in situ is not going to work!!!" << std::endl;
}


int InSitu::loadDatabases()
{
    if (jsonFilename != "")
    {
        std::ifstream jsonFile(jsonFilename);
        nlohmann::json jsonInput;
        jsonFile >> jsonInput;

        //numDatabases = jsonInput["databases"].size();
        for (int i=0; i<jsonInput["databases"].size(); i++)
        {
            std::string serverAddr = jsonInput["databases"][i]["address"];

            if ( std::find(db_addresses.begin(), db_addresses.end(), serverAddr) == db_addresses.end() ) // not found
            {
                std::string protocol = jsonInput["databases"][i]["protocol"];
                int providerId = jsonInput["databases"][i]["provider_id"];

                db_handles.push_back( initDB(protocol, serverAddr, providerId) );
                db_addresses.push_back( serverAddr );

                numDatabases++;
                std::cout << "database found at: " << serverAddr << std::endl;
            }
        }
        std::cout << "num Databases: " << numDatabases << std::endl;
    }

    return 1;
}


yk_database_handle_t InSitu::initDB(std::string protocol, std::string serverAddr, int providerId)
{
    margo_instance_id mid = margo_init(protocol.c_str(), MARGO_CLIENT_MODE, 0, 0);
    assert(mid);

    hg_addr_t server_addr = HG_ADDR_NULL;
    std::string svr_addr_str = protocol + "://" + serverAddr;
    hg_return_t hret = margo_addr_lookup(mid, svr_addr_str.c_str(), &server_addr);
    assert(hret == HG_SUCCESS);

    yk_return_t ret;
    yk_client_t client = YOKAN_CLIENT_NULL;

    ret = yk_client_init(mid, &client);
    assert(ret == YOKAN_SUCCESS);

    yk_database_handle_t db_handle = YOKAN_DATABASE_HANDLE_NULL;
    ret = yk_database_handle_create(client, server_addr, providerId, true, &db_handle);
    assert(ret == YOKAN_SUCCESS);

    return db_handle;
}



int InSitu::tsDone(int ts)
{
    int dbIndex = ts%numDatabases;

    std::cout << "tsDone, dbIndex: " << dbIndex << std::endl;
    if (tsNum.find(ts) == tsNum.end()) 
        tsNum[ts] = 1;
    else
        tsNum[ts] = tsNum[ts]+1;


    std::string key = "_" + std::to_string(ts) + "/status";
    if (tsNum[ts] == numRanks)
    {
        putValue(dbIndex, key, "ready");
        loadDatabases();    // periodically check for new databases
    }
    else
        putValue(dbIndex, key, std::to_string(tsNum[ts]));

    return 1;
}


char * InSitu::compress(float * data, size_t numElements, size_t & csize)
{
    blosc2_init();

    size_t dataSize = sizeof(data[0]) * numElements;
    //std::cout << "dataSize: " << dataSize << std::endl;
    size_t osize = dataSize + BLOSC2_MAX_OVERHEAD;

    char * output = (char *) malloc(osize);
	csize = blosc1_compress(9, BLOSC_BITSHUFFLE, sizeof(data[0]), dataSize, &data[0], output, osize);
    
    std::cout << "Data size: " << dataSize << ", compressed size: " << csize << ", osize: " << osize << std::endl;

    if (csize < 0)
		throw std::runtime_error("Compression error. Error code: " + std::to_string(osize));

	if (csize > 0)
        output = (char *) realloc(output, csize);

    blosc2_destroy();
    return output;
}


int InSitu::putValue(int dbIndex, std::string key, std::string value)
{
    // Storing metadata
    yk_return_t ret = yk_put(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.c_str(), key.length(), value.c_str(), value.length());

    if (ret == YOKAN_SUCCESS)
        return 1;
    
    return 0;
}


int InSitu::putData(int dbIndex, std::string key, char * value, size_t val_len)
{
    // Specific to data that we will store compressed in the database
    yk_return_t ret = yk_put(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.c_str(), key.length(), value, val_len);

    if (ret == YOKAN_SUCCESS)
        return 1;
    
    return 0;
}



void InSitu::sendInfo(int myRank, int ts, std::string key, std::string value)
{
    std::string key_prefix = "_" +  std::to_string(ts) + "/" + std::to_string(myRank) + "/" ;
    std::string k = key_prefix + key;

    int dbIndex = ts%numDatabases;

    putValue(dbIndex, k, value);
}



void InSitu::sendData(int myRank, int ts, std::string name, std::string type, std::string dataType, size_t numElements, float * data)
{
    std::string key_prefix = "_" +  std::to_string(ts) + "/" + std::to_string(myRank) + "/" +  name;

    std::string key, value;
    yk_return_t ret;

    int dbIndex = ts%numDatabases;

    key = key_prefix + "/num_elems";
    value = std::to_string(numElements);
    putValue(dbIndex, key, value);

    key = key_prefix + "/type";
    putValue(dbIndex, key, dataType);

    size_t compressedSize;
    char* compressedData = compress(data, numElements, compressedSize);

    key = key_prefix + "/value";
    std::cout << "compressed string size: " << compressedSize << std::endl;
    putData(dbIndex, key, compressedData, compressedSize);

    key = key_prefix + "/compressed_size";
    value = std::to_string(compressedSize);
    putValue(dbIndex, key, value);
}


// Data:

// _ts/rank/name/num_elems: <value>
// _ts/rank/name/type: <value>
// _ts/rank/name/value: <compressed_data>
// _ts/rank/name/compressed_size: <value>

