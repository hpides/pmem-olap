#!/bin/bash

git clone https://github.com/baotonglu/dash.git
cd dash
git clone https://github.com/XiangpengHao/VeryPM.git

# Make small modifications
git apply ../dash_ssb.patch
sed -i -e "s#std::cout << \"Directory_Doubling#//#g" src/ex_finger.h

cd ..
