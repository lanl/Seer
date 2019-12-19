import sys
import json
import os
from mpi4py import MPI

ip = sys.argv[1]
json_input_file = sys.argv[2]

# Check if the input file exists
if os.path.exists(json_input_file) == False:
	print("Json file " + json_input_file + " does not exist!")
	exit()


# Lauch sever from node 0
comm = MPI.COMM_WORLD
rank = comm.Get_rank()

#print("rank: ", rank)
#print("ip: ", ip)
#print("json_input_file: ", json_input_file)


if rank == 0:
	# Load JSON data
	with open(json_input_file) as json_file:
		json_data = json.load(json_file)

	# Write the data
	json_data["mochi-database"]["address"] = json_data["mochi-database"]["conn-type"] + "://" + ip

	# write the file out
	with open(json_input_file, 'w') as outfile:
		json.dump(json_data, outfile, indent=4)

	print(json_input_file + " created!")

comm.Barrier()