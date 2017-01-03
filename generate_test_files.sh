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

#generate 200 MB file
base64 /dev/urandom | head -c 204800000 > 200M.txt

#generate 300 MB file
base64 /dev/urandom | head -c 307200000 > 300M.txt

#generate 400 MB file
base64 /dev/urandom | head -c 409600000 > 400M.txt
