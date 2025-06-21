//vfs-lsort - lista alfabetica de archivos en el directorio raiz

#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct listado {
    struct inode info;
    uint32_t num;
    char nombre[FILENAME_MAX_LEN];
};

static int cmp(const void *a,const void *b){
    const struct listado *x=a,*y=b;
    return strncmp(x->nombre,y->nombre,FILENAME_MAX_LEN);
}

int main(int argc,char *argv[]){
    if(argc!=2){
        fprintf(stderr,"uso: %s <imagen_vfs>\n",argv[0]);
        return 1;
    }

    const char *img=argv[1];
    struct inode root;
    if(read_inode(img,ROOTDIR_INODE,&root)!=0){
        fprintf(stderr,"error: no se pudo leer el inodo raiz\n");
        return 1;
    }

    int capacidad=root.blocks*DIR_ENTRIES_PER_BLOCK;
    struct listado *vec=calloc(capacidad,sizeof(struct listado));
    if(!vec){
        fprintf(stderr,"error: memoria insuficiente\n");
        return 1;
    }

    int n=0;
    for(uint16_t b=0;b<root.blocks;b++){
        int blk=get_block_number_at(img,&root,b);
        if(blk<=0) continue;

        uint8_t buf[BLOCK_SIZE];
        if(read_block(img,blk,buf)!=0) continue;

        struct dir_entry *ents=(struct dir_entry*)buf;
        for(uint32_t j=0;j<DIR_ENTRIES_PER_BLOCK;j++){
            if(ents[j].inode==0) continue;
            if(read_inode(img,ents[j].inode,&vec[n].info)!=0) continue;
            vec[n].num=ents[j].inode;
            strncpy(vec[n].nombre,ents[j].name,FILENAME_MAX_LEN);
            n++;
        }
    }

    qsort(vec,n,sizeof(struct listado),cmp);
    for(int i=0;i<n;i++){
        print_inode(&vec[i].info,vec[i].num,vec[i].nombre);
    }

    free(vec);
    return 0;
}