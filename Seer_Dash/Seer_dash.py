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
mochi_on = True			# Temporaty value
connected_to_server = False           # initially not connected
timestep_loaded = -1                  # initially not loaded    
timestep_current = 0                  # initally 0 but nothing there
timestep_max = 0

simulation_variables = []
mpi_ranks = []
numRanks = 0

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
			_loginNode = _loginNode.strip()
			_mochiNodeAddress = _mochiNodeAddress.strip()
			_mochiServerAddress = _mochiServerAddress.strip()
			_userName = _userName.strip()
			_dbName = _dbName.strip()

			print(_loginNode)
			print("_mochiNodeAddress",_mochiNodeAddress)
			print("_mochiServerAddress",_mochiServerAddress)
			print(_userName)
			print(_dbName)

			serverConnect.connect(_loginNode, _mochiNodeAddress, _mochiServerAddress, _userName, _dbName)

			tic = time.perf_counter()
			
			#print("Connected!!!")
			#serverConnect.gatherInitialData()

			mochiAddr = _mochiServerAddress + ":1234"

			# Get variables
			cmd = "source runSeerClientScript.sh " + mochiAddr + " " + _dbName + " variables"
			print(cmd)

			global simulation_variables
			sim_vars = serverConnect.execute(cmd)
			print("sim_vars",sim_vars[0])
			simulation_variables = serverConnect.getListOfVariables(sim_vars)
			print("simulation variable", simulation_variables)


			# Get number of ranks
			global numRanks
			global mpi_ranks
			cmd = "source runSeerClientScript.sh " + mochiAddr + " " + _dbName + " numRanks"
			print(cmd)

			numRanksOut = serverConnect.execute(cmd)
			#print("numRanksOut", numRanksOut[0])
			numRanks = int(numRanksOut[0])
			print("numRanks", numRanks)

			_mpi_ranks = []
			_mpi_ranks.extend(range(0, numRanks))

			__mpi_ranks = [str(x) for x in _mpi_ranks]
			mpi_ranks = serverConnect.getListOfVariables(__mpi_ranks)
			#print("mpi_ranks", mpi_ranks)



			# Get number of timesteps
			cmd = "source runSeerClientScript.sh " + mochiAddr + " " + _dbName + " current_timestep"
			#print(cmd)
			current_ts = serverConnect.execute(cmd)

			global timestep_max
			timestep_max = int(current_ts[0])
			#print("current_timestep", timestep_max)


			connected_to_server = True

			toc = time.perf_counter()
			elapsed_time = toc - tic
			print("Ger Sim vars data  took ", elapsed_time, " seconds")
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
	if _timestep > 0 and _simVar != None:

		global simVar
		global timestep
		global mpiViewRank

		
		if (_simVar != simVar or timestep != int(_timestep)):

			#
			# Get all the ranks
			#
			print("\n\n----------------------------------")
			print("update Global View")
			print("timestep", _timestep)
			print("simVar",_simVar)

			# Set these parameters
			simVar = _simVar
			timestep = int(_timestep)


			tic_0 = time.perf_counter()	# start

			global serverConnect
			#df = serverConnect.getSimData()
			
			#ts = int( int(_timestep) / 25 ) * 25
			ts = int( _timestep) 


			global numRanks
			numRanks = 4	# For testing

			frames = []
			for r in range(numRanks):
				_df = serverConnect.getSimRankData( _simVar, ts, int(r))
				frames.append(_df)
			df = pd.concat(frames)

			if len(df.index) > 500000:
				df = df.sample(n = 250000)

			toc_0 = time.perf_counter()	# stop

			tic_1 = time.perf_counter() # start

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
			print("Getting  data  took ", elapsed_time_0, " seconds")
			print("Drawing data  took ", elapsed_time_1, " seconds")



		if (mpiViewRank != _mpiRank):
			#
			# Get individual rank
			# 		print(_timestep)
			print("\n\n----------------------------------")
			print("update one rank View")
			print("timestep", _timestep)
			print("simVar",_simVar)
			print("mpiRank",_mpiRank)

			mpiViewRank = mpiViewRank

			tic_0 = time.perf_counter()	# start 

			
			#ts = int( int(_timestep) / 25 ) * 25
			ts = int( int(_timestep) ) 

			if _mpiRank == None:
				myRank = 0
			else:
				myRank = int(_mpiRank)

			df = serverConnect.getSimRankData( _simVar, ts, myRank)

			toc_0 = time.perf_counter()	# stop



			tic_1 = time.perf_counter() # start


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

			print("updateSingleRank gather data  took ", elapsed_time_1, " seconds")
			print("updateSingleRank draw data  took ", elapsed_time_2, " seconds")


	return children1, children2
	

'''
@app.callback(
	Output('graph_3d_rank', 'children'),
	Input(component_id='mpiRank', component_property='value'),
	[State('graph_3d_rank', 'children')]
)
def updateSingleRank( _mpiRank, children):
	global simVar
	global timestep
	global mpiViewRank

	_simVar = simVar
	_timestep = timestep

	if _timestep > 0:
		print("updateSingleRank")
		print(_mpiRank)

		tic_0 = time.perf_counter()

		global serverConnect
		#ts = int( int(_timestep) / 25 ) * 25
		ts = int( int(_timestep) ) 

		if _mpiRank == None:
			myRank = 0
		else:
			myRank = int(_mpiRank)

		df = serverConnect.getSimRankData( _simVar, ts, myRank)

		toc_0 = time.perf_counter()



		tic_1 = time.perf_counter()

		fig = go.Figure(data=[go.Scatter3d(x=df['x'], y=df['y'], z=df['z'],
										mode ='markers',
										marker = dict(
											size = 1,
											color=df[_simVar],
											colorscale ='turbo',
											opacity = 1
										))])
		fig.update_layout(margin=dict(l=0, r=0, b=0, t=0), template='plotly_dark', height=700)

		children.clear()
		children.append( dcc.Graph(figure=fig) )

		toc_1 = time.perf_counter()

		elapsed_time_1 = toc_0 - tic_0
		elapsed_time_2 = toc_1 - tic_1

		print("updateSingleRank gather data  took ", elapsed_time_1, " seconds")
		print("updateSingleRank draw data  took ", elapsed_time_2, " seconds")

	return children
'''






if __name__ == '__main__':
	app.run_server(debug=True, host='127.0.0.1', port='8051')