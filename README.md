# hangman-game
## Setup

Choose a directory to hold locally installed packages.
``````
$ export MY_INSTALL_DIR=$HOME/.local

$ mkdir -p $MY_INSTALL_DIR

$ export PATH="$MY_INSTALL_DIR/bin:$PATH"
``````

Build and install gRPC and Protocol Buffers

``````
$ cd build
$ cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ..
$ make -j 4
$ make install
``````



