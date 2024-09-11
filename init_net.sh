#!/bin/bash

# 建立网桥设备 br0
ip link add name skdocker0 type bridge
# 给网桥设置 ip 地址
ip addr add 172.16.0.1/16 dev skdocker0
# 最后别忘了开启它
ip link set skdocker0 up
# 开启 ipv4 转发
sysctl -w net.ipv4.ip_forward=1