#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <imagen_vfs> <archivo1> [archivo2 ...]\n", argv[0]);
        return 1;
    }

    const char *image_path = argv[1];

    for (int i = 2; i < argc; ++i) {
        const char *filename = argv[i];

        if (!name_is_valid(filename)) {
            fprintf(stderr, "Nombre de archivo inválido: %s\n", filename);
            continue;
        }

        int inode_nbr = dir_lookup(image_path, filename);
        if (inode_nbr > 0) {
            // El archivo ya existe, no hacer nada (comportamiento estándar de touch)
            continue;
        }

        int new_inode = create_empty_file_in_free_inode(image_path, DEFAULT_PERM);
        if (new_inode < 0) {
            fprintf(stderr, "No se pudo crear el archivo: %s\n", filename);
            continue;
        }

        if (add_dir_entry(image_path, filename, new_inode) != 0) {
            fprintf(stderr, "No se pudo agregar la entrada de directorio para: %s\n", filename);
            // Opcional: podrías liberar el inodo si quieres limpiar
            continue;
        }
    }

    return 0;
}