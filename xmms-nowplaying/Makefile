CC = gcc
CFLAGS = -Wall -g -O2 -I. `/usr/bin/gtk-config --cflags` \
	`/usr/bin/xmms-config --cflags` \
	-DDATA_DIR=\"`/usr/bin/xmms-config --data-dir`\"

LIBS =  `/usr/bin/gtk-config --libs` `/usr/bin/xmms-config --libs`

all: xmms-playing.o
	${CC} ${CFLAGS} ${LIBS} xmms-playing.o -o xmms-playing



