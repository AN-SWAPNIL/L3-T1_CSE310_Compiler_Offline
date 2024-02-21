yacc -d -y 2005009.y
echo 'Generated the parser C file as well the header file'
g++ -w -c -o y.o y.tab.c
echo 'Generated the parser object file'
flex 2005009.l
echo 'Generated the scanner C file'
g++ -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'
g++ y.o l.o -lfl -o 2005009.out
echo 'Ready for execution'
./2005009.out input.c # Change this line to execute the program