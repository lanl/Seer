from re import M, template
from threading import Event
import pandas as pd

from paramiko import SSHClient
from scp import SCPClient, SCPException

class Seer_Dash_Helper:
	def __init__(self):
		pass


	def connect(self, loginNode, mochiNodeAddr, username):
		self.loginNodeAddr = loginNode
		self.mochiNodeAddr = mochiNodeAddr
		self.username = username

		# Do ssh jump host (ssh -J) to go to compute node directly
		self.vm = paramiko.SSHClient()
		self.vm.set_missing_host_key_policy(paramiko.AutoAddPolicy())

		self.vm.connect(self.loginNodeAddr,self.username)
		vmtransport = vm.get_transport()

		dest_addr = (self.mochiNodeAddr, 22)
		local_addr = (self.loginNodeAddr, 22)

		vmchannel = vmtransport.open_channel("direct-tcpip", dest_addr, local_addr)

		self.jhost = paramiko.SSHClient()
		self.jhost.set_missing_host_key_policy(paramiko.AutoAddPolicy())

		self.jhost.connect(self.mochiNodeAddr, username=self.username, sock=vmchannel)


		# Attempt to maintian connection open
		channel = self.jhost.invoke_shell()
		self.stdin = channel.makefile('wb')
		self.stdout = channel.makefile('r')


		# For copying files
		self.scp = SCPClient(self.vm.get_transport())
  

	def gatherInitialData(self):
		# Get the number of timesteps

		# Get the scalars

		pass


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
	


	def executeCommand(self, cmd):
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