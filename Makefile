VERSION = 1
PATCHLEVEL = 0
EXTRAVERSION = rc2

LIBRARY	= multitouch.so
MODULES = src
XMODULES = driver

o_src	= capabilities hwstate mtstate gestures mconfig mtouch trig

o_driver= multitouch

TARGETS	+= src/test

OBJECTS	= $(addsuffix .o,\
	$(foreach mod,$(MODULES),\
	$(addprefix $(mod)/,$(o_$(mod)))))
XOBJECTS= $(addsuffix .o,\
	$(foreach mod,$(XMODULES),\
	$(addprefix $(mod)/,$(o_$(mod)))))

TBIN	= $(addprefix bin/,$(TARGETS))
TLIB	= $(addprefix obj/,$(LIBRARY))
TOBJ	= $(addprefix obj/,$(addsuffix .o,$(TARGETS)))
OBJS	= $(addprefix obj/,$(OBJECTS))
XOBJS	= $(addprefix obj/,$(XOBJECTS))
LIBS	= -lmtdev -lm

DLIB	= usr/lib/xorg/modules/input

INCLUDE = -Iinclude -I/usr/include/xorg -I/usr/include/pixman-1
OPTS	= $(CFLAGS) -O3 -fPIC

.PHONY: all clean
.PRECIOUS: obj/%.o

all:	$(OBJS) $(TLIB) $(TOBJ) $(TBIN)

bin/%:	obj/%.o $(OBJS)
	@mkdir -p $(@D)
	gcc $< -o $@ $(OBJS) $(LIBS)

$(TLIB): $(OBJS) $(XOBJS)
	@rm -f $(TLIB)
	gcc -shared $(OBJS) $(XOBJS) $(LIBS) -Wl,-soname -Wl,$(LIBRARY) -o $@

obj/%.o: %.c
	@mkdir -p $(@D)
	gcc $(INCLUDE) $(OPTS) -c $< -o $@

obj/%.o: %.cc
	@mkdir -p $(@D)
	gcc $(INCLUDE) $(OPTS) -c $< -o $@

clean:
	rm -rf bin obj

distclean: clean
	rm -rf debian/*.log debian/files debian/xf86-input-multitouch*

install: $(TLIB)
	install -d "$(DESTDIR)/$(DLIB)"
	install -m 755 $(TLIB) "$(DESTDIR)/$(DLIB)"
