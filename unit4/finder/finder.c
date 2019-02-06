// Recursively searches for a query in a directory.

#define _BSD_SOURCE
#define _GNU_SOURCE

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAXLENGTH 50

// Struct to hold the file name and type
typedef struct
{
    string name;
    string type;
}
path;

// Struct to hold the directory info
typedef struct
{
    string name;
    int npaths;
    path *paths;
}
directory;

// String to hold the word to seek
string key;

// The function to search for files in a directory and populate the struct
directory populate(directory dir);

// The function to recursively iterate through directories and handle files
int seek(directory dir);


int main(int argc, char *argv[])
{

    // Create initial directory and set name string
    directory dir;
    dir.name = "./";

    // check argumets
    if (argc == 1 || argc > 3 || (argc == 3 && argv[2][strlen(argv[2]) - 1] != '/'))
    {
        printf("Uses: ./finder search [dir/]\n");
        return 1;
    }
    // if 2 parametrs
    else if (argc == 3)
    {
        dir.name = argv[2];
    }
    key = argv[1];

    return seek(dir);
}

directory populate(directory dir)
{
    // Initialize all pointers and values in the given struct
    dir.npaths = 0;
    dir.paths = NULL;
    DIR *dirp;
    struct dirent *entry;

    dirp = opendir(dir.name);
    if (dirp == NULL)
    {
        printf("Opening directory failed. Check your input filepath!\n");
        return dir;
    }

    while ((entry = readdir(dirp)) != NULL)
    {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // Allocate zeroed-out memory for the construction of the file name
            string name = calloc(1, strlen(dir.name) + strlen(entry->d_name) + 2);
            strcat(name, dir.name);
            strcat(name, entry->d_name);
            strcat(name, "/");

            // Reallocate memory to expand the array
            dir.paths = realloc(dir.paths, (dir.npaths + 1) * sizeof(path));

            // Add a new element to the array containing names and types
            path newPath = {.name = name, .type = "directory"};
            dir.paths[dir.npaths] = newPath;

            // Increase file count for the directory
            dir.npaths++;
        }

        // Else if entry is a file, increase file count and populate the struct
        else if (entry->d_type == DT_REG)
        {
            // Allocate zeroed-out memory for the construction of the file name
            string name = calloc(1, strlen(dir.name) + strlen(entry->d_name) + 1);
            strcat(name, dir.name);
            strcat(name, entry->d_name);

            // Reallocate memory to expand the array
            dir.paths = realloc(dir.paths, (dir.npaths + 1) * sizeof(path));

            // Add a new element to the array containing names and types
            path newPath = {.name = name, .type = "file"};
            dir.paths[dir.npaths] = newPath;

            // Increase file count for the directory
            dir.npaths++;
        }
    }
    // Close directory stream using system call closedir and return struct
    closedir(dirp);
    return dir;
}

// Recursive function to iterate through directories and search files
int seek(directory dir)
{
    // initial variables
    dir = populate(dir);
    char filetext[50];

    // open found file
    FILE *found = fopen("./staff_found.txt", "a");

    // if file not opened
    if (found == NULL)
    {
        return -1;
    }

    // if directory not empty
    if (dir.npaths > 0)
    {
        // look in all path name
        for (int i = 0; i < dir.npaths; i++, dir.paths++)
        {
            // if directory
            if (strcmp(dir.paths -> type, "directory") == 0)
            {
                dir.name = dir.paths -> name;
                seek(dir);
            }
            // if file
            else if (strcmp(dir.paths -> type, "file") == 0)
            {
                // look in file
                FILE *fp = fopen(dir.paths->name, "r");

                // if file opened
                if (fp != NULL)
                {
                    // while not end of file
                    while (fgets(filetext, 50, fp) != NULL)
                    {
                        // look for key in file
                        if (strstr(filetext, key) != NULL)
                        {
                            // write to staff_found.txt if file is found
                            fprintf(found, "%s\n", dir.paths->name);
                        }
                    }
                }
                // close fp file
                fclose(fp);
            }
        }
    }
    // close found file
    fclose(found);
    return -1;
}
