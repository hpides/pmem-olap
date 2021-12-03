################################################################################
# AUTOMATICALLY GENERATED FILE -- DO NOT EDIT.
#
# This file is generated automatically by libpqxx's template2mak.py script, and
# will be rewritten from time to time.
#
# If you modify this file, chances are your modifications will be lost.
#
# The template2mak.py script should be available in the tools directory of the
# libpqxx source archive.
#
# Generated from template './win32/MinGW.mak.template'.
################################################################################
# MinGW makefile for libpqxx.  Don't use this; use the configure script instead!
# Based on original contributed by Pasquale Fersini <basquale.fersini@libero.it>

include win32/common

CXX = g++.exe

OBJ = \
  src/array.o \
  src/binarystring.o \
  src/connection.o \
  src/cursor.o \
  src/encodings.o \
  src/errorhandler.o \
  src/except.o \
  src/field.o \
  src/largeobject.o \
  src/notification.o \
  src/pipeline.o \
  src/result.o \
  src/robusttransaction.o \
  src/row.o \
  src/sql_cursor.o \
  src/statement_parameters.o \
  src/strconv.o \
  src/stream_from.o \
  src/stream_to.o \
  src/subtransaction.o \
  src/transaction.o \
  src/transaction_base.o \
  src/util.o \
  src/version.o \


LDFLAGS = -L$(LIBPQPATH) --export-all-symbols --add-stdcall-alias -fpic
LIBS = -lpq -lm -lws2_32

CPPFLAGS = -Iinclude -I$(PGSQLINC) -I$(LIBPQINC) \
  -DBUILDING_DLL -DPQXX_SHARED

BIN = libpqxx.dll

.PHONY: all clean

all: libpqxx.dll

clean:
	rm -f $(OBJ) $(BIN)

DLLWRAP=dllwrap.exe
DEFFILE=libpqxx.def
STATICLIB=libpqxx.a

$(BIN): $(OBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --driver-name c++ --implib $(STATICLIB) $(OBJ) $(LDFLAGS) $(LIBS) -o $(BIN)

src/array.o: src/array.cxx
	$(CXX) $(CPPFLAGS) -c src/array.cxx -o src/array.o $(CXXFLAGS)

src/binarystring.o: src/binarystring.cxx
	$(CXX) $(CPPFLAGS) -c src/binarystring.cxx -o src/binarystring.o $(CXXFLAGS)

src/connection.o: src/connection.cxx
	$(CXX) $(CPPFLAGS) -c src/connection.cxx -o src/connection.o $(CXXFLAGS)

src/cursor.o: src/cursor.cxx
	$(CXX) $(CPPFLAGS) -c src/cursor.cxx -o src/cursor.o $(CXXFLAGS)

src/encodings.o: src/encodings.cxx
	$(CXX) $(CPPFLAGS) -c src/encodings.cxx -o src/encodings.o $(CXXFLAGS)

src/errorhandler.o: src/errorhandler.cxx
	$(CXX) $(CPPFLAGS) -c src/errorhandler.cxx -o src/errorhandler.o $(CXXFLAGS)

src/except.o: src/except.cxx
	$(CXX) $(CPPFLAGS) -c src/except.cxx -o src/except.o $(CXXFLAGS)

src/field.o: src/field.cxx
	$(CXX) $(CPPFLAGS) -c src/field.cxx -o src/field.o $(CXXFLAGS)

src/largeobject.o: src/largeobject.cxx
	$(CXX) $(CPPFLAGS) -c src/largeobject.cxx -o src/largeobject.o $(CXXFLAGS)

src/notification.o: src/notification.cxx
	$(CXX) $(CPPFLAGS) -c src/notification.cxx -o src/notification.o $(CXXFLAGS)

src/pipeline.o: src/pipeline.cxx
	$(CXX) $(CPPFLAGS) -c src/pipeline.cxx -o src/pipeline.o $(CXXFLAGS)

src/result.o: src/result.cxx
	$(CXX) $(CPPFLAGS) -c src/result.cxx -o src/result.o $(CXXFLAGS)

src/robusttransaction.o: src/robusttransaction.cxx
	$(CXX) $(CPPFLAGS) -c src/robusttransaction.cxx -o src/robusttransaction.o $(CXXFLAGS)

src/row.o: src/row.cxx
	$(CXX) $(CPPFLAGS) -c src/row.cxx -o src/row.o $(CXXFLAGS)

src/sql_cursor.o: src/sql_cursor.cxx
	$(CXX) $(CPPFLAGS) -c src/sql_cursor.cxx -o src/sql_cursor.o $(CXXFLAGS)

src/statement_parameters.o: src/statement_parameters.cxx
	$(CXX) $(CPPFLAGS) -c src/statement_parameters.cxx -o src/statement_parameters.o $(CXXFLAGS)

src/strconv.o: src/strconv.cxx
	$(CXX) $(CPPFLAGS) -c src/strconv.cxx -o src/strconv.o $(CXXFLAGS)

src/stream_from.o: src/stream_from.cxx
	$(CXX) $(CPPFLAGS) -c src/stream_from.cxx -o src/stream_from.o $(CXXFLAGS)

src/stream_to.o: src/stream_to.cxx
	$(CXX) $(CPPFLAGS) -c src/stream_to.cxx -o src/stream_to.o $(CXXFLAGS)

src/subtransaction.o: src/subtransaction.cxx
	$(CXX) $(CPPFLAGS) -c src/subtransaction.cxx -o src/subtransaction.o $(CXXFLAGS)

src/transaction.o: src/transaction.cxx
	$(CXX) $(CPPFLAGS) -c src/transaction.cxx -o src/transaction.o $(CXXFLAGS)

src/transaction_base.o: src/transaction_base.cxx
	$(CXX) $(CPPFLAGS) -c src/transaction_base.cxx -o src/transaction_base.o $(CXXFLAGS)

src/util.o: src/util.cxx
	$(CXX) $(CPPFLAGS) -c src/util.cxx -o src/util.o $(CXXFLAGS)

src/version.o: src/version.cxx
	$(CXX) $(CPPFLAGS) -c src/version.cxx -o src/version.o $(CXXFLAGS)


