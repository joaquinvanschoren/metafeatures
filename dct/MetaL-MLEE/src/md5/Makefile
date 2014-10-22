md5isum	: mddriver.c md5c.o global.h
	gcc -O4 -o md5sum mddriver.c md5c.o
	strip md5sum

md5c.o	: md5c.c md5.h global.h
	gcc -O4 -c md5c.c

clean:
	$(RM) *.o md5sum

.PHONY: clean

