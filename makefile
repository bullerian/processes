TARGET				 := psspawner
SRC					 := main.c
INCL				 := include
OBJS  			 	 = $(SRC:.c=.o)
BIN_DIR				 = output
OUTPUT_OPTION 		 = -o $(BIN_DIR)/$@
RM      			 = /bin/rm -f
CPPFLAGS 		 	 += $(addprefix -I, $(INCL))
CFLAGS			 	 += -Wall -O0 -g -ggdb
VPATH 				 = $(BIN_DIR)
DESTDIR 			 = /usr/local/bin
vpath %.c src

.PHONY: all install unistall install-strip clean
all: $(TARGET)

$(TARGET) : $(OBJS)
		$(LINK.o) $(addprefix $(BIN_DIR)/,$^) $(OUTPUT_OPTION)

%.o: %.c
		$(COMPILE.c) $(OUTPUT_OPTION) $<

install : $(TARGET)
		install $(BIN_DIR)/$(TARGET) $(DESTDIR)

unistall :
		$(RM) $(DESTDIR)/$(TARGET)

install-strip :
		$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' \
							  install

clean:
	$(RM) $(BIN_DIR)/*
