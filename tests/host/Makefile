# Host (Linux/macOS) tests of the recording back end. They need only a C
# compiler: the recorder uses no RISC OS call. Sources here and in c/ and h/
# have no extension, so they are linked into a build directory under the
# names a host compiler wants.
#
#   make -C tests/host        build and run the tests

CC      ?= cc
CFLAGS  ?= -O1 -g
CSTD     = -std=c90 -pedantic -Wall -Wextra

TOP      = ../..
BUILD    = build

.PHONY: test clean
test: $(BUILD)/t_gcrecord
	$(BUILD)/t_gcrecord

$(BUILD)/gcrecord.c: $(TOP)/c/gcrecord
	@mkdir -p $(BUILD)
	@ln -sf $(abspath $<) $@

$(BUILD)/gcontext.h: $(TOP)/h/gcontext
	@mkdir -p $(BUILD)
	@ln -sf $(abspath $<) $@

$(BUILD)/gcrecord.h: $(TOP)/h/gcrecord
	@mkdir -p $(BUILD)
	@ln -sf $(abspath $<) $@

$(BUILD)/t_gcrecord: t_gcrecord.c t_check.c t_check.h $(BUILD)/gcrecord.c $(BUILD)/gcontext.h $(BUILD)/gcrecord.h
	$(CC) $(CFLAGS) $(CSTD) -I$(BUILD) -I. -o $@ t_gcrecord.c t_check.c $(BUILD)/gcrecord.c

clean:
	$(RM) -r $(BUILD)
