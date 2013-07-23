#!/bin/bash

rm -f ../*.it
./openwns -f configInHLoS.py &
./openwns -f configInHNLoS.py &
./openwns -f configRMaLoS.py &
./openwns -f configRMaNLoS.py &
./openwns -f configSMaLoS.py &
./openwns -f configSMaNLoS.py &
./openwns -f configUMaLoS.py &
./openwns -f configUMaNLoS.py &
./openwns -f configUMiLoS.py &
./openwns -f configUMiNLoS.py &
