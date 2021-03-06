spt: sptglgmain.o wxstn.o panel.o spa.o hshbme280.o tsl2561.o gps.o nmea.o serial.o
	gcc -pthread -L/usr/local/glg/lib -L. -o spt sptglgmain.o wxstn.o panel.o spa.o hshbme280.o tsl2561.o gps.o nmea.o serial.o \
		-lwiringPi -lpigpio -lpthread -lm \
		-lglg_int -lglg -lglg_map_stub -lXm -lXt -lX11 -lXmu -lXft \
        -lXext -lXp -lz -ljpeg -lpng -lfreetype -lfontconfig -lm -ldl
sptglgmain.o : sptglgmain.c sptglgmain.h panel.h wxstn.h
	gcc -c -g -I/usr/local/glg/include sptglgmain.c
wxstn.o: wxstn.c wxstn.h
	gcc -g -c wxstn.c
panel.o: panel.c panel.h
	gcc -g -c panel.c
spa.o: spa.c spa.h
	gcc -g -c spa.c
hshbme280.o: hshbme280.c hshbme280.h
	gcc -g -c hshbme280.c
tsl2561.o: tsl2561.c tsl2561.h
	gcc -g -c tsl2561.c
gps.o: gps.c gps.h
	gcc -g -c gps.c
nmea.o: nmea.c nmea.h
	gcc -g -c nmea.c
serial.o: serial.c serial.h
	gcc -g -c serial.c
clean:
	touch *
	rm *.o
