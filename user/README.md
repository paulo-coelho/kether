# User-space raw ethernet client/server

Provides an echo server (`echo.c`) and client (`mclient.c`) along with a basic API to send/receive data on top of ethernet frames (`eth.{c,h}`).

## Building and running

    make
    ./build/echo -h      # usage description
    ./build/mclient -h   # usage description
