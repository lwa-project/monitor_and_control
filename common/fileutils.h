#ifndef MCS_FILEUTILS_H  /* this keeps this header from getting rolled in more than once */
#define MCS_FILEUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

int LWA_move_file(char *src, char *dest) {
    /* move a file from src to dest, returns 0 on success */
    if (src == NULL || dest == NULL) {
        return 1;
        }

    if (rename(src, dest) == 0) {
        return 0;
        }

    int ret;
    pid_t pid = fork();
    
    if (pid == 0) {
        execl("/bin/mv", "/bin/mv", src, dest, NULL);
        _exit(1);
    } else if (pid > 0) {
         pid_t ws = waitpid(pid, &ret, 0);
        if (ws == -1) {
            return 2;
        }
        
        if (WIFEXITED(ret)) {
            return WEXITSTATUS(ret);
        } else if (WIFSIGNALED(ret)) {
            return 3;
        } else if (WIFSTOPPED(ret)) {
            return 4;
        }
    } else {
      return 2;
      }
    
    return 5;
}

int LWA_copy_file(char *src, char *dest) {
    /* copy a file from src to dest, returns 0 on success */
    if (src == NULL || dest == NULL) {
        return 1;
        }

    int ret;
    pid_t pid = fork();
    
    if (pid == 0) {
        execl("/bin/cp", "/bin/cp", src, dest, NULL);
        _exit(1);
    } else if (pid > 0) {
         pid_t ws = waitpid(pid, &ret, 0);
        if (ws == -1) {
            return 2;
        }
        
        if (WIFEXITED(ret)) {
            return WEXITSTATUS(ret);
        } else if (WIFSIGNALED(ret)) {
            return 3;
        } else if (WIFSTOPPED(ret)) {
            return 4;
        }
    } else {
      return 2;
      }
    
    return 5;
}

int LWA_remove_file(char *filename) {
  /* remove a file, returns 0 on success */
  if (filename == NULL) {
      return 1;
      }

   return unlink(filename);
}

int LWA_remove_tree(char *dirname) {
    /* recursively remove a directory and its contents, returns 0 on success */
    if (dirname == NULL) {
        return 1;
        }

    int ret;
    pid_t pid = fork();
  
    if (pid == 0) {
        execl("/bin/rm", "/bin/rm", "-r", "-f", dirname, NULL);
        _exit(1);
    } else if (pid > 0) {
        pid_t ws = waitpid(pid, &ret, 0);
        if (ws == -1) {
            return 2;
        }
        
        if (WIFEXITED(ret)) {
            return WEXITSTATUS(ret);
        } else if (WIFSIGNALED(ret)) {
            return 3;
        } else if (WIFSTOPPED(ret)) {
            return 4;
        }
    } else {
      return 2;
      }
    
    return 5;
}

int LWA_empty_tree(char *dirname) {
    /* recursively empty out a directory (but leave the directory itself), return 0 on success */
    if (dirname == NULL) {
        return 1;
        }

    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        return 1;
        }

    int overall_status = 0, status;
    struct dirent *entry;
    char *path = malloc(PATH_MAX);
    if (path == NULL) {
        closedir(dir);
        return 1;
        }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
            }    
          
        int ret = snprintf(path, PATH_MAX, "%s/%s", dirname, entry->d_name);
        if (ret >= PATH_MAX) {
            overall_status |= 2;
            continue;
            }
          
         status = LWA_remove_tree(path);
         if (status) {
             overall_status |= 4;
             }
          }

    closedir(dir);
    free(path);
    return overall_status;
}

int LWA_make_directory(char *dirname) {
  /* make a new directory, return 0 on success */
  if (dirname == NULL) {
      return 1;
      }

      return mkdir(dirname, 0755);
}

int LWA_make_tree(char *dirname) {
    /* recursively make a directory tree (like "mkdir -p"), returns 0 on success */
    if (dirname == NULL) {
        return 1;
        }

    int ret;
    pid_t pid = fork();
    
    if (pid == 0) {
        execl("/bin/mkdir", "/bin/mkdir", "-m", "755", "-p", dirname, NULL);
        _exit(1);
    } else if (pid > 0) {
         pid_t ws = waitpid(pid, &ret, 0);
        if (ws == -1) {
            return 2;
        }
        
        if (WIFEXITED(ret)) {
            return WEXITSTATUS(ret);
        } else if (WIFSIGNALED(ret)) {
            return 3;
        } else if (WIFSTOPPED(ret)) {
            return 4;
        }
    } else {
      return 2;
      }
    
    return 5;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef MCS_FILEUTILS_H 

//==================================================================================
//=== HISTORY ======================================================================
//==================================================================================
// Jul 8, 2025: Initial version
