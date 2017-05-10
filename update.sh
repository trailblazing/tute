#!/bin/bash
if [ "$QT5DIR" = "" ]; then
    echo "run \"export QT5DIR=/path/to/qt5 installation\" first. Eg.: \"export QT5DIR=/home/user_name/working/qt/opt/5.7/gcc_64\" "
    exit 1;
fi;
git pull;
qmake tute.pro;
make -j4 all;
sudo make install;
