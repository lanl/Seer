import numpy as np
from pathlib import Path
from pysz import SZ
import sys

# prepare your data in numpy array format
data = np.fromfile('/projects/insituperf/compressed.sz3', dtype=np.uint8)

# init SZ (both SZ2 and SZ3 are supported)
# Please change the path to the SZ dynamic library file in your system
lib_extention = {
    "darwin": "libSZ3c.dylib",
    "windows": "SZ3c.dll",
}.get(sys.platform, "libSZ3c.so")

sz = SZ("/projects/insituperf/SZ3/install/lib64/{}".format(lib_extention))


# decompress, both input and output data are numpy array
data_dec = sz.decompress(data, (256,256,256), np.float32)
data_dec.tofile("/projects/insituperf/uncompressedxxx.raw")