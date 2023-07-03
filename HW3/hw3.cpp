#include "parser.c"
#include "helpers.cpp"
#include "helpers.h"
#include "parser.h"
#include "fat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
using namespace std;

int main(int argc, char * argv[]) {
    int last_path_dir_count = 0;
    string last_path[256];

    FILE *fd = NULL;
    char *buffer = NULL;
    size_t characters;
    size_t buf_size = 256;
    char *input_buffer = NULL;
    BPB_struct bpbStruct;
    string current_direct;
    current_direct.append("/> ");
    parsed_input *parsed = (parsed_input *) malloc(sizeof(parsed_input));
    union struct_FatFileEntry *fatFileEntry = new FatFileEntry;
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }
    fd = fopen(argv[1], "rb");
    if (!fd) {
        fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
        return -1;
    }
    fread(&bpbStruct, sizeof(bpbStruct), 1, fd);
    while (1) {
        cout << current_direct;
        getline(&input_buffer, &buf_size, stdin);
        parse(parsed, input_buffer);
        if (parsed->type == QUIT) {
            break;
        } else {
            switch (parsed->type) {
                case CD: {
                    int *yazildi_mi=new int ;
                    int*kalinan_yer=new int;
                    *kalinan_yer=0;
                    string* looked_file_name=new string ;
                    uint32_t *nekadari_yazildi=new uint32_t ;
                    uint32_t temp_for_cluster;
                    *yazildi_mi=0;
                    *nekadari_yazildi=0;
                    char *a = new char;
                    int next_cluster = 0, count_temp = last_path_dir_count;
                    string path_temp[256];
                    assignPaths(last_path, path_temp, last_path_dir_count);
                    if (parsed->arg1[0] == '/') {
                        last_path_dir_count = 0;
                    }
                    last_path_dir_count = pathFounder(parsed, last_path, last_path_dir_count, 0);
                    if (last_path_dir_count == -1) {
                        last_path_dir_count = count_temp;
                    }

                    for (int i = 0; i < last_path_dir_count; i++) {
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int old_cluster=0;
                        uint32_t older=next_cluster;
                        for (int j = 0; j < cluster_count; j++) {
                            if(*yazildi_mi==0){
                                temp_for_cluster=old_cluster;
                                old_cluster = next_cluster;
                            }

                            next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, a,yazildi_mi,nekadari_yazildi,looked_file_name,kalinan_yer,older);
                            if (next_cluster != -3) {
                                break;
                            }
                            if(next_cluster==-3 && *yazildi_mi==1){
                                next_cluster=temp_for_cluster;

                                j--;
                                continue;
                            }
                            else if(next_cluster==-3 && *nekadari_yazildi!=0){
                                j--;
                                next_cluster = old_cluster;
                            }
                            else if (j != cluster_count - 1) {
                                next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                            }
                        }

                        if (next_cluster == -3) {
                            last_path_dir_count = count_temp;
                            assignPaths(path_temp, last_path, last_path_dir_count);
                            break;
                        }
                    }
                    string result_string;
                    result_string = writePath(last_path_dir_count, last_path);
                    current_direct = result_string;
                    /*if(a!=NULL){
                        delete(a);
                    }
                    if(kalinan_yer!=NULL){
                        delete(kalinan_yer);
                    }
                    if(looked_file_name!=NULL){
                        delete(looked_file_name);
                    }
                    if(nekadari_yazildi!=NULL){
                        delete(nekadari_yazildi);
                    }
                    if(yazildi_mi!=NULL){
                        delete(yazildi_mi);
                    }*/
                    break;
                }


                case LS: {
                    int*kalinan_yer4=new int;
                    *kalinan_yer4=0;
                    int *yazildi_mi4=new int ;
                    string* looked_file_name4=new string ;
                    uint32_t *nekadari_yazildi4=new uint32_t ;
                    *yazildi_mi4=0;
                    *nekadari_yazildi4=0;
                    char *seq_number4 = new char;
                    string all_content[2500];
                    if (parsed->arg1 == NULL) {
                        int next_cluster = 0;
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int old_cluster = 0;
                        int temp_for_cluster=0;
                        for (int i = 0; i < last_path_dir_count; i++) {
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster=0;
                            uint32_t older=next_cluster;
                            for (int j = 0; j < cluster_count; j++) {
                                if(*yazildi_mi4==0){
                                    temp_for_cluster=old_cluster;
                                    old_cluster = next_cluster;
                                }

                                next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number4,yazildi_mi4,nekadari_yazildi4,looked_file_name4,kalinan_yer4,older);
                                if (next_cluster != -3) {
                                    break;
                                }
                                if(next_cluster==-3 && *yazildi_mi4==1){
                                    next_cluster=temp_for_cluster;

                                    j--;
                                    continue;
                                }
                                else if(next_cluster==-3 && *nekadari_yazildi4!=0){
                                    j--;
                                    next_cluster = old_cluster;
                                }
                                else if (j != cluster_count - 1) {
                                    next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                }
                            }
                        }
                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int index;
                        int *left4 = new int;
                        *left4 = 0;
                        int* returner4=new int;
                        returner4=0;
                        *kalinan_yer4=0;
                        *yazildi_mi4=0;
                        *nekadari_yazildi4=0;
                        uint32_t ilk_cluster=next_cluster;
                        looked_file_name4->clear();
                        for (int j = 0; j < cluster_count; j++) {
                            if(*yazildi_mi4==0){
                                temp_for_cluster=old_cluster;
                                old_cluster = next_cluster;
                            }
                            index =  findAllSubdir(all_content, fd, bpbStruct, next_cluster, seq_number4, j, left4,
                                                   last_path_dir_count,yazildi_mi4,nekadari_yazildi4,kalinan_yer4,old_cluster,returner4,looked_file_name4,ilk_cluster);
                            if(index==-3 && *yazildi_mi4==1){
                                next_cluster=temp_for_cluster;

                                j--;
                                continue;
                            }
                            if(index==-3 && *nekadari_yazildi4!=0){
                                j--;
                                next_cluster = old_cluster;
                            }
                            else{
                                next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                            }

                        }
                        saltlsprinter(all_content, index);
                        /*if(left4!=NULL){
                            delete(left4);
                        }
                        if(returner4!=NULL){
                            delete(returner4);
                        }*/

                    } else if (!strcmp(parsed->arg1, "-l")) {
                        char*a =new char;
                        if (parsed->arg2 == NULL) {
                            int next_cluster = 0;
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster = 0;
                            for (int i = 0; i < last_path_dir_count; i++) {
                                for (int j = 0; j < cluster_count; j++) {
                                    old_cluster = next_cluster;
                                    next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number4,yazildi_mi4,nekadari_yazildi4,looked_file_name4,kalinan_yer4,0);
                                    if (next_cluster != -3) {
                                        break;
                                    }
                                    if (j != cluster_count - 1) {
                                        next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                    }
                                }
                            }
                            int index;
                            int *left = new int;
                            *left = 0;
                            for (int j = 0; j < cluster_count; j++) {
                                (*left) = findAllSubdirForTireL(all_content, fd, bpbStruct, next_cluster, a, j,
                                                                left, last_path_dir_count);
                                next_cluster = get_next(argv[1], bpbStruct, next_cluster);
                            }
                            index = *left;
                            tirelsprinter(all_content, index);
                            free(left);
                        } else {
                            char* a=new char;
                            int count_temp = last_path_dir_count, return_fake = 0;
                            string path_temp[256];
                            assignPaths(last_path, path_temp, last_path_dir_count);
                            return_fake = fakeCD(&last_path_dir_count, last_path, parsed, fd, bpbStruct, 1, argv[1]);
                            if (return_fake == -1) {
                                break;
                            }
                            int next_cluster = 0;
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster = 0;
                            for (int i = 0; i < last_path_dir_count; i++) {
                                for (int j = 0; j < cluster_count; j++) {
                                    old_cluster = next_cluster;
                                    next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number4,yazildi_mi4,nekadari_yazildi4,looked_file_name4,kalinan_yer4,0);
                                    if (next_cluster != -3) {
                                        break;
                                    }
                                    if (j != cluster_count - 1) {
                                        next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                    }
                                }
                            }
                            cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int index;
                            int *left = new int;
                            *left = 0;
                            for (int j = 0; j < cluster_count; j++) {
                                (*left) = findAllSubdirForTireL(all_content, fd, bpbStruct, next_cluster, a, j,
                                                                left, last_path_dir_count);
                                next_cluster = get_next(argv[1], bpbStruct, next_cluster);
                            }
                            index = *left;
                            tirelsprinter(all_content, index);
                            last_path_dir_count = count_temp;
                            assignPaths(path_temp, last_path, last_path_dir_count);
                            free(left);
                        }
                    } else {
                        int count_temp = last_path_dir_count, return_fake = 0;
                        string path_temp[256];
                        assignPaths(last_path, path_temp, last_path_dir_count);
                        return_fake = fakeCD(&last_path_dir_count, last_path, parsed, fd, bpbStruct, 0, argv[1]);
                        if (return_fake == -1) {
                            /*if(seq_number4!=NULL){
                                delete(seq_number4);
                            }*/
                            break;
                        }
                        int next_cluster = 0;
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int old_cluster = 0;
                        int temp_for_cluster=0;
                        for (int i = 0; i < last_path_dir_count; i++) {
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster=0;
                            uint32_t older=next_cluster;
                            for (int j = 0; j < cluster_count; j++) {
                                if(*yazildi_mi4==0){
                                    temp_for_cluster=old_cluster;
                                    old_cluster = next_cluster;
                                }

                                next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number4,yazildi_mi4,nekadari_yazildi4,looked_file_name4,kalinan_yer4,older);
                                if (next_cluster != -3) {
                                    break;
                                }
                                if(next_cluster==-3 && *yazildi_mi4==1){
                                    next_cluster=temp_for_cluster;

                                    j--;
                                    continue;
                                }
                                else if(next_cluster==-3 && *nekadari_yazildi4!=0){
                                    j--;
                                    next_cluster = old_cluster;
                                }
                                else if (j != cluster_count - 1) {
                                    next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                }
                            }
                        }
                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int index;
                        int *left = new int;
                        *left = 0;
                        int* returner4=new int;
                        returner4=0;
                        *kalinan_yer4=0;
                        *yazildi_mi4=0;
                        *nekadari_yazildi4=0;
                        looked_file_name4->clear();
                        int ilk_cluster=next_cluster;
                        for (int j = 0; j < cluster_count; j++) {
                            if(*yazildi_mi4==0){
                                temp_for_cluster=old_cluster;
                                old_cluster = next_cluster;
                            }
                            index = findAllSubdir(all_content, fd, bpbStruct, next_cluster, seq_number4, j, left,
                                                  last_path_dir_count,yazildi_mi4,nekadari_yazildi4,kalinan_yer4,old_cluster,returner4,looked_file_name4,ilk_cluster);
                            if(index==-3 && *yazildi_mi4==1){
                                next_cluster=temp_for_cluster;

                                j--;
                                continue;
                            }
                            else if(index==-3 && *nekadari_yazildi4!=0){
                                j--;
                                next_cluster = old_cluster;
                            }
                            else{
                                next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                            }

                        }
                        saltlsprinter(all_content, index);
                        last_path_dir_count = count_temp;
                        assignPaths(path_temp, last_path, last_path_dir_count);
                        /*if(left!=NULL) delete(left);*/
                    }
                   /* if(yazildi_mi4!=NULL){
                        delete(yazildi_mi4);
                    }
                    if(nekadari_yazildi4!=NULL){
                        delete(nekadari_yazildi4);
                    }
                    if(seq_number4!=NULL){
                        delete(seq_number4);
                    }
                    if(looked_file_name4!=NULL){
                        delete(looked_file_name4);
                    }
                    if(kalinan_yer4!=NULL){
                        delete(kalinan_yer4);
                    }*/
                    break;
                }
                case MKDIR: {
                    int*kalinan_yer1=new int;
                    *kalinan_yer1=0;
                    int *yazildi_mi1=new int ;
                    string* looked_file_name1=new string ;
                    uint32_t *nekadari_yazildi1=new uint32_t ;
                    *yazildi_mi1=0;
                    *nekadari_yazildi1=0;
                    char *seq_number1 = new char;
                    string search_this[4000];
                    int path_count;
                    path_count = pathFounder(parsed, last_path, last_path_dir_count, 0);
                    int is_cluster_enought;
                    int *is_finished1=new int;
                    int *yazilan_kisim1=new int;
                    *yazilan_kisim1=0;
                    *is_finished1=0;
                    uint32_t ilk_cluster;
                    if (path_count == last_path_dir_count + 1) {
                        int next_cluster = 0;
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t old_cluster = 0;
                        old_cluster = next_cluster;
                        ilk_cluster=next_cluster;
                        int temp_for_cluster=0;
                        for (int i = 0; i < last_path_dir_count; i++) {
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster = 0;
                            uint32_t older=next_cluster;
                            for (int j = 0; j < cluster_count; j++) {
                                if (*yazildi_mi1 == 0) {
                                    temp_for_cluster = old_cluster;
                                    old_cluster = next_cluster;
                                }

                                next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number1, yazildi_mi1,
                                                             nekadari_yazildi1, looked_file_name1, kalinan_yer1,older);
                                if (next_cluster != -3) {
                                    break;
                                }
                                if (next_cluster == -3 && *yazildi_mi1 == 1) {
                                    next_cluster = temp_for_cluster;

                                    j--;
                                    continue;
                                }
                                else if (next_cluster == -3 && *nekadari_yazildi1 != 0) {
                                    j--;
                                    next_cluster = old_cluster;
                                }
                                else if (j != cluster_count - 1) {
                                    next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                }
                            }
                        }
                        uint32_t temperer=next_cluster;
                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int index;
                        string all_content[25000];
                        int *left = new int;
                        *left = 0;
                        int* returner4=new int;
                        returner4=0;
                        char*seq_number4=new char ;
                        int*kalinan_yer4= new int ;
                        *kalinan_yer4=0;
                        int*yazildi_mi4=new int ;
                        *yazildi_mi4=0;
                        uint32_t *nekadari_yazildi4=new uint32_t ;
                        *nekadari_yazildi4=0;
                        string *looked_file_name4=new string ;
                        looked_file_name4->clear();
                        for (int j = 0; j < cluster_count; j++) {
                            if(*yazildi_mi4==0){
                                temp_for_cluster=old_cluster;
                                old_cluster = next_cluster;
                            }
                            index = findAllSubdir(all_content, fd, bpbStruct, next_cluster, seq_number4, j, left,
                                                  last_path_dir_count,yazildi_mi4,nekadari_yazildi4,kalinan_yer4,old_cluster,returner4,looked_file_name4,ilk_cluster);
                            if(index==-3 && *yazildi_mi4==1){
                                next_cluster=temp_for_cluster;

                                j--;
                                continue;
                            }
                            if(index==-3 && *nekadari_yazildi4!=0){
                                j--;
                                next_cluster = old_cluster;
                            }
                            else{
                                next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                            }

                        }
                        for(int i=0;i<index;i++){

                            if(all_content[i]==last_path[last_path_dir_count]){
                                goto same;
                            }
                        }
                        next_cluster=temperer;
                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t * tilde_counter=new uint32_t ;
                        *tilde_counter=0;
                        for (int j = 0; j < cluster_count; j++) {
                            if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count],ilk_cluster,tilde_counter);
                            }

                            if (is_cluster_enought != -3) { // check fat and cont.
                                fclose(fd);
                                fd = fopen(argv[1], "rb");
                                break;
                            }
                            if (j != cluster_count - 1) {
                                next_cluster = get_next(argv[1], bpbStruct, next_cluster);
                            }
                        }
                        if (is_cluster_enought == -3) { //yeni cluster yapıp ekle
                            next_cluster = find_cluster1(argv[1], next_cluster, bpbStruct);
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                            if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count],ilk_cluster,tilde_counter);
                            }
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                        }
                        /*if(tilde_counter!=NULL){
                            delete(tilde_counter);
                        }*/

                    }
                    else {
                        int count_temp = last_path_dir_count, return_fake = 0;
                        uint32_t ilk_cluster=0;
                        string path_temp[256];
                        assignPaths(last_path, path_temp, last_path_dir_count);
                        return_fake = fakeCD(&last_path_dir_count, last_path, parsed, fd, bpbStruct, 0, argv[1]);
                        if (return_fake == -1) {
                          /*  if(seq_number1!=NULL){
                                delete(seq_number1);
                            }*/
                            break;
                        }
                        int next_cluster = next_cluster;
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t old_cluster = 0;
                            int temp_for_cluster=0;
                        for (int i = 0; i < last_path_dir_count; i++) {
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster=0;
                            uint32_t older=next_cluster;
                            for (int j = 0; j < cluster_count; j++) {
                                if(*yazildi_mi1==0){
                                    temp_for_cluster=old_cluster;
                                    old_cluster = next_cluster;
                                }

                                next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number1,yazildi_mi1,nekadari_yazildi1,looked_file_name1,kalinan_yer1,older);
                                if (next_cluster != -3) {
                                    break;
                                }
                                if(next_cluster==-3 && *yazildi_mi1==1){
                                    next_cluster=temp_for_cluster;

                                    j--;
                                    continue;
                                }
                                else if(next_cluster==-3 && *nekadari_yazildi1!=0){
                                    j--;
                                    next_cluster = old_cluster;
                                }
                                else if (j != cluster_count - 1) {
                                    next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                }
                            }
                        }

                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t *tilde_counter=new uint32_t ;
                        *tilde_counter=0;
                        for (int j = 0; j < cluster_count; j++) {
                            if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count],ilk_cluster,tilde_counter);
                            }

                            if (is_cluster_enought != -3) { // check fat and cont.
                                fclose(fd);
                                fd = fopen(argv[1], "rb");
                                break;
                            }
                            if (j != cluster_count - 1) {
                                next_cluster = get_next(argv[1], bpbStruct, next_cluster);
                            }
                        }
                        if (is_cluster_enought == -3) { //yeni cluster yapıp ekle
                            next_cluster = find_cluster1(argv[1], next_cluster, bpbStruct);
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                            if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetFreeInDataRegion(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number1,
                                                                                last_path[last_path_dir_count],ilk_cluster,tilde_counter);
                            }
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                           /* if(tilde_counter!=NULL) delete(tilde_counter);*/
                        }
                        last_path_dir_count = count_temp;
                        assignPaths(path_temp, last_path, last_path_dir_count);

                    }
                    same:
                    /*
                    if(seq_number1!= NULL){
                        delete(seq_number1);
                    }
                    if(yazilan_kisim1!= NULL){
                        delete(yazilan_kisim1);
                    }
                    if(is_finished1!= NULL){
                        delete(is_finished1);
                    }
                    if(yazildi_mi1!= NULL){
                        delete(yazildi_mi1);
                    }
                    if(nekadari_yazildi1!= NULL){
                        delete(nekadari_yazildi1);
                    }
                    if(looked_file_name1!= NULL){
                        delete(looked_file_name1);
                    }
                    if(kalinan_yer1!= NULL){
                        delete(kalinan_yer1);
                    }
                    */
                    break;
                }
                case TOUCH: {
                    int*kalinan_yer2=new int;
                    *kalinan_yer2=0;
                    int *yazildi_mi2=new int ;
                    string* looked_file_name2=new string ;
                    uint32_t *nekadari_yazildi2=new uint32_t ;
                    *yazildi_mi2=0;
                    *nekadari_yazildi2=0;
                    char *seq_number2 = new char;
                    string search_this[256];
                    int path_count;
                    path_count = pathFounder(parsed, last_path, last_path_dir_count, 0);
                    int is_cluster_enought;
                    int *is_finished2=new int;
                    int *yazilan_kisim2=new int;
                    *yazilan_kisim2=0;
                    *is_finished2=0;
                    uint32_t ilk_cluster;
                    if (path_count == last_path_dir_count + 1) {
                        int next_cluster = 0;
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t old_cluster = 0;
                        old_cluster = next_cluster;
                        ilk_cluster=next_cluster;
                        int temp_for_cluster=0;
                        for (int i = 0; i < last_path_dir_count; i++) {
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster = 0;
                            uint32_t older=next_cluster;
                            for (int j = 0; j < cluster_count; j++) {
                                if (*yazildi_mi2 == 0) {
                                    temp_for_cluster = old_cluster;
                                    old_cluster = next_cluster;
                                }

                                next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number2, yazildi_mi2,
                                                             nekadari_yazildi2, looked_file_name2, kalinan_yer2,older);
                                if (next_cluster != -3) {
                                    break;
                                }
                                if (next_cluster == -3 && *yazildi_mi2 == 1) {
                                    next_cluster = temp_for_cluster;

                                    j--;
                                    continue;
                                }
                                else  if (next_cluster == -3 && *nekadari_yazildi2 != 0) {
                                    j--;
                                    next_cluster = old_cluster;
                                }
                                else if (j != cluster_count - 1) {
                                    next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                }
                            }
                        }

                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t * tilde_counter=new uint32_t ;
                        *tilde_counter=0;
                        for (int j = 0; j < cluster_count; j++) {
                           if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            }

                            if (is_cluster_enought != -3) { // check fat and cont.
                                fclose(fd);
                                fd = fopen(argv[1], "rb");
                                break;
                            }
                            if (j != cluster_count - 1) {
                                next_cluster = get_next(argv[1], bpbStruct, next_cluster);
                            }
                        }
                        if (is_cluster_enought == -3) { //yeni cluster yapıp ekle
                            next_cluster = find_cluster1(argv[1], next_cluster, bpbStruct);
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                            uint32_t * tilde_counter=new uint32_t ;
                            *tilde_counter=0;
                            if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            }
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                        }

                    } else {
                        int count_temp = last_path_dir_count, return_fake = 0;
                        uint32_t ilk_cluster=0;
                        string path_temp[256];
                        assignPaths(last_path, path_temp, last_path_dir_count);
                        return_fake = fakeCD(&last_path_dir_count, last_path, parsed, fd, bpbStruct, 0, argv[1]);
                        if (return_fake == -1) {
                           /* if(seq_number2!=NULL) delete(seq_number2);*/
                            break;
                        }
                        int next_cluster = next_cluster;
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t old_cluster = 0;
                        int temp_for_cluster=0;
                        for (int i = 0; i < last_path_dir_count; i++) {
                            int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                            int old_cluster=0;
                            uint32_t older=next_cluster;
                            for (int j = 0; j < cluster_count; j++) {
                                if(*yazildi_mi2==0){
                                    temp_for_cluster=old_cluster;
                                    old_cluster = next_cluster;
                                }

                                next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number2,yazildi_mi2,nekadari_yazildi2,looked_file_name2,kalinan_yer2,older);
                                if (next_cluster != -3) {
                                    break;
                                }
                                if(next_cluster==-3 && *yazildi_mi2==1){
                                    next_cluster=temp_for_cluster;

                                    j--;
                                    continue;
                                }
                                else  if(next_cluster==-3 && *nekadari_yazildi2!=0){
                                    j--;
                                    next_cluster = old_cluster;
                                }
                                else if (j != cluster_count - 1) {
                                    next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                                }
                            }
                        }
                        cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        uint32_t * tilde_counter=new uint32_t ;
                        *tilde_counter=0;
                        for (int j = 0; j < cluster_count; j++) {
                            if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            }

                            if (is_cluster_enought != -3) { // check fat and cont.
                                fclose(fd);
                                fd = fopen(argv[1], "rb");
                                break;
                            }
                            if (j != cluster_count - 1) {
                                next_cluster = get_next(argv[1], bpbStruct, next_cluster);
                            }
                        }
                        if (is_cluster_enought == -3) { //yeni cluster yapıp ekle
                            next_cluster = find_cluster1(argv[1], next_cluster, bpbStruct);
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                            uint32_t * tilde_counter=new uint32_t ;
                            *tilde_counter=0;
                           if (last_path_dir_count != 0) {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count - 1],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            } else {
                                is_cluster_enought = findAndSetForTouch(fd, bpbStruct, next_cluster,
                                                                                last_path_dir_count, last_path, 0,
                                                                                argv[1], seq_number2,
                                                                                last_path[last_path_dir_count],ilk_cluster,is_finished2,yazilan_kisim2,tilde_counter);
                            }
                            fclose(fd);
                            fd = fopen(argv[1], "rb");
                        }
                        last_path_dir_count = count_temp;
                        assignPaths(path_temp, last_path, last_path_dir_count);

                    }
                    /*
                    if(seq_number2!= NULL){
                        delete(seq_number2);
                    }
                    if(yazilan_kisim2= NULL){
                        delete(yazilan_kisim2);
                    }
                    if(is_finished2!= NULL){
                        delete(is_finished2);
                    }
                    if(yazildi_mi2!= NULL){
                        delete(yazildi_mi2);
                    }
                    if(nekadari_yazildi2!= NULL){
                        delete(nekadari_yazildi2);
                    }
                    if(looked_file_name2!= NULL){
                        delete(looked_file_name2);
                    }
                    if(kalinan_yer2!= NULL){
                        delete(kalinan_yer2);
                    }
*/
                    break;
                }
                case MV: {

                    break;
                }
                case CAT: {
                    int *kalinan_yer3 = new int;
                    *kalinan_yer3 = 0;
                    int *yazildi_mi3 = new int;
                    string *looked_file_name3 = new string;
                    uint32_t *nekadari_yazildi3 = new uint32_t;
                    uint16_t size;
                    *yazildi_mi3 = 0;
                    *nekadari_yazildi3 = 0;
                    char *seq_number3 = new char;
                    string all_content[2500];
                    int count_temp = last_path_dir_count, return_fake = 0;
                    string path_temp[256];
                    assignPaths(last_path, path_temp, last_path_dir_count);
                    return_fake = fakeCD(&last_path_dir_count, last_path, parsed, fd, bpbStruct, 0, argv[1]);
                    if (return_fake == -1) {
                       /* if(seq_number3!=NULL) delete(seq_number3);*/
                        break;
                    }
                    int next_cluster = 0;
                    int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                    int old_cluster = 0;
                    int temp_for_cluster = 0;
                    for (int i = 0; i < last_path_dir_count; i++) {
                        int cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                        int old_cluster = 0;
                        uint32_t older=next_cluster;
                        for (int j = 0; j < cluster_count; j++) {
                            if (*yazildi_mi3 == 0) {
                                temp_for_cluster = old_cluster;
                                old_cluster = next_cluster;
                            }

                            next_cluster = findDirectory(&last_path[i], fd, bpbStruct, next_cluster, seq_number3,
                                                         yazildi_mi3, nekadari_yazildi3, looked_file_name3, kalinan_yer3,older);
                            if (next_cluster != -3) {
                                break;
                            }
                            if (next_cluster == -3 && *yazildi_mi3 == 1) {
                                next_cluster = temp_for_cluster;

                                j--;
                                continue;
                            }
                            if (next_cluster == -3 && *nekadari_yazildi3 != 0) {
                                j--;
                                next_cluster = old_cluster;
                            }
                            if (j != cluster_count - 1) {
                                next_cluster = get_next(argv[1], bpbStruct, old_cluster);
                            }
                        }
                    }
                    cluster_count = find_files_clusters(argv[1], bpbStruct, next_cluster);
                    int index;
                    int *left = new int;
                    *left = 0;
                    size=0;
                    for (int j = 0; j < cluster_count; j++) {
                        catter(all_content,fd,bpbStruct,next_cluster,size);
                        next_cluster = get_next(argv[1], bpbStruct, next_cluster);

                    }


                    saltlsprinter(all_content, 1);
                    last_path_dir_count = count_temp;
                    assignPaths(path_temp, last_path, last_path_dir_count);
                   /* if(left!=NULL){
                        delete(left);
                    }
                    if(kalinan_yer3!=NULL){
                        delete(kalinan_yer3);
                    }
                    if(looked_file_name3!=NULL){
                        delete(looked_file_name3);
                    }
                    if(seq_number3!=NULL){
                        delete(seq_number3);
                    }
                    if(nekadari_yazildi3!=NULL){
                        delete(nekadari_yazildi3);
                    }
                    if(yazildi_mi3!=NULL){
                        delete(yazildi_mi3);
                    }*/
                    break;
                }
            }
            clean_input(parsed);
        }
    }
    int i=0;
    for(;i<bpbStruct.NumFATs-1;i++){
        FILE * file;
        fd=fopen (argv[1], "rb");
        file=fopen (argv[1], "rb+");
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount),SEEK_SET);
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.BytesPerSector)*(i+1),SEEK_SET);
        int k=0;
        uint32_t* copier=new uint32_t ;
        for(;k<(bpbStruct.extended.FATSize*bpbStruct.BytesPerSector);k+=4){
            fread(copier,4,1,fd);
            fwrite(copier,4,1,file);
        }
      /*if(copier!=NULL)  delete(copier);*/
        fclose(file);
        fclose(fd);
    }


    if(bpbStruct.NumFATs==1){
        fclose(fd);
    }
    /*if(buffer!=NULL){
        delete(buffer);
    }
    if(parsed!=NULL){
        delete(parsed);
    }
    if(fatFileEntry!=NULL){
        delete(fatFileEntry);
    }*/

    return 0;
}