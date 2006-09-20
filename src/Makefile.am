bin_PROGRAMS = client


# set the include path found by configure
INCLUDES = -I/usr/include/Inventor/Qt -I/usr/share/qt3/include \
	-I/usr/include/qt3 -I/home/portilla/Live/live/BasicUsageEnvironment/include \
	-I/home/portilla/Live/live/liveMedia/include -I//home/portilla/Live/live/groupsock/include \
	-I//home/portilla/Live/live/UsageEnvironment/include

# the library search path.



AM_CXXFLAGS = -I. -O2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -Wall -DBSD=1
client_LDADD = \
	-L/home/portilla/Live/live/BasicUsageEnvironment/libBasicUsageEnvironment.a -L/home/portilla/Live/live/UsageEnvironment/libUsageEnvironment.a \
	-L/home/portilla/Live/live/groupsock/libgroupsock.a -L/home/portilla/Live/live/liveMedia/libliveMedia.a





client_SOURCES = client.cpp
noinst_HEADERS = client.h
