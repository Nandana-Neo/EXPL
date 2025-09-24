#!/usr/bin/bash
# input : enter number of the stage : x
# files in the folder input_files/STAGEx will be tested
# Tests if the compilation will return any error
./build.sh
set -e
set -o pipefail
for file in ../input_files/STAGE$1/*; do
    echo "Testing: ${file#*/STAGE*/}"
    ./compile.sh -i ${file#../}
    echo "DONE: ${file#*/STAGE*/}"
done
