cmake_minimum_required(VERSION 3.19)
project(system_programming C)

set(CMAKE_C_STANDARD 11)

include_directories(HW1)
include_directories(HW2)
include_directories(HW3)
include_directories(HW4)
include_directories("Midterm Project")
include_directories(Start/data_structures)
include_directories(Start/directory)
include_directories(Start/error)
include_directories(Start/fork)
include_directories(Start/lseek)
include_directories(Start/Project1/source)
include_directories(Start/test)
include_directories(Start/threads)

add_executable(system_programming
        HW1/helper.c
        HW1/helper.h
        HW1/linked_list.c
        HW1/linked_list.h
        HW1/main.c
        HW1/search.c
        HW1/search.h
        HW2/helper.c
        HW2/helper.h
        HW2/lagrange.c
        HW2/lagrange.h
        HW2/main.c
        HW3/helper.c
        HW3/helper.h
        HW3/main.c
        HW3/peer.c
        HW4/helper.c
        HW4/helper.h
        HW4/main.c
        HW4/queue.c
        HW4/queue.h
        "Midterm Project/helper.c"
        "Midterm Project/helper.h"
        "Midterm Project/main.c"
        "Midterm Project/main.h"
        Start/data_structures/c_queue.c
        Start/data_structures/c_queue.h
        Start/data_structures/linked_list.c
        Start/data_structures/linked_list.h
        Start/data_structures/queue.c
        Start/data_structures/queue.h
        Start/directory/main.c
        Start/directory/main2.c
        Start/directory/signal.h
        Start/directory/test.c
        Start/directory/test2.c
        Start/directory/test3.c
        Start/error/main.c
        Start/error/print.c
        Start/error/print.h
        Start/error/test.c
        Start/fork/main1.c
        Start/fork/print.c
        Start/fork/print.h
        Start/lseek/driver.c
        Start/lseek/main.h
        Start/memory/test.c
        Start/memset/main.c
        Start/mid/main.c
        Start/mid/remove.c
        Start/Project1/source/header.h
        Start/Project1/source/main.c
        Start/Project2/main.c
        Start/Project2/mytest.c
        Start/Project2/test.c
        Start/read/client.c
        Start/read/readline.c
        Start/read/server.c
        Start/read/test.c
        Start/signal/helper.c
        Start/signal/main.c
        Start/signal/mask.c
        Start/signal/mask2.c
        Start/signal/mask3.c
        Start/signal/mymask.c
        Start/signal/mymask10.c
        Start/signal/mymask2.c
        Start/signal/mymask3.c
        Start/signal/mymask4.c
        Start/signal/mymask5.c
        Start/signal/mymask6.c
        Start/signal/mymask7.c
        Start/signal/mymask8.c
        Start/signal/mymask9.c
        Start/signal/runnable.c
        Start/signal/test.c
        Start/stat/main.c
        Start/stat/test.c
        Start/stat/test2.c
        Start/stat/test3.c
        Start/test/main.c
        Start/test/queue.c
        Start/test/queue.h
        Start/threads/linked_list.c
        Start/threads/linked_list.h
        Start/threads/main.c
        Start/threads/queue.c
        Start/threads/queue.h
        Start/threads/test.c Start/test/test.c Start/final/test.c "Final Project/server.c" "Final Project/helper.c" "Final Project/helper.h" Start/final/list.c Start/final/list.h Start/final/hash.c Start/final/hash.h Start/final/test2.c Start/list/list.c Start/list/list.h Start/list/test.c "Final Project/linked_list.c" "Final Project/linked_list.h" Start/client_server/server.c "Final Project/client.c" "Final Project/sql_engine.c" "Final Project/sql_engine.h" Start/list/sql.c Start/list/sql.h)
