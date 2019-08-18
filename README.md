# http-tunnel
*http-tunnel* is client and server application which allows tunneling any TCP connection trought HTTP requests and responses. It does not use WebSocket so it can pass even restrictive HTTP proxies. Each packet is according to direction encoded as address in GET request or as one line of plain text response. It can be used in combination with SHH server with port forwarding enabled or with SOCKS proxy server to relay internet connectivity to restricted environment.

## Features
- tries to establish keep alive connection and thus reduce latency
- encodes all packets using base64 to pretend plain text content
- encrypts all packets using AES
- is able to multiplex many TCP connections in one HTTP tunnel
- dynamically changes request rate according to activity in all connections

## Drawbacks
- server is not able to serve more clients at the same time (however it is possible to run multiple instances on different base addresses using Nginx)
- does not handle packet loss in any way (so TCP connection may get corrupted and it depend on application layer whether it can detect such situation)

## Compilation
*http-tunnel* requires [POCO](https://pocoproject.org/) libraries. On Debian **libpoco-dev** package is required for compilation. Run **make** in **Tunnel/Release** directory to compile.

## Usage
Both client and server can be run using same executable by supplying appropriate arguments. Command line arguments are following:
- **-P**, **--proxy=PORT** starts *http-tunnel* in client mode, in this mode TCP connections are accepted at selected port and tunneled by outgoing HTTP connection
- **-R**, **--relay=PORT** starts *http-tunnel* in server mode, in this mode HTTP server is started and tunneled TCP connections are relayed to selected port at localhost
- **-t**, **--timeout=SECONDS** sets time in seconds after which is an inactive connection closed, default is 300 seconds
- **-s**, **--password=STRING** sets the key for AES encryption, any string of any length can be used, it is highly recommended to set it, default value is yK!xhk+S*59fnJua
- **-h**, **--host=HOSTNAME** sets hostname of the relay server in client mode, is not used in server mode
- **-p**, **--port=PORT** sets HTTP server port in both modes, default is 80
- **-b**, **--base=ADDRESS** sets base URL at which tunnel is mapped in both modes, defaults to /
- **-f**, **--fake=HOSTNAME** sets hostname to use in host field of HTTP request, defaults to real hostname
- **-v**, **--verbosity=LEVEL** sets logger verbosity, 0 disables logging, 8 is the most verbose, default is 0

## Docker
*http-tunnel* can be run in Docker as image **jvanik/http-tunnel**. Use same arguments as if running without Docker.
