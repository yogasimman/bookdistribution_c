if [ ! -d "execution" ]; then
    mkdir execution
fi

gcc -c -Wall -Wextra -Wshadow  -pedantic server.c -o execution/server.o
#-Wnon-virtual-dtor for C++ 
gcc -c -Wall -Wextra -Wshadow  -pedantic socket.c -o execution/socket.o

gcc -c -Wall -Wextra -Wshadow  -pedantic read.c -o execution/read.o

gcc -c -Wall -Wextra -Wshadow -pedantic log.c -o execution/log.o

gcc -c -Wall -Wextra -Wshadow -pedantic json.c -o execution/json.o

gcc -o execution/server execution/server.o execution/socket.o execution/read.o execution/log.o execution/json.o

