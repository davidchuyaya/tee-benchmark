#!/bin/bash
# From https://learn.microsoft.com/en-us/azure/virtual-network/virtual-network-test-latency?tabs=linux#test-vms-with-latte-or-sockperf
sudo apt-get install build-essential -y
sudo apt-get install git -y -q
sudo apt-get install -y autotools-dev
sudo apt-get install -y automake
sudo apt-get install -y autoconf
sudo apt-get install -y libtool
sudo apt update
sudo apt upgrade -y

git clone https://github.com/mellanox/sockperf
cd sockperf/
./autogen.sh
./configure --prefix=

#make is slow, may take several minutes
make

#make install is fast
sudo make install