
INCLUDE_DIRS = -I . -I ./osal               \
               -I . -I ./casper/js_compiler \
							 -I . -I ../jsoncpp/include   \
							 -I . -I ../lemon             \
							 -I . -I ../lemon/build       \
							 -I . -I ./casper/see

LIB= ~/work/jsoncpp/build/src/lib_json/libjsoncpp.a \
     ~/work/lemon/build/lemon/libemon.a

BISON_OBJECTS= ./casper/js_compiler/js_parser.o \
               ./casper/see/parser.o

RAGEL_OBJECTS= ./casper/js_compiler/js_scanner.o \
               ./casper/see/see_scanner.o

INTERM = casper/js_compiler/js_parser.hh         \
         casper/js_compiler/js_parser.cc         \
				 casper/js_compiler/stack.hh             \
				 casper/js_compiler/location.hh          \
				 casper/js_compiler/position.hh          \
				 casper/js_compiler/js_parser.output     \
				 casper/js_compiler/js_scanner.cc        \
				 casper/see/parser.hh                    \
				 casper/see/parser.cc                    \
				 casper/see/stack.hh                     \
				 casper/see/location.hh                  \
				 casper/see/position.hh                  \
				 casper/see/parser.output                \
				 casper/see/see_scanner.cc               \
				 excelscriptor

OBJECTS = casper/js_compiler/js_parser.o         \
					casper/see/parser.o                    \
				  casper/scanner.o                       \
					casper/term.o                          \
					casper/number_parser.o                 \
					casper/abstract_data_source.o          \
					casper/see/see_scanner.o               \
					casper/js_compiler/js_scanner.o        \
					casper/js_compiler/interpreter.o       \
					casper/js_compiler/ast_node.o          \
					casper/see/see.o											 \
					excelscriptor.o 						           \
					osal/osal_date.o                       \
					osal/posix/posix_time.o                \
					osal/posix/posix_file.o                \
					osal/base_file.o                       \
					osal/exception.o                       \
					osal/utils/pow10.o                     \
					casper/js_compiler/ast.o               \
					casper/see/row_shifter.o               \
					casper/see/formula.o                   \
					casper/see/table.o                     \
					casper/see/sum_if.o                    \
					casper/see/sum_ifs.o                   \
					casper/see/vlookup.o                   \
					casper/see/sum.o                       \
					casper/see/line_code_parser.o

PLATFORM:=$(shell uname -s)
ifeq (Darwin, $(PLATFORM))
  YACC=/usr/local/Cellar/bison/3.0.4_1/bin/bison
else
  YACC=bison
endif

excelscriptor: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) -Wl, $(LIB) -Wl,


RAGEL=ragel
DEFINES = -D CASPER_NO_ICU
CFLAGS = $(INCLUDE_DIRS) $(DEFINES) -c -g -O2
CXXFLAGS = $(INCLUDE_DIRS) -std=c++11 -O2 -Wall $(DEFINES) -c -g

# bison
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
	rm -f $(OBJECTS) $(INTERM)

ragel: $(RAGEL_OBJECTS)
	@echo "* RAGEL done"

bison: $(BISON_OBJECTS)
	@echo "* BISON done"
