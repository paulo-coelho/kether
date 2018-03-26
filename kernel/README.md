# Raw ethernet client/server kernel modules

This code represents a echo client/server built as a kernel module.

It sends and receives data straight on top of ethernet frames, with configurable callbacks for different ethernet protocol types.

## Compilation

    make

## Running

Clients and server are expected to run on different machines.

Server side, using interface eth0:

    sudo insmod build/kecho if_name=eth0 # the server

Client side, client interface eth0, server mac address in dst_addr:

    sudo insmod build/kclient if_name=eth0 dst_add=00:00:00:00:00:01

