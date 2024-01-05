CPU := $(shell uname -p)
ifeq ($(CPU),arm)
export LD_LIBRARY_PATH=/opt/homebrew/lib/:$LD_LIBRARY_PATH
INCD = -I /opt/homebrew/include/
LIBD = -L /opt/homebrew/lib/
endif

CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD += -I include

ALL_SRCF := $(shell find $(SRCD) -type f -name '*.c')
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
TEST_SRC := $(shell find $(TSTD) -type f -name '*.c')
TEST_OBJ := $(patsubst $(TSTD)/%,$(BLDD)/%,$(TEST_SRC:.c=.o))

TEST := unit_tests
EXEC := hw5

CFLAGS := -Wall -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wundef -pedantic
DFLAGS := -g -DDEBUG
PRINT_STATEMENTS := -DERROR -DSUCCESS -DWARN -DINFO

STD := -std=gnu11
TEST_LIB := -lcriterion
LIBS := -lm

CFLAGS += $(STD)
CFLAGS += $(DFLAGS)

TEST_RESULTS := "test_results.json"

MAKEFLAGS := -j

all: setup $(BIND)/$(TEST) $(BIND)/$(EXEC) 

debug: CFLAGS += $(DFLAGS) $(PRINT_STATEMENTS) 
debug: all

setup: 
	@mkdir -p $(BIND)
	@mkdir -p $(BLDD)
	@mkdir -p $(TSTD).in
	@mkdir -p $(TSTD).out
	
$(BIND)/$(TEST): $(ALL_OBJF) $(TEST_OBJ)
	$(CC) $(FUNC_FILES) $(TEST_OBJ) $(INCD) $(TEST_LIB) $(LIBD) -o $@ $(LIBS)

$(BLDD)/%.o: $(TSTD)/%.c
	$(CC) $(CFLAGS) $(INCD) -c -o $@ $<

$(BLDD)/%.o: $(SRCD)/%.c 
	$(CC) $(CFLAGS) $(INCD) -c -o $@ $<

$(BIND)/$(EXEC): $(ALL_OBJF)
	$(CC) $(BLDD)/$(EXEC).o -o $@ $(LIBS)

test: 
	@rm -fr $(TSTD).in
	@mkdir -p $(TSTD).in
	@rm -fr $(TSTD).out
	@mkdir -p $(TSTD).out
	@$(BIND)/$(TEST) --full-stats --verbose --json=$(TEST_RESULTS) -j1

clean:
	rm -fr $(BLDD) $(BIND) $(TSTD).in $(TSTD).out *.out $(TEST_RESULTS)

.PHONY: all clean debug criterion setup test update_tests
