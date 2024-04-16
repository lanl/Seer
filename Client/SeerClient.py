import json
import ctypes
import struct
import blosc2
import numpy as np

from pymargo.core import Engine
import pyyokan_common as yokan
from pyyokan_client import Client
from pyyokan_server import Provider

class SeerClient:
    def __init__(self, configFile):
        file = open('data.json')
        jsonData = json.load(file)

        self.databases = []
        for item in jsonData['databases']:
            protocol = item['protocol']
            server_addr = item['address']
            provider_id = item['provider_id']
            self.databases.append( self.__init_engine(protocol, server_addr, provider_id) )

        return
    
    
    def __init_engine(self, protocol, server_addr, provider_id):
        # init engine
        engine = Engine(protocol)
        mid = engine.get_internal_mid()
        addr = engine.lookup(server_addr)
        hg_addr = addr.get_internal_hg_addr()
        provider = Provider(mid=mid, provider_id=provider_id, config='{"database":{"type":"map"}}')
        client = Client(mid=mid)
        db = client.make_database_handle(address=hg_addr, provider_id=provider_id)
        return db
    

    def list_all_keys(self, db):
        num_keys = db.count()
        
        max_length = 1024
        prefix = ''
        out_keys = []
        for i in range(0, num_keys):
            out_keys.append( bytearray(max_length+len(prefix)+1) )
        
        from_key = ''
        ksizes = db.list_keys(keys=out_keys, from_key=from_key, filter=prefix)
        
        keys = []
        for i in range(len(ksizes)):
            key_size = ksizes[i]
            
            k = out_keys[i]
            key = (k[:key_size]).decode('ascii')
            keys.append(key)
            
        return keys
    

    def split_key(self, key, pos):
        parts = key.split('/')
        name = parts[pos]
        return name


    def list_fields(self, db, timestep):
        
        all_keys = self.list_all_keys(db)
        
        x = []
        for k in all_keys:
            parts = k.split('/')
            if len(parts) == 4:
                name = parts[2]
                x.append(name)
                
        return list(set(x))

    
    def list_attributes(self, db, key):
        all_keys = self.list_all_keys(db)

        x = []
        for k in all_keys:
            parts = k.split('/')
            
            if len(parts) == 4:
                name = parts[2]
                field = parts[3]
                if name == key:
                    x.append(field)
            
        x = list(set(x))

        return x


    def get_value(self, db, key):
        ''' Get data from the server for that key '''

        # length of the value associated with the key
        l = db.length(key)

        out_val = bytearray(l)          # create buffer
        db.get(key=key, value=out_val)  # get the data
        v = out_val.decode("ascii")     # convert to ascii
        return v


    def get_data(self, db, key):
        ''' Get data from the server for that key '''

        # length of the value associated with the key
        l = db.length(key)

        out_val = bytearray(l)          # create buffer
        db.get(key=key, value=out_val)  # get the data
        return out_val


    def get_fieldValue(self, db, _name, timestep):
        all_keys = self.list_all_keys(db)

        x = []
        for k in all_keys:
            parts = k.split('/')

            if len(parts) == 4:
                name = parts[2]
                field = parts[3]
                ts = parts[0]
                _ts = '_'+str(timestep)

                if _name == name and ts == _ts:
                    if field == 'value':
                        x.append(k)

        values = []
        for k in x:
            val = self.get_data(db, k)

            num_elem_key = k.replace('value','num_elems')
            num_elems = self.get_value(db, num_elem_key)
            n_e = str(num_elems) + 'f'

            a_bytesobj2 = blosc2.decompress(val)
            x = struct.unpack(n_e, a_bytesobj2)
            values.append(x)

        return values