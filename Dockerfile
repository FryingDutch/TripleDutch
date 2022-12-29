# base image
#!/bin/bash

FROM ubuntu:latest AS base

# install cmake, gcc, g++, boost, and git
FROM base AS builder
RUN apt-get update &&\
    apt-get install -yq cmake gcc g++ &&\
    apt-get install -yq libcurl4-openssl-dev &&\
    apt-get install -yq libmysqlcppconn-dev &&\
    apt-get install -yq libboost-all-dev &&\
    apt-get install -yq libssl-dev &&\
    apt-get install -yq git &&\
    mkdir TripleDutch
WORKDIR /TripleDutch

# get crow's include/ dir
RUN git clone --branch v0.3 https://github.com/CrowCpp/crow crow &&\
    mv crow/include/ include/ &&\
# make a directory we'll use to build
    mkdir build

# copy all of the source files to the image
WORKDIR /TripleDutch
COPY ./ ./

#build
WORKDIR /TripleDutch/build
# ENTRYPOINT ["/bin/bash"]
RUN cmake .. &&\
    make

# FROM base AS finalimage
# COPY --from=builder /TripleDutch/build/src/tdserver /
# COPY --from=builder /usr/lib/aarch64-linux-gnu /usr/lib/aarch64-linux-gnu
ENTRYPOINT ["/TripleDutch/build/src/tdserver"]