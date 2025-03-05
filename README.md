# S3K Proof-of-Concept

## Build and Run Project

Here we describe how you can build and run this project on a Linux system.

### Install required tools

For this project, you need the [RISC-V GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain), [QEMU](https://www.qemu.org/), and [Meson](https://mesonbuild.com/).

#### QEMU and Meson

Check your distribution for how to install QEMU (more specifically, `qemu-system-riscv64`), and Meson.

#### RISC-V GNU Toolchain

You will likely have to build the RISC-V GNU Toolchain by yourself as the version of the toolchain found in common distributions are likely incorrect.
For this project, we need the `riscv64-unknown-elf-gcc` compiler targeting the `rv64imac` version of RISC-V.

##### Docker container

We have a docker container [hakarlsson/riscv-picolibc](https://hub.docker.com/repository/docker/hakarlsson/riscv-picolibc/general) with the RISC-V GNU Toolchain, and Meson.

**TODO: How to use the container.**

##### Build from source

**Prerequisites for the toolchain:**
To install the toolchain, first install its [prerequisites](https://github.com/riscv-collab/riscv-gnu-toolchain?tab=readme-ov-file#prerequisites).
If you want to run GDB in TUI mode, we recommend that you also install the ncurses library.

**Build and Install toolchain:**
To install the toolchain do as follows:
```
# Clone the toolchain with git
git clone https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain
# Install toolchain with a minimal 64-bit RISC-V for bare-metal systems.
./configure --prefix=$INSTALL_LOCATION --with-arch=rv64imac --with-abi=lp64 --with-cmodel=medany
# Alternatively, 
./configure --prefix=$INSTALL_LOCATION --enable-multilib --with-cmodel=medany
# Build and install the toolchain to $INSTALL_LOCATION
make
```

We recommend that you set the install location (`$INSTALL_LOCATION`) to a directory in your `$HOME`, such as `$HOME/tools/riscv`.
Otherwise, you may have to run the above `make` command using `sudo`.

**Add toolchain to your $PATH variable:**
To ensure that your toolchain is available in your shell, add `$INSTALL_LOCATION/bin` to your `$PATH`:
```
echo "export PATH='$INSTALL_LOCATION/bin:$PATH'" >> ~/.bashrc
source ~/.bashrc
```

### Build the Project

Once you have built and installed the required tools, you can build the project by:
```
meson setup my_build --cross-file cross/rv64imac.toml
meson compile -C my_build
```

### Run the Project

Run the project as follows:
```
./my_build/qemu.sh
```
