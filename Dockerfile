FROM mario-rxcpp:latest

RUN mkdir /myproject
WORKDIR /myproject
COPY . .

# setup config project
RUN cmake -S . -B build -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
# compile project
RUN cmake --build build

ENTRYPOINT ["tail", "-f", "/dev/null"]

# docker build -t ex2rxcpp .