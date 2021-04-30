#include "video.h"

vbe_mode_info_t vmi;
static char *video_mem;
static char *frame_buffer;
unsigned int vram_size;
int bytesPerPixel;

int (video_set_mode)(uint16_t mode){
    reg86_t r;
    
    memset(&r, 0, sizeof(r));	/* zero the structure */
    
    r.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
    r.bx = 1<<14|mode; // set bit 14: linear framebuffer
    r.intno = 0x10;

    if( sys_int86(&r) != OK ) {
        printf("set_vbe_mode: sys_int86() failed \n");
        return 1;
    }
    return 0;
}

int (video_allow_memory_mapping)(){

    int q;
    struct minix_mem_range mr; /*physical memory range*/
    unsigned int vram_base;  /*VRAM’s physical addresss*/
     /*VRAM’s size, but you can usethe frame-buffer size, instead*/
    
    
    /*Allow memory mapping*/

    vram_base = vmi.PhysBasePtr;

    bytesPerPixel = (vmi.BitsPerPixel+7)/8;
    vram_size = vmi.XResolution*vmi.YResolution*bytesPerPixel;

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    /*--grant a process the permission to map a given address range--*/

    if( OK != (q = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        panic("sys_privctl (ADD_MEM) failed: %d\n", q);
        return 1;
    }
        
    /*Map memory--map the video RAM to its address space*/
    
    video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);
  
    if(video_mem == MAP_FAILED){
        panic("couldn’t map video memory");
        return 1;
    }
    
    
    frame_buffer =  (char*) malloc(vram_size);

    memset(frame_buffer,0,vram_size);

    return 0;
}


int(video_get_mode_info)(uint16_t mode){


    mmap_t map;

    reg86_t r;

    lm_alloc(sizeof(vmi),&map);
    
    memset(&r, 0, sizeof(r));	/* zero the structure */
    
    r.ax = 0x4F01; // VBE call, function 02 -- set VBE mode
    r.es = PB2BASE(map.phys);
    r.di = PB2OFF(map.phys);
    r.cx = 1<<14|mode; // set bit 14: linear framebuffer
    r.intno = 0x10;

    if( sys_int86(&r) != OK ) {
        printf("set_vbe_mode: sys_int86() failed \n");
        lm_free(&map);
        return 1;
    }
    
    vbe_mode_info_t * pVmi;
    pVmi = map.virt;

    lm_free(&map);
    
    vmi = *pVmi;

    return 0;

}

int(video_buffer_to_vg)(){
    memcpy(video_mem,frame_buffer,vram_size);
    memset(frame_buffer,0,vram_size);
    return 0;
}
int(video_frame_to_buffer)(uint8_t *map,xpm_image_t img,uint16_t x,uint16_t y){
       
    uint16_t width = img.width;
    uint16_t height = img.height;
    if(vmi.XResolution<x) x = vmi.XResolution;
    if(vmi.YResolution <y) y = vmi.YResolution ;
    if (vmi.XResolution < width + x) width = vmi.XResolution-x; 
    if (vmi.YResolution < height + y) height = vmi.YResolution-y; 
    for(uint16_t i = 0; i < height; i++){
        for(uint16_t j = 0; j < width; j++){
            if(map[((i*width)+j)*bytesPerPixel]!=0)
                memcpy(&frame_buffer[((y+i)*vmi.XResolution + (x + j))*bytesPerPixel],&map[((i*width)+j)*bytesPerPixel],bytesPerPixel);
        }
    }
    return 0;
}

int(video_free_buffer)(){
    free(frame_buffer);
    return 0;
}

int (vg_draw_rectangle) (uint16_t x, uint16_t y,uint16_t width, uint16_t height,uint32_t color){  
   if( x + width > vmi.XResolution){
      width = vmi.XResolution -x;
    } 
    if( y + height > vmi.YResolution){
      height= vmi.YResolution -y;
    }  

    for(int j = y; j<y+height; j++){
      for(int i = x; i<x+width; i++){
            memcpy(&frame_buffer[(((vmi.XResolution * j) + i )* bytesPerPixel)],&color,bytesPerPixel);
      }
    }
    return 0;
}
