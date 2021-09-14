source loadSpack.sh
spack load -r /onv3yay #py-mochi-sdskv
clear
echo $1
echo $2
echo $3
echo $4
python seerClientScript.py $2 $3 $4

# source runSeerClientScript.sh <node-gateway-address> <node-infiniband-address:port> <db_name> <variable_list>
# source runSeerClientScript.sh 192.168.100.61:1234 192.168.81.83:1234 foo_test8 variables