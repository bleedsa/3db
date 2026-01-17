#!/bin/sh

make -j6 all
scp o/3db/3db.com 10.0.0.2:~skye/
scp o/3i/3i.com   10.0.0.2:~skye/
