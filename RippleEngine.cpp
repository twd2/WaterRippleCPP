#include "RippleEngine.hpp"

RippleEngine::RippleEngine(unsigned int W, unsigned int H):PoolWidth(W), PoolHeight(H)
{
    buf1=new double[W*H];
    buf2=new double[W*H];
    reset();
}
RippleEngine::~RippleEngine()
{
    delete [] buf1;
    delete [] buf2;
}
void RippleEngine::reset()
{
    memset(buf1,0,sizeof(buf1));
    memset(buf2,0,sizeof(buf2));
}

void RippleEngine::next()
{
    for(int i=PoolWidth+1;i<PoolWidth*PoolHeight-PoolWidth-1;++i)
    {
        buf2[i]=((buf1[i-1]+
                  buf1[i+1]+
                  buf1[i-PoolWidth]+
                  buf1[i+PoolWidth]+
                  buf1[i-1-PoolWidth]+
                  buf1[i-1+PoolWidth]+
                  buf1[i+1-PoolWidth]+
                  buf1[i+1+PoolWidth])
                  /4.0)
                 -(buf2[i]);
        buf2[i] *= 1.0-(1/32.0*2.3333);
        if (abs(buf2[i])<0.000001) buf2[i]=0;
    }
    double * tmp;
    tmp=buf1;
    buf1=buf2;
    buf2=tmp;
}
void RippleEngine::render(const unsigned char *src, size_t imgsize, unsigned char *des)
{
    int xoff,yoff,k=PoolWidth;
    for(int i=1;i<PoolHeight-1;++i)
    {
        for(int j=0;j<PoolWidth;++j)
        {
            xoff=yoff=0;
            xoff=(int)((buf1[k-1]-buf1[k+1])*0.44);
            yoff=(int)((buf1[k-PoolWidth]-buf1[k+PoolWidth])*0.44);
            
            int pos1,pos2;
            pos1=PoolWidth*3*(i+yoff) + 3*(j+xoff)  -1;
            pos2=PoolWidth*3*i + 3*j                -1;
            
            for(int d=0;d<3;++d)
            {
                if(pos1>0 && pos1<imgsize && pos2>0 && pos2<imgsize)
                {
                    double x=buf1[k]/5;
                    if(x>255) x=255;
                    if(x<0) x=0;
                    double c=src[++pos1];
                    double brightness=-(x/255);
                    c=(255-c)*brightness+c;
                    if(c>255) c=255;
                    if(c<0) c=0;
                    
                    des[++pos2]=(unsigned char)c;
                }
            }
            ++k;
        }
    }
}
void RippleEngine::dropStone(int x, int y, int size, int weight)
{
		if((x+size)>PoolWidth ||(y+size)>PoolHeight||(x-size)<0||(y-size)<0)
			return;
	    for(int posx=max(0,x-size);posx<min(x+size, PoolWidth);++posx)
	        for(int posy=max(0,y-size);posy<min(y+size, PoolHeight);++posy)
	            if((posx-x)*(posx-x) + (posy-y)*(posy-y) < size*size)
	                buf1[PoolWidth*posy+posx] -= weight;
}
