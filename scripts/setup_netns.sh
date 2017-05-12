#!/bin/bash

set -x
PID=$1
SRC_HOST=$3
DST_HOST=$2
INNER_DEV=veth0_$PID

ip link delete $INNER_DEV

iptables -F
iptables -X
iptables -t nat -F
iptables -t nat -X
iptables -t mangle -F
iptables -t mangle -X
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT

mkdir -p /var/run/netns                                                   
ln -s "/proc/$1/ns/net" "/var/run/netns/$PID" 

set -e
ip link add $INNER_DEV type veth peer name eth0
#ip netns add $PID
ip link set eth0 netns $PID
ip netns exec $PID ip link set eth0 up
ip netns exec $PID ip addr add $DST_HOST/24 dev eth0
ip link set $INNER_DEV up && ip addr add $SRC_HOST/24 dev $INNER_DEV
ip netns exec $PID ip route add default via $SRC_HOST
ip netns exec $PID ip link set lo up

#echo 1 > /proc/sys/net/ipv4/ip_forward
#iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE

#sysctl net.ipv4.conf.all.forwarding=1
#iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE
