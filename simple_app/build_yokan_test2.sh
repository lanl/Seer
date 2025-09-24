mpic++ /vast/projects/exasky/pascal/HACC/Seer/simple_app/yokan_test.cpp -std=c++17 -o test_yokan2 \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-smyxtx7lf2xuahpdbghbxyrbzjy5csgs/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-ndhwiiyzwquol2fvysgycyzfiymzbg6l/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mercury-2.4.0-t2h2yrl7dfgscedoea3qekkiayxmxu5b/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/argobots-1.2-h6nupvcrgfvpy4md2mtwkpankc3u3ibj/include \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-smyxtx7lf2xuahpdbghbxyrbzjy5csgs/lib -l yokan-client \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-ndhwiiyzwquol2fvysgycyzfiymzbg6l/lib -l margo 


export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-smyxtx7lf2xuahpdbghbxyrbzjy5csgs/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-ndhwiiyzwquol2fvysgycyzfiymzbg6l/lib/:$LD_LIBRARY_PATH


#
#-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/nlohmann-json-3.11.3-a5ggtkz4pnf7gwreapknujok4kiiomtb/include \

#-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-abt-io-0.8.1-xobqe5k7pmb6r4sy2npd7vvkphuvuqem/include \
# -I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-ssg-0.5.3-vhiwabh5pjcaure3qdbpwdkllkdvhbtt/include \


# -I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-thallium-0.15.0-ptex6ipz6fpdirvkweuek5rdq4dzowos/include \
# -I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-bedrock-0.15.3-zglb36jrweuarz5dp23zf7aovd65wlbe/include \


