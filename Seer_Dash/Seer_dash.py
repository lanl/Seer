import time

import dash
import dash_bootstrap_components as dbc
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
from dash.exceptions import PreventUpdate

import plotly.graph_objects as go
import plotly.express as px

import pandas as pd

import Seer_dash_helper as sdh



# Some global variables
mochi_on = True			# Temporaty value, on by default

connected_to_server = False           # initially not connected
timestep_current = 0                  # initally 0 but nothing there


simulation_variables = []
numRanks = 0
mpi_ranks = []
timestep_max = 0


simVar = ""
timestep = -1
mpiViewRank = -1


# Connect to server
serverConnect = sdh.Seer_Dash_Helper()


# Initialize the app
app = dash.Dash(__name__, external_stylesheets=[dbc.themes.SOLAR])

app.layout = html.Div([
	# Header
	html.H1(children='Seer Dashboard', style={'text-align':'left', 'marginLeft': 15, 'marginTop': 20}),
	html.Hr(style={'marginLeft': 5, 'marginRight': 5, 'height': '2px', "color" :"white", "background-color" : "white"}),

	html.Div(children=[
		# First Row, First Column
		html.Div([

			# Connection Prameters
			html.Div([
				html.H5(children='Connection Parameters', style={'text-align':'left', 'marginLeft': 2, 'marginTop': 20}),
				html.Hr(style={'marginLeft': 1, 'marginRight': 1, 'height': '1px', 'border': '0px', "color" :"white", "background-color" : "white"}),

				html.Div([
					dcc.Input(
						id='loginNode',
						placeholder='Enter login node',
						type='text',
						value=''
					)
				], style={'marginLeft': 15,'marginBottom': 20}),

				html.Div([
					dcc.Input(
						id='mochiNodeAddress',
						placeholder='Enter Mochi node address',
						type='text',
						value=''
					)
				], style={'marginLeft': 15,'marginBottom': 20}),

				html.Div([
					dcc.Input(
						id='mochiServerAddress',
						placeholder='Enter Mochi server address',
						type='text',
						value=''
					)
				], style={'marginLeft': 15,'marginBottom': 20}),

				html.Div([
					dcc.Input(
						id='username',
						placeholder='Enter username',
						type='text',
						value=''
					)
				], style={'marginLeft': 15,'marginBottom': 20}),

				html.Div([
					dcc.Input(
						id='dbName',
						placeholder='Enter database name',
						type='text',
						value=''
					)
				], style={'marginLeft': 15,'marginBottom': 20}),


				html.Div([
					html.Button('Submit', id='submit_var')
				], style={'marginLeft': 15,'marginBottom': 20})
			]),

			# Simulation Parameters
			html.Div([
				html.H5(children='Simulation Variables', style={'text-align':'left', 'marginLeft': 2, 'marginTop': 40}),
				html.Hr(style={'marginLeft': 1, 'marginRight': 1, 'height': '1px', 'border': '0px', "color" :"white", "background-color" : "white"}),
				
				html.Div([
					html.Label('Select variable: '),
					dcc.Dropdown(
						id='simVar',
						options = simulation_variables
					)
				], style={'marginLeft': 15, 'marginBottom': 20, 'width':'90%'}),


				html.Div([
					html.Label('Timestep: '),
					dcc.Slider(
						id='timestep',
						min=0,
						max=timestep_max,
						step=1,
						value=timestep_current
					)
				], style={'marginLeft': 15, 'width':'90%'})

			])
		])
	], style={'marginLeft': 20, 'marginTop': 20, 'width':'250px', 'height':'800px', 'vertical-align': 'top', 'display': 'inline-block'}),


	# First Row, Second Column
	html.Div(children=[
		
		html.Div([
			html.Div([
					html.Label('Select MPI Rank: '),
					dcc.Dropdown(
						id='mpiRank',
						options = mpi_ranks
					)
				], style={'marginTop': 20, 'marginLeft': 25, 'marginBottom': 20, 'width':'15%'}),


			# intially empty for 3d rank
			html.Div([
			], id='graph_3d_rank', style={'marginLeft': 40, 'marginTop': 10, 'width':'90%', 'display': 'inline-block',  'height':'450px'})

		], style={'width':'60%', 'display':'inline-block', 'height':'600px'}),


		# intially empty for all ranks
		html.Div([
		], id='graph_3d', style={'marginLeft': 40, 'width':'30%', 'display': 'inline-block', 'height':'600px', 'vertical-align': 'top'}),

	], style={'marginLeft': 20, 'marginTop': 20, 'width':'85%', 'display': 'inline-block'})
])




@app.callback(
	[Output('simVar', 'options'),
	 Output('mpiRank', 'options'),
	 Output('timestep', 'max')],
	[Input(component_id='submit_var',component_property='n_clicks'),
	 Input(component_id='loginNode', component_property='value'),
	 Input(component_id='mochiNodeAddress', component_property='value'),
	 Input(component_id='mochiServerAddress', component_property='value'),
	 Input(component_id='username', component_property='value'),
	 Input(component_id='dbName', component_property='value')]
	)
def updateSimVars(n_clicks, _loginNode, _mochiNodeAddress, _mochiServerAddress, _userName, _dbName):
	global mochi_on
	global connected_to_server
	global serverConnect

	#print("mochi_on", mochi_on)
	#print("connected_to_server", connected_to_server)
	if mochi_on and connected_to_server == False:
		#print("mochi_on")
		if n_clicks is None:
			raise PreventUpdate
		else:
			# remove leading and trailing spaces
			_loginNode 			= _loginNode.strip()
			_mochiNodeAddress 	= _mochiNodeAddress.strip()
			_mochiServerAddress = _mochiServerAddress.strip()
			_userName 			= _userName.strip()
			_dbName 			= _dbName.strip()

			jsonData = sdh.readJson()
			if jsonData == None:
				print("config.json is missing!!!")
				exit()
			
			mochiAddr = _mochiServerAddress + ":" + jsonData["system"]["mochi-port"]

			print("_loginNode",_loginNode)
			print("_mochiNodeAddress",_mochiNodeAddress)
			print("mochiAddr",mochiAddr)
			print("_mochiServerAddress",_mochiServerAddress)
			print("_userName",_userName)
			print("_dbName",_dbName)


			tic = time.perf_counter()


			#
			# Connect to server
			serverConnect.connect(_loginNode, _mochiNodeAddress, _mochiServerAddress, _userName, _dbName)


			#
			# Get variables
			global simulation_variables

			cmd = "source runSeerClientScript.sh " + mochiAddr + " " + _dbName + " [\"variables\"]"
			#print(cmd)

			_sim_vars  = serverConnect.execute(cmd,["variables"])
			__sim_vars =  _sim_vars["variables"]
			sim_vars   = __sim_vars.split(",")
			#print("sim_vars",sim_vars[0])

			simulation_variables = serverConnect.getListOfVariables(sim_vars)
			print("simulation variable", simulation_variables)


			#
			# Get number of ranks
			global numRanks
			global mpi_ranks

			cmd = "source runSeerClientScript.sh " + mochiAddr + " " + _dbName + " [\"numRanks\"]"
			#print(cmd)

			numRanksOut = serverConnect.execute(cmd,["numRanks"])
			numRanks =  int(numRanksOut["numRanks"])
			print("numRanks", numRanks)


			_mpi_ranks = []
			_mpi_ranks.extend(range(0, numRanks))

			__mpi_ranks = [str(x) for x in _mpi_ranks]
			mpi_ranks = serverConnect.getListOfVariables(__mpi_ranks)
			#print("mpi_ranks", mpi_ranks)



			#
			# Get number of timesteps
			global timestep_max

			cmd = "source runSeerClientScript.sh " + mochiAddr + " " + _dbName + " [\"current_timestep\"]"
			#print(cmd)

			current_ts = serverConnect.execute(cmd,["current_timestep"])
			timestep_max = int(current_ts["current_timestep"])
			#print("current_timestep", timestep_max)



			connected_to_server = True


			#
			# Compute time
			toc = time.perf_counter()

			elapsed_time = toc - tic
			print("\nGet Sim vars data  took ", elapsed_time, " seconds!!!\n")

	# 	# Fill in variables
	# 	# Fill in # numsteps

	return simulation_variables, mpi_ranks, timestep_max




@app.callback(
	[Output('graph_3d', 'children'),
	 Output('graph_3d_rank', 'children')],
	[Input(component_id='mpiRank', component_property='value'),
	 Input(component_id='simVar', component_property='value'),
	 Input(component_id='timestep', component_property='value')],
	[State('graph_3d', 'children'),
	 State('graph_3d_rank', 'children')]
	)
def updateFig(_mpiRank, _simVar, _timestep, children1, children2):
	global simVar
	global timestep
	global mpiViewRank
	global numRanks
	global serverConnect

	oneRankUpdate = False


	if ((connected_to_server == True) and (_timestep > 0 and _simVar != None)):

		if (_simVar != simVar or timestep != int(_timestep)):
			#
			# Get all the ranks
			#

			oneRankUpdate = True	# force one rank update if variables change


			#
			# Set these parameters
			simVar = _simVar
			timestep = int(_timestep)
			jsonData = sdh.readJson()

			print("\n\n----------------------------------")
			print("update Global View")
			print("timestep", _timestep)
			print("simVar",_simVar)

			


			tic_0 = time.perf_counter()

			#
			# Get the data for the timestep and variable
			
			ts_increment = int(jsonData["simulation"]["timestep-increment"])
			#ts = int( _timestep)
			ts = int( int(_timestep) / ts_increment ) * ts_increment	
			
			
			# frames = []
			# #numRanks = 2	# For testing purposes only
			# # TODO: At some point, switch to gather all the ranks at the same time!!!
			# for r in range(numRanks):
			# 	_df = serverConnect.getSimRankData( _simVar, ts, int(r))
			# 	frames.append(_df)
			# df = pd.concat(frames)

			df = serverConnect.getSimMultiRankData( _simVar, ts, numRanks)


			# Prevent overloading of plotly 3D plot
			
			if len(df.index) > 500000:
				df = df.sample(n = 500000)


			toc_0 = time.perf_counter()





			tic_1 = time.perf_counter()

			#
			# Create a viz for the data

			fig1 = go.Figure(data=[go.Scatter3d(x=df['x'], y=df['y'], z=df['z'],
											mode ='markers',
											marker = dict(
												size = 1,
												color=df[_simVar],
												colorscale ='turbo',
												opacity = 1
											))])
			fig1.update_layout(margin=dict(l=0, r=0, b=0, t=0), template='plotly_dark', height=700)

			children1.clear()
			children1.append( dcc.Graph(figure=fig1) )

			toc_1 = time.perf_counter()	# stop




			elapsed_time_0 = toc_0 - tic_0
			elapsed_time_1 = toc_1 - tic_1
			print("getSimMultiRankData number of rows:",  len(df.index))
			print("\nGetting all the data took ", elapsed_time_0, " seconds!!!")
			print("Drawing all the data took ", elapsed_time_1, " seconds!!!\n")



		if (mpiViewRank != _mpiRank or oneRankUpdate == True):
			#
			# Individual rank viz
			#

			mpiViewRank = _mpiRank


			print("\n\n----------------------------------")
			print("update one rank View")
			print("timestep", _timestep)
			print("simVar",_simVar)
			print("mpiRank",_mpiRank)

			


			tic_0 = time.perf_counter()	 


			#
			# Get data for the rank

			ts = int( int(_timestep) ) 
			#ts = int( int(_timestep) / 25 ) * 25	# HACC

			if _mpiRank == None:
				myRank = 0
			else:
				myRank = int(_mpiRank)


			df = serverConnect.getSimRankData( _simVar, ts, myRank)
			

			toc_0 = time.perf_counter()




			tic_1 = time.perf_counter()

			#
			# Update the viz for one rank

			fig2 = go.Figure(data=[go.Scatter3d(x=df['x'], y=df['y'], z=df['z'],
											mode ='markers',
											marker = dict(
												size = 1,
												color=df[_simVar],
												colorscale ='turbo',
												opacity = 1
											))])
			fig2.update_layout(margin=dict(l=0, r=0, b=0, t=0), template='plotly_dark', height=700)

			children2.clear()
			children2.append( dcc.Graph(figure=fig2) )

			toc_1 = time.perf_counter()	# stop





			elapsed_time_1 = toc_0 - tic_0
			elapsed_time_2 = toc_1 - tic_1

			print("getSimRankData number of rows:",  len(df.index))

			print("\nGetting data for a single rank took ", elapsed_time_1, " seconds!!!")
			print("Drawing a single rank took ", elapsed_time_2, " seconds!!!\n\n")


	return children1, children2
	


if __name__ == '__main__':
	app.run_server(debug=True, host='127.0.0.1', port='8051')