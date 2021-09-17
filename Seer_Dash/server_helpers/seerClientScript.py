import sys
from pymargo.core import Engine
from pysdskv.client import *

engine = Engine('ofi+tcp')
server_addr = 'ofi+tcp://' + sys.argv[1]
provider_id = 1
db_name = sys.argv[2]
#key = sys.argv[3]
keys = sys.argv[3]
print(keys)


client = SDSKVClient(engine)
addr = engine.lookup(server_addr)
provider_handle = client.create_provider_handle(addr, provider_id)
db = provider_handle.open(db_name)

l = len(keys)
keyList = keys[1:l-1].split(',')

for key in keyList:
    print("*_*_*")
    vars = db.get(key)
    print(vars)
    print("*_*_*")

# python seerClientScript.py <infiniband address:port> <mochi-db-name> <["variable1-name-used-as-key", "variable2-name-used-as-key",...]>
# e.g
#    python seerClientScript.py 192.168.101.180:1234 foo_test0 ["numRanks"]
