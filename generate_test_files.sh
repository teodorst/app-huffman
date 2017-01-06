#!/bin/bash

mkdir tests
cd tests

#generate 10 MB file
echo 'Generating a 10MB random file...'
base64 /dev/urandom | head -c 10240000 > 10M.in

#generate 20 MB file
echo 'Generating a 20MB random file...'
base64 /dev/urandom | head -c 20480000 > 20M.in

#generate 30 MB file
echo 'Generating a 30MB random file...'
base64 /dev/urandom | head -c 30720000 > 30M.in

#generate 40 MB file 
echo 'Generating a 40MB random file...'
base64 /dev/urandom | head -c 40960000 > 40M.in

#generate 50 MB file
echo 'Generating a 50MB random file...'
base64 /dev/urandom | head -c 51200000 > 50M.in

#generate 100 MB file
echo 'Generating a 100MB random file...'
base64 /dev/urandom | head -c 102400000 > 100M.in

#generate 200 MB file
echo 'Generating a 200MB random file...'
base64 /dev/urandom | head -c 204800000 > 200M.in

#generate 300 MB file
echo 'Generating a 300MB random file...'
base64 /dev/urandom | head -c 307200000 > 300M.in

#generate 400 MB file
echo 'Generating a 400MB random file...'
base64 /dev/urandom | head -c 409600000 > 400M.in
