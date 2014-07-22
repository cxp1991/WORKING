COMMENT=-L/usr/local/lib -lpjsua-i686-pc-linux-gnu -lpjsip-ua-i686-pc-linux-gnu -lpjsip-simple-i686-pc-linux-gnu -lpjsip-i686-pc-linux-gnu -lpjmedia-codec-i686-pc-linux-gnu -lpjmedia-videodev-i686-pc-linux-gnu -lpjmedia-i686-pc-linux-gnu -lpjmedia-audiodev-i686-pc-linux-gnu -lpjnath-i686-pc-linux-gnu -lpjlib-util-i686-pc-linux-gnu -lresample-i686-pc-linux-gnu -lmilenage-i686-pc-linux-gnu -lsrtp-i686-pc-linux-gnu -lgsmcodec-i686-pc-linux-gnu -lspeex-i686-pc-linux-gnu -lilbccodec-i686-pc-linux-gnu -lg7221codec-i686-pc-linux-gnu -lportaudio-i686-pc-linux-gnu  -lpj-i686-pc-linux-gnu -lm -lnsl -lrt -lpthread  -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread  -L/usr/local/lib -lavformat -lavcodec -lswscale -lavutil  -I/usr/local/include -I/usr/local/include -DPJ_AUTOCONF=1-O2 -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1 `pkg-config --cflags --libs gstreamer-base-0.10`
GST_PLUGIN=gstpjnath.o gstpjnathsink.o gstpjnathsrc.o
all: 
	gcc -Wall -g -c gstpjnathsink.c $(COMMENT)
	gcc -Wall -g -c gstpjnathsrc.c $(COMMENT)
	gcc -Wall -g -c gstpjnath.c $(COMMENT)
	ld  -r  $(GST_PLUGIN) -o gstreamer_plugin.o
	 
clean:	
	rm -rf run *.o
