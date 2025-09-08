CC = gcc
CFLAGS = -g -Wall -ansi -pedantic -IHeader_Files

TARGET = assembler

CLEAN_OBJ = assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o


$(TARGET): assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o
	$(CC) $(CFLAGS) assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o -o $(TARGET)
	rm -f *.o

assembler.o: Source_Files/assembler.c Header_Files/assembler.h Header_Files/data_memory.h Header_Files/errors.h Header_Files/first_pass.h Header_Files/externals.h Header_Files/files.h Header_Files/pre_processor.h Header_Files/second_pass.h Header_Files/tables.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/assembler.c -o assembler.o

pre_processor.o: Source_Files/pre_processor.c Header_Files/pre_processor.h Header_Files/config.h Header_Files/files.h Header_Files/boolean.h Header_Files/lines_map.h Header_Files/typedef.h Header_Files/context.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/pre_processor.c -o pre_processor.o

util.o: Source_Files/util.c Header_Files/util.h Header_Files/boolean.h Header_Files/context.h Header_Files/lines_map.h Header_Files/config.h Header_Files/data_memory.h Header_Files/directives.h Header_Files/errors.h Header_Files/instructions.h Header_Files/instruction_memory.h Header_Files/labels.h Header_files/addresses.h Header_Files/pre_processor.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/util.c -o util.o

instructions.o: Source_Files/instructions.c Header_Files/instructions.h Header_Files/config.h Header_Files/boolean.h Header_Files/errors.h Header_Files/context.h Header_Files/encoder.h Header_Files/instruction_memory.h Header_Files/util.h Header_Files/data_memory.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/instructions.c -o instructions.o

instruction_memory.o: Source_Files/instruction_memory.c Header_Files/instruction_memory.h Header_Files/config.h Header_Files/boolean.h Header_Files/typedef.h Header_Files/util.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/instruction_memory.c -o instruction_memory.o

data_memory.o: Source_Files/data_memory.c Header_Files/data_memory.h Header_Files/config.h Header_Files/boolean.h Header_Files/typedef.h Header_Files/errors.h Header_Files/util.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/data_memory.c -o data_memory.o

directives.o: Source_Files/directives.c Header_Files/directives.h Header_Files/config.h Header_Files/boolean.h Header_Files/typedef.h Header_Files/context.h Header_Files/data_memory.h Header_Files/util.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/directives.c -o directives.o

labels.o: Source_Files/labels.c Header_Files/labels.h Header_Files/boolean.h Header_Files/typedef.h Header_Files/context.h Header_Files/errors.h Header_Files/util.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/labels.c -o labels.o

addresses.o: source_files/addresses.c Header_files/addresses.h Header_Files/errors.h Header_Files/context.h Header_Files/instructions.h Header_Files/data_memory.h Header_Files/encoder.h Header_Files/instruction_memory.h Header_Files/labels.h Header_Files/util.h Header_Files/externals.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/addresses.c -o addresses.o

encoder.o: Source_Files/encoder.c Header_Files/encoder.h Header_Files/config.h Header_Files/instructions.h Header_files/addresses.h Header_Files/util.h Header_Files/typedef.h Header_Files/context.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/encoder.c -o encoder.o

files.o: Source_Files/files.c Header_Files/files.h Header_Files/config.h Header_Files/boolean.h Header_Files/externals.h Header_Files/data_memory.h Header_Files/instruction_memory.h Header_Files/labels.h Header_Files/util.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/files.c -o files.o

second_pass.o: Source_Files/second_pass.c Header_Files/second_pass.h Header_Files/boolean.h Header_Files/files.h Header_files/addresses.h Header_Files/context.h Header_Files/util.h Header_Files/labels.h Header_Files/errors.h Header_Files/directives.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/second_pass.c -o second_pass.o

first_pass.o: Source_Files/first_pass.c Header_Files/first_pass.h Header_Files/data_memory.h Header_Files/directives.h Header_Files/files.h Header_Files/instructions.h Header_Files/instruction_memory.h Header_Files/labels.h Header_Files/util.h Header_Files/typedef.h Header_Files/context.h Header_Files/boolean.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/first_pass.c -o first_pass.o

externals.o: Source_Files/externals.c Header_Files/externals.h Header_Files/util.h Header_Files/typedef.h Header_Files/errors.h Header_Files/labels.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/externals.c -o externals.o

errors.o: Source_Files/errors.c Header_Files/errors.h Header_Files/context.h Header_Files/config.h Header_Files/lines_map.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/errors.c -o errors.o

lines_map.o: Source_Files/lines_map.c Header_Files/lines_map.h Header_Files/util.h Header_Files/typedef.h Header_Files/errors.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/lines_map.c -o lines_map.o

tables.o: Source_Files/tables.c Header_Files/tables.h Header_Files/instructions.h Header_files/sys_memory.h
	$(CC) $(CFLAGS) -c Source_Files/tables.c -o tables.o

sys_memory.o:  Source_Files/sys_memory.c Header_files/sys_memory.h Header_files/addresses.h Header_files/data_memory.h Header_files/labels.h Header_files/pre_processor.h Header_files/errors.h Header_files/externals.h Header_files/lines_map.h Header_files/instruction_memory.h
	$(CC) $(CFLAGS) -c Source_Files/sys_memory.c -o sys_memory.o
clean:
	rm -f $(CLEAN_OBJ) *.o


