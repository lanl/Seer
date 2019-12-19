# (C) 2018 The University of Chicago
# See COPYRIGHT in top-level directory.
import sys
from pymargo.core import Engine
from pysdskv.client import *

engine = Engine('ofi+tcp')
#engine = Engine('na+sm')

server_addr = sys.argv[1]
provider_id = int(sys.argv[2])
db_name = sys.argv[3]

client = SDSKVClient(engine)
addr = engine.lookup(server_addr)
provider_handle = client.create_provider_handle(addr, provider_id)
db = provider_handle.open(db_name)

dbs = provider_handle.databases
print ("(len(dbs): ", (len(dbs)))



val = db.get("xxx_0")
print("db.get('xxx_0') returned " + str(val))

print ("db.exists(xxx_0)",db.exists("xxx_0") )

val2 = db.get( "xxx_1" )
print("db.get('xxx_1') returned " + str(val2))

# Run:
# python ../client.py ofi+tcp://192.168.101.186:1234 1 foo2