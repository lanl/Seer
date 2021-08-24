import dash
import dash_bootstrap_components as dbc
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
from dash.exceptions import PreventUpdate

import plotly.graph_objects as go
import plotly.express as px

import Seer_dash_helper as sdh



# Some global variables
mochi_on = False			# Temporaty value
connected_to_server = False           # initially not connected
timestep_loaded = -1                  # initially not loaded    
timestep_current = 0                  # initally 0 but nothing there
timestep_max = 0

simulation_variables = []


# Connect to server
serverConnect = sdh.Seer_Dash_Helper()


# Initialize the app
app = dash.Dash(__name__, external_stylesheets=[dbc.themes.SOLAR])

app.layout = html.Div([
	# Header
	html.H1(children='Seer Dasboard', style={'text-align':'left', 'marginLeft': 15, 'marginTop': 20}),
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
			#dcc.Graph( id="scatter_plot")
		], id='graph_3d', style={'marginLeft': 40, 'marginTop': 20, 'width':'60%', 'height':'800px', 'display': 'inline-block'})


		# html.Div([
		#     dcc.Graph(id="scatter-plot2"),
		#         html.P("Petal Width:"),
				
		# ], style={'marginLeft': 40, 'marginTop': 20, 'width':'30%', 'height':'500px',  'display': 'inline-block'})

	], style={'marginLeft': 20, 'marginTop': 20, 'width':'80%', 'display': 'inline-block'})


	#  # Second Row, Second Column
	# html.Div(children=[
	#     html.H1(children='Seer Dasboard', style={'text-align':'left', 'marginLeft': 15, 'marginTop': 20}),
	# ])
])


@app.callback(
	[Output('simVar', 'options'),
	 Output('timestep', 'max')],
	[Input(component_id='submit_var',component_property='n_clicks'),
	Input(component_id='loginNode', component_property='value'),
	Input(component_id='mochiServerAddress', component_property='value'),
	Input(component_id='username', component_property='value'),
	Input(component_id='dbName', component_property='value')]
)
def updateSimVars(n_clicks, _loginNode, _mochiServerAddress, _userName, _dbName):
	global mochi_on
	global connected_to_server
	global serverConnect

	if mochi_on and connected_to_server == False:
		if n_clicks is None:
			raise PreventUpdate
		else:
			serverConnect.connect(_loginNode, _mochiServerAddress, _userName)
			serverConnect.gatherInitialData()

			connected_to_server = True
	# 	# Fill in variables
	# 	# Fill in # numsteps


	
	global simulation_variables
	if connected_to_server == False:
		#if (_loginNode != '' and _mochiServerAddress != '') and (_userName != '' and _dbName != ''):
		if n_clicks is None:
			raise PreventUpdate
		else:
			print(_loginNode)
			print(_mochiServerAddress)
			print(_userName)
			print(_dbName)
			global simulation_variables

			list_of_vars, numTs = serverConnect.getListOfVars()
			
			global timestep_max
			timestep_max = numTs

			for var in list_of_vars:
				simulation_variables.append(var)

			connected_to_server = True


	return simulation_variables, timestep_max



@app.callback(
	Output('graph_3d', 'children'),
	[Input(component_id='simVar', component_property='value'),
	Input(component_id='timestep', component_property='value')],
	[State('graph_3d', 'children')]
)
def updateFig(_simVar, _timestep, children):
	if _timestep > 0 and _simVar != None:
		print(_timestep)
		print(_simVar)

		global serverConnect
		df = serverConnect.getTestData()

		fig = go.Figure(data=[go.Scatter3d(x=df['x'], y=df['y'], z=df['z'],
										mode ='markers',
										marker = dict(
											size = 1,
											color = df['phi'],
											colorscale ='turbo',
											opacity = 1
										))])
		fig.update_layout(margin=dict(l=0, r=0, b=0, t=0), template='plotly_dark', height=700)

		children.clear()
		children.append( dcc.Graph(figure=fig) )

	return children
	
	


if __name__ == '__main__':
	app.run_server(debug=True)