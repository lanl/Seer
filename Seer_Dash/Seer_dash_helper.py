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


	def listToString(self, l): 
		keysStr = "[" 

		for el in l:
			keysStr = keysStr + "\"" + el + "\","

		keysStr = keysStr.rstrip(keysStr[-1])   #remove last ","
		keysStr = keysStr + "]"
		
		return keysStr
		

	def parseText(self, text, seperator, variables):
		outputDict = {}
		index = 0
		#print("\n\n\ntext:", text)
		#print("len(text)", len(text))

		i = 0
		while (i < len(text)):
			line = text[i]
			#print("Line:", i, ":", text[i])

			if line.startswith(seperator, 0, len(seperator)):
				i = i+1
				output_line = text[i]
				#print("output_line 1:", output_line)
				outputDict[ variables[index] ] = output_line    #store line in dictionary
				
				i = i+1
				index = index + 1

			i = i+1

		#print("variables",variables)
		#print("outputDict",outputDict)

		return outputDict


	def getListOfVariables(self, vars):
		listOfVars = []
		for v in vars:
			listOfVars.append({'label':v, 'value':v})

		return listOfVars
	

	def close(self):
		self.jhost.close()
		self.scp.close()



	def connect(self, loginNode, mochiNodeAddr, mochiServerAddress, username, dbName):
		self.loginNodeAddr = loginNode
		self.mochiNodeAddr = mochiNodeAddr
		self.mochiServerAddress = mochiServerAddress
		self.username = username
		self._dbName = dbName

		print("\nself.loginNodeAddr", self.loginNodeAddr)
		print("self.mochiNodeAddr", self.mochiNodeAddr)
		print("self.mochiServerAddress", self.mochiServerAddress)
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
		print("connecting established, took ", elapsed_time, " seconds\n\n")

		# For copying files
		self.scp = SCPClient(self.vm.get_transport())


	
	def execute(self, cmd, keys=[]):
		print("\nexecute cmd:", cmd)

		tic = time.perf_counter()

		max_reconnect = 5
		reconnect = 0
		#output_data = ""
		outputDic = {}

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
			elapsed_time_2 = toc_2 - tic_2
			print("parsing 1 data  took ", elapsed_time_2, " seconds")



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

				#print("shout:",shout)
				outputDic = self.parseText(shout, "*_*_*", keys)
				#print("outputDic:",outputDic)

				toc_3 = time.perf_counter()
				elapsed_time_3 = toc_3 - tic_3
				print("parsing 2 data  took ", elapsed_time_3, " seconds")


				break
			else:
				self.connect(self.loginNodeAddr, self.mochiNodeAddr, self.mochiServerAddress, self.username, self._dbName)
				print("Reconnecting")
				reconnect = reconnect + 1
			
			
		
		elapsed_time_1 = toc_1 - tic_1
		print("running command  took ", elapsed_time_1, " seconds")

		
		toc = time.perf_counter()
		elapsed_time = toc - tic
		print("Getting data took ", elapsed_time, " seconds\n\n")
				

		return outputDic



	def getSimRankData(self, var_name, ts, myRank=0):
		print("\n\getSimRankData")
		print("var_name", var_name)
		print("ts", ts)
		print("myRank", myRank)
		

		_mochiServerAddress = self.mochiServerAddress + ":1234"

		tic_0 = time.perf_counter()

		# pos_x = []
		# pos_y = []
		# pos_z = []
		# var_data = []

		
		keys = []
		keys.append("x_ts_" + str(ts) + "_rank_" + str(myRank))
		keys.append("y_ts_" + str(ts) + "_rank_" + str(myRank))
		keys.append("z_ts_" + str(ts) + "_rank_" + str(myRank))
		keys.append(var_name + "_ts_" + str(ts) + "_rank_" + str(myRank))

		keyString = self.listToString(keys)
		#print("keys:",keys)
		#print(keyString)

		cmd = "source runSeerClientScript.sh " + _mochiServerAddress + " " + self._dbName + " " + keyString
		#print("cmd:",cmd)
		

		outputDic = self.execute(cmd, keys)

		# print(outputDic)
		# print(keys)

		# print(keys[0], outputDic[keys[0]])
		# print(keys[1], outputDic[keys[1]])
		# print(keys[2], outputDic[keys[2]])
		# print(keys[3], outputDic[keys[3]])

		toc_0 = time.perf_counter()
		


		# Deserialize data
		tic_1 = time.perf_counter()

		flat_x   = outputDic[keys[0]].split(',')
		flat_y   = outputDic[keys[1]].split(',')
		flat_z   = outputDic[keys[2]].split(',')
		flat_var = outputDic[keys[3]].split(',')

		_temp_x = [float(ele) for ele in flat_x]
		_temp_y = [float(ele) for ele in flat_y]
		_temp_z = [float(ele) for ele in flat_z]
		_temp_var = [float(ele) for ele in flat_var]

		toc_1 = time.perf_counter()



		tic_4 = time.perf_counter()

		df = pd.DataFrame( list(zip(_temp_x, _temp_y, _temp_z, _temp_var)), columns =['x', 'y', 'z', var_name] )
		df['dummy_size'] = 0.01

		toc_4 = time.perf_counter()



		elapsed_time_0 = toc_0 - tic_0
		print("getting var data command  took ", elapsed_time_0, " seconds")

		elapsed_time_1 = toc_1 - tic_1
		print("deserialize var data command  took ", elapsed_time_1, " seconds")

		elapsed_time_4 = toc_4 - tic_4
		print("convert to df var data command  took ", elapsed_time_4, " seconds")

		return df