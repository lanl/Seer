from mpi4py import MPI
from pymargo.core import Engine
import pyyokan_common as yokan
from pyyokan_client import Client
from pyyokan_server import Provider

import json
import ctypes
import struct
import blosc2
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()


def get_json_data(config_file):
    f = open(config_file)
    json_data = json.load(f)
    return json_data


def get_databases(json_data):
    dbs = []
    
    dict = {}
    for db in json_data['databases']:
        address = db['address']
        protocol = db['protocol']
        provider_id = db['provider_id']
        server_addr = protocol + '://' + address
        
        dict = {'server_addr': server_addr}
        dict = {'provider_id': provider_id}
        dict = {'protocol': protocol}
        
        dbs.append(dict)
    
    return dbs


def connect_to_server(protocol, server_addr1, provider_id):
    engine1 = Engine(protocol)
    mid1 = engine1.get_internal_mid()
    addr1 = engine1.lookup(server_addr1)
    hg_addr1 = addr1.get_internal_hg_addr()
    provider1 = Provider(mid=mid1, provider_id=provider_id, config='{"database":{"type":"map"}}')
    client1 = Client(mid=mid1)
    db1 = client1.make_database_handle(address=hg_addr1, provider_id=provider_id)
    print("Hello from ", rank)