# Start with a base image of ubuntu
FROM ubuntu:22.04

# Add a user that is the same as the host user
ARG USER_ID
ARG GROUP_ID

# Update and upgrade the system
RUN apt update && apt upgrade -y

# Install all the needed dependencies
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Rome apt install -y \
    autoconf \
    automake \
    bison \
    build-essential \
    cmake \
    curl \
    device-tree-compiler \
    doxygen \
    flex \
    git \
    graphicsmagick-libmagick-dev-compat \
    graphviz \
    gtkwave \
    libfdt-dev \
    libftdi-dev \
    libftdi1 \
    libjpeg-dev \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libsndfile1-dev \
    libtool \
    libusb-1.0-0-dev \
    ninja-build \
    pkg-config \
    python3-pip \
    qtbase5-dev \
    libqt5charts5-dev \
    rsync \
    texinfo \
    wget \
    qt6-base-dev \
    libqt6serialport6-dev \
    libgl1-mesa-dev \
    unzip \
    sudo \
    gdb

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

# Pass the current directory to the container
COPY deps/systemc-2.3.3 /systemc

# Change directory to systemc
WORKDIR systemc

# Configure SystemC
# RUN ./configure --prefix=/usr/local/systemc CXXFLAGS="-DSC_CPLUSPLUS=201402L"
RUN chmod +x config/bootstrap
RUN ./config/bootstrap

RUN chmod +x configure
RUN ./configure --prefix=/usr/local/systemc

# Build SystemC 2.3.3
RUN make -j 8

# Check SystemC 2.3.3
RUN make check -j 8

# Install SystemC 2.3.3
RUN make install -j 8

# Change the permissions of the systemc folder
RUN chmod -R 777 /usr/local/systemc

# Change directory to /
WORKDIR /

# Copy the systemc-ams-2.3.4 folder to the container
COPY deps/systemc-ams-2.3.4 /systemc-ams

# Change directory to systemc-ams-2.3.4
WORKDIR systemc-ams

RUN chmod +x ./configure
# # Configure SystemC AMS
RUN ./configure --with-systemc=/usr/local/systemc --prefix=/usr/local/systemc-ams --disable-systemc_compile_check CXXFLAGS="-DSC_CPLUSPLUS=201402L"

# Build SystemC AMS
RUN make -j 8

# Install SystemC AMS
RUN make install -j 8

# Change the permissions of the systemc-ams folder
RUN chmod -R 777 /usr/local/systemc-ams

# Change directory to /
WORKDIR /

# Set the default command to launch the desired shell (bash in this case)
CMD ["/bin/bash"]

# Create a user with the same user id and group id as the host user
RUN groupadd -g 1004 docker_group && useradd -u 1004 -g docker_group docker_user

USER docker_user

WORKDIR /home/docker_user