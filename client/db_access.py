from pymargo.core import Engine
import pyyokan_common as yokan
from pyyokan_client import Client
from pyyokan_server import Provider

import numpy as np

# Data
# key:  _<ts>/<type>/.../<name>/value : <value>
#       _<ts>/<type>/.../<name>/type : <value
#       _<ts>/<type>/.../<length>/
# 

class DBAccess:
    ''' Query a database '''


    def __init__(self, provider_id, protocol, addr):
        db_config = "{'database':{'type':'map'}}"
        server_addr = protocol + "://" + addr

        engine = Engine(protocol)
        mid = engine.get_internal_mid()
        addr = engine.lookup(server_addr)
        hg_addr = addr.get_internal_hg_addr()
        provider = Provider(mid=mid, provider_id=provider_id, config=db_config)
        client = Client(mid=mid)
        self.db = client.make_database_handle(address=hg_addr, provider_id=provider_id)


    def __deserialize(self, str_data, num_elems, data_type):
        ''' Deserialize data '''
        if num_elems == 1:
            if data_type == 'float':
                return np.float32(str_data)
            elif data_type == 'double':
                return np.float64(str_data)
            elif data_type == 'int':
                return np.int(str_data)
        else:
            serialized_data = str_data[1:-1]
            orig = np.fromstring(serialized_data, dtype=data_type, sep=',')
            return orig


    def __get_data(self, key):
        ''' Get data from the server for that key '''

        # length of the value associated with the key
        l = db.length(key)

        out_val = bytearray(l)          # create buffer
        db.get(key=key, value=out_val)  # get the data
        v = out_val.decode("ascii")     # convert to ascii
        return v


    def get_all_keys(self, key_prefix=''):
        ''' Get all the keys stored in the database '''

        # some initializations
        self.num_keys = self.db.count()

        max_length = 1024
        prefix = key_prefix
        from_key = ''
        out_keys = []

        for i in range(0, num_keys):
            out_keys.append( bytearray(max_length+len(prefix)+1) )

        # Get the keys        
        ksizes = db.list_keys(keys=out_keys, from_key=from_key, filter=prefix)
        
        # Remove extra things from the keys
        keys = []
        for i in range(len(ksizes)):
            key_size = ksizes[i]
            
            k = out_keys[i]
            key = (k[:key_size]).decode('ascii')
            keys.append(key)
            
        return keys
        

    def get_field_info(self, field, ts):
        ''' Get the list of keys '''
        
        num_keys = self.db.count()
        keys = self.get_all_keys()

        # Filter by name and timestep
        result_1 = list( filter(lambda x: x.startswith( '_'+str(ts) ), keys)     )
        result_2 = list( filter(lambda x: x.count(field)             , result_1) )

        return result_2
        

    def get_value(self, field, ts):
        ''' Get data from the server '''

        # Get the list of keys
        keys = self.get_field_info(field, ts)
  
        data_type_key = ( list(filter(lambda x: x.endswith('type'),      keys)) )[0]
        num_elems_key = ( list(filter(lambda x: x.endswith('num_elems'), keys)) )[0]
        query_key     = ( list(filter(lambda x: x.endswith('value'),     keys)) )[0]
        
        data_type = __get_data(data_type_key)
        num_elems = __get_data(num_elems_key)
        str_data  = __get_data(query_key)
        
        data = deserialize(str_data, int(num_elems), data_type)
        return data



    def remove_keys(self, save_to_disk = False):
        ''' Delete the key '''
        return





