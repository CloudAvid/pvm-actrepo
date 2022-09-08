# PVM Action Repository Library

- [PVM Action Repository Library](#pvm-action-repository-library)
  - [Introduction](#introduction)
  - [Installation](#installation)
  - [Documentation](#documentation)
  - [Contribution and Bug reports](#contribution-and-bug-reports)
  - [About](#about)

## Introduction

**PVM-ACTREPO** is PVM core communication system in clustering environments.

## Installation

Installation of `PVM-ACTREPO` is based on `Autoconf` and `Autotools` build system.
> Therefore, make sure they are installed properly before you proceed.

1. First step is to clone the project in desired directory:

    ```bash
    # Your favorite directory
    $ git clone https://github.com/CloudAvid/pvm-actrepo.git
    ```

2. Then for `Makefiles` creation issue these commands at cloned directory.

    ```bash
    # Creates Configuration files.
    $ ./autogen.sh

    # Creates Makefiles for building project.
    $ ./configure
    ```

    > You'd be prompted with **unmet requirements** which **must be** fulfilled before proceeding.

3. After successful `Makefile` creation; let us proceed to `PVM-ACTREPO` compilation and installation.
      - At this point (specificity *installation* part) you need `superuser` privilege.

    ```bash
    # Compile the project.
    $ make
    # If you like, we could install it as well
    $ make install
    ```

> Use pkg-config to discover the necessary include and linker arguments. Issue this:

```bash
# Displays pvm-ipc necessary liker and compile flags.
pkg-config pvm-ipc --cflags –libs
```

## Documentation

Read the project [wiki](https://github.com/CloudAvid/pvm-actrepo/wiki).

## Contribution and Bug reports

Please check [contribution](CONTRIBUTING.md) standard for full explanation.

## About

This project brought you by [CloudAvid](https://www.cloudavid.com) developer team.
