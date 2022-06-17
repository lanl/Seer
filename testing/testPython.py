import sys

def listToString(l): 
    
    # initialize an empty string
    keysStr = "[" 

    for el in l:
        keysStr = keysStr + "\"" + el + "\","

    keysStr = keysStr.rstrip(keysStr[-1])   #remove last ","
    keysStr = keysStr + "]"
    
    return keysStr



server_addr = 'ofi+tcp://' + sys.argv[1]
db_name = sys.argv[2]
keys = sys.argv[3]

print("-----")
print("server_addr", server_addr)
print("db_name",db_name)
print("keys",keys)

print("-----")

l = len(keys)
li = keys[1:l-1].split(',')

for k in li:
    print(k)



ts = 5
myRank = 45
var_name = "temperature"
_mochiServerAddress = server_addr

# keys = "["
# keys = keys + "\"x_ts_" + str(ts) + "_rank_" + str(myRank) + "\","
# keys = keys + "\"y_ts_" + str(ts) + "_rank_" + str(myRank) + "\","
# keys = keys + "\"z_ts_" + str(ts) + "_rank_" + str(myRank) + "\","
# keys = keys + "\"" + var_name + "_ts_" + str(ts) + "_rank_" + str(myRank) + "\""
# keys = keys + "]"

keys = []
keys.append("x_ts_" + str(ts) + "_rank_" + str(myRank))
keys.append("y_ts_" + str(ts) + "_rank_" + str(myRank))
keys.append("z_ts_" + str(ts) + "_rank_" + str(myRank))
keys.append(var_name + "_ts_" + str(ts) + "_rank_" + str(myRank))

keyString = listToString(keys)
print(keys)
print(keyString)


cmd = "source runSeerClientScript.sh " + _mochiServerAddress + " " + db_name + " " + keyString
print("cmd:",cmd)
