# Toy Virtual Machine

A toy virtual machine for the [tas](https://github.com/g0mb4/tas) project.

![alt text][logo]

[logo]: img/tvm.png "Toy Virtual Machine"

# Usage of tvm

Use "**File** -> **Open**" to load a binary file, then press "**Step**" for stepping or "**Run**" to run the program until it halts.

## Command line usage

You can use the command line intarface to load a file:
```bash
tvm <binary_file>
```

or you can run the program on start:
```bash
tvm --start <binary_file>
```

# Compilation of tvm
The project is built using **Qt5**, so the best way to compile it is using QtCreator.
