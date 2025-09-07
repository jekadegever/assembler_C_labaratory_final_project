cmake_minimum_required(VERSION 3.30)
project(Final_project_C_labaratory C)

set(CMAKE_C_STANDARD 90)

add_executable(Final_project_C_labaratory
        Pre_processor.c
        Pre_processor.h
        Config.h
        Util.c
        Util.h
        Instructions_handler.c
        Instructions_handler.h
        Instruction_memory.c
        Instruction_memory.h
        Data_memory.c
        Data_memory.h
        Directive_handler.c
        Directive_handler.h
        labels.c
        labels.h
        memory_relocation.c
        memory_relocation.h
        Encoder.c
        assembler.c
        assembler.h
        Encoder.h
        files.c
        files.h
        first_pass.c
        first_pass.h
        boolean.h
        second_pass.c
        second_pass.h
        externals.c
        externals.h
        lines_map.c
        lines_map.h
        errors.c
        errors.h
        typedef.h
        context.h
)
