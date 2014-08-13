#ifndef RIPPLEENGINE_HPP
#define RIPPLEENGINE_HPP

#include<cstring>

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define abs(a) ((a)<0?(-(a)):(a))

class RippleEngine
{
    private:
        double *buf1, *buf2;
	    int PoolWidth, PoolHeight;
    public:
        RippleEngine(unsigned int, unsigned int);
        ~RippleEngine();
        void reset();
        void next();
        void render(const unsigned char *, size_t, unsigned char *);
        void dropStone(int, int, int, int);
};

#endif //ifndef RIPPLEENGINE_HPP
