import sys
from pymargo.core import Engine
from pysdskv.client import *

engine = Engine('ofi+tcp')
server_addr = 'ofi+tcp://' + sys.argv[1]
provider_id = 1
db_name = sys.argv[2]
keys = sys.argv[3]

client = SDSKVClient(engine)
addr = engine.lookup(server_addr)
provider_handle = client.create_provider_handle(addr, provider_id)
db = provider_handle.open(db_name)

for key in keys:
    vars = db.get(key)
    print("*_*_*")
    print(vars)
    print("*_*_*")

#python seerClientScript.py <node-infiniband-address:port> <db_name> <variable_list>
#python tinyScript.py 192.168.101.180:1234 foo_test0 vairbales