FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /tmp/

RUN apt-get update && apt-get install -y \
    ninja-build \ 
    cmake \
    git \
    wget \
    arduino \
    gcc-avr \
    avrdude \
    avr-libc \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install scrutinydebugger==0.14.1
# For dev, we can work with a specific branch
#RUN git clone https://github.com/scrutinydebugger/scrutiny-main -b v0.11.0 && pip3 install -e scrutiny-main