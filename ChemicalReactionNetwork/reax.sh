#!/bin/bash

#  Script.sh
#  Chem_react
#
#  Created by Andrew Diggs on 5/26/22.
#  
E1=1.15
E2=0.23
E3=0.30
E4=$1
OUT="FBM${E4}.txt"
./REAX $OUT $E1 $E2 $E3 $E4
