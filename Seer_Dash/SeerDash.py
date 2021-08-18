

class SeerDash:
    def __init__(self):
        pass


    def connect(self, protocol, address, p_id, db_name):
        # Connect to the remote server
        pass


    def getSimVariableList(self):
        # Retreive the list of variables from the simulation (simulation data, papi counters, timers, ...)
        pass


    def getNumTimesteps(self):
        # Get the number of timestpes from the simulation
        pass


    def getVariable(self, variable_name, timestep):
        # for a variable, retreive the data; returns a list
        pass


    def getVariablePerRank(self, variable_name, timestep, sim_rank):
        # for a variable, retreive the data for a simulation rank; returna a list
        pass


    