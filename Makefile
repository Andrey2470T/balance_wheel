IRRLICHT_PATH = /home/andrey/irrlicht-1.8.4

CPPFLAGS += -I$(IRRLICHT_PATH)/include -I/usr/X11R6/include

ifndef NDEBUG
	CXXFLAGS += -g -Wall 
else
	CXXFLAGS += -O3
endif

CXXFLAGS += -std=c++17

LIBS += -L$(IRRLICHT_PATH)/lib/Linux -lIrrlicht -L/usr/X11R6/lib64 -lGL -lXxf86vm -lXext -lX11

TARGET = balance_wheel

.PHONY : all clean

all : clean $(TARGET)

$(TARGET) : 
	g++ $(CPPFLAGS) $(CXXFLAGS) main.cpp -o $(TARGET) $(LIBS)

clean : 
	rm -f $(TARGET).o
	rm -f $(TARGET)