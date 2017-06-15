#!/bin/bash
#
# find all images 

images=$(find /mnt/images -name "*.img" -o -name "*.img0" )
array=($images)
aLen=${#images[@]}

printf "%s \n" ${images[@]}
