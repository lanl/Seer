source loadSpack.sh
spack load -r /onv3yay #py-mochi-sdskv 
clear
echo $1
echo $2
echo $3
python seerClientScript.py $1 $2 $3
