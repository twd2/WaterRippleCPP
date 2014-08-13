#include "FrameBufferHelper.hpp"
FrameBufferHelper::FrameBufferHelper()
{
    opened=false;
}
FrameBufferHelper::~FrameBufferHelper()
{
    if(opened)
    {
        cout<<"deleting"<<endl;
        munmap(fb_ptr, screensize);  
        close(fbfd);
    }
}
bool FrameBufferHelper::open()
{
    return open("fb0");
}
bool FrameBufferHelper::open(const char * dev)
{
    char *filename=new char[100];
    strcpy(filename, "/dev/");
    strcat(filename,dev);
    cout<<"Using "<<filename<<endl;
    fbfd=::open((const char *)filename, O_RDWR);
    delete [] filename;
    if(!fbfd)
    {
        cerr<<"Error: cannot open framebuffer device."<<endl;
        return false;
    }
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))  
    {  
        cerr<<"Error: reading fixed information."<<endl;  
        return false;
    }  
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))  
    {  
        cerr<<"Error: reading variable information."<<endl;  
        return false;
    }  
    
    //printf("R:%d,G:%d,B:%d \n", vinfo.red, vinfo.green, vinfo.blue );  
    cout<<vinfo.xres<<"x"<<vinfo.yres<<", "<<vinfo.bits_per_pixel<<" bits per pixel"<<endl;
    linelength=vinfo.xres*vinfo.bits_per_pixel/8;
    
    for(int i=2;i>=1;--i)
    {
        linelength=vinfo.xres*vinfo.bits_per_pixel/8*i;
        if( (linelength & 3) !=0)
        {
            linelength=(linelength>>2+1)<<2;
        }
        screensize=linelength*(size_t)vinfo.yres;//(size_t)vinfo.xres*(size_t)vinfo.yres*(size_t)vinfo.bits_per_pixel/8;
        width=vinfo.xres;
        height=vinfo.yres;
        cout<<"screen size="<<screensize<<" bytes"<<endl;
        
        fb_ptr=(unsigned char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
        
        if (i==1 && (int)fb_ptr == -1)  
        {  
            cerr<<"Error: failed to map framebuffer device to memory."<<endl;  
            return false;
        }
        else if ((int)fb_ptr != -1)
        {
            cout<<"*"<<i<<endl;  
            break;
        }
    }
    opened=true;
    return true;
}
void FrameBufferHelper::render(int xoffset, int yoffset, const unsigned char * data, size_t width, size_t height, size_t nc)
{ //unsigned char curr_pos1[4];
    for(int y=0;y<height;++y)
    {
        if(yoffset+y<0) 
        {
            y=max(0-yoffset,0)-1;
            //cout<<"continuey"<<endl;
            continue;
        }
        if(yoffset+y>=this->height) break;
        unsigned char *y_pos=fb_ptr+linelength*(yoffset+y);
        const unsigned char *y_data=data+(y*width*nc);
        for(int x=0;x<width;++x)
        {
            if(xoffset+x<0) 
            {
                x=max(0-xoffset,0)-1;
                //cout<<"continuex"<<endl;
                continue;
            }
            if(xoffset+x>=this->width) break;
            unsigned char *curr_pos=y_pos+((xoffset+x)*vinfo.bits_per_pixel>>3);
            const unsigned char *curr_data=y_data+(x*nc);
            //cout<<x<<","<<y<<","<<(this->width*(yoffset+y)+xoffset+x)*vinfo.bits_per_pixel/8<<endl;
            if(vinfo.bits_per_pixel==32)
            {
                if(nc==3)
                {
                   
                    curr_pos[2]=curr_data[0]; //R
                    curr_pos[1]=curr_data[1]; //G
                    curr_pos[0]=curr_data[2]; //B
                    curr_pos[3]=255;
                }
                else
                {
                    curr_pos[0]=curr_data[0];
                    curr_pos[1]=curr_data[0];
                    curr_pos[2]=curr_data[0];
                    curr_pos[3]=255;
                }
            }
            else if(vinfo.bits_per_pixel==24)
            {
                if(nc==3)
                {
                    curr_pos[2]=curr_data[0];
                    curr_pos[1]=curr_data[1];
                    curr_pos[0]=curr_data[2];
                    
                }
                else
                {
                    curr_pos[0]=curr_data[0];
                    curr_pos[1]=curr_data[0];
                    curr_pos[2]=curr_data[0];
                }
            }
            else if(vinfo.bits_per_pixel==16)
            {
                if(nc==3)
                {
                    int c=RGB888toRGB565(curr_data[0],curr_data[1],curr_data[2]);
                    curr_pos[0]=c&0xFF;
                    curr_pos[1]=c>>8;
                }
                else
                {
                    int c=RGB888toRGB565(curr_data[0],curr_data[0],curr_data[+0]);
                    curr_pos[0]=c&0xFF;
                    curr_pos[1]=c>>8;
                }
            }
        }
    }
}
void FrameBufferHelper::render(const unsigned char * data, size_t width, size_t height, size_t nc)
{
    render(0,0,data,width,height,nc);
}
int FrameBufferHelper::RGB888toRGB565(int r,int g,int b)
{
    return ((unsigned(r) << 8) & 0xF800) |   
           ((unsigned(g) << 3) & 0x7E0)  |  
           ((unsigned(b) >> 3));
}
