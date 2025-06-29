#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

#define BLOCK_SIZE 1024

int main(int argc, char *argv[]) {
    //comprobamos número de argumentos
    if (argc < 3) {
        fprintf(stderr, "uso: %s <vfs.img> <archivo1> [archivo2 ...]\n", argv[0]);
        return 1;
    }

    const char *vfs_img = argv[1];
    //recorremos cada nombre de archivo pasado
    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];
        //buscamos el inode correspondiente al nombre
        int inode_num = dir_lookup(vfs_img, filename);
        if (inode_num < 0) {
            fprintf(stderr, "archivo no encontrado: %s\n", filename);
            continue;
        }

        struct inode file_inode;
        //leemos metadata del inode
        if (read_inode(vfs_img, inode_num, &file_inode) != 0) {
            fprintf(stderr, "no se pudo leer el inode de: %s\n", filename);
            continue;
        }

        //si está vacío, nada que mostrar
        if (file_inode.size == 0) {
            continue;
        }

        char buffer[BLOCK_SIZE];
        int total_read = 0;
        //leemos en bloques hasta completar el tamaño
        while (total_read < (int)file_inode.size) {
            int to_read = file_inode.size - total_read;
            if (to_read > BLOCK_SIZE) to_read = BLOCK_SIZE;

            int bytes_read = inode_read_data(
                vfs_img,
                inode_num,
                buffer,
                to_read,
                total_read
            );
            if (bytes_read <= 0) break;

            fwrite(buffer, 1, bytes_read, stdout);
            total_read += bytes_read;
        }
    }

    //fin del programa
    return 0;
}
