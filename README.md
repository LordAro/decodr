# decodr

Something that attempts to decode ELF binaries.

Currently largely just a reimplementation of `readelf`, but with hopes to become a decompiler.

## Usage

    decodr file

* `file` - ELF binary file to decode

## Compilation

    make

No other dependencies other than a C++14 capable compiler.

There are a few test programs provided in the test/ dir. These can be compiled with `make tests`.
