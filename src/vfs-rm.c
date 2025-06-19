//vfs-rm - elimina archivos regulares y libera recursos

#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){
    if(argc<3){
        fprintf(stderr,"uso: %s <imagen_vfs> <archivo1> [archivo2 ...]\n",argv[0]);
        return 1;
    }

    const char *img=argv[1];

    for(int i=2;i<argc;i++){
        const char *nombre=argv[i];
        int ino=dir_lookup(img,nombre);
        if(ino<=0){
            fprintf(stderr,"aviso: %s no existe\n",nombre);
            continue;
        }

        struct inode in;
        if(read_inode(img,ino,&in)!=0){
            fprintf(stderr,"error: no se pudo leer inodo de %s\n",nombre);
            continue;
        }

        if((in.mode&INODE_MODE_FILE)!=INODE_MODE_FILE){
            fprintf(stderr,"aviso: %s no es un archivo regular\n",nombre);
            continue;
        }

        if(inode_trunc_data(img,&in)!=0){
            fprintf(stderr,"error: fallo al truncar %s\n",nombre);
            continue;
        }

        if(write_inode(img,ino,&in)!=0){
            fprintf(stderr,"error: no se pudo guardar inodo de %s\n",nombre);
            continue;
        }

        if(free_inode(img,ino)!=0){
            fprintf(stderr,"error: no se pudo liberar inodo de %s\n",nombre);
            continue;
        }

        if(remove_dir_entry(img,nombre)!=0){
            fprintf(stderr,"error: no se pudo quitar la entrada de %s\n",nombre);
            continue;
        }
    }

    return 0;
}
