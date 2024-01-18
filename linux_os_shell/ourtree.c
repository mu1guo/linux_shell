#include "ourtree.h"

void show_directory_structure_recursive(const char *path, int depth) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                for (int i = 0; i < depth - 1; i++)
                    printf("   |");
                printf("   |-- \033[1;34m%s/\033[0m\n", entry->d_name); // Blue color for directories
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
                show_directory_structure_recursive(new_path, depth + 1);
            }
        } else {
            for (int i = 0; i < depth - 1; i++)
                printf("   |");
            printf("   |-- %s\n", entry->d_name); // White color for regular files
        }
    }

    closedir(dir);
}

void show_directory_structure(const char *path) {
    printf("\033[1;33m%s/\033[0m\n", path); // Yellow color for the main directory
    show_directory_structure_recursive(path, 1);
}
