# Format: bedrock na+sm -c mochi-yokan-config.json &

if [ $# -lt 1 ]; then
  echo "Usage: $0 <config e.g. mochi-yokan-config.json>"
  exit 1
fi

#bedrock na+sm -c $1 &
bedrock tcp -c $1 &

# run as:
# run/launchMohiServer.sh test_app/mochi-yokan-config.json
