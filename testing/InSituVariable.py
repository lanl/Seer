class Variable:
    def __init__(self, name, topology):
        self.name = name
        self.topology = topology  # options: grid | point
        self.ts_values = {}

    def set_variable(self, ts, values):
        ts_values[ts] = values

    def get_variable_ts(self, ts):
        try:
            return ts_values[ts]
        except:
			return None


class Dataset:
    def __init__(self):
        self.variables = {}

    def add_variable(self, var, topology):
        self.variables[var] = Variable(var, topology)

    def add_timestep(self, var, ts, values):
        self.variables[var].set_variable(ts, values)

    def get_variable(self, var, ts):
        return self.variables[var].get_variable_ts(ts)