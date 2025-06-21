#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

#define BLOCK_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <vfs.img> <archivo1> [archivo2 ...]\n", argv[0]);
        return 1;
    }

    const char *vfs_img = argv[1];
    if (vfs_mount(vfs_img) != 0) {
        fprintf(stderr, "No se pudo montar la imagen VFS: %s\n", vfs_img);
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];
        int inode_num = dir_lookup(filename);

        if (inode_num < 0) {
            fprintf(stderr, "Archivo no encontrado: %s\n", filename);
            continue;
        }

        struct inode file_inode;
        if (read_inode(inode_num, &file_inode) != 0) {
            fprintf(stderr, "No se pudo leer el inode de: %s\n", filename);
            continue;
        }

        if (file_inode.size == 0) {
            // Archivo vacío, no imprime nada
            continue;
        }

        char buffer[BLOCK_SIZE];
        int bytes_read = 0;
        int total_read = 0;
        while (total_read < file_inode.size) {
            int to_read = file_inode.size - total_read;
            if (to_read > BLOCK_SIZE) to_read = BLOCK_SIZE;

            bytes_read = inode_read_data(&file_inode, total_read, buffer, to_read);
            if (bytes_read <= 0) break;

            fwrite(buffer, 1, bytes_read, stdout);
            total_read += bytes_read;
        }
    }

    vfs_umount();
    return 0;