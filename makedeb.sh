#!/bin/bash
cp server/build/server server-1.0/usr/local/zpiserver/server
dpkg-deb --build server-1.0
