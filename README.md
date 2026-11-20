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

## Recording back end

`gcontext_initrecord()` makes a context that records every operation it is
given, rather than plotting it, so that code which draws can be tested without
a screen. The records (`gcrecord.h`) say what was called, with which arguments,
in what order and under which clip rectangle, and can be dumped as text. Fonts
have metrics the test sets, so text measures predictably. The back end uses no
RISC OS call: it is tested on a host as well (`make -C tests/host`, which needs
only a C compiler) and on RISC OS (`MakefileTestRecord,fe1` builds `test_record`).
