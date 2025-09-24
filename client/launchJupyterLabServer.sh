if [ $# -lt 1 ]; then
  SCRIPT_NAME=$(basename "${BASH_SOURCE[0]}")
  echo "Wrong number of argyments provided!!! Use as follows: source $SCRIPT_NAME <port e.g. 8871>"
  return 1 2>/dev/null || { echo "This script must be sourced to gracefully terminate."; }
fi

echo "On the client do:"
echo "ssh -N -f -L $1:$(hostname -f):$1 $(whoami)@darwin-fe.lanl.gov"
echo "then in a browser: http://localhost:$1"
echo "---------------------------------------------------------"
echo ""
echo "To properly use Jupyter lab in a pip virtual enviornment, register it as follows:"
echo "python -m ipykernel install --user --name=<name of virtual environment>"
echo "---------------------------------------------------------"
echo ""
jupyter lab --no-browser --port=$1 --ip=0.0.0.0 &