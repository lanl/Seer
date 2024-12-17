#!/bin/bash

launch_script=$1
config_file=$2
output_file=$3

source $1  $2 > $3

address = grep "Bedrock daemon now running" mochi_output.sh | awk -F'://' '{print $2}'
echo $address