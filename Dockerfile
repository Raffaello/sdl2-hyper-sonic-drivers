FROM ubuntu:24.04

RUN apt-get -y update && apt-get -y upgrade && apt -y install
RUN apt -y install \
    libgtest-dev \
    libgmock-dev \
    libsdl2-dev \
    cmake \
    git \
    g++
RUN cd /tmp && \
    git clone https://github.com/munt/munt.git && \
    cd munt && cd mt32emu && \
    cmake -DCMAKE_BUILD_TYPE:STRING=Release . && make && make install

RUN mkdir /code && cd /code
WORKDIR /code

# cmake -D${{ matrix.use_mt32 }} -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} ..
# cmake --build . --config=${{ matrix.build_type }} -j4
# ctest --output-on-failure
