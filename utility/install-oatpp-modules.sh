#!/bin/bash

rm -rf tmp

mkdir tmp
cd tmp

##########################################################
## install oatpp
function install-modules () {
MODULE_NAME=$1

git clone --depth=1 https://github.com/oatpp/$MODULE_NAME

cd $MODULE_NAME
mkdir build
cd build

cmake ..
make
make install

cd ../../
}

install-modules oatpp
install-modules oatpp-swagger

##########################################################

cd ../

# rm -rf tmp
