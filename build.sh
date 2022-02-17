#!/bin/bash

source /usr/local/bin/color.sh

CMAKE_BUILD_DIR="../out"
PROJECT_NAME="NanotubesAnalysis"
BUILD_TYPE="Debug"
POSTFIX="-d"

configure() {
    echo -e "Configuring cmake..."
    cmake -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE -S . -B $CMAKE_BUILD_DIR
}

build() {
    echo -e "Building..."
    cmake --build $CMAKE_BUILD_DIR --config $BUILD_TYPE || (configure && cmake --build $CMAKE_BUILD_DIR --config $BUILD_TYPE)
    cp -R ./res $CMAKE_BUILD_DIR/bin
}

build_and_run() {
    build && run
}

run() {
    echo -e "Running..."
    $CMAKE_BUILD_DIR/bin/$PROJECT_NAME$POSTFIX || echo -e "${RED}Working build wasn't found${NOCOLOR}"
}

clean_all() {
    echo -e "Cleaning..."
    rm -rf $CMAKE_BUILD_DIR/{*,.*} &> /dev/null
    echo -e "${GREEN}All cleaned!${NOCOLOR}"
}

change_build_type() {
    echo -e "Build type -> ${GREEN}${BUILD_TYPE}${NOCOLOR}\n"
    echo -e "Choose build type:\n (${RED}d${NOCOLOR})Debug, (${RED}r${NOCOLOR})Release"
    read -n 1 -s input
    case $input in
        d)
            BUILD_TYPE="Debug"
            POSTFIX="-d"
            ;;
        r)
            BUILD_TYPE="Release"
            POSTFIX=""
            ;;
        *)
            ;;
    esac
}

while true
do
echo -e "\n \
Build type -> ${GREEN}${BUILD_TYPE}${NOCOLOR}\n \
(${RED}1${NOCOLOR}) configure cmake, \
(${RED}2${NOCOLOR}) build, \
(${RED}3${NOCOLOR}) build & run, \
(${RED}4${NOCOLOR}) run, \
(${RED}5${NOCOLOR}) clean all, \
(${GREEN}q${NOCOLOR}) exit \
"
read -n 1 -s input
case $input in
    1)
        configure
        ;;
    2)
        build
        ;;
    3)
        build_and_run
        ;;
    4)
        run
        ;;
    5)
        clean_all
        ;;
    *)
        exit
        ;;
esac
done
