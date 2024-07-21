import json
from db_access import DBAccess

class InClient:
    ''' Get data from the servers '''

    def __init__(self, filename, rank=0, num_ranks=1):
        self.rank = rank
        self.num_ranks = num_ranks
        self.my_data_servers = []
        self.provider_id = []

        self.__get_server_list(filename)

    
    def __get_server_list(self, filename):
        ''' Get list of servers '''

        # Read the json file
        f = open(filename,'r')
        data = json.load(f)

        # Get all the servers
        serverList = []
        for i in data['data-servers']:
            serverList.append(i)

        num_servers = len(serverList)
        print("num_servers:",num_servers)

        # Find my list of data servers
        for i in range(num_servers):
            index = i*self.num_ranks + self.rank
            if index >= num_servers:
                return

            print(serverList[index])
            self.my_data_servers.append( serverList[index]['addr'] )
            self.provider_id.append( serverList[index]['provider_id'] )

        print(self.my_data_servers)
        print(self.provider_id)

    
    def get_all_keys(self, ts=0):
        ''' Get all the metadata about the data stored '''

        provider_id_0 = self.provider_id[0]
        server_0 = self.my_data_servers[0]
       
        protocol = server_0.split('://')[0]
        addr     = server_0.split('://')[1]

        print("provider_id_0", provider_id_0)
        print("protocol", protocol)
        print("addr", addr)

        server_0_conn = DBAccess(provider_id_0, protocol, addr)
        keys = server_0_conn.get_all_keys('_'+str(ts))

        return keys


    def get_metadata(self, field, ts):
        ''' Get metadata about a specific field '''
    
        provider_id_0 = self.provider_id[0]
        server_0 = self.my_data_servers[0]
        protocol = server_0.split('://')[0]
        addr     = server_0.split('://')[1]

        server_0_conn = DBAccess(provider_id_0, protocol, addr)
        keys = server_0_conn.get_field_info(field, ts)

        return keys


    def get_data(self, field, ts):
        ''' Get data for a field '''
        all_data = {}

        for i in range(0, len(self.my_data_servers)):
            server = self.my_data_servers[i]
            provider_id = self.provider_id[i]

            protocol = self.server.split('://')[0]
            addr     = self.server.split('://')[1]
            server_conn = DBAccess(provider_id, protocol, addr)

            val = server_conn.get_value(field, ts)
            all_data[i] = val

        return all_data



# def main():
#     c = Client(0, 2, 'server-data.json')
#     slist = c.gather_data()
#     for i in slist:
#         print(i)

# if __name__ == "__main__":
#     main()

