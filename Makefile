output : 
	gcc main.c encode.c decode.c
	./a.out -e beautiful.bmp secret.txt output.bmp
	./a.out -d output.bmp decode.txt
	vi decode.txt
