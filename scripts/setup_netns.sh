#!/bin/bash

set -x
NS_NAME=$1
SRC_HOST=$3
DST_HOST=$2

ip link delete veth0
ip netns delete $NS_NAME

iptables -F
iptables -X
iptables -t nat -F
iptables -t nat -X
iptables -t mangle -F
iptables -t mangle -X
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT

set -e
ip link add veth0 type veth peer name eth0
ip netns add $NS_NAME
ip link set eth0 netns $NS_NAME
ip netns exec $NS_NAME ip link set eth0 up
ip netns exec $NS_NAME ip addr add $DST_HOST/24 dev eth0
ip link set veth0 up && ip addr add $SRC_HOST/24 dev veth0
ip netns exec $NS_NAME ip route add default via $SRC_HOST
ip netns exec $NS_NAME ip link set lo up

echo 1 > /proc/sys/net/ipv4/ip_forward
#iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE

#sysctl net.ipv4.conf.all.forwarding=1
#iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE
