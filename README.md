# Nouveau

A statically-typed general-purpose compiled programming language.

## Getting Started

Step-by-step instructions to get the compiler up and running.

### Prerequisites

> You must have `cmake` and `g++` installed on your device for any OS.

Linux / MacOS :

-   make

Windows :

-   Visual Studio with the "Desktop Development in C++" extension

### Installation

1. Clone the repository

    ```bash
    git clone https://github.com/novaotp/nouveau
    ```

1. Build the compiler

    ```bash
    cmake -S . -B build -DBUILD_TESTS=ON

    # Linux
    make ./build

    # Windows
    cmake --build build --config Debug
    ```

1. Execute a file

    ```bash
    # Linux
    ./build/nouveau ./path/to/your/file.nv

    # Windows
    ./build/Debug/nouveau.exe ./path/to/your/file.nv
    ```

### Testing

1. Run the test suite

    ```bash
    cd ./build

    # Linux
    ./nouveau_tests

    # Windows
    ./Debug/nouveau_tests.exe
    ```
