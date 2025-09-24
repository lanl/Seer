mpic++ /vast/projects/exasky/pascal/HACC/Seer/client/exampleApp/access.cpp -std=c++17 -o runClient \
-I /vast/projects/exasky/pascal/HACC/Seer/client/src/ \
-I /vast/projects/exasky/pascal/HACC/SZ3/install/include/ \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc2-2.15.1-hvruy4it4vi5th2m6ahqlmsfwctbmy4q/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-ybbsooyen74hefehnw2xzeuppd5z3mtz/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-joti6nw7pzarikty7z5b2dknapcazhju/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/nlohmann-json-3.11.3-fem5ir3hz5ep5dlxsw55vsneynjblnjd/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-abt-io-0.8.1-xobqe5k7pmb6r4sy2npd7vvkphuvuqem/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-ssg-0.5.3-vhiwabh5pjcaure3qdbpwdkllkdvhbtt/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-thallium-0.15.0-pv7yokpaarzvo3dte3usjrvy3bi35bya/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-bedrock-0.15.3-jwgrymz4fnkoqxtcrd3vzmzftkoqu224/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mercury-2.4.0-54cofacawksajgx5lkfud7q35vgf42j4/include \
-I /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/argobots-1.2-4eszk2dlhf6zr252eau7b7k2h4fj6flj/include \
-L /projects/insituperf/SZ3/install/lib64 -l SZ3c \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc2-2.15.1-hvruy4it4vi5th2m6ahqlmsfwctbmy4q/lib64/ -l blosc2 \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-ybbsooyen74hefehnw2xzeuppd5z3mtz/lib -l yokan-client \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-joti6nw7pzarikty7z5b2dknapcazhju/lib -l margo \
-L /vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/zstd-1.5.7-solll6jyy7hgwavzyba6zu5x33ghp3qz/lib -l zstd

# -I /projects/insituperf/seer_o/client/src/ \
# -I /projects/insituperf/SZ3/install/include/ \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/nlohmann-json-3.11.2-axdmuuszkpr5sl42467aveuc2rubmg3k/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-abt-io-0.6.0-ajkjkzqhqeml4l7a2nedezup6kqsl7mr/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-thallium-0.12.0-yxhgxsubfjsz57grdxjdtz5zffnhtfsn/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-ssg-0.5.3-osjcelmofnrz6fmogj4ibyujc4drummo/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-bedrock-0.9.1-trc45mwsnojzpqhtfdje3ionrponizg3/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mercury-2.3.1-nsflrfvyumlrmhahhsapli6vi2pl5ocw/include \
# -I /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/argobots-1.2rc1-a2nl6yqhrkfpngr4jke43fnu5jwuyz3f/include \
# -L /projects/insituperf/SZ3/install/lib64 -l SZ3c \
# -L /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/lib64/ -l blosc2 \
# -L /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/lib -l yokan-client \
# -L /vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/lib -l margo \
# -l zstd



