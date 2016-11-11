#!/usr/bin/env bash

# Example of the run script for running the fraud detection algorithm with C++.

# Required: 
#  -C++11

echo "Comiling ./src/digitalwallte.cpp using c++11 compiler\n"
rm *.o runcode
g++ -std=c++11 -O3 -ftree-vectorize ./src/digitalwallet.cpp -o ./runcode 
echo "Runnig code ...\n"
./runcode ./paymo_input/batch_payment.txt ./paymo_input/stream_payment.txt ./paymo_output/

