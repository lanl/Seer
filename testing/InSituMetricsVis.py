import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import plotly.express as px
import plotly.graph_objects as go
import numpy as np

class Metric:
	
	def __init__(self, name):
		self.name = name
		self.ts_values = {}
		
	# def __init__(self, name, ts, values):
	# 	self.name = name
	# 	self.set_values_at_ts(ts, values)
		

	def set_values_at_ts(self, ts, vals):
		""" vals: array containing the values
			ts: timestep for this set of values"""
		self.ts_values[ts] = vals
		

	def get_values_at_ts(self, ts):
		try:
			return self.ts_values[ts]
		except:
			return None
	
	
	def get_name(self):
		return self.name
	

	def plot(self, ts):
		"""Plot metric for a SINGLE timestep"""
		data_y = self.ts_values[ts]
		core_range = list(range(0, len(data_y)))
  
		# Configure data for plotly
		data = [go.Bar(
			x = core_range,
			y = data_y
		)]
		fig = go.Figure(data=data)
  
		# Add axis label
		fig.update_layout(
			xaxis_title="cores",
			yaxis_title=self.name
		)
  
		fig.show()

  
  
	def plot_ts(self):
		"""plot metric for ALL timesteps"""
  
		# Get the data and transpose it
		ts_core =[]
		for ts in range(len(self.ts_values)):
			ts_core.append(self.ts_values[ts])
		core_ts = np.transpose(ts_core)
  
		ts_range = list(range(0, len(core_ts)))
  

		# Draw Figure
		fig = go.Figure()

		# Add traces
		for c in range(len(core_ts)):
			fig.add_trace(go.Scatter(x=ts_range, y=core_ts[c], mode='lines+markers', name="Core " + str(c)))
		
		# Add axis label
		fig.update_layout(
			xaxis_title="timestep",
			yaxis_title=self.name
		)
  
		fig.show()

	

class InSituMetricsVis:
	
	def __init__(self):
		self.metrics = {}
		
	def add_metric(self, metric):
		self.metrics[metric] = Metric(metric)
  
	def add_timestep(self, metric, ts, values):
		self.metrics[metric].set_values_at_ts(ts, values)
		
	def get_values(self, metric, ts):
		return self.metrics[metric].get_values_at_ts(ts)

	def plot(self, metric, ts=-1):
		if ts == -1:
			self.metrics[metric].plot_ts()
		else:
			self.metrics[metric].plot(ts)



		
		
	
		
	
	
	