# Raw ethernet client/server kernel modules

This code represents an echo client/server built as a kernel module.

It allows sending and receiving data straight on top of ethernet frames, with configurable callbacks for different ethernet protocol types.

## Compilation

    make

## Running

Clients and server are expected to run on different machines.

Server side, using interface eth0:

    sudo insmod build/kecho if_name=eth0 # the server

Client side, client interface eth0, server mac address in dst_addr, with `nclients` outstanding clients:

    sudo insmod build/kclient if_name=eth0 dst_add=00:00:00:00:00:01 nclients=4

