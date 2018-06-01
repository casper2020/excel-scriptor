
INCLUDE_DIRS = -I /Users/bruno/work/v8/depot_tools/v8/include

OBJECTS = main_calc.o      \
					v8_shell.o

OBJECTS2 = 	/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/libv8_base.a							      \
						/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/libv8_libbase.a						    \
						/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/libv8_external_snapshot.a	    \
						/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/libv8_libplatform.a				    \
						/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/libv8_libsampler.a				      \
						/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/third_party/icu/libicuuc.a			\
						/Users/bruno/work/v8/depot_tools/v8/out.gn/x64.release/obj/third_party/icu/libicui18n.a

main_calc: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -O2 -Wl, $(OBJECTS2) -Wl, -ldl

YACC=bison
RAGEL=ragel
DEFINES = -D CASPER_NO_ICU
CFLAGS = $(INCLUDE_DIRS) $(DEFINES) -c -g -O2
CXXFLAGS = $(INCLUDE_DIRS) -std=c++11 -Wall $(DEFINES) -c -g -O2

#v8_shell.o:
#	$(CXX) -o $@ -Wl, $(OBJECTS2) -Wl, -ldl -std=c++11

# ragel
%.cc:%.yy
	@echo "* [$(TARGET)] bison  $< ..."
	$(YACC) $< -v --locations -o $@

# ragel
%.cc:%.rl
	@echo "* [$(TARGET)] rl  $< ..."
	$(RAGEL) $(RAGEL_FLAGS) $< -G2 -o $@

# c++
.c.o:
	@echo "* [$(TARGET)] c   $< ..."
	$(C) $(CFLAGS) $< -o $@

# c++
.cc.o:
	@echo "* [$(TARGET)] cc  $< ..."
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm $(OBJECTS) main_calc
