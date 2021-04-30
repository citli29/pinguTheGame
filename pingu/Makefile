PROG=proj

.PATH: ${.CURDIR}/devices

SRCS = proj.c game.c i8042.c keyboard.c mouse.c timer.c video.c utils.c  rtc.c
# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic 
#-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
