O := o
CXX := ape/bin/cosmoc++
CXXFLAGS := -O3 -std=gnu++20 -fshort-enums \
	    -Wall -Wextra -Wno-unused-parameter \
	    -Iinc

ifndef REL
	CXXFLAGS += -g -mdbg
endif

ifdef NAT
	CXX = clang++
else
	CXXFLAGS += -mcosmo -mclang
endif

ifdef ASAN
	CXXFLAGS += -fsanitize=address
endif

# make object files from cc files
LIBO := $(patsubst lib/%.cc,$(O)/%.o,$(wildcard lib/*.cc))
TDBO := $(patsubst 3db/%.cc,$(O)/%.o,$(wildcard 3db/*.cc))

# make binaries from cc files
BINCOM := $(patsubst bin/%.cc,$(O)/%.com,$(wildcard bin/*.cc))
RUNCOM := $(patsubst %.cc,$(O)/%.com,$(wildcard run/*.cc))

all: $(O)/ $(O)/3db.com $(BINCOM)

$(O)/:
	mkdir -p $(O)

$(LIBO): $(O)/%.o: lib/%.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(TDBO): $(O)/%.o: 3db/%.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(BINCOM): $(O)/%.com: bin/%.cc $(LIBO)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

$(RUNCOM): $(O)/run/%.com: run/%.cc $(LIBO)
	mkdir -p $(O)/run/
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

$(O)/3db.com: $(LIBO) $(TDBO)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LNKXXFLAGS)

test: $(O)/ $(LIBO) $(RUNCOM)
	@etc/test.sh

clean:
	rm -rf o
