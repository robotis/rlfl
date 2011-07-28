#
#	RLFL Makefile.
#
#	<jtm@robot.is>
#
CFLAGS=$(FLAGS) -I$(SRCDIR) -Wall -fno-strict-aliasing -pipe
OFLAGS=-O3 -fPIC -funroll-loops -DDEBUG=0
DFLAGS=-g -fPIC -DDEBUG=1
PFLAGS=-I/usr/include/python3.2
#
#	NO EDIT ---->
#
SRCDIR=src
INCDIR=include
TEMP=/tmp/rlf
LIBN=rlfl.so
PYMN=rlfl.so
CC=gcc
.SUFFIXES: a .o .h .c

$(TEMP)/rlfo/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(OFLAGS) -s -o $@ -c $<
	
$(TEMP)/rlfpo/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(OFLAGS) $(PFLAGS) -s -o $@ -c $<
	
LIBOBJS_COMMON= \
	$(TEMP)/rlfo/random.o \
	$(TEMP)/rlfo/list_t.o \
	$(TEMP)/rlfo/rlfl.o \
	$(TEMP)/rlfo/los.o \
	$(TEMP)/rlfo/dijkstra.o \
	$(TEMP)/rlfo/path_astar.o \
	$(TEMP)/rlfo/path_basic.o \
	$(TEMP)/rlfo/project.o \
	$(TEMP)/rlfo/fov_circular_raycasting.o \
	$(TEMP)/rlfo/fov_recursive_shadowcasting.o \
	$(TEMP)/rlfo/fov_diamond_raycasting.o \
	$(TEMP)/rlfo/fov_permissive.o \
	$(TEMP)/rlfo/fov_restrictive.o \
	$(TEMP)/rlfo/fov_digital.o
	
LIBOBJS_PYTHON= \
	$(LIBOBJS_COMMON) \
	$(TEMP)/rlfpo/rlftopy.o
	
# shared lib
rlfl : $(TEMP)/rlfo $(LIBOBJS_COMMON) 
	gcc -shared -o $(LIBN) \
	$(LIBOBJS_COMMON) $(CFLAGS)
	
# shared lib (debug)
rlfl-debug : $(TEMP)/rlfo $(LIBOBJS_COMMON)
	gcc -shared -o $(LIBN) \
	$(LIBOBJS_COMMON) $(CFLAGS)
	
# python module	
rlfl-python : $(TEMP)/rlfo $(LIBOBJS_COMMON) $(TEMP)/rlfpo $(LIBOBJS_PYTHON)
	gcc -shared -o $(PYMN) \
	$(LIBOBJS_PYTHON) $(CFLAGS) $(PFLAGS)
	
# python module	(debug)	
rlfl-python-debug : $(TEMP)/rlfo $(LIBOBJS_COMMON) $(TEMP)/rlfpo $(LIBOBJS_PYTHON)
	gcc -shared -o $(PYMN) \
	$(LIBOBJS_PYTHON) $(CFLAGS) $(PFLAGS)
	
$(TEMP)/rlfo :
	mkdir -p $@
	
$(TEMP)/rlfpo :
	mkdir -p $@
		
clean : 
	\rm -rf $(TEMP)/rlfo/* $(TEMP)/rlfpo/* *.so *.pyc



