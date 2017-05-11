#!/bin/bash

NS_NAME=$1
SRC_HOST=$2
DST_HOST=$3

ip link delete veth0
ip netns delete $NS_NAME

ip link add veth0 type veth peer name eth0
ip netns add $NS_NAME
ip link set eth0 netns $NS_NAME
ip netns exec $NS_NAME ip link set eth0 up
ip netns exec $NS_NAME ip addr add $DST_HOST/24 dev eth0
ip link set veth0 up && ip addr add $SRC_HOST/24 dev veth0
ip netns exec $NS_NAME ip route add default via $SRC_HOST