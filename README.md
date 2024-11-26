# Nouveau

> ⚠️ I realized I worked on way too many features in the parser before even
> having a working compiler.  
> This archive is meant to keep the code I've
> developed for features I'll work on incrementally.

A statically-typed general-purpose compiled programming language.

## Getting Started

Step-by-step instructions to get the compiler up and running.

### Prerequisites

> You must have `cmake` and `g++` installed on your device for any OS.

Linux / MacOS :

- make

Windows :

- Visual Studio with the "Desktop Development in C++" extension

### Installation

1. Clone the repository

   ```bash
   git clone https://github.com/novaotp/nouveau
   ```

1. Build the compiler

   ```bash
   cd ./build
   cmake .. -DBUILD_TESTS=ON

   # Linux
   make

   # Windows
   cmake --build . --config Debug
   ```

1. Execute a file

   ```bash
   # Linux
   ./nouveau ../path/to/your/file.nv

   # Windows
   ./Debug/nouveau ../path/to/your/file.nv
   ```

### Testing

1. Run the test suite

   ```bash
   # Linux
   ./nouveau_tests

   # Windows
   ./Debug/nouveau_tests
   ```
