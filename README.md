# TCC
A multi-level compiler stack to compile TensorFlow frozen graphs into optimized binaries.

# Quick Start
* Install `Protobuf` (tested with `protobuf-3.7.1`) by following offical instructions [here](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md).
* Clone submodules.

    ```
    git submodule update --init --recursive
    ```

* Compile `TCC` from source.

    ```
    mkdir build
    cd build
    cmake .. && make
    ```

* Run `TCC` compiler.

    ```
    ./apps/tcc ./PATH/TO/FROZEN/GRAPH
    ```
