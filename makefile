CC = gcc
CFLAGS = -g -Wall -ansi -pedantic -IHeader_Files

TARGET = assembler

CLEAN_OBJ = assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o


$(TARGET): assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o
	$(CC) $(CFLAGS) assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o -o $(TARGET)
	rm -f *.o

assembler.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/assembler.c -o assembler.o

pre_processor.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/pre_processor.c -o pre_processor.o

util.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/util.c -o util.o

instructions.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/instructions.c -o instructions.o

instruction_memory.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/instruction_memory.c -o instruction_memory.o

data_memory.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/data_memory.c -o data_memory.o

directives.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/directives.c -o directives.o

labels.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/labels.c -o labels.o

addresses.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/addresses.c -o addresses.o

encoder.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/encoder.c -o encoder.o

files.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/files.c -o files.o

second_pass.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/second_pass.c -o second_pass.o

first_pass.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/first_pass.c -o first_pass.o

externals.o: Source_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/externals.c -o externals.o

errors.o: Source_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/errors.c -o errors.o

lines_map.o: Source_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/lines_map.c -o lines_map.o

tables.o: Source_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/tables.c -o tables.o

sys_memory.o:  Source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/sys_memory.c -o sys_memory.o
clean:
	rm -f $(CLEAN_OBJ) *.o


