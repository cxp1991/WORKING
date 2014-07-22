DEPENDENCES=Main.o Connect_Server/Connect_Server.o P2P_Stream/P2P_Stream.o Rpi_Setup/Rpi_Setup.o Utils/base64.o Uart/Uart.o

rpi : Main.o Connect_Server.o P2P_Stream.o Rpi_Setup.o base64.o Uart.o
	gcc $(DEPENDENCES) -o rpi `pkg-config --cflags --libs nice gstreamer-1.0`

Main.o: 
	gcc -w -c Main.c `pkg-config --cflags --libs nice gstreamer-1.0`
	
Connect_Server.o:
	cd Connect_Server; make Connect_Server
	
P2P_Stream.o:
	cd P2P_Stream; make P2P_Stream
	
Rpi_Setup.o:
	cd Rpi_Setup; make Rpi_Setup
	
base64.o:
	cd Utils; make base64
	
Uart.o:
	cd Uart; make uart
	
clean:
	rm  $(DEPENDENCES) rpi