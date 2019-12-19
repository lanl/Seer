import sys
import json
import os
from pymargo.core import Engine
from pysdskv.client import *
from mpi4py import MPI

json_input_file = sys.argv[1]


# Terminate sever from node 0
comm = MPI.COMM_WORLD
rank = comm.Get_rank()


if rank == 0:
	# Check if the input file exists
	if os.path.exists(json_input_file) == False:
		print("Json file " + json_input_file + " does not exist!")
		exit()

	# Load JSON data
	with open(json_input_file) as json_file:
		json_data = json.load(json_file)

	# Initialize engine and address
	engine_type = json_data["mochi-database"]["conn-type"] 
	server_addr = json_data["mochi-database"]["address"] + json_data["mochi-database"]["port"]
	provider_id = json_data["mochi-database"]["multiplex"]

	print("Engine ", engine_type)
	print("server_addr ", server_addr)
	print("provider_id ", provider_id)

	# Connect
	engine = Engine(engine_type)
	client = SDSKVClient(engine)
	addr = engine.lookup(server_addr)
	provider_handle = client.create_provider_handle(addr, provider_id)

	# Shutdown
	del provider_handle
	client.shutdown_service(addr)
	del addr
	del client
	engine.finalize()

	print("Server terminated!")

comm.Barrier()
