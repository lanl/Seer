import sys
import json
import os
from mpi4py import MPI

json_input_file = sys.argv[1]

# Launch sever from node 0
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


	# Formulate the lauch argument
	launch_string = "sdskv-server-daemon "

	if json_data["mochi-database"]["conn-type"] == "na+sm":
		launch_string = launch_string + "na+sm" + \
						json_data["mochi-database"]["name"] + ":" + json_data["mochi-database"]["db-type"] + \
						" -f address"
						
	else:
		launch_string = launch_string + json_data["mochi-database"]["address"] + json_data["mochi-database"]["port"]+ " " + \
						json_data["mochi-database"]["name"] + ":" + json_data["mochi-database"]["db-type"]


	print("Launch string: " + launch_string)
	os.system(launch_string)

	print("Server launched!")

	

comm.Barrier()


# Run:
# python InWrap/python-utisl/runServer.py InWrap/inputs/input-test-structured.json