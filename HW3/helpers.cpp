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
#include <string>
#include <iostream>
#include <ctime>
#include <bits/stdc++.h>
using namespace std;
int findDirectory(string* result,FILE* fd,BPB_struct bpbStruct,int next_cluster,char *a,int* is_finished,uint32_t * yazilan_kisim,string* looked_file_name,int*kalinan_yer,uint32_t old_cluster){    //For cd
    double i=0;
    int how_many_lfn=0;
    int file_find_flag=0;
    int lfnbitti_flag=0;

    union struct_FatFileEntry *fatFileEntry=new FatFileEntry ;
    if(*is_finished!=0){
        if(next_cluster>1){
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-1)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+32,SEEK_SET);
        }
        else{
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster+1)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+32,SEEK_SET);

        }
        file_find_flag=1;

    }
    else if(*yazilan_kisim!=0){
        if(next_cluster>1){
            if(old_cluster==0){
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*kalinan_yer)+32,SEEK_SET);
            }
            else{
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*kalinan_yer),SEEK_SET);
            }
        }
        else{
            if(old_cluster==0){
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*kalinan_yer),SEEK_SET);
            }
            else{
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*kalinan_yer),SEEK_SET);
            }

        }
        *yazilan_kisim=0;
    }
    else{
        if(next_cluster>1){
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
        }
        else{
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);


        }    }

    for(;(i ==bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster && (file_find_flag==1 || *is_finished)) || (i<bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster);i+=32){

        if(file_find_flag==0 && (*is_finished==0)){
            fread(fatFileEntry,32,1,fd);
        }
        else{
            while(1){
                how_many_lfn++;

                fseek(fd,-64,SEEK_CUR);
                fread(fatFileEntry,32,1,fd);
                if((fatFileEntry->msdos.attributes==16 && fatFileEntry->msdos.filename[0]=='.') ||((how_many_lfn-1)*32>=i-32 &&(*is_finished==0)) ){
                    *is_finished=1;
                    goto end_of_cd;
                }
                else if(fatFileEntry->msdos.attributes==16&&(*is_finished)==1){
                    lfnbitti_flag=1;
                    goto lfnbitti_foris;
                }
                else if(fatFileEntry->msdos.attributes!=15){
                    goto lfnbitti;
                }
                for(int k=0; k<13 ; k++){
                    if(k<5){
                        if(fatFileEntry->lfn.name1[k]!=0){
                            looked_file_name->push_back(fatFileEntry->lfn.name1[k]);
                        }
                        else{
                            file_find_flag=0;
                            break;
                        }
                    }
                    else if(k<11){
                        if(fatFileEntry->lfn.name2[k-5]!=0){
                            looked_file_name->push_back(fatFileEntry->lfn.name2[k-5]);
                        }
                        else{
                            file_find_flag=0;
                            break;
                        }
                    }
                    else{
                        if(fatFileEntry->lfn.name3[k-11]!=0){
                            looked_file_name->push_back(fatFileEntry->lfn.name3[k-11]);
                        }
                        else{
                            file_find_flag=0;
                            break;
                        }
                    }
                }
                if(file_find_flag==0){
                    break;
                }
            }
            lfnbitti:
            if(*looked_file_name==*result){
                end_of_cd:
                if(*is_finished==1 ){
                    lfnbitti_foris:
                    if(*looked_file_name==*result){
                        *is_finished=0;
                        return *yazilan_kisim;
                    }
                    else if(lfnbitti_flag){
                        *is_finished=0;
                        return -3;
                    }

                }

                *a=fatFileEntry->msdos.filename[0];

                fseek(fd,(how_many_lfn-1)*32,SEEK_CUR);

                fread(fatFileEntry,32,1,fd);
               if(*is_finished==1){
                   *yazilan_kisim=fatFileEntry->msdos.firstCluster+(fatFileEntry->msdos.eaIndex<<16);

                   if(old_cluster==0){
                       *kalinan_yer=((how_many_lfn-1)*32);
                   }
                   else{
                       *kalinan_yer=((how_many_lfn-1)*32)+64;
                   }
                   return -3;
               }
                looked_file_name->clear();
                next_cluster=fatFileEntry->msdos.firstCluster+(fatFileEntry->msdos.eaIndex<<16);

                return next_cluster;
            }
            else{
                if(*is_finished==1){
                    *is_finished=0;
                    looked_file_name->clear();
                    return -3;
                }
                looked_file_name->clear();
                fseek(fd,how_many_lfn*32,SEEK_CUR);
                fread(fatFileEntry,32,1,fd);
                how_many_lfn=0;
            }
        }
        if(fatFileEntry->msdos.attributes == 0){
            return -3;
        }
        if((fatFileEntry->lfn.attributes!=15 && i>=64) || (fatFileEntry->msdos.attributes!=15 && old_cluster==0)){
            file_find_flag=1;
        }
    }
    if(i>=bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster){
        next_cluster=-3;
    }
    //free(fatFileEntry);
    return next_cluster;
}
int findAllSubdir(string *result,FILE* fd,BPB_struct bpbStruct,int next_cluster, char* seq_numb,int len,int* left_at,int path_len,int* is_finished,uint32_t * yazilan_kisim,int*kalinan_yer,uint32_t old_cluster,int* returner,string* looked_file_name,int ilk_cluster1){  //For ls
    int returned_value=*left_at;

    double i=0;
    int how_many_lfn=0;
    int file_find_flag=0;
    int lfnbitti_flag=0;

    union struct_FatFileEntry *fatFileEntry=new FatFileEntry ;
    if(*is_finished!=0){
        if(next_cluster>1){
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-1)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+32,SEEK_SET);
        }
        else{
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster+1)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+32,SEEK_SET);

        }
        file_find_flag=1;

    }
    else if(*yazilan_kisim!=0){
        if(*yazilan_kisim==-1){
            *yazilan_kisim=0;
        }
        if(next_cluster>1){
            if(old_cluster==0){
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*yazilan_kisim+1)*32,SEEK_SET);
            }
            else{
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*yazilan_kisim+1)*32+64,SEEK_SET);
            }
        }
        else{
            if(old_cluster==0){
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*yazilan_kisim+1)*32,SEEK_SET);
            }
            else{
                fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+(*yazilan_kisim+1)*32+64,SEEK_SET);
            }

        }
        *yazilan_kisim=0;
    }
    else{
        if(next_cluster>1){
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
        }
        else{
            fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

        }    }

    for(;(i ==bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster && (file_find_flag==1 || *is_finished)) || (i<bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster);i+=32){
        if(i<64 && next_cluster!=0){
            goto gelis;
        }
        if(file_find_flag==0 && (*is_finished==0)){
            fread(fatFileEntry,32,1,fd);
        }
        else{
            while(1){
                how_many_lfn++;

                fseek(fd,-64,SEEK_CUR);
                fread(fatFileEntry,32,1,fd);
                if((fatFileEntry->msdos.attributes!=15 && fatFileEntry->msdos.filename[0]=='.') ||((how_many_lfn-1)*32>=i-32 &&(*is_finished==0))){
                    *is_finished=1;
                    goto end_of_ls;
                }
                else if(fatFileEntry->msdos.attributes!=16&&(*is_finished)==1){

                    lfnbitti_flag=1;
                    goto son_lfn;
                }
                else if(fatFileEntry->msdos.attributes!=15){
                    goto son_lfn;
                }
                for(int k=0; k<13 ; k++){
                    if(k<5){
                        if(fatFileEntry->lfn.name1[k]!=0){
                            looked_file_name->push_back(fatFileEntry->lfn.name1[k]);
                        }
                        else{
                            if(*is_finished){
                                lfnbitti_flag=1;
                            }
                            file_find_flag=0;
                            break;
                        }
                    }
                    else if(k<11){
                        if(fatFileEntry->lfn.name2[k-5]!=0){

                            looked_file_name->push_back(fatFileEntry->lfn.name2[k-5]);
                        }
                        else{
                            if(*is_finished){
                                lfnbitti_flag=1;
                            }
                            file_find_flag=0;
                            break;
                        }
                    }
                    else{
                        if(fatFileEntry->lfn.name3[k-11]!=0){
                            looked_file_name->push_back(fatFileEntry->lfn.name3[k-11]);
                        }
                        else{
                            if(*is_finished){
                                lfnbitti_flag=1;
                            }
                            file_find_flag=0;
                            break;
                        }
                    }
                }
                if(file_find_flag==0){
                    end_of_ls:
                    if(3==4){
                        son_lfn:
                        file_find_flag=0;
                        fseek(fd,32,SEEK_CUR);
                    }

                    if(old_cluster==0){
                        *kalinan_yer=((how_many_lfn-2)*32);
                    }
                    else{
                        *kalinan_yer=((how_many_lfn-1)*32)+64;
                    }

                    if(!lfnbitti_flag && *is_finished==1){
                        *left_at=returned_value;
                        *yazilan_kisim=how_many_lfn-1;
                        return -3;
                    }
                    if(*is_finished==1){
                        *is_finished=0;
                        result[returned_value++]=*looked_file_name;
                        if(*yazilan_kisim==0){
                            *yazilan_kisim=-1;
                        }

                        looked_file_name->clear();
                        *left_at=returned_value;

                        return -3;
                    }
                    *is_finished=0;

                    result[returned_value++]=*looked_file_name;
                    looked_file_name->clear();
                    break;
                }
            }
            fseek(fd,how_many_lfn*32,SEEK_CUR);
            fread(fatFileEntry,32,1,fd);
            how_many_lfn=0;
        }
        if(fatFileEntry->msdos.attributes == 0){
            break;
        }
        if((fatFileEntry->lfn.attributes!=15 && i>=64) || (fatFileEntry->msdos.attributes!=15 && ilk_cluster1==0)){
            file_find_flag=1;
        }
        gelis:
        int da=0;
    }
    //free(fatFileEntry);
    *left_at=returned_value;
    return returned_value;
}
int findAllSubdirForTireL(string* result,FILE* fd,BPB_struct bpbStruct,int next_cluster,char* seq_numb,int len,int *left,int path_len){  //For ls
    int returned_value=*left;
    double i=0;
    int how_many_lfn=0;
    int file_find_flag=0;
    string looked_file_name;
    union struct_FatFileEntry *fatFileEntry=new FatFileEntry ;
    if(next_cluster>1){
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
    }
    else{
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

    }
    for(;(i ==bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster && file_find_flag==1) || (i<bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster);i+=32){
        if(file_find_flag==0){
            fread(fatFileEntry,32,1,fd);
        }
        else{
            while(1) {
                how_many_lfn++;
                fseek(fd, -64, SEEK_CUR);
                fread(fatFileEntry, 32, 1, fd);
                if((fatFileEntry->lfn.attributes==15) || len==0){
                    for (int k = 0; k < 13; k++) {
                        if (k < 5) {
                            if (fatFileEntry->lfn.name1[k] != 0) {
                                looked_file_name.push_back(fatFileEntry->lfn.name1[k]);
                            } else {
                                file_find_flag = 0;
                                break;
                            }
                        } else if (k < 11) {
                            if (fatFileEntry->lfn.name2[k - 5] != 0) {
                                looked_file_name.push_back(fatFileEntry->lfn.name2[k - 5]);
                            } else {
                                file_find_flag = 0;
                                break;
                            }
                        } else {
                            if (fatFileEntry->lfn.name3[k - 11] != 0) {
                                looked_file_name.push_back(fatFileEntry->lfn.name3[k - 11]);
                            } else {
                                file_find_flag = 0;
                                break;
                            }
                        }
                    }
                    if (file_find_flag == 0) {
                        string tut;
                        fseek(fd,(how_many_lfn-1)*32,SEEK_CUR);
                        fread(fatFileEntry,32,1,fd);
                        if(fatFileEntry->msdos.attributes==32){
                            result[returned_value]="-rwx------ 1 root root ";
                            tut=to_string(fatFileEntry->msdos.fileSize);
                            result[returned_value].append(tut);
                            result[returned_value].append(" ");
                        }
                        else{
                            result[returned_value]="drwx------ 1 root root 0 ";
                        }
                        tut=fatgetter(fatFileEntry->msdos.creationTime,fatFileEntry->msdos.creationDate);
                        result[returned_value].append(tut);
                        result[returned_value++].append(looked_file_name);

                        looked_file_name.clear();

                        break;
                    }
                }


            }
            fseek(fd,(how_many_lfn-1)*32,SEEK_CUR);
            fread(fatFileEntry,32,1,fd);
            how_many_lfn=0;
        }
        if(fatFileEntry->msdos.attributes == 0){
            break;
        }
        if((fatFileEntry->lfn.attributes==16 || fatFileEntry->lfn.attributes==32) && fatFileEntry->lfn.attributes!=0){
            if(i>64){
                file_find_flag=1;
            }
            else if(len==0 && path_len==0){
                file_find_flag=1;
            }
        }
    }

    //free(fatFileEntry);
    return returned_value;
}
/*int findAllSubdirForTireL(string* result,FILE* fd,BPB_struct bpbStruct,int next_cluster,char* seq_numb,int len,int *left,int path_len) {
}*/
int pathFounder(parsed_input* parsed,string* search_this,int last_path_dir_count,int which){
    int counter=last_path_dir_count;
    char* temp;
    if(which==0){
        temp=strtok(parsed->arg1,"/");
    }
    else{
        temp=strtok(parsed->arg2,"/");
    }

    for(;temp!=NULL;){
        if(!strcmp(temp,"..")){
            if(counter>0){
                counter--;
            }
            else{
                return-1;
            }
        }
        else if (!strcmp(temp,".")){

        }
        else{
            search_this[counter++]=temp;

        }
        temp=strtok(NULL,"/");
    }
    return counter;
}

void assignPaths(string* copy,string* past, int count){
    for(int i=0;i<count;i++){
        past[i]=copy[i];
    }
}
string writePath(int last_path_dir_count,string* last_path){
    string result_string;
    for(int i=0;i<=last_path_dir_count;i++){
        if(last_path_dir_count==0){
            result_string.append("/> ");
        }
        else{
            result_string.append("/");
            result_string.append(last_path[i]);
        }
        if(i==last_path_dir_count-1){
            result_string.append("> ");
            break;
        }

    }
    return result_string;
}
void saltlsprinter(string* print,int index){
    for(int i=0;i<index;i++){
    	if(print[i]!="") cout<<print[i]<<" ";
    }
    if(index!=0){
        if((index==1 && print[0]=="")){
            goto endddd;
        }
        cout<<"\n";
    }
    endddd:
    int a=0;

}
int fakeCD(int *last_path_dir_count,string* last_path,parsed_input* parsed,FILE* fd,BPB_struct bpbStruct,int which,char* image){
    char* a= new char;
    int*kalinan_yer=new int;
    *kalinan_yer=0;
    int *yazildi_mi=new int ;
    uint32_t *nekadari_yazildi=new uint32_t ;
    string* looked_file_name=new string ;
    *yazildi_mi=0;
    *nekadari_yazildi=0;
    int next_cluster=0,count_temp=*last_path_dir_count;
    string path_temp[256];
    assignPaths(last_path,path_temp,*last_path_dir_count);
    if(which==0){
        if(parsed->arg1[0]=='/') {
            *last_path_dir_count = 0;
        }
        *last_path_dir_count=pathFounder(parsed,last_path,*last_path_dir_count,0);
    }
    else{
        if(parsed->arg2[0]=='/') {
            *last_path_dir_count = 0;
        }
        *last_path_dir_count=pathFounder(parsed,last_path,*last_path_dir_count,1);
    }
    if(*last_path_dir_count==-1){
        *last_path_dir_count=count_temp;
    }
    uint32_t temp_for_cluster;
    for (int i = 0; i < *last_path_dir_count; i++) {
        int cluster_count = find_files_clusters(image, bpbStruct, next_cluster);
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
            if(next_cluster==-3 && *nekadari_yazildi!=0){
                j--;
                next_cluster = old_cluster;
            }
            if (j != cluster_count - 1) {
                next_cluster = get_next(image, bpbStruct, old_cluster);
            }
        }

        if (next_cluster == -3) {
            *last_path_dir_count = count_temp;
            assignPaths(path_temp, last_path, *last_path_dir_count);
            return -1;
        }
    }
    /*free(a);
    free(yazildi_mi);
    free(nekadari_yazildi);
    free(looked_file_name);
    free(kalinan_yer);*/
    return 1;
}

string fatgetter(uint16_t creationTime,uint16_t creationDate){
    string returner,tarih,saat;
    int a=creationTime>>11;
    saat.append(" ");
    if(to_string(creationTime >> 11 &(0b0000000000011111)).length()==1){
        saat="0";
        saat.append(to_string(creationTime >> 11 &(0b0000000000011111)));
    }
    else{
        saat=to_string(creationTime >> 11 &(0b0000000000011111));
    }


    saat.append(":");
    if(to_string((creationTime >>5)&(0b0000000000111111)).length()==1){
        saat.append("0");
    }
    saat.append(to_string((creationTime >>5)&(0b0000000000111111)));

    saat.append(" ");
    tarih=to_string(((creationDate >> 9)&(0b0000000001111111))+1980);
    tarih.append(" ");
    tarih.append(mounth(((creationDate >> 5)&(0b000000000001111))));
    tarih.append(" ");
    tarih.append(to_string(((creationDate)&(0b0000000000011111))));
    returner=tarih;
    returner.append(" ");
    returner.append(saat);
    return returner;
}
string mounth(uint16_t moun){

    switch (moun) {
        case(0b0000000000000000):{
            return "Jan";
            break;
        }
        case(1):{
            return "Feb";
            break;
        }
        case(2):{
            return "Mar";
            break;
        }
        case(3):{
            return "Apr";
            break;
        }
        case(4):{
            return "May";
            break;
        }
        case(5):{
            return "Jun";
            break;
        }
        case(6):{
            return "Jul";
            break;
        }
        case(7):{
            return "Aug";
            break;
        }
        case(8):{
            return "Sep";
            break;
        }
        case(9):{
            return "Oct";
            break;
        }
        case(10):{
            return "Nov";
            break;
        }
        case(11):{
            return "Dec";
            break;
        }
        default:{
            return "hata";
        }
    }
}
int mounthtoBinary(const char *moun){
    if(!strcmp(moun,"Jan")) return 0;
    if(!strcmp(moun,"Feb")) return 1;
    if(!strcmp(moun,"Mar")) return 2;
    if(!strcmp(moun,"Apr")) return 3;
    if(!strcmp(moun,"May")) return 4;
    if(!strcmp(moun,"Jun")) return 5;
    if(!strcmp(moun,"Jul")) return 6;
    if(!strcmp(moun,"Aug")) return 7;
    if(!strcmp(moun,"Sep")) return 8;
    if(!strcmp(moun,"Oct")) return 9;
    if(!strcmp(moun,"Nov")) return 10;
    if(!strcmp(moun,"Dec")) return 11;

}
void tirelsprinter(string* print,int index){
    for(int i=0;i<index;i++){
        cout<<print[i]<<"\n";
    }

}
unsigned char lfn_checksum(const unsigned char *pFCBName) {
    int i;
    unsigned char sum = 0;

    for (i = 11; i; i--)
        sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;

    return sum;
}

uint32_t find_cluster1(char* image,int nextcluster,struct_BPB_struct bpbStruct){
    FILE *file;
    uint32_t cluster_place=0;
    file= fopen(image,"rb+");
    fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount),SEEK_SET);
    uint32_t* checker=new uint32_t;
    uint32_t* deger=new uint32_t;
    while(1){
        cluster_place++;

        fread(checker,4,1,file);
        if(*checker==0){
            fseek(file,-4,SEEK_CUR);
            *deger=268435455;
            fwrite(deger,4,1,file);
            break;
        }
    }
    //free(checker);

    if(nextcluster==0){
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount+4*(2)),SEEK_SET);
    }
    else{
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount+4*(nextcluster)),SEEK_SET);
    }
    uint32_t* location=new uint32_t ;
    *location=0;

    int b=0;
    uint32_t* next= new uint32_t ;
    *next=cluster_place-1;
    fwrite(next,4,1,file);
    fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount),SEEK_SET);

    //free(next);
    //free(location);
    fclose(file);
    cluster_place=cluster_place &(0x0FFFFFFF);
    return cluster_place-1;
}
uint32_t find_cluster(char* image,int nextcluster,struct_BPB_struct bpbStruct){
    FILE *file;
    uint32_t cluster_place=0;
    file= fopen(image,"rb+");
    fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount),SEEK_SET);
    uint32_t* checker=new uint32_t;
    uint32_t* deger=new uint32_t;
    while(1){
        cluster_place++;
        fread(checker,4,1,file);
        if(*checker==0){
            fseek(file,-4,SEEK_CUR);
            *deger=268435455;
            fwrite(deger,4,1,file);
            break;
        }
    }
    //free(checker);

    fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount),SEEK_SET);
    uint32_t* location=new uint32_t ;
    *location=0;
   // free(location);
    fclose(file);
    cluster_place=cluster_place &(0x0FFFFFFF);
    //free(deger);
    return cluster_place-1;
}
int find_files_clusters(char* image,BPB_struct bpbStruct,int next_cluster){
    FILE* fileptr;
    uint32_t counter=0;
    int i=0;
    uint32_t *reader=new uint32_t ;
    if(next_cluster==-3){
        //free(reader);
        return 0;
    }
    fileptr = fopen (image, "rb+");
    if(next_cluster==0){
        fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount+4*(2)),SEEK_SET);
    }
    else{
        fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount+4*(next_cluster)),SEEK_SET);
    }
    fread(reader,4,1,fileptr);
    if(*reader>=268435448 ||*reader==0){ //0a bak
        next_cluster=-3;
    }
    else{
        next_cluster=*reader;

    }
    counter++;
    //free(reader);
    counter+= find_files_clusters(image,bpbStruct,next_cluster);

    fclose(fileptr);
    return counter;
}

uint32_t get_next(char* image,BPB_struct bpbStruct,int next_cluster){
    FILE* fileptr;
    uint32_t counter=0;
    int i=0;
    uint32_t *reader=new uint32_t ;
    fileptr = fopen (image, "rb+");
    if(next_cluster==0){
        fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount+4*(2)),SEEK_SET);
    }
    else{
        fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount+4*(next_cluster)),SEEK_SET);
    }
    fread(reader,4,1,fileptr);
    if(*reader>=268435448 ||*reader==0){
        counter=-3;
    }
    else{
        counter=*reader;
    }
    //free(reader);
    fclose(fileptr);
    counter=counter & (0x8FFF);

    return counter;
}
void set_parentmodified(int next_cluster,uint16_t date,uint16_t timet,struct_BPB_struct bpbStruct,FILE* fd,string* last_path,int last_path_count){

}
void set_first_two(uint32_t cluster,char* img,int old_cluster,struct_BPB_struct bpbStruct){
    struct_FatFileEntry *fatFileEntry=new struct_FatFileEntry;
    struct_FatFileEntry*fatFileEntry1=new struct_FatFileEntry;
    fatFileEntry->msdos.filename[0]='.';
    fatFileEntry->msdos.filename[1]='.';
    for(int i=2;i<8;i++){
        fatFileEntry->msdos.filename[i]= ' ';
    }
    for(int i=0;i<3;i++){
        fatFileEntry->msdos.extension[i]=' ';
    }
    fatFileEntry->msdos.attributes=16;
    fatFileEntry->msdos.firstCluster=old_cluster & 0x0000FFFF;
    fatFileEntry->msdos.eaIndex=old_cluster & 0xFFFF0000;
    FILE* file;
    file = fopen (img, "rb+");
    int cluster_minus=cluster-2;
    int fake_cluster=cluster;
    if(cluster>1){
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((cluster_minus)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+32,SEEK_SET);
    }
    else{
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(fake_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster)+32,SEEK_SET);

    }
    fwrite(fatFileEntry,sizeof(*fatFileEntry),1,file);
    fatFileEntry1->msdos.filename[0]='.';
    for(int i=1;i<8;i++){
        fatFileEntry1->msdos.filename[i]= ' ';
    }
    for(int i=0;i<3;i++){
        fatFileEntry1->msdos.extension[i]=' ';
    }
    fatFileEntry1->msdos.firstCluster=cluster & 0x0000FFFF;
    fatFileEntry1->msdos.eaIndex=cluster & 0xFFFF0000;
    fatFileEntry1->msdos.attributes=16;
    if(cluster>1){
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((cluster_minus)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
    }
    else{
        fseek(file,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(fake_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

    }
    fwrite(fatFileEntry1,sizeof(*fatFileEntry),1,file);
    fclose(file);
}

int findAndSetFreeInDataRegion(FILE* fd,BPB_struct bpbStruct,int next_cluster,int len,string* path,int chooser,char* image,char* seq_number,string parent_name,uint32_t old_cluster,uint32_t * tilde_counter){
    double i=0;
    int left_at=0;

    string loc_file_name;
    int file_find_flag=0;
    string looked_file_name;
    if(chooser==0){
        loc_file_name=path[len];
    }
    else{
        loc_file_name=path[len+1];
    }

    int iter_count=(loc_file_name.length()/13);
    if(loc_file_name.length()%13==0 && loc_file_name.length()!=0){
        iter_count--;
    }
    union struct_FatFileEntry *fatFileEntry=new FatFileEntry ;
    if(next_cluster>1){
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
    }
    else{
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

    }
    for(;i<bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster;i+=32) {
        fread(fatFileEntry, 32, 1, fd);
        if(fatFileEntry->msdos.filename[0]=='~'){
            (*tilde_counter)++;
        }
        if (fatFileEntry->msdos.attributes == 0) {
            if((i==0 && next_cluster==0) || (i==64 && next_cluster!=0)){
                fread(fatFileEntry, 32, 1, fd);
            }
            for (int j = 0; j <= iter_count; j++) {
                i+=32;
                if(i>=bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster){
                    goto end;
                }
                fread(fatFileEntry, 32, 1, fd);
            }
            for(int j=0;j<=iter_count;j++) i-=32;
            uint16_t seq_num=0;
            for (int j = 0; j <=iter_count; j++) {

                fseek(fd, -64, SEEK_CUR);
                fread(fatFileEntry, 32, 1, fd);
                fatFileEntry->lfn.attributes = 15;

                if(i==iter_count){
                    seq_num+=0x40+1;
                    fatFileEntry->lfn.sequence_number = seq_num;
                }
                else{
                    fatFileEntry->lfn.sequence_number = ++seq_num;
                }

                fatFileEntry->lfn.reserved = 0;
                fatFileEntry->lfn.firstCluster = 0;
                for (int k = 0; k < 5; k++) {
                    if((left_at )== loc_file_name.length()){
                        fatFileEntry->lfn.name1[k] =0;
                        left_at++;
                    }
                    else if((left_at )> loc_file_name.length()){
                        fatFileEntry->lfn.name1[k] =65535;
                    }
                    else{
                        fatFileEntry->lfn.name1[k] = loc_file_name[left_at++];
                    }
                }
                for (int k = 0; k < 6 ; k++) {
                    if((left_at )== loc_file_name.length()){
                        fatFileEntry->lfn.name2[k] =0;
                        left_at++;
                    }
                    else if((left_at )> loc_file_name.length()){
                        fatFileEntry->lfn.name2[k] =65535;
                    }
                    else{
                        fatFileEntry->lfn.name2[k] = loc_file_name[left_at++];
                    }
                }
                for (int k = 0; k < 2; k++) {
                    if((left_at )== loc_file_name.length()){
                        fatFileEntry->lfn.name3[k] =0;
                        left_at++;
                    }
                    else if((left_at )> loc_file_name.length()){
                        fatFileEntry->lfn.name3[k] =65535;
                    }
                    else{
                        fatFileEntry->lfn.name3[k] = loc_file_name[left_at++];
                    }
                }
                unsigned char *checkersum =new unsigned char;
                checkersum[0]='~';
                checkersum[1]='0'+(*tilde_counter+1);
                for(int kale=2;kale<8;kale++){
                    checkersum[kale]=' ';
                }
                for(int kale=0;kale<3;kale++){
                    checkersum[kale+8]=' ';
                }
                fatFileEntry->lfn.checksum= lfn_checksum((checkersum));
                FILE* fileptr;
                fileptr = fopen (image, "rb+");
                if(next_cluster>1){
                    fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
                }
                else{
                    fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

                }
                fseek(fileptr,i+((iter_count-j)*32),SEEK_CUR);
                fwrite(fatFileEntry,sizeof(*fatFileEntry),1,fileptr);
                fclose(fileptr);
            }
            fseek(fd, iter_count * 32, SEEK_CUR);
            fread(fatFileEntry, 32, 1, fd);
            fatFileEntry->msdos.attributes=16;
            fatFileEntry->msdos.filename[0]='~';
            fatFileEntry->msdos.filename[1]= '0'+(*tilde_counter+1);
            for(int kale=2;kale<8;kale++){
                fatFileEntry->msdos.filename[kale]=' ';
            }
            for(int kale=0;kale<3;kale++){
                fatFileEntry->msdos.extension[kale]=' ';
            }
            time_t now = time(0);
            char* dt = ctime(&now);
            char * pEnd;
            char* cur_tim_char=new char;
            uint16_t temp=0;
            for(int kl=20;kl<24;kl++){
                cur_tim_char[kl-20]=dt[kl];
            }
            temp=strtol(cur_tim_char,&pEnd,10);
            u_int16_t date=0;
            date=temp;
            date=(date-1980)<<9;
            for(int kl=4;kl<7;kl++){
                cur_tim_char[kl-4]=dt[kl];
            }
            cur_tim_char[3]='\0';
            temp= mounthtoBinary(cur_tim_char);
            temp=temp<<5;
            date=date |(temp);
            for(int kl=8;kl<10;kl++){
                cur_tim_char[kl-8]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10);
            date=date| temp;
            fatFileEntry->msdos.creationDate=date;
            for(int kl=11;kl<13;kl++){
                cur_tim_char[kl-11]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10);
            uint16_t timet=temp;
            timet=timet<<11;
            for(int kl=14;kl<16;kl++){
                cur_tim_char[kl-14]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10);
            temp=temp<<5;
            timet=timet | temp;
            for(int kl=17;kl<29;kl++){
                cur_tim_char[kl-17]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10)/2;
            timet=timet | temp;
            fatFileEntry->msdos.reserved=0;
            fatFileEntry->msdos.creationTime=timet;
            fatFileEntry->msdos.fileSize=0;
            fatFileEntry->msdos.creationTimeMs=0;
            fatFileEntry->msdos.modifiedDate=date;
            fatFileEntry->msdos.modifiedTime=timet;
            set_parentmodified(next_cluster,date,timet,bpbStruct,fd,path,len);
            uint32_t four=find_cluster(image,next_cluster,bpbStruct);
            fatFileEntry->msdos.firstCluster=four & 0x0000FFFF;
            fatFileEntry->msdos.eaIndex=four & 0xFFFF0000;
            //set_first_two(four,image,old_cluster,bpbStruct);
            fatFileEntry->msdos.lastAccessTime=timet;
            fatFileEntry->msdos.eaIndex=0;
            FILE* fileptr;
            fileptr = fopen (image, "rb+");
            if(next_cluster>1){
                fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
            }
            else{
                fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

            }
            fseek(fileptr,i+((iter_count+1)*32),SEEK_CUR);
            fwrite(fatFileEntry,sizeof(*fatFileEntry),1,fileptr);
            fclose(fileptr);
            free(cur_tim_char);
            break;
        }
        bura:
        int a=0;
    }
    end:
    if(i>=bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster){
        next_cluster=-1;
    }
    return next_cluster-2;
}


int findAndSetForTouch(FILE* fd,BPB_struct bpbStruct,int next_cluster,int len,string* path,int chooser,char* image,char* seq_number,string parent_name,uint32_t old_cluster,int* is_finished,int* yazilan_kisim,uint32_t* tilde_counter){
    double i=0;
    int left_at=0;

    string loc_file_name;
    int file_find_flag=0;
    string looked_file_name;
    if(chooser==0){
        loc_file_name=path[len];
    }
    else{
        loc_file_name=path[len+1];
    }

    int iter_count=(loc_file_name.length()/13);
    if(loc_file_name.length()%13==0 && loc_file_name.length()!=0){
        iter_count--;
    }
    union struct_FatFileEntry *fatFileEntry=new FatFileEntry ;
    if(next_cluster>1){
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
    }
    else{
        fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

    }
    for(;i<bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster;i+=32) {
        fread(fatFileEntry, 32, 1, fd);
        if(fatFileEntry->msdos.filename[0]=='~'){
            (*tilde_counter)++;
        }
        if (fatFileEntry->msdos.attributes == 0) {
            if((i==0 && next_cluster==0) || (i==64 && next_cluster!=0)){
                fread(fatFileEntry, 32, 1, fd);
            }
            for (int j = 0; j <= iter_count; j++) {
                i+=32;
                if(i>=bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster){
                    goto end;
                }
                fread(fatFileEntry, 32, 1, fd);
            }
            for(int j=0;j<=iter_count;j++) i-=32;
            uint16_t seq_num=0;
            for (int j = 0; j <=iter_count; j++) {

                fseek(fd, -64, SEEK_CUR);
                fread(fatFileEntry, 32, 1, fd);
                fatFileEntry->lfn.attributes = 15;

                if(i==iter_count){
                    seq_num+=0x40+1;
                    fatFileEntry->lfn.sequence_number = seq_num;
                }
                else{
                    fatFileEntry->lfn.sequence_number = ++seq_num;
                }

                fatFileEntry->lfn.reserved = 0;
                fatFileEntry->lfn.firstCluster = 0;
                for (int k = 0; k < 5; k++) {
                    if((left_at )== loc_file_name.length()){
                        fatFileEntry->lfn.name1[k] =0;
                        left_at++;
                    }
                    else if((left_at )> loc_file_name.length()){
                        fatFileEntry->lfn.name1[k] =65535;
                    }
                    else{
                        fatFileEntry->lfn.name1[k] = loc_file_name[left_at++];
                    }
                }
                for (int k = 0; k < 6 ; k++) {
                    if((left_at )== loc_file_name.length()){
                        fatFileEntry->lfn.name2[k] =0;
                        left_at++;
                    }
                    else if((left_at )> loc_file_name.length()){
                        fatFileEntry->lfn.name2[k] =65535;
                    }
                    else{
                        fatFileEntry->lfn.name2[k] = loc_file_name[left_at++];
                    }
                }
                for (int k = 0; k < 2; k++) {
                    if((left_at )== loc_file_name.length()){
                        fatFileEntry->lfn.name3[k] =0;
                        left_at++;
                    }
                    else if((left_at )> loc_file_name.length()){
                        fatFileEntry->lfn.name3[k] =65535;
                    }
                    else{
                        fatFileEntry->lfn.name3[k] = loc_file_name[left_at++];
                    }
                }
                unsigned char *checkersum =new unsigned char;
                checkersum[0]='~';
                checkersum[1]='0'+(*tilde_counter+1);
                for(int kale=2;kale<8;kale++){
                    checkersum[kale]=' ';
                }
                for(int kale=0;kale<3;kale++){
                    checkersum[kale+8]=' ';
                }
                fatFileEntry->lfn.checksum= lfn_checksum((checkersum));
                FILE* fileptr;
                fileptr = fopen (image, "rb+");
                if(next_cluster>1){
                    fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
                }
                else{
                    fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

                }
                fseek(fileptr,i+((iter_count-j)*32),SEEK_CUR);
                fwrite(fatFileEntry,sizeof(*fatFileEntry),1,fileptr);
                fclose(fileptr);
            }
            fseek(fd, iter_count * 32, SEEK_CUR);
            fread(fatFileEntry, 32, 1, fd);
            fatFileEntry->msdos.attributes=32;
            fatFileEntry->msdos.filename[0]='~';
            fatFileEntry->msdos.filename[1]= '0'+(*tilde_counter+1);
            for(int kale=2;kale<8;kale++){
                fatFileEntry->msdos.filename[kale]=' ';
            }
            for(int kale=0;kale<3;kale++){
                fatFileEntry->msdos.extension[kale]=' ';
            }
            time_t now = time(0);
            char* dt = ctime(&now);
            char * pEnd;
            char* cur_tim_char=new char;
            uint16_t temp=0;
            for(int kl=20;kl<24;kl++){
                cur_tim_char[kl-20]=dt[kl];
            }
            temp=strtol(cur_tim_char,&pEnd,10);
            u_int16_t date=0;
            date=temp;
            date=(date-1980)<<9;
            for(int kl=4;kl<7;kl++){
                cur_tim_char[kl-4]=dt[kl];
            }
            cur_tim_char[3]='\0';
            temp= mounthtoBinary(cur_tim_char);
            temp=temp<<5;
            date=date |(temp);
            for(int kl=8;kl<10;kl++){
                cur_tim_char[kl-8]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10);
            date=date| temp;
            fatFileEntry->msdos.creationDate=date;
            for(int kl=11;kl<13;kl++){
                cur_tim_char[kl-11]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10);
            uint16_t timet=temp;
            timet=timet<<11;
            for(int kl=14;kl<16;kl++){
                cur_tim_char[kl-14]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10);
            temp=temp<<5;
            timet=timet | temp;
            for(int kl=17;kl<29;kl++){
                cur_tim_char[kl-17]=dt[kl];
            }
            cur_tim_char[2]='\0';
            temp=strtol(cur_tim_char,&pEnd,10)/2;
            timet=timet | temp;
            fatFileEntry->msdos.reserved=0;
            fatFileEntry->msdos.creationTime=timet;
            fatFileEntry->msdos.fileSize=0;
            fatFileEntry->msdos.creationTimeMs=0;
            fatFileEntry->msdos.modifiedDate=date;
            fatFileEntry->msdos.modifiedTime=timet;
            set_parentmodified(next_cluster,date,timet,bpbStruct,fd,path,len);
            fatFileEntry->msdos.firstCluster=0;
            fatFileEntry->msdos.eaIndex=0;
            fatFileEntry->msdos.lastAccessTime=timet;
            fatFileEntry->msdos.eaIndex=0;
            FILE* fileptr;
            fileptr = fopen (image, "rb+");
            if(next_cluster>1){
                fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
            }
            else{
                fseek(fileptr,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+(next_cluster*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);

            }
            fseek(fileptr,i+((iter_count+1)*32),SEEK_CUR);
            fwrite(fatFileEntry,sizeof(*fatFileEntry),1,fileptr);
            fclose(fileptr);
            free(cur_tim_char);
            break;
        }
        bura:
        int a=0;
    }
    end:
    if(i>=bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster){
        next_cluster=-1;
    }
    return next_cluster-2;
}

void catter(string* all_content, FILE* fd,BPB_struct bpbStruct,int next_cluster,uint16_t size){
    int i=0;
    int flag=0;
    char okunacak[32];
    fseek(fd,(bpbStruct.BytesPerSector*bpbStruct.ReservedSectorCount)+(bpbStruct.extended.FATSize*bpbStruct.NumFATs*bpbStruct.BytesPerSector)+((next_cluster-2)*bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster),SEEK_SET);
    for(;i<bpbStruct.BytesPerSector*bpbStruct.SectorsPerCluster ;i+=32) {
        fread(okunacak, 32, 1, fd);
        for(int j=0;j<32;j++){
            if(okunacak[j]==0){
                flag=1;
                break;
            }
            else{
                all_content[0].push_back(okunacak[j]);
            }
        }
        if(flag==1){
            break;
        }

    }
}
