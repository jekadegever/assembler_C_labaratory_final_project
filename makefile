CC = gcc
CFLAGS = -g -Wall -ansi -pedantic

TARGET = assembler

CLEAN_OBJ = assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o memory_relocation.o encoder.o files.o first_pass.o second_pass.o


$(TARGET): assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o memory_relocation.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o
	$(CC) $(CFLAGS) assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o memory_relocation.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o -o $(TARGET)
	rm -f *.o
assembler.o: assembler.c assembler.h data_memory.h errors.h first_pass.h externals.h files.h pre_processor.h second_pass.h tables.h
	$(CC) $(CFLAGS) -c assembler.c -o assembler.o

pre_processor.o: pre_processor.c pre_processor.h config.h files.h boolean.h lines_map.h typedef.h context.h errors.h
	$(CC) $(CFLAGS) -c pre_processor.c -o pre_processor.o

util.o: util.c util.h boolean.h externals.h  context.h lines_map.h config.h data_memory.h directives.h errors.h instructions.h instruction_memory.h labels.h memory_relocation.h pre_processor.h
	$(CC) $(CFLAGS) -c util.c -o util.o


instructions.o: instructions.c instructions.h config.h boolean.h errors.h  context.h encoder.h instruction_memory.h util.h data_memory.h
	$(CC) $(CFLAGS) -c instructions.c -o instructions.o

instruction_memory.o: instruction_memory.c instruction_memory.h config.h boolean.h typedef.h util.h errors.h
	$(CC) $(CFLAGS) -c instruction_memory.c -o instruction_memory.o

data_memory.o: data_memory.c data_memory.h config.h boolean.h typedef.h errors.h util.h
	$(CC) $(CFLAGS) -c data_memory.c -o data_memory.o

directives.o: directives.c directives.h config.h boolean.h typedef.h context.h data_memory.h util.h errors.h
	$(CC) $(CFLAGS) -c directives.c -o directives.o

labels.o: labels.c labels.h boolean.h typedef.h context.h errors.h util.h
	$(CC) $(CFLAGS) -c labels.c -o labels.o

memory_relocation.o: memory_relocation.c memory_relocation.h  errors.h context.h instructions.h data_memory.h encoder.h instruction_memory.h labels.h util.h externals.h
	$(CC) $(CFLAGS) -c memory_relocation.c -o memory_relocation.o

encoder.o: encoder.c encoder.h config.h instructions.h memory_relocation.h util.h typedef.h context.h errors.h
	$(CC) $(CFLAGS) -c encoder.c -o encoder.o

files.o: files.c files.h config.h boolean.h externals.h data_memory.h instruction_memory.h labels.h util.h errors.h
	$(CC) $(CFLAGS) -c files.c -o files.o

second_pass.o: second_pass.c second_pass.h boolean.h files.h memory_relocation.h context.h util.h labels.h files.h errors.h directives.h
	$(CC) $(CFLAGS) -c second_pass.c -o second_pass.o

first_pass.o: first_pass.c first_pass.h data_memory.h directives.h files.h instructions.h instruction_memory.h labels.h util.h typedef.h context.h boolean.h errors.h
	$(CC) $(CFLAGS) -c first_pass.c -o first_pass.o

externals.o: externals.c externals.h util.h typedef.h errors.h labels.h
	$(CC) $(CFLAGS) -c externals.c -o externals.o

errors.o: errors.c errors.h  context.h config.h lines_map.h
	$(CC) $(CFLAGS) -c errors.c -o errors.o

lines_map.o: lines_map.c lines_map.h util.h typedef.h  errors.h
	$(CC) $(CFLAGS) -c lines_map.c -o lines_map.o

tables.o: tables.c tables.h instructions.h
	$(CC) $(CFLAGS) -c tables.c -o tables.o

clean:
	rm -f $(CLEAN_OBJ)  *.o

