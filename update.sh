#!/bin/bash
git pull;
qmake tute.pro;
make -j4 all;
sudo make install;
