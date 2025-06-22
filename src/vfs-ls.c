#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <imagen_vfs>\n", argv[0]);
        return 1;
    }

    const char *image_path = argv[1];

    // Leer el inodo del directorio raíz
    struct inode root_inode;
    if (read_inode(image_path, ROOTDIR_INODE, &root_inode) != 0) {
        fprintf(stderr, "No se pudo leer el inode raíz\n");
        return 1;
    }

    // El directorio raíz está en el primer bloque de datos del inode raíz
    // Leemos todas las entradas del directorio raíz
    int num_entries = root_inode.size / sizeof(struct dir_entry);
    struct dir_entry entry;

    for (int i = 0; i < num_entries; ++i) {
        // Lee la i-ésima entrada del directorio raíz
        int offset = i * sizeof(struct dir_entry);
        if (inode_read_data(image_path, ROOTDIR_INODE, &entry, sizeof(struct dir_entry), offset) != sizeof(struct dir_entry))
            continue;

        if (entry.inode == 0)
            continue; // Entrada vacía

        struct inode file_inode;
        if (read_inode(image_path, entry.inode, &file_inode) != 0)
            continue;

        print_inode(&file_inode, entry.inode, entry.name);
    }

    return 0;
}