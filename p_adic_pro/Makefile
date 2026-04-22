CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm

SRCDIR = Src
SRCS = $(SRCDIR)/main.c $(SRCDIR)/ui.c $(SRCDIR)/padic.c $(SRCDIR)/ast.c $(SRCDIR)/padic_root.c $(SRCDIR)/lang.c
OBJS = $(SRCS:.c=.o)
TARGET = padic_converter

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET) *~

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run