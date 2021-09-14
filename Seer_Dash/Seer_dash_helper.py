from re import M, template
from threading import Event
import pandas as pd
import re
import time
import paramiko
import itertools

from paramiko import SSHClient, AutoAddPolicy
from scp import SCPClient, SCPException

class Seer_Dash_Helper:
	def __init__(self):
		pass

		

	def connect(self, loginNode, mochiNodeAddr, username, dbName):
		self.loginNodeAddr = loginNode
		self.mochiNodeAddr = mochiNodeAddr
		self.username = username
		self._dbName = dbName

		print("self.loginNodeAddr", self.loginNodeAddr)
		print("self.mochiNodeAddr", self.mochiNodeAddr)
		print("self.username", self.username)
		print("self._dbName", self._dbName)

		tic = time.perf_counter()

		# Do ssh jump host (ssh -J) to go to compute node directly
		self.vm = SSHClient()
		self.vm.set_missing_host_key_policy(AutoAddPolicy())


		self.vm.connect(self.loginNodeAddr, username=self.username)
		vmtransport = self.vm.get_transport()


		dest_addr = (self.mochiNodeAddr, 22)
		local_addr = (self.loginNodeAddr, 22)

		vmchannel = vmtransport.open_channel("direct-tcpip", dest_addr, local_addr)


		self.jhost = SSHClient()
		self.jhost.set_missing_host_key_policy(AutoAddPolicy())


		self.jhost.connect(self.mochiNodeAddr, username=self.username, sock=vmchannel)

		# Attempt to maintian connection open
		self.channel = self.jhost.invoke_shell()
		self.stdin = self.channel.makefile('wb')
		self.stdout = self.channel.makefile('r')

		toc = time.perf_counter()
		elapsed_time = toc - tic
		print("connecting established, took ", elapsed_time, " seconds")

		# For copying files
		self.scp = SCPClient(self.vm.get_transport())

	def create_session(self):
		return self.ssh_client.invoke_shell()

   
	def gatherInitialData(self):
		# Get the number of timesteps

		# Get the scalars

		pass

	@staticmethod
	def _print_exec_out(cmd, out_buf, err_buf, exit_status):
		print('command executed: {}'.format(cmd))
		print('STDOUT:')
		for line in out_buf:
			print(line, end="")
		print('end of STDOUT')
		print('STDERR:')
		for line in err_buf:
			print(line, end="")
		print('end of STDERR')
		print('finished with exit status: {}'.format(exit_status))
		print('------------------------------------')
		pass
	
	def execute(self, cmd):
		#print(cmd)

		# cmd = cmd.strip('\n')
		# print("111")
		# self.stdin.write(cmd + '\n')
		# print("222")
		# shin = self.stdin
		# print("333")
		# self.stdin.flush()
		# print("444")
		# print(self.stdout.read())
		# print("555")

		# """

		# :param cmd: the command to be executed on the remote computer
		# :examples:  execute('ls')
		# 			execute('finger')
		# 			execute('cd folder_name')
		# """

		tic = time.perf_counter()

		max_reconnect = 5
		reconnect = 0
		output_data = ""

		while reconnect < max_reconnect:

			tic_1 = time.perf_counter()

			cmd = cmd.strip('\n')
			self.stdin.write(cmd + '\n')
			finish = 'end of stdOUT buffer. finished with exit status'
			echo_cmd = 'echo {} $?'.format(finish)
			self.stdin.write(echo_cmd + '\n')
			shin = self.stdin
			self.stdin.flush()

			toc_1 = time.perf_counter()


			
			tic_2 = time.perf_counter()
			
			shout = []
			sherr = []
			exit_status = 0
			for line in self.stdout:
				#print(line)
				if str(line).startswith(cmd) or str(line).startswith(echo_cmd):
					# up for now filled with shell junk from stdin
					shout = []
				elif str(line).startswith(finish):
					# our finish command ends with the exit status
					exit_status = int(str(line).rsplit(maxsplit=1)[1])
					if exit_status:
						# stderr is combined with stdout.
						# thus, swap sherr with shout in a case of failure.
						sherr = shout
						shout = []
					break
				else:
					# get rid of 'coloring and formatting' special characters
					shout.append(re.compile(r'(\x9B|\x1B\[)[0-?]*[ -/]*[@-~]').sub('', line).
								replace('\b', '').replace('\r', ''))

			toc_2 = time.perf_counter()

			tic_3 = time.perf_counter()

			# first and last lines of shout/sherr contain a prompt
			if shout and echo_cmd in shout[-1]:
				shout.pop()
			if shout and cmd in shout[0]:
				shout.pop(0)
			if sherr and echo_cmd in sherr[-1]:
				sherr.pop()
			if sherr and cmd in sherr[0]:
				sherr.pop(0)

			#output_data = []
			#print("len(shout)",len(shout))
			if len(shout) != 0:
				storeOutput = False
				for line in shout:	#output_buffer
					if storeOutput == False:
						#print("Line:", line)
						if line == "*_*_*\n":
							storeOutput = True
					else:
						#print("stored Line:", line)
						#output_data.append(line)
						output_data = line
						storeOutput = False
						break
				break
			else:
				self.connect(self.loginNodeAddr, self.mochiNodeAddr, self.username, self._dbName)
				print("Reconnecting")
				reconnect = reconnect + 1
			

			toc_3 = time.perf_counter()


			elapsed_time_2 = toc_2 - tic_2
			print("parsing 1 data  took ", elapsed_time_2, " seconds")

			elapsed_time_3 = toc_3 - tic_3
			print("parsing 2 data  took ", elapsed_time_3, " seconds")
			

			"""
			tic_2 = time.perf_counter()

			if self.stdout != None:
				print("Hi")
				storeOutput = False
				for line in self.stdout:
					print(line)
					if storeOutput == False:
						if str(line).startswith("*_*_*"):
							print("starts with")
							storeOutput = True
					else:
						output_data = str(line)
						data = self.stdout.readlines()
						print("output_data", output_data)
						self.stdout.flush()

						storeOutput = False
						break
				break
			else:
				print("Ho")
				self.connect(self.loginNodeAddr, self.mochiNodeAddr, self.username, self._dbName)
				print("Reconnecting")
				reconnect = reconnect + 1

			
			toc_2 = time.perf_counter()
			elapsed_time_2 = toc_2 - tic_2
			print("parsing 1 data  took ", elapsed_time_2, " seconds")
			"""

		tic_4 = time.perf_counter()

		#print("output_data",output_data)
		_output_list = []
		if output_data == "":
			print("No data was gathered!!!!")
		else:
			_output_list = output_data.split(',')
			_output_list[-1] =_output_list[-1].strip()

		toc_4 = time.perf_counter()
		elapsed_time_4 = toc_4 - tic_4
		print("parsing 3 data  took ", elapsed_time_4, " seconds")
		
		elapsed_time_1 = toc_1 - tic_1
		print("running command  took ", elapsed_time_1, " seconds")

		

		toc = time.perf_counter()
		elapsed_time = toc - tic
		print("Getting data  took ", elapsed_time, " seconds")
				
		return _output_list



		#self._print_exec_out(cmd=cmd, out_buf=shout, err_buf=sherr, exit_status=exit_status)
		#return shin, shout, sherr


	def flattenList(self, bigList):
		flat=[]
		for i in bigList:
			for j in i:
				flat.append(j)

		return flat

	def getSimRankData(self, var_name, ts, myRank=0):
		print("getSimData")
		print("var_name", var_name)
		print("ts", ts)
		print("myRank", myRank)
		

		_mochiNodeAddr = self.mochiNodeAddr + ":1234"

		tic_0 = time.perf_counter()

		pos_x = []
		pos_y = []
		pos_z = []
		var_data = []
		
		key = "x_ts_" + str(ts) + "_rank_" + str(myRank)
		cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
		pos_x.append( self.execute(cmd) )
		
		key = "y_ts_" + str(ts) + "_rank_" + str(myRank)
		cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
		pos_y.append( self.execute(cmd) )
		
		key = "z_ts_" + str(ts) + "_rank_" + str(myRank)
		cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
		pos_z.append( self.execute(cmd) )


		key = var_name + "_ts_" + str(ts) + "_rank_" + str(myRank)
		cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
		var_data.append( self.execute(cmd) )

		toc_0 = time.perf_counter()
		

		print("len(var_data[0])", len(var_data[0]))


		tic_1 = time.perf_counter()

		# Deserialize data
		pos_data_x = []
		pos_data_y = []
		pos_data_z = []
		variable_data = []

		if len(pos_x[0]) > 0:
			pos_x[0].pop() 

		if len(pos_y[0]) > 0:
			pos_y[0].pop()

		if len(pos_z[0]) > 0:
			pos_z[0].pop()

		if len(var_data[0]) > 0:
			var_data[0].pop()

		pos_data_x.append( pos_x[0] )
		pos_data_y.append( pos_y[0] )
		pos_data_z.append( pos_z[0] )
		variable_data.append( var_data[0] )

		toc_1 = time.perf_counter()

		#print("pos_data_x", pos_data_x)
		#print("variable_data", variable_data)

		tic_2 = time.perf_counter()


		flat_x = self.flattenList(pos_data_x)
		flat_y = self.flattenList(pos_data_y)
		flat_z = self.flattenList(pos_data_z)
		flat_var = self.flattenList(variable_data)


		toc_2 = time.perf_counter()


		# print("flat_x", flat_x)
		# print("flat_y", flat_y)
		# print("flat_z", flat_z)
		# print("flat_var", flat_var)


		tic_3 = time.perf_counter()


		_temp_x = [float(ele) for ele in flat_x]
		_temp_y = [float(ele) for ele in flat_y]
		_temp_z = [float(ele) for ele in flat_z]
		_temp_var = [float(ele) for ele in flat_var]

		toc_3 = time.perf_counter()

		tic_4 = time.perf_counter()

		df = pd.DataFrame( list(zip(_temp_x, _temp_y, _temp_z, _temp_var)), columns =['x', 'y', 'z', var_name] )
		df['dummy_size'] = 0.01

		toc_4 = time.perf_counter()

		elapsed_time_0 = toc_0 - tic_0
		print("getting var data command  took ", elapsed_time_0, " seconds")

		elapsed_time_1 = toc_1 - tic_1
		print("deserialize var data command  took ", elapsed_time_1, " seconds")

		elapsed_time_2 = toc_2 - tic_2
		print("flatten var data command  took ", elapsed_time_2, " seconds")

		elapsed_time_3 = toc_3 - tic_3
		print("convert to float var data command  took ", elapsed_time_3, " seconds")

		elapsed_time_4 = toc_4 - tic_4
		print("convert to df var data command  took ", elapsed_time_4, " seconds")

		return df



	def getSimData(self, var_name, ts):
		print("getSimData")
		print("var_name", var_name)
		print("ts", ts)
		
		numRanks = 2

		_mochiNodeAddr = self.mochiNodeAddr + ":1234"

		pos_x = []
		pos_y = []
		pos_z = []
		var_data = []
		for r in range(numRanks):
			key = "x_ts_" + str(ts) + "_rank_" + str(r)
			cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
			pos_x.append( self.execute(cmd) )
			
			key = "y_ts_" + str(ts) + "_rank_" + str(r)
			cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
			pos_y.append( self.execute(cmd) )
			
			key = "z_ts_" + str(ts) + "_rank_" + str(r)
			cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
			pos_z.append( self.execute(cmd) )



			key = var_name + "_ts_" + str(ts) + "_rank_" + str(r)
			cmd = "source runSeerClientScript.sh " + _mochiNodeAddr + " " + self._dbName + " " + key
			var_data.append( self.execute(cmd) )
			

			#print("!key", key)
			#print("!cmd", cmd)
			#print("!var_data[]", var_data[r])


		print("len(var_data[0])", len(var_data[0]))


		# Deserialize data
		pos_data_x = []
		pos_data_y = []
		pos_data_z = []
		variable_data = []
		for r in range(numRanks):
			# Remove the last empty
			pos_x[r].pop()
			pos_y[r].pop()
			pos_z[r].pop()
			var_data[r].pop()

			pos_data_x.append( pos_x[r] )
			pos_data_y.append( pos_y[r] )
			pos_data_z.append( pos_z[r] )
			variable_data.append( var_data[r] )

		#print("pos_data_x", pos_data_x)
		#print("variable_data", variable_data)


		flat_x = self.flattenList(pos_data_x)
		flat_y = self.flattenList(pos_data_y)
		flat_z = self.flattenList(pos_data_z)
		flat_var = self.flattenList(variable_data)


		# print("flat_x", flat_x)
		# print("flat_y", flat_y)
		# print("flat_z", flat_z)
		# print("flat_var", flat_var)


		_temp_x = [float(ele) for ele in flat_x]
		_temp_y = [float(ele) for ele in flat_y]
		_temp_z = [float(ele) for ele in flat_z]
		_temp_var = [float(ele) for ele in flat_var]

		df = pd.DataFrame( list(zip(_temp_x, _temp_y, _temp_z, _temp_var)), columns =['x', 'y', 'z', var_name] )
		df['dummy_size'] = 0.01

		return df



	def getTestData(self):	#temporary
		df = pd.read_csv("ts_499_1M.csv")
		df['dummy_size'] = 0.01

		newDf = df.sample(n = 250000)

		print("the data")

		return newDf


	def getListOfVars(self):	#temporary
		listOfVars = []
		listOfVars.append({'label':'pressure', 'value':'pressure'})
		listOfVars.append({'label':'temperature', 'value':'temperature'})
		listOfVars.append({'label':'energy', 'value':'energy'})

		numTs = 4

		print("the list")

		return listOfVars, numTs


	def getListOfVariables(self, vars):
		listOfVars = []
		for v in vars:
			listOfVars.append({'label':v, 'value':v})

		return listOfVars
	


	def executeCommand(self, cmd):
		print("executeCommand", cmd)
		stdin, stdout, stderr = self.client.exec_command(cmd)
		output = stdout.read()
  
		response = []
		for line in output:
			response.append(line)
		return response


	def download_file(self, file: str):
		self.scp.get(file)
  
  
	def close(self):
		self.jhost.close()
		self.scp.close()