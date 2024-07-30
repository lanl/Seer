if [ $# -lt 1 ]; then
  echo "Usage: $0 <port e.g. 8871>"
  exit 1
fi

echo "On the client do:"
echo "ssh -N -f -L $1:$(hostname -f):$1 pascalgrosset@darwin-fe.lanl.gov"
echo "then in a browser: http://localhost:$1"
echo "---------------------------------------------------------"
echo ""
export PYVISTA_TRAME_SERVER_PROXY_PREFIX='/proxy/'
ipython kernel install --name "venv-seer" --user
jupyter-notebook --no-browser --port=$1 --ip=0.0.0.0 &


# run as e.g.:
# source client/launchJupyterNotebookServer.sh 8871
