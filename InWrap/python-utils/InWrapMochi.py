import sys
from pymargo.core import Engine
from pysdskv.client import *
import matplotlib.pyplot as plt
import numpy as np
import random 

class InWrapMochi:

	def __init__(self, transport, address, p_id, db_n):
		engine = Engine(transport)
		server_addr = address
		provider_id = p_id
		db_name = db_n

		# init the engine
		self.client = SDSKVClient(engine)
		self.addr = self.engine.lookup(server_addr)
		provider_handle = self.client.create_provider_handle(self.addr, provider_id)
		self.db = provider_handle.open(db_name)

		# create a dictionary for values
		self.command_dic = {}
		self.used_unique_ids = []
		self.count = 0


	def add_keyVal(self, key, value):
		self.command_dic[key + "-" + str(self.count)] = value
		self.count = self.count + 1


	def del_key(self, key):
		self.db.erase(key)


	def get_val(self, key):
		return self.db.get(key)


	def exists(self, key):
		return self.db.exists(key)


	def list_keyVal(self):
		key,val = self.db.list_keyvals()
		return key,val


	def shutdown(self):
		self.client.shutdown_service(self.addr)


	def send(self):

		# Loop until you can add a new key
		while ( self.db.exists("NEW_KEY") ): 
			continue


		# Generate a random number to pad keys with
		rand_num = random.randint(99999,1000000)
		while rand_num in self.used_unique_ids:
			rand_num = random.randint(99999,1000000)

		self.used_unique_ids.append(rand_num)


		# Add keys to mochi DB
		key_list = []
		self.db.put("NEW_KEY", "0")	# adding keys
		for key,val in self.command_dic.items():
			key_padded = key + ":" + str(rand_num);	# add unique tag to each
			self.db.put(key_padded, val)			# put key in mochi db

			key_list.append(key)	# save the key for later deletion

			print("key: ", key_padded)
			print("val: ", val)

		self.db.put("NEW_KEY", str(rand_num))	# adding keys done


		# Delete keys
		for k in key_list:
			del self.command_dic[k]




# Run:
"""
import sys
sys.path.insert(1, '/home/pascal/projects/InWrap/python-utils')
import InWrapMochi
conn = InWrapMochi.InWrapMochi("na+sm","na+sm://2584/0",1,"foo")
"""