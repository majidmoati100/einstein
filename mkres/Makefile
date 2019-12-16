OPTIMIZE=-O6
CFLAGS=-Wall $(OPTIMIZE)
LNFLAGS=-lz

TARGET=mkres
SOURCES=main.cpp compressor.cpp lexal.cpp format.cpp messages.cpp msgformatter.cpp \
	../buffer.cpp ../convert.cpp ../streams.cpp ../table.cpp ../unicode.cpp
HEADERS=compressor.h lexal.h format.h messages.h msgformatter.h \
	../buffer.h ../convert.h ../streams.h ../table.h ../unicode.h
OBJECTS=main.o compressor.o unicode.o streams.o lexal.o table.o \
	convert.o buffer.o format.o messages.o msgformatter.o

.cpp.o:
	$(CXX) -c $(CFLAGS) $<

all: $(TARGET)

depend:
	@makedepend $(SOURCES) 2> /dev/null

$(TARGET): $(OBJECTS)
	$(CXX) $(LNFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

clean: 
	rm -f $(OBJECTS) $(TARGET) core

# DO NOT DELETE THIS LINE -- make depend depends on it.

