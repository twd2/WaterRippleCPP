#ifndef FB_HPP
#define FB_HPP

#include <iostream>
#include <cstring>
#include <cstdio>
#include <unistd.h>  
#include <fcntl.h>  
#include <linux/fb.h>  
#include <sys/mman.h>  
#include <sys/ioctl.h>  
#include <arpa/inet.h> 
using std::cout;
using std::cerr;
using std::endl;

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define abs(a) ((a)<0?(-(a)):(a))

class FrameBufferHelper
{
    private:

    public:
        int fbfd;
        struct fb_var_screeninfo vinfo;  
        struct fb_fix_screeninfo finfo;
        bool opened;
        unsigned char *fb_ptr;
        size_t width, height, screensize, linelength;
        FrameBufferHelper();
        ~FrameBufferHelper();
        bool open();
        bool open(const char *);
        void render(int, int, const unsigned char *, size_t, size_t, size_t);
        void render(const unsigned char *, size_t, size_t, size_t);
        int RGB888toRGB565(int, int, int);
};

#endif //ifndef FB_HPP
