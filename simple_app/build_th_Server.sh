mpic++ /vast/projects/exasky/pascal/HACC/Seer/simple_app/thallium_server.cpp -std=c++17 -o server_thallium \
-I /vast/projects/exasky/pascal/HACC/Seer/src/headerOnly/ \
-I /vast/projects/exasky/pascal/HACC/SZ3/install/include/ \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc2-2.15.1-fbe56n4kdqfglsxcsdtzpxbiejhbcysh/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-njadzndq6fkjnrva3utxazt4psj6pc2v/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-e36frcvduknklrl3byh2bwogtulbuoe6/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/nlohmann-json-3.11.3-a5ggtkz4pnf7gwreapknujok4kiiomtb/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-thallium-0.15.0-ptex6ipz6fpdirvkweuek5rdq4dzowos/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-bedrock-0.15.3-zglb36jrweuarz5dp23zf7aovd65wlbe/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mercury-2.4.0-eg43xipuua3xhh6nqnp3ypagdhj3yvx5/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/argobots-1.2-h6nupvcrgfvpy4md2mtwkpankc3u3ibj/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/cereal-1.3.2-g7ouf5lmglnxzbbmunyg7z3oj5qkfmjy/include \
-L /projects/insituperf/SZ3/install/lib64 -l SZ3c \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mercury-2.4.0-eg43xipuua3xhh6nqnp3ypagdhj3yvx5/lib -l mercury \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/argobots-1.2-h6nupvcrgfvpy4md2mtwkpankc3u3ibj/lib -l abt \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc2-2.15.1-fbe56n4kdqfglsxcsdtzpxbiejhbcysh/lib64 -l blosc2 \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-njadzndq6fkjnrva3utxazt4psj6pc2v/lib -l yokan-client \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-e36frcvduknklrl3byh2bwogtulbuoe6/lib -l margo \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/zstd-1.5.7-ewmpvegf46ozfjuow73ioul4s55q57fg/lib -l zstd


spack load zstd

export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc2-2.15.1-fbe56n4kdqfglsxcsdtzpxbiejhbcysh/lib64/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-njadzndq6fkjnrva3utxazt4psj6pc2v/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-e36frcvduknklrl3byh2bwogtulbuoe6/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/projects/exasky/pascal/HACC/SZ3/install/lib64/:$LD_LIBRARY_PATH

