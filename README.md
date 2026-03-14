# Graphic context library for RISC OS

This repository holds a library to abstract the graphics context
for use inside RISC OS components.
It is intended for use with the RISC OS 64 project (which aims to bring RISC OS
towards a 64-bit system).

The graphics context provides an abstracted interface for graphics operations,
so that the RISC OS components do not have to worry about the direct access to
the graphics primitives and can instead target an interface that allows them
to be re-targetted more easily.

The idea is that if the interface changes in RISC OS, or the interfaces need to
be tested on other systems, merely matching the interface will be all that is
required.

The library is part of the RISC OS 64 project, and is currently used within
the WindowManager components. Historically, this library has been used in many
private projects, a [slide presentation system](https://github.com/gerph/riscos-presenter).
