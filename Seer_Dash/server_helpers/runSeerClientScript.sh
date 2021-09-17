source loadSpack.sh
spack load -r /onv3yay #py-mochi-sdskv 
clear
echo $1
echo $2
echo $3
python seerClientScript.py $1 $2 $3

# source runSeerClientScript.sh 192.168.81.37:1234 foo_test_unstruc_1 ["variable1","variable2",...]
# there should be no space between in ["variable1","variable2",...]