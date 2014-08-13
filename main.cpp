#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/time.h>
#include "png.h"
#include "RippleEngine.hpp"
#include "FrameBufferHelper.hpp"
using std::cout;
using std::cerr;
using std::endl;
inline unsigned long long myclock()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}
int FPS=0;
unsigned long long lastFPS=0;
void refreshFPS()
{
    static int fps=0;
    ++fps;
    if(myclock()-lastFPS>=1000)
    {
        FPS=fps;
        fps=0;
        lastFPS=myclock();
    }
}
inline int random(int min, int max)
{
    return rand()%(max-min)+min;
}
int main(int argc, const char **argv)
{
    srand(myclock());
    FILE *pngfd;
    const char *filename="png.png";
    if(argc>=2)
    {
        filename=argv[1];
    }
    cout<<"Using "<<filename<<endl;
    pngfd=fopen(filename,"rb");
    if(!pngfd)
    {
        cerr<<"Open png file error"<<endl;
        return 1;
    }
    png_byte pngheader[8];
    int readlen=fread(pngheader, sizeof(png_byte), sizeof(pngheader)/sizeof(png_byte), pngfd);
    bool ispng=!png_sig_cmp(pngheader,0,sizeof(pngheader)/sizeof(png_byte));
    if(!ispng)
    {
        fclose(pngfd);
        cerr<<"Not a png file"<<endl;
        return 1;
    }
    png_structp png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr==NULL)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(pngfd);
        cerr<<"info_ptf==NULL"<<endl;
        return 1;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(pngfd);
        cerr<<"libpng error"<<endl;
        return 1;
    }
    png_init_io(png_ptr, pngfd);
    png_set_sig_bytes(png_ptr,sizeof(pngheader)/sizeof(png_byte));
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, 0);
    int cc     = png_get_channels(png_ptr, info_ptr);
    int bpc    = png_get_bit_depth(png_ptr, info_ptr);
    int width  =  png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int imglinelength=width*bpc/8*cc;
    int imgsize= height*imglinelength;
    cout<<"width="<<width<<", height="<<height<<", bits per channles="<<bpc<<", channles="<<cc<<endl;
    
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    
    unsigned char *imgdata_src=new unsigned char[imgsize];
    unsigned char *imgdata_tmp=new unsigned char[imgsize];
    
    //to flat
    for(int i=0;i<height;++i)
    {
        memcpy(imgdata_src+(imglinelength*i),row_pointers[i],imglinelength);
    }
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(pngfd);
    
    FrameBufferHelper fb;
    if(!fb.open("fb0"))
    {
        return 1;
    }
    else
    {
        cout<<"Opened fb"<<endl;
    }
    
    unsigned char * old=new unsigned char[fb.screensize];
    memcpy(old,fb.fb_ptr,fb.screensize);
    
    //system("clear");
    
    memset(fb.fb_ptr,255,fb.screensize);
    
    RippleEngine engine(width, height);
    
    memset(imgdata_tmp,0,imgsize);
    //memcpy(imgdata_tmp,imgdata_src,imgsize);
    
    unsigned long long lastRipple=myclock();
    unsigned long long lastDrop=myclock();
    engine.dropStone(random(0,width),random(0,height),random(0,10),random(0,512));
    while(true)
    {
        //return 0;
        cout<<"\rFPS="<<FPS<<"              ";
        
        
        refreshFPS();
        
        //memcpy(imgdata_tmp,imgdata_src,imgsize);
        if(myclock()-lastRipple>=16)
        {
            engine.next();
            engine.render(imgdata_src, imgsize, imgdata_tmp);
            lastRipple=myclock();
            //cout<<"RIPPLE!"<<endl;
        }
        if(myclock()-lastDrop>=25)
        {
            engine.dropStone(random(0,width),random(0,height),random(0,10),random(0,512));
            //cout<<random(0,width)<<endl;
            lastDrop=myclock();
            //cout<<"DROP!"<<endl;
        }
        //memset(fb.fb_ptr,0,fb.screensize);
        //memset(imgdata_tmp,0,imgsize);
        //cout<<(int)imgdata_tmp[0]<<endl;
        //fb.render(10,200,imgdata_tmp,width,height,cc);    
        
        fb.render(0,0,imgdata_tmp,width,height,cc);    
    }
    
    memcpy(fb.fb_ptr,old,fb.screensize);
    
    delete [] old;
    delete [] imgdata_src;
    delete [] imgdata_tmp;
    
    return 0;
}

