# Nouveau

A statically-typed general-purpose compiled programming language.

## Getting Started

Step-by-step instructions to get the compiler up and running.

### Prerequisites

> You must have `g++` installed on your device for any OS.

Linux / MacOS :

-   make

### Installation

1. Clone the repository

    ```bash
    git clone https://github.com/novaotp/nouveau
    ```

1. Build the compiler

    ```bash
    cd ./build
    cmake ..

    # Linux
    make

    # Windows
    cmake --build . --config Debug
    ```

1. Execute a file

    ```bash
    ./bin/Debug/nouveau ../path/to/your/file.nv
    ```

### Testing

1. Run the test suite

    ```bash
    make -s test
    ```
