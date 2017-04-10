CC ?= gcc

CFLAGS=-Werror

SRC_DIR=src
BIN_DIR=bin
OBJ_DIR=obj

START=$(BIN_DIR)/aucont_start
STOP=$(BIN_DIR)/aucont_stop
LIST=$(BIN_DIR)/aucont_list
EXEC=$(BIN_DIR)/aucont_exec

DEPS_SRC=$(SRC_DIR)/argparse.c $(SRC_DIR)/log.c $(SRC_DIR)/engine.c\
	$(SRC_DIR)/common_ns.c $(SRC_DIR)/utc_ns.c 
DEPS_OBJS=$(DEPS_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

START_DEPS=
STOP_DEPS=
LIST_DEPS=
EXEC_DEPS=

START_SRC=$(SRC_DIR)/start.c
STOP_SRC=$(SRC_DIR)/stop.c
LIST_SRC=$(SRC_DIR)/list.c
EXEC_SRC=$(SRC_DIR)/exec.c

START_OBJS=$(START_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
STOP_OBJS=$(STOP_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIST_OBJS=$(LIST_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC_OBJS=$(EXEC_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all:$(START) $(STOP) $(LIST) $(EXEC)
	
$(START):$(BIN_DIR) $(START_OBJS) $(DEPS_OBJS)
	$(CC) $(CFLAGS) $(START_OBJS) $(DEPS_OBJS) -o $(START)
	
$(STOP):$(BIN_DIR) $(STOP_OBJS) $(DEPS_OBJS)
	$(CC) $(CFLAGS) $(STOP_OBJS) $(DEPS_OBJS) -o $(STOP)
	
$(LIST):$(BIN_DIR) $(LIST_OBJS) $(DEPS_OBJS)
	$(CC) $(CFLAGS) $(LIST_OBJS) $(DEPS_OBJS) -o $(LIST)
	
$(EXEC):$(BIN_DIR) $(EXEC_OBJS) $(DEPS_OBJS)
	$(CC) $(CFLAGS) $(EXEC_OBJS) $(DEPS_OBJS) -o $(EXEC)


$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM -MT $@ -MF $(OBJ_DIR)/$*.d $<

-include $(START_OBJS:.o=.d)
-include $(STOP_OBJS:.o=.d)
-include $(LIST_OBJS:.o=.d)
-include $(EXEC_OBJS:.o=.d)
-include $(DEPS_OBJS:.o=.d)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
	
.PHONY:clean
	

	
