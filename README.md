# Linux Tools #
This is a collection of tools that I am collecting that are useful for making a Linux system do _something_.
Many of these tools are intended to perform "dirty tricks" with a subsystem to cause something to happen.

For instance, the `bin/sendRawEth` binary will send **raw** Ethernet packets to a destination MAC.

# Adding a Program #
To add a program to this repository, you have to:
  1. Add the source file(s) to the `src/` directory.
  2. Add a sub-Makefile with the same name as the main `.c` program with the extension `.mk` instead.
  See the `srcs/` directory for an example on how this should look.
  3. Add the files that handle that particular program to the `SRCS` Makefile variable inside the program's sub-Makefile.
  4. Hopefully everything just works for you after that...
  If not, reach out to me.
