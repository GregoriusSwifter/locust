ROOT = $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

LOCUST_DIR = $(ROOT)liblocust

BUILD_DIR = $(ROOT)build
TEST_DIR = $(LOCUST_DIR)/tests
STATS_DIR = $(ROOT)stats

INC_DIRS = -I$(LOCUST_DIR)/include/

LIB_C_MODE = -std=c11 -pedantic -D__TARGET_OS_FREEBSD -D__TARGET_ARCH_X86_64
LIB_OPT_MODE = -Oz -nostdlib -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fmerge-all-constants -fno-ident -ffast-math -nostdlib -fno-unroll-loops
LIB_DBG_MODE = -O0 -nostdlib -g3 -DDEBUG
LIB_C_FLAGS = $(LIB_OPT_MODE) -fPIC -fomit-frame-pointer -c $(LIB_C_MODE) $(LIB_C_WARNMODE) -static $(INC_DIRS)
LIB_CDBG_FLAGS = $(LIB_DBG_MODE) -fPIC -fomit-frame-pointer -c $(LIB_C_MODE) $(LIB_C_WARNMODE) -static $(INC_DIRS)
LIB_C_FILES = $(shell find $(LOCUST_DIR)/src/ -iname '*.c')
LIB_C_OBJ = $(subst $(ROOT), $(BUILD_DIR)/, $(LIB_C_FILES:.c=.c.o))
LIB_CDBG_OBJ = $(subst $(ROOT), $(BUILD_DIR)/, $(LIB_C_FILES:.c=.c_dbg.o))

CRTBEGIN_OBJ = $(shell $(CC) $(TEST_C_FLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(TEST_C_FLAGS) -print-file-name=crtend.o)

LIB_ASM_FLAGS = -Oz -nostdlib -c -integrated-as
LIB_ASM_FILES = $(shell find $(LOCUST_DIR)/src/ -iname '*.s')
LIB_ASM_OBJ = $(subst $(ROOT), $(BUILD_DIR)/, $(LIB_ASM_FILES:.s=.s.o))

LIB_AR = ar
LIB_AR_FLAGS = -r -s

LIB_OPT_TARGET = liblocust.a
LIB_DBG_TARGET = liblocust_dbg.a

TEST_C_MODE = -std=c11 -D__TARGET_OS_FREEBSD -D__TARGET_ARCH_X86_64
TEST_C_WARNMODE = -W -Wall -Wextra -Winline -Wno-unused-function -Wno-unused-variable -Wno-unused-label -Wno-unused-parameter -Wno-unused-value -ferror-limit=5 -Wtype-limits -Wstrict-overflow=5 -fstrict-overflow -Wsign-compare
TEST_OPT_MODE = -Oz -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fmerge-all-constants -fno-ident -ffast-math -fno-unroll-loops
TEST_DBG_MODE = -O0 -g3 -DDEBUG
TEST_C_FLAGS = $(TEST_OPT_MODE) -fPIC -fomit-frame-pointer $(TEST_C_MODE) $(TEST_C_WARNMODE) $(INC_DIRS) -L$(BUILD_DIR) -static -llocust -pthread
TEST_CDBG_FLAGS = $(TEST_DBG_MODE) -fPIC -fomit-frame-pointer $(TEST_C_MODE) $(TEST_C_WARNMODE) $(INC_DIRS) -L$(BUILD_DIR) -static -llocust_dbg -pthread
TEST_C_FILES = $(shell find $(TEST_DIR) -iname '*.test.c')
TEST_C_OBJ = $(subst $(TEST_DIR)/, $(BUILD_DIR)/test/, $(TEST_C_FILES:.test.c=.test.exe))
TEST_CDBG_OBJ = $(subst $(TEST_DIR)/, $(BUILD_DIR)/test/, $(TEST_C_FILES:.test.c=.test.exe))

all: lib lib_debug
.PHONY : all

.PHONY : clean
clean:
	rm -rf $(BUILD_DIR)/*
	mkdir -p $(dir $(LIB_C_OBJ))
	mkdir -p $(dir $(LIB_CDBG_OBJ))
	mkdir -p $(dir $(LIB_ASM_OBJ))
	mkdir -p $(dir $(TEST_C_OBJ))

$(BUILD_DIR)/%.c_dbg.o: $(ROOT)%.c
	$(CC) $(LIB_CDBG_FLAGS) -o $@ $<

$(BUILD_DIR)/%.c.o: $(ROOT)%.c
	$(CC) $(LIB_C_FLAGS) -o $@ $<

$(BUILD_DIR)/%.s.o: $(ROOT)%.s
	$(CC) $(LIB_ASM_FLAGS) -o $@ $<

.PHONY : strip
strip:
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.comment --remove-section=.eh_frame --remove-section=.eh_frame_hdr
	sstrip -z

.PHONY : lib
lib: $(LIB_ASM_OBJ) $(LIB_C_OBJ)
	$(LIB_AR) $(LIB_AR_FLAGS) $(BUILD_DIR)/$(LIB_OPT_TARGET) $(LIB_ASM_OBJ) $(LIB_C_OBJ)

.PHONY : lib_debug
lib_debug: $(LIB_ASM_OBJ) $(LIB_CDBG_OBJ)
	$(LIB_AR) $(LIB_AR_FLAGS) $(BUILD_DIR)/$(LIB_DBG_TARGET) $(LIB_ASM_OBJ) $(LIB_CDBG_OBJ)

$(BUILD_DIR)/test/%.test.exe: $(TEST_DIR)/%.test.c
	$(CC) -o $@ $< $(TEST_C_FLAGS)

$(BUILD_DIR)/test/%.test_dbg.exe: $(TEST_DIR)/%.test.c
	$(CC) -o $@ $< $(TEST_CDBG_FLAGS)


.PHONY : test
test: $(TEST_C_OBJ)
	for x in $(TEST_C_OBJ); do $$x; done

install: all
	echo true

.PHONY : memcheck
memcheck:
	valgrind --read-var-info=yes --leak-check=yes --leak-check=full --show-leak-kinds=all --track-origins=yes > $(STATS_DIR)/stats.memcheck.txt 2>&1

.PHONY : helgrind
helgrind:
	valgrind --tool=helgrind --read-var-info=yes > $(STATS_DIR)/stats.helgrind.txt 2>&1

.PHONY : drd
drd:
	valgrind --tool=drd --read-var-info=yes > $(STATS_DIR)/stats.drd.txt 2>&1

.PHONE : ebnf
ebnf:
	python3.2 -m compiler.parser > $(STATS_DIR)/stats.ebnf.txt 2>&1

.PHONE : cloc
cloc:
	cloc --exclude-dir=build --exclude-dir=.git --exclude-dir=tests ./liblocust/ > $(STATS_DIR)/stats.cloc.txt 2>&1

.PHONY : benchmark
benchmark:
	$(CC) $(C_MODE) -Oz -g3 -DDEBUG -static -I/usr/include -I/usr/local/include -I../runtime -L/usr/lib -L/usr/local/lib -o ./stats.profile.custom_oz.txt $(BUILD_FULL_DIR)/$(TARGET) -x c $(STATS_DIR)/stats.profile.custom.c_ -lc
	$(CC) $(C_MODE) -Oz -g3 -DDEBUG -static -I/usr/include -I/usr/local/include -I../runtime -L/usr/lib -L/usr/local/lib -o ./stats.profile.std_oz.txt $(BUILD_FULL_DIR)/$(TARGET) -x c $(STATS_DIR)/stats.profile.std.c_ -lc
	$(CC) $(C_MODE) -O2 -g3 -DDEBUG -static -I/usr/include -I/usr/local/include -I../runtime -L/usr/lib -L/usr/local/lib -o ./stats.profile.custom_o3.txt $(BUILD_FULL_DIR)/$(TARGET) -x c $(STATS_DIR)/stats.profile.custom.c_ -lc
	$(CC) $(C_MODE) -O3 -g3 -DDEBUG -static -I/usr/include -I/usr/local/include -I../runtime -L/usr/lib -L/usr/local/lib -o ./stats.profile.std_o3.txt $(BUILD_FULL_DIR)/$(TARGET) -x c $(STATS_DIR)/stats.profile.std.c_ -lc
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes $(STATS_DIR)/stats.profile.custom_oz.txt;
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes $(STATS_DIR)/stats.profile.std_oz.txt;
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes $(STATS_DIR)/stats.profile.custom_o3.txt;
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes $(STATS_DIR)/stats.profile.std_o3.txt;


stats: cloc
	echo TRUE

.PHONY : debug
debug:
	scan-build35 --use-cc clang -analyze-headers -o $(STATS_DIR) make 2>&1 | tail -n 1 | sed "s/'scan-view [^']*'/'scan-view &'/"

