#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <alloca.h>
#include <stdint.h>
#include <unistd.h>

struct cmp_file {
    char *first_file;
    char *second_file;
} typedef cmp_file;

volatile unsigned int running_pthreads = 0,
        max_amount_of_pthread = 0;
pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER,
                add_mutex_lstat = PTHREAD_MUTEX_INITIALIZER;

void *cmp_two_files(void *data){

    cmp_file tmp_data = *((cmp_file*) data);
    struct stat info_first_file, info_second_file;
    
    if ((tmp_data.first_file == NULL) && (tmp_data.second_file == NULL)){
        fprintf(stderr, "Problem with data struct");
        pthread_cancel(pthread_self());
    }

    if (lstat(tmp_data.first_file, &info_first_file) == -1){
        fprintf(stderr, "lstat problem %s:\t%s Path of file: %s", tmp_data.first_file, strerror(errno), tmp_data.first_file);
        pthread_cancel(pthread_self());
    }
    
    if (lstat(tmp_data.second_file, &info_second_file) == -1){
        fprintf(stderr, "lstat problem %s:\t%s Path of file: %s", tmp_data.second_file, strerror(errno), tmp_data.second_file);
        pthread_cancel(pthread_self());
    }

    char report[100];

    if (info_second_file.st_size == info_first_file.st_size){

        FILE *fr_file = fopen(tmp_data.first_file, "r"),
                *sc_file = fopen(tmp_data.second_file, "r");
        if (fr_file == NULL){
            fprintf(stderr, "Error: can't open file %s", tmp_data.first_file,
                    strerror(errno));
            pthread_cancel(pthread_self());
        }

        if (sc_file == NULL){
            fprintf(stderr, "Error: can't open file %s", tmp_data.second_file,
                    strerror(errno));
            pthread_cancel(pthread_self());
        }

        intmax_t order_s = 0;
        char f_char, s_char;

        while (((f_char = fgetc(fr_file)) != EOF) && ((s_char = fgetc(sc_file)) != EOF)){
            if (f_char != s_char)
                break;
            ++order_s;
        }

        if (order_s != (intmax_t)info_first_file.st_size)
            snprintf(report, 100, "Files are different. Difference in %lld byte.\n", order_s);
        else
            snprintf(report, 100, "Files are equal. Size of files: %lld\n", order_s);

        fclose(fr_file), fclose(sc_file);
    }
    else {
        snprintf(report, 100, "Sizes of files are different. Size of first: %ld. Size of second: %ld\n", (intmax_t)info_first_file.st_size,
                 (intmax_t)info_second_file.st_size);
    }

    printf("ID: %lu  First name of file: %s  Second name of file: %s %s  ",
           pthread_self(),
           tmp_data.first_file,
           tmp_data.second_file,
           report);

    free(tmp_data.first_file), free(tmp_data.second_file);
    pthread_mutex_lock(&running_mutex);
    running_pthreads--;
    pthread_mutex_unlock(&running_mutex);
    pthread_exit(0);
}

char *cat_str(char *mem, char *f_part, char *m_part, char *fi_part){
    strcpy(mem, f_part);
    strcat(mem, "/");
    strcat(mem, fi_part);
    return mem;
}

char *iter_cycle(DIR *tmp_dir, char *name_dir){
    struct dirent *info_dir;
    struct stat info_file;

    char *result = malloc(strlen(name_dir) + NAME_MAX + 2);
    while ((info_dir = readdir(tmp_dir))){
        if (strcmp(".", info_dir->d_name) && strcmp("..", info_dir->d_name)){
            result = cat_str(result, name_dir,
                                          "/", info_dir->d_name);

            if (lstat(result, &info_file) == -1){
                fprintf(stderr, "Problem with lstat", strerror(errno));
                exit(EXIT_FAILURE);
            }
            if (!S_ISDIR(info_file.st_mode))
                return result;
        }
    }
    free (result);
    return NULL;
}

int get_pthread(char *first_file, char *second_file){
    pthread_t tid;
    pthread_attr_t attr;

    while (running_pthreads >= max_amount_of_pthread)
        sleep(1);

    // inc running_pthreads
    pthread_mutex_lock(&running_mutex);
    running_pthreads++;
    pthread_mutex_unlock(&running_mutex);

    //create cmp_data in heap
    cmp_file *data = malloc(sizeof (*data));
    data->first_file = malloc(sizeof (char*));
    data->second_file = malloc(sizeof (char *));

    // (*data).first_file = copy(first_file)
    char *tmp = malloc(strlen(first_file) + 1);
    strcpy(tmp, first_file);
    (*data).first_file = tmp;

    // (*data).second_file = copy(second_file)
    tmp = malloc(strlen(second_file) + 1);
    strcpy(tmp, second_file);
    (*data).second_file = tmp;

    pthread_attr_init(&attr);
    while (pthread_create(&tid, &attr, cmp_two_files, data) != 0){
        fprintf(stderr, "Problem with pthread_create", strerror(errno));
    }
}


int cmp_two_dir(char *first_name_of_dir, char *second_name_of_dir){
    DIR *first_dir = opendir(first_name_of_dir),
        *second_dir = opendir(second_name_of_dir);
    if (first_dir == NULL){
        fprintf(stderr, "%s: \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (second_dir == NULL){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *f_tmp_file, *s_tmp_file;

    while ((f_tmp_file = iter_cycle(first_dir, first_name_of_dir)) != NULL){
        while ((s_tmp_file = iter_cycle(second_dir, second_name_of_dir)) != NULL){
            get_pthread(f_tmp_file, s_tmp_file);
            free(s_tmp_file);
        }
        free(f_tmp_file);
        rewinddir(second_dir);
    }
    closedir(first_dir), closedir(second_dir);
}

int main(int argc, char** argv){
    // change to 4
    if (argc < 4) {
        fprintf(stderr, "usaging: %s <first_dir> <second_dir> <amount_of_thread>\n", argv[0], strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("First dir: %s Second dir: %s\n", argv[1], argv[2]);
    char *first_dir = argv[1], *second_dir = argv[2],
            *char_threads = argv[3], *endptr;

    errno = 0;
    max_amount_of_pthread = strtoll(char_threads, &endptr, 10);
    if (errno != 0){
        fprintf(stderr, "Error: number is incorrect %s\n", argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }
    if ((char_threads == endptr) || (endptr - argv[3] < strlen(char_threads))){
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }

    cmp_two_dir(first_dir, second_dir);

    while (running_pthreads > 0)
        sleep(1);

    return 0;
}