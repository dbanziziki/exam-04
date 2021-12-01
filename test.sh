#!/bin/bash

test_line() {
    ./microshell $@
}

make

test_line /bin/ls

test_line /bin/ls "|" /usr/bin/grep micro

test_line /bin/cat Makefile "|" /usr/bin/grep gcc

test_line /bin/cat Makefile "|" /usr/bin/grep gcc ";" /bin/ls -la "|" /usr/bin/grep sub

test_line /bin/cat Makefile "|" /usr/bin/grep gcc ";" /bin/ls -la "|" /usr/bin/grep sub ";" /bin/cat Makefile

test_line /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell

make fclean