#include"parser.c"
#include <stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdbool.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include<sys/wait.h>
typedef struct bundle_keeper {
    bundle_execution bundle;
    char **argvi[256]; 
    
    bool executed;
} bundle_keeper;
typedef struct pipe_keeper {
    int b_count;
    int arg_count[256];
    int location_keep[256];
    
    int total;
} pipe_keeper;
void forker_for_direction(parsed_command command,bundle_keeper bundler[],int located,int arg_size) {
    pid_t pid[arg_size];
    int k, child_status;
    if(bundler[located].bundle.output!=NULL && bundler[located].bundle.input!=NULL){
        
        for (k = 0; k < arg_size; k++){
            if ((pid[k] = fork()) == 0) {
                int file_in = open(bundler[located].bundle.input,O_RDONLY );
                int file_out = open(bundler[located].bundle.output,O_WRONLY | O_APPEND);
                if(file_out==-1){
                    creat(bundler[located].bundle.output,S_IRWXG | S_IRWXU | S_IRWXO);
                    file_out = open(bundler[located].bundle.output,O_WRONLY | O_APPEND);
                }
                
                dup2(file_in,0);
                dup2(file_out,1);
                execvp(bundler[located].argvi[k][0],bundler[located].argvi[k]);
                exit(100+k); 
            }
        }
        for (k = 0; k < arg_size; k++) { 
            pid_t wpid = wait(&child_status);
        }
    }
    else if(bundler[located].bundle.output!=NULL){
        
        for (k= 0; k < arg_size; k++){
            if ((pid[k] = fork()) == 0) {
                int file_out = open(bundler[located].bundle.output,O_WRONLY | O_APPEND);
                if(file_out==-1){
                    creat(bundler[located].bundle.output,S_IRWXG | S_IRWXU | S_IRWXO);
                    file_out = open(bundler[located].bundle.output,O_WRONLY | O_APPEND);
                }
                dup2(file_out,1);
                execvp(bundler[located].argvi[k][0],bundler[located].argvi[k]);
                exit(100+k); 
            }
        }
        for (k = 0; k < arg_size; k++) { 
            pid_t wpid = wait(&child_status);
        }
    }
    else if(bundler[located].bundle.input!=NULL){
        
        for (k = 0; k < arg_size; k++){
            if ((pid[k] = fork()) == 0) {
                int file_in = open(bundler[located].bundle.input,O_RDONLY);
                dup2(file_in,0);
                execvp(bundler[located].argvi[k][0],bundler[located].argvi[k]);
                exit(100+k); 
            }
        }
        for (k = 0; k < arg_size; k++) { 
            pid_t wpid = wait(&child_status);
        }
    }
    else{
        for (k = 0; k < arg_size; k++){
            if ((pid[k] = fork()) == 0) {
                execvp(bundler[located].argvi[k][0],bundler[located].argvi[k]);
                exit(100+k); 
            }
        }
        for (k = 0; k < arg_size; k++) { 
            pid_t wpid = wait(&child_status);
        }
    }
    
}
void forker_for_son(parsed_command command,bundle_keeper bundler[],pipe_keeper keep,int yer,int total_minus){
    pid_t pid[keep.total];
    int k, child_status,temp=0;
    int gerek=total_minus+keep.b_count-1;
    
    int b[gerek];
    int p=0;
    for(int i=1;i<keep.b_count;i++){
        if(keep.arg_count[i]==1){
            gerek--;
        }
    }
    int fd[gerek][2];
    for(int ko=0;ko<gerek;ko++){
        pipe(fd[ko]);
        b[ko]=-1;
    }   
    int c=0;
    a:
    if(yer!=keep.b_count){
       for (k = 0; k < keep.arg_count[yer]; k++){
            if ((pid[k+c] = fork()) == 0) {
               if(yer==0){
                    if(bundler[keep.location_keep[yer]].bundle.input){ 
                         int file_in = open(bundler[keep.location_keep[yer]].bundle.input,O_RDONLY);
                        dup2(file_in,0);
                        
                    }
                    for(int tut=0;tut<gerek;tut++){
                        if(tut!=temp){
                            close(fd[tut][0]);
                            close(fd[tut][1]);
                        }
                    }
                    close(fd[temp][0]);
                    dup2(fd[temp][1],1);
                    close(fd[temp][1]);      
                    execvp(bundler[keep.location_keep[yer]].argvi[k][0],bundler[keep.location_keep[yer]].argvi[k]);
                    exit(100+k+c); 
                }
                else if(yer==keep.b_count-1){
                    if(bundler[keep.location_keep[yer]].bundle.output){
                        int file_out = open(bundler[keep.location_keep[yer]].bundle.output,O_WRONLY | O_APPEND);
                        if(file_out==-1){
                            creat(bundler[keep.location_keep[yer]].bundle.output,S_IRWXG | S_IRWXU | S_IRWXO);
                            file_out = open(bundler[keep.location_keep[yer]].bundle.output,O_WRONLY | O_APPEND);
                        }
                         
                        dup2(file_out,1);
                         
                       
                    }
                    for(int tut=0;tut<gerek;tut++){
                        if(tut!=temp-keep.arg_count[yer]+k ){
                            close(fd[tut][0]);
                            close(fd[tut][1]);
                        }
                    }
                    close(fd[temp-keep.arg_count[yer]+k][1]);
                    dup2(fd[temp-keep.arg_count[yer]+k][0],0);
                    close(fd[temp-keep.arg_count[yer]+k][0]);
                    execvp(bundler[keep.location_keep[yer]].argvi[k][0],bundler[keep.location_keep[yer]].argvi[k]);
                    exit(100+k+c); 
                }
                else{
                   for(int tut=0;tut<gerek;tut++){
                        if((tut!=temp-keep.arg_count[yer]+k) && (tut!=temp)){
                            
                            close(fd[tut][0]);
                            close(fd[tut][1]);
                        }
                    }
                    close(fd[temp][0]);
                    dup2(fd[temp][1],1);
                    close(fd[temp][1]); 
                    close(fd[temp-keep.arg_count[yer]+k][1]);
                    dup2(fd[temp-keep.arg_count[yer]+k][0],0);
                    close(fd[temp-keep.arg_count[yer]+k][0]);
                    execvp(bundler[keep.location_keep[yer]].argvi[k][0],bundler[keep.location_keep[yer]].argvi[k]);
                    exit(100+k+c); 
                    
                }
                
            
                
            }
        }
        int gecici=c;
        for (int sayac = gecici; sayac <gecici+keep.arg_count[yer] ; sayac++) { 
            if(yer!=0 ){
                close(fd[temp-keep.arg_count[yer]+(sayac-gecici)][0]);
                close(fd[temp-keep.arg_count[yer]+(sayac-gecici)][1]);
            }
            pid_t wpid = wait(&child_status);
        }
        c++;
        c+=keep.arg_count[yer]-1;
        char buffer_repeater[4096]; 
        ssize_t num_red;
        if((yer+1)!=keep.b_count && keep.arg_count[yer+1]!=1){
            
            b[p++]=temp;
            num_red=0;
            
            num_red+=read(fd[temp][0], &buffer_repeater, 4096);   
            close(fd[temp][0]);
            close(fd[temp][1]);
            for(int last=temp+1;last<=temp+keep.arg_count[yer+1];last++){
                write(fd[last][1], &buffer_repeater, num_red);
            }
        }
        
        
        bundler[keep.location_keep[yer]].executed=true;
         
        yer++;
        
        temp+=1;
        bool stat=false;
        if(keep.arg_count[yer]!=1) temp+=keep.arg_count[yer];
       
        
        
        goto a;
    }
    

}
int main(int argc, char * argv[]){
    char *input_buffer;
    bundle_keeper bundler[256];
    size_t bufsize = 256;
    size_t getted;
    int is_creation=0,a;
    int i=0,j;
    input_buffer = (char *)malloc(bufsize * sizeof(char));
    if( input_buffer == NULL)
    {
        exit(-1);
    }
    parsed_input* parsed= (parsed_input*)malloc(sizeof(parsed_input));
    getted = getline(&input_buffer,&bufsize,stdin);  
    a=parse(input_buffer,is_creation,parsed);  
    bundler[i].bundle.name=parsed->command.bundle_name;
    bundler[i].executed=false;
    for(;parsed && parsed->command.type!=3;){
        if(parsed->command.type==0 || (parsed->command.type>3 && parsed->argv!=0)){
            is_creation=1;
            if(strcmp(bundler[i].bundle.name,parsed->command.bundle_name)!=0 || (strcmp(bundler[i].bundle.name,parsed->command.bundle_name)==0 && bundler[i].executed)){
                bundler[++i].bundle.name=parsed->command.bundle_name;
                bundler[i].executed=false;
                
            }
            else {
                if(parsed->command.type!=0) {
                    bundler[i].argvi[j++]=parsed->argv;
                
                }  
            }
        }
        else if(parsed->command.type==1){
            is_creation=0;
            j=0;

        }
        else if(parsed->command.type==2){
            int located=-1,bundle_size=i,arg_size=0;
            is_creation=0;
            for(int tmp=0;tmp<=bundle_size;tmp++){
                if(strcmp(bundler[tmp].bundle.name,parsed->command.bundles[0].name)==0 && !(bundler[tmp].executed)){
                    located=tmp;
                    break;
                }
            }
            
            if(located!=-1){
                for(int tmp=0;tmp<256 ;tmp++){
                    if(bundler[located].argvi[tmp]==0){
                        arg_size=tmp;
                        break;
                    }
                }
                bundler[located].bundle.input=parsed->command.bundles->input;
                bundler[located].bundle.output=parsed->command.bundles->output;
                if(parsed->command.bundle_count==1){ //For only i/o direct
                    forker_for_direction(parsed->command,bundler,located,arg_size);
                    bundler[located].executed=true;
                    parsed->command.bundles->input=NULL;
                }
                
                else{ //pipe
                    is_creation=0;
                    pipe_keeper keep;
                    bool flag=false;
                    keep.total=0;
                    int yer=0;
                    keep.b_count=parsed->command.bundle_count;
                    for(int cntup=0;cntup<parsed->command.bundle_count;cntup++){
                        int located=-1,bundle_size=i,arg_size=0;
                        
                        for(int tmp=0;tmp<=bundle_size;tmp++){
                            if(strcmp(bundler[tmp].bundle.name,parsed->command.bundles[cntup].name)==0 && !(bundler[tmp].executed)){
                                keep.location_keep[cntup]=tmp;
                                bundler[tmp].bundle.input=parsed->command.bundles[cntup].input;
                                bundler[tmp].bundle.output=parsed->command.bundles[cntup].output;
                                break;
                            }
                        }
                         for(int tmp=0;tmp<256 ;tmp++){
                            if(bundler[keep.location_keep[cntup]].argvi[tmp]==0){
                                keep.arg_count[cntup]=tmp;
                                break;
                            }
                        }
                    }
                    int tmp;
                    for(tmp=0;tmp<keep.b_count;tmp++){
                        keep.total+=keep.arg_count[tmp];
                    }
                    int total_minus=keep.total-keep.arg_count[0];
                    
                    forker_for_son(parsed->command,bundler,keep,0,total_minus);
                }
            }
           
            


        } 
    
        
        
        getted = getline(&input_buffer,&bufsize,stdin);   
        a=parse(input_buffer,is_creation,parsed);          
       

    }
    free(input_buffer);
    free(parsed);
    return 0;
}
