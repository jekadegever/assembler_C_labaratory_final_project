CC = gcc
CFLAGS = -g -Wall -ansi -pedantic -IHeader_Files

TARGET = assembler

CLEAN_OBJ = assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o


$(TARGET): assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o
	$(CC) $(CFLAGS) assembler.o pre_processor.o util.o externals.o instructions.o instruction_memory.o data_memory.o directives.o labels.o addresses.o encoder.o files.o first_pass.o second_pass.o errors.o lines_map.o tables.o sys_memory.o -o $(TARGET)
	rm -f *.o

assembler.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/assembler.c -o assembler.o

pre_processor.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/pre_processor.c -o pre_processor.o

util.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/util.c -o util.o

instructions.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/instructions.c -o instructions.o

instruction_memory.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/instruction_memory.c -o instruction_memory.o

data_memory.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/data_memory.c -o data_memory.o

directives.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/directives.c -o directives.o

labels.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/labels.c -o labels.o

addresses.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/addresses.c -o addresses.o

encoder.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/encoder.c -o encoder.o

files.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/files.c -o files.o

second_pass.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/second_pass.c -o second_pass.o

first_pass.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/first_pass.c -o first_pass.o

externals.o: source_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/externals.c -o externals.o

errors.o: source_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/errors.c -o errors.o

lines_map.o: source_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/lines_map.c -o lines_map.o

tables.o: source_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/tables.c -o tables.o

sys_memory.o:  source_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files Header_files
	$(CC) $(CFLAGS) -c Source_Files/sys_memory.c -o sys_memory.o
clean:
	rm -f $(CLEAN_OBJ) *.o


