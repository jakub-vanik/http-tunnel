FROM debian:buster
WORKDIR /root
RUN apt-get update && \
    apt-get -y install git make g++ libpoco-dev
RUN git clone https://github.com/jakub-vanik/http-tunnel.git && \
    cd http-tunnel/Tunnel/Release && \
    make && \
    cp tunnel ../../.. && \
    cd ../../.. && \
    rm -r http-tunnel
ENTRYPOINT ["./tunnel"]
CMD ["-R", "1080"]
