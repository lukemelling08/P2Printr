cmake_minimum_required(VERSION 4.4)
project(p2printr C)

set(CMAKE_C_STANDARD 17)

add_executable(p2printr main.c
        protocol.h
        net.c
        net.h
        discovery.c
        chat.h
        commands.c
        commands.h
        ui.c
        ui.h
        chat.c)
