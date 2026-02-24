#!/bin/bash

xmake
xmake project -k compile_commands
./script/deploy.sh
./script/ssh-auto.sh
