#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //comprobamos número de args
    if (argc < 3) {
        fprintf(stderr, "uso: %s <imagen_vfs> <archivo1> [archivo2 ...]\n", argv[0]);
        return 1;
    }

    const char *image_path = argv[1];

    for (int i = 2; i < argc; ++i) {
        const char *filename = argv[i];

        //validar nombre
        if (!name_is_valid(filename)) {
            fprintf(stderr, "nombre de archivo inválido: %s\n", filename);
            continue;
        }

        //si dir_lookup devuelve >0, el archivo ya existe
        int inode_nbr = dir_lookup(image_path, filename);
        if (inode_nbr > 0) {
            fprintf(stderr, "error: el archivo ya existe: %s\n", filename);
            continue;
        }

        //crear inodo libre
        int new_inode = create_empty_file_in_free_inode(image_path, DEFAULT_PERM);
        if (new_inode < 0) {
            fprintf(stderr, "error: no hay inodos libres para crear: %s\n", filename);
            continue;
        }

        //añadir al directorio
        if (add_dir_entry(image_path, filename, new_inode) != 0) {
            fprintf(stderr, "error: no se pudo agregar entrada de directorio para: %s\n", filename);
            continue;
        }

        //éxito
        printf("archivo creado: %s (inode %d)\n", filename, new_inode);
    }

    return 0;
}
