#!/bin/bash
cp server/build/server server-1.0/usr/local/zpiserver/server
cp agent_lin/build/client agent_lin-1.0/usr/local/zpiclient/agent_lin
dpkg-deb --build server-1.0
dpkg-deb --build agent_lin-1.0
