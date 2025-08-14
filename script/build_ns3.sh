cd ns-3-dev || exit
    ./ns3 clean
    ./ns3 configure --enable-examples --enable-tests --enable-sudo
    ./ns3 build
    
cd -