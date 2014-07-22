uart: 
	gcc -c  uart_controller.c command.c uart.c -lpthread `pkg-config --cflags --libs nice gstreamer-1.0`
	ld -r uart_controller.o command.o uart.o -o Uart.o
	rm -rf uart_controller.o command.o uart.o 

