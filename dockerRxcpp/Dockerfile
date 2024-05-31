FROM alpine:3.19.1

# git cmake ninja and g++
RUN apk add --no-cache \
    build-base \
    cmake \
    ninja \
    zip \
    unzip \
    curl \
    git

# some env var for vcpkg
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
# clone vcpkg
RUN git clone https://github.com/microsoft/vcpkg /vcpkg
WORKDIR /vcpkg
# install vcpkg
RUN ./bootstrap-vcpkg.sh
# add vcpkg to the PATH
ENV PATH="/vcpkg:${PATH}"

# install RxCpp 64bits
RUN vcpkg install rxcpp:x64-linux

ENTRYPOINT ["/bin/sh"]

# docker build -t mario-rxcpp .