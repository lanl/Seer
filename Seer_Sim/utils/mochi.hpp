#pragma once

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>

#include "timer.hpp"

namespace Seer  
{ 

#include <margo.h>
#include "sdskv-client.h"


class MochiInterface
{
	uint8_t mplex_id;
	char *sdskv_svr_addr_str;
	char *db_name;

	margo_instance_id mid;
	hg_addr_t svr_addr;
	sdskv_database_id_t db_id;
	sdskv_client_t kvcl;
	sdskv_provider_handle_t kvph;

	std::stringstream log;

  public:
	MochiInterface(){};
	MochiInterface(std::string serverAddress, uint8_t mplexID, std::string dbName);
	~MochiInterface(){ cleanup(); }
	
	int cleanup();
	int closeServer();

	int init(std::string serverAddress, uint8_t mplexID, std::string dbName);


	int getValue(std::string key, std::string &value);
	std::string getValue(std::string key);
	int putKeyValue(std::string key, std::string value);
	int existsKey(std::string key);
	int eraseKey(std::string key);

	std::vector<std::string> listKeys(
			const std::string& start_key,
			const std::string& prefix,
			hg_size_t max_keys=2048,
			hg_size_t key_size=0);

	std::vector<std::string> listKeysWithPrefix(std::string prefix);
};



inline MochiInterface::MochiInterface(std::string serverAddress, uint8_t mplexID, std::string dbName)
{
	Timer clock;
	clock.start("constructor");

	init(serverAddress, mplexID, dbName);

	clock.stop("constructor");
	log << "MochiInterface constructor took: " << clock.getDuration("constructor") << " s" << std::endl;
}


inline int MochiInterface::init(std::string serverAddress, uint8_t mplexID, std::string dbName)
{
	Timer clock;
	clock.start("init");

	sdskv_svr_addr_str = const_cast<char*>(serverAddress.c_str());
	mplex_id = mplexID;
	db_name = const_cast<char*>(dbName.c_str());


	// initialize Margo using the transport portion of the server
	// address (i.e., the part before the first : character if present)
	char cli_addr_prefix[64] = {0};
	for(unsigned i=0; (i<63 && sdskv_svr_addr_str[i] != '\0' && sdskv_svr_addr_str[i] != ':'); i++)
		cli_addr_prefix[i] = sdskv_svr_addr_str[i];


	// start margo
	mid = margo_init(cli_addr_prefix, MARGO_SERVER_MODE, 0, 0);
	if (mid == MARGO_INSTANCE_NULL)
	{
		std::cerr << "Error: margo_init()" << std::endl;
		return (-1);
	}

	int ret = sdskv_client_init(mid, &kvcl);
	if (ret != 0)
	{
		std::cerr << "Error: sdskv_client_init()" << std::endl;
		margo_finalize(mid);
		return -1;
	}


	// look up the SDSKV server address
	hg_return_t hret = margo_addr_lookup(mid, sdskv_svr_addr_str, &svr_addr);
	if (hret != HG_SUCCESS)
	{
		std::cerr << "Error: margo_addr_lookup()" << std::endl;
		sdskv_client_finalize(kvcl);
		margo_finalize(mid);
		return -1;
	}

	// create a SDSKV provider handle
	ret = sdskv_provider_handle_create(kvcl, svr_addr, mplex_id, &kvph);
	if (ret != 0)
	{
		std::cerr << "Error: sdskv_provider_handle_create()" << std::endl;
		margo_addr_free(mid, svr_addr);
		sdskv_client_finalize(kvcl);
		margo_finalize(mid);
		return (-1);
	}


	ret = sdskv_open(kvph, db_name, &db_id);
	if (ret != 0) 
	{
		std::cerr << "Error: could not open database " <<  db_name << std::endl;

		sdskv_provider_handle_release(kvph);
		margo_addr_free(mid, svr_addr);
		sdskv_client_finalize(kvcl);
		margo_finalize(mid);
		return(-1);
	}

	clock.stop("init");
	log << "MochiInterface init took: " << clock.getDuration("init") << " s" << std::endl;

	return 1; // All cool!
}



inline int MochiInterface::cleanup()
{
	sdskv_provider_handle_release(kvph);
	margo_addr_free(mid, svr_addr);
	sdskv_client_finalize(kvcl);
	margo_finalize(mid);

	return 1;
}


inline int MochiInterface::closeServer()
{
	return sdskv_shutdown_service(kvcl, svr_addr);
}



inline int MochiInterface::putKeyValue(std::string key, std::string value)
{
	Timer clock;
	clock.start("putkey");

   int ret = sdskv_put(kvph, db_id,
			(const void *)key.data(),   key.size(),
			(const void *)value.data(), value.size());

	if (ret != 0) 
	{
		std::cerr << "Error: sdskv_put(" << key << ") failed" << std::endl;
		return -1;
	}

	clock.stop("putkey");
	log << "MochiInterface putKeyValue took: " << clock.getDuration("putkey") << " s" << std::endl;

	return 1;
}


inline std::vector<std::string> MochiInterface::listKeysWithPrefix(std::string prefix)
{
	return listKeys("", prefix);
}


inline std::vector<std::string> MochiInterface::listKeys(
			const std::string& start_key,
			const std::string& prefix,
			hg_size_t max_keys,
			hg_size_t key_size)
{
	Timer clock;
	clock.start("listKeys");

	int ret;
	if(max_keys == 0)
		return std::vector<std::string>();

	std::vector<hg_size_t> key_sizes(max_keys, key_size);
	std::vector<std::string> keys(max_keys);
	std::vector<void*> keys_addr(max_keys, nullptr);

	if(key_size == 0) {
		ret = sdskv_list_keys_with_prefix(kvph, db_id,
				start_key.data(), start_key.size(),
				prefix.data(), prefix.size(),
				nullptr,
				key_sizes.data(),
				&max_keys);
		if(ret != SDSKV_SUCCESS && ret != SDSKV_ERR_SIZE) {
			throw std::runtime_error(std::string("sdskv_list_keys_with_prefix returned ")+std::to_string(ret));
		}
	}

	for(unsigned i = 0; i < max_keys; i++) {
		keys[i].resize(key_sizes[i]);
		keys_addr[i] = const_cast<char*>(keys[i].data());
	}

	ret = sdskv_list_keys_with_prefix(kvph, db_id,
			start_key.data(), start_key.size(),
			prefix.data(), prefix.size(),
			keys_addr.data(),
			key_sizes.data(),
			&max_keys);
	keys.resize(max_keys);
	for(unsigned i = 0; i < max_keys; i++) {
		keys[i].resize(key_sizes[i]);
	}


	if(ret != SDSKV_SUCCESS)
		throw std::runtime_error(std::string("sdskv_list_keys_with_prefix returned ")+std::to_string(ret));

	clock.stop("listKeys");
	log << "MochiInterface listKeys took: " << clock.getDuration("listKeys") << " s" << std::endl;

	return keys;
}



inline int MochiInterface::eraseKey(std::string key)
{
	Timer clock;
	clock.start("eraseKey");

	int ret = sdskv_erase(kvph, db_id,
			(const void *)key.data(),   key.size());

	if (ret != 0) 
	{
		std::cerr << "Error: sdskv_erase(" << key << ") failed" << std::endl;
		return -1;
	}

	clock.stop("eraseKey");
	log << "MochiInterface eraseKey took: " << clock.getDuration("eraseKey") << " s" << std::endl;

	return 1;
}


inline int MochiInterface::existsKey(std::string key)
{
	Timer clock;
	clock.start("existsKey");

	int exists = 0;
	int ret = sdskv_exists(kvph, db_id,
			(const void *)key.data(), key.size(), &exists);

	if (ret != 0) 
	{
		std::cerr << "Error: sdskv_erase(" << key << ") failed" << std::endl;
		return -1;
	}

	clock.stop("existsKey");
	log << "MochiInterface existsKey took: " << clock.getDuration("existsKey") << " s" << std::endl;

	return exists;
}

inline std::string MochiInterface::getValue(std::string key)
{
	std::string val;
	getValue(key, val);
	return val;
}


inline int MochiInterface::getValue(std::string key, std::string &value)
{
	Timer clock;
	clock.start("getValue");

	size_t max_value_size = 2048;
	size_t value_size = max_value_size;
	std::vector<char> v(max_value_size);


	int ret = sdskv_get(kvph, db_id,
				(const void *)key.data(), key.size(),
				(void *)v.data(), &value_size);

	if (ret != 0) 
	{
		std::cerr << "Error: sdskv_get() failed" << std::endl;
		return -1;
	}

	std::string s(v.begin(), v.begin()+value_size);
	value = s;

	log << "MochiInterface getValue took: " << clock.getDuration("getValue") << " s" << std::endl;

	return 1;
}


} // Namespace Seer