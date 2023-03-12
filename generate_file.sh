tr -dc '\0-\177' < /dev/urandom | fold -w100 | head -n 100000 > test.txt
