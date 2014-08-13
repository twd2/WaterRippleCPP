ripple: main.o RippleEngine.o FrameBufferHelper.o
	g++ -O2 -o ripple main.o RippleEngine.o FrameBufferHelper.o -lpng
ripple_static: ripple
	g++ -O2 -static -o ripple_static main.o RippleEngine.o FrameBufferHelper.o -lpng
main.o: main.cpp RippleEngine.hpp FrameBufferHelper.hpp
	g++ -O2 -c main.cpp 
RippleEngine.o: RippleEngine.hpp RippleEngine.cpp
	g++ -O2 -c RippleEngine.hpp RippleEngine.cpp
FrameBufferHelper.o: FrameBufferHelper.hpp FrameBufferHelper.cpp
	g++ -O2 -c FrameBufferHelper.hpp FrameBufferHelper.cpp
clean: 
	-rm ripple main.o RippleEngine.o RippleEngine.hpp.gch FrameBufferHelper.hpp.gch FrameBufferHelper.o
release: ripple png.png start.sh
	make
	-mkdir release
	cp ripple release/
	cp png.png release/
	chmod +x start.sh
	cp start.sh release/
	#make clean
