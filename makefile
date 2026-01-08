O := o
CXX := ape/bin/cosmoc++
CXXFLAGS := -std=gnu++23 -fshort-enums \
	    -Wall -Wextra -Wno-unused-parameter \
	    -Iinc

ifndef REL
	CXXFLAGS += -g -mdbg -DDBG
else
	CXXFLAGS += -O3 -g -fsanitize=undefined
endif

ifdef NAT
	CXX = g++
else
	CXXFLAGS += -mcosmo -mclang
endif

ifdef ASAN
	CXXFLAGS += -fsanitize=address
endif

# make object files from cc files
LIBO := $(patsubst lib/%.cc,$(O)/%.o,$(wildcard lib/*.cc))
TDBO := $(patsubst %.cc,$(O)/%.o,$(wildcard 3db/*.cc))
TIO := $(patsubst %.cc,$(O)/%.o,$(wildcard 3i/*.cc))

# make binaries from cc files
BINCOM := $(patsubst %.cc,$(O)/%.com,$(wildcard bin/*.cc))
RUNCOM := $(patsubst %.cc,$(O)/%.com,$(wildcard run/*.cc))

all: $(O)/ $(O)/3db/3db.com $(O)/3i/3i.com $(BINCOM)

$(O)/:
	mkdir -p $(O)

$(LIBO): $(O)/%.o: lib/%.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(TDBO): $(O)/3db/%.o: 3db/%.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(TIO): $(O)/3i/%.o: 3i/%.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(BINCOM): $(O)/bin/%.com: bin/%.cc $(LIBO)
	mkdir -p $(O)/bin/
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

$(RUNCOM): $(O)/run/%.com: run/%.cc $(LIBO)
	mkdir -p $(O)/run/
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

$(O)/3db/3db.com: $(LIBO) $(TDBO)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

$(O)/3i/3i.com: $(LIBO) $(TIO)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

test: $(O)/ $(LIBO) $(RUNCOM)
	@etc/test.sh

clean:
	rm -rf o
