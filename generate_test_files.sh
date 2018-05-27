#!/bin/bash

#generate 10 MB file
base64 /dev/urandom | head -c 10240000 > 10M.txt

#generate 20 MB file
base64 /dev/urandom | head -c 20480000 > 20M.txt

#generate 30 MB file
base64 /dev/urandom | head -c 30720000 > 30M.txt

#generate 40 MB file 
base64 /dev/urandom | head -c 40960000 > 40M.txt

#generate 50 MB file
base64 /dev/urandom | head -c 51200000 > 50M.txt

#generate 100 MB file
base64 /dev/urandom | head -c 102400000 > 100M.txt

#generate 500 MB file
base64 /dev/urandom | head -c 512000000 > 500M.txt

#generate 1 GB file
base64 /dev/urandom | head -c 1024000000 > 1GB.txt

#generate 2 GB file
base64 /dev/urandom | head -c 2048000000 > 2GB.txt

#generate 4 GB file
base64 /dev/urandom | head -c 4096000000 > 4GB.txt
