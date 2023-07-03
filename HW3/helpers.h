#ifndef HW3_HELPERS_H
#define HW3_HELPERS_H
#include "fat32.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
//For CD
void findDirectory(string* result,FILE* file,char* a,int* is_finished,uint32_t * yazilan_kisim,string* looked_file_name,int*kalinan_yer,uint32_t old_cluster);
void assignPaths(string* copy,string* past, int count);
int pathFounder(parsed_input* parsed,string* search_this);

// For LS
int findAllSubdir(string *result,FILE* fd,BPB_struct bpbStruct,int next_cluster, char* seq_numb,int len,int *left_at,int path_len,int* is_finished,uint32_t * yazilan_kisim,int*kalinan_yer,uint32_t old_cluster,int* returner,string* looked_file_name,int ilk_cluster1);
void saltlsprinter(string* print,int index);
int fakeCD(int *last_path_dir_count,string* last_path,parsed_input* parsed,FILE* fd,BPB_struct bpbStruct ,int which,char* image);
int findAllSubdirForTireL(string* result,FILE* fd,BPB_struct bpbStruct,int next_cluster,char* seq_numb,int len,int *left,int path_len);
string fatgetter(uint16_t creationTime,uint16_t creationDate);
string mounth(uint16_t moun);
void tirelsprinter(string* print,int index);

//For MKDIR
unsigned char lfn_checksum(const unsigned char *pFCBName);
int findAndSetFreeInDataRegion(FILE* fd,BPB_struct bpbStruct,int next_cluster,int len,string* path,int chooser,char* image,char* seq_number,string parent_name,uint32_t old_cluster,uint32_t * tilde_counter);
int mounthtoBinary(const char *moun);
uint32_t find_cluster1(char* image,int nextcluser,struct_BPB_struct bpbStruct);
uint32_t find_cluster(char* image,int nextcluser,struct_BPB_struct bpbStruct);
int find_files_clusters(char* image,BPB_struct bpbStruct,int next_cluster);
uint32_t get_next(char* image,BPB_struct bpbStruct,int next_cluster);
void set_parentmodified(int next_cluster,uint16_t date,uint16_t timet,struct_BPB_struct bpbStruct,FILE* fd,string* last_path,int last_path_count);
void set_first_two(uint32_t cluster,char* img,int old_cluster,struct_BPB_struct bpbStruct);


int findAndSetForTouch(FILE* fd,BPB_struct bpbStruct,int next_cluster,int len,string* path,int chooser,char* image,char* seq_number,string parent_name,int old_cluster,int* is_finished,int* yazilan_kisim, uint32_t* tildecounte);

void catter(string* all_content, FILE* fd,BPB_struct bpbStruct,int next_cluster,uint16_t size);

#endif //HW3_HELPERS_H
