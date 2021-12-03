#!/bin/bash

git clone https://github.com/baotonglu/dash.git
cd dash
# Get older version used for paper
git checkout 7e1551f03dfbd901b38af405b52d6b919235c379 
git clone https://github.com/XiangpengHao/VeryPM.git

# Make small modifications
git apply ../dash_ssb.patch
sed -i -e "s#std::cout << \"Directory_Doubling#//#g" src/ex_finger.h

cd ..
