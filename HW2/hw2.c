
#include "hw2_output.c"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include<string.h>
struct calls{
    int sec;
    char* command;

};
struct proper_private{
    int pr_id;
    int si;
    int sj;
    int wait_time;
    int area_number;
    int * gathering_areas;
    int left_at;
};
struct sneaky_smoker{
    int sm_id;
    int smoke_time;
    int cell_count;
    int * smoking_areas;
    int lefted;
};
int *area,*priv,*priv_sol,*priv_smoke,sol_number=0,call_number=0,smoker_number=0,col,row,counter_for_fast=0;
int current_con=1,flag=0;
size_t bufsize = 256;
struct proper_private * private_list=NULL;
struct calls * call_list=NULL;
struct sneaky_smoker * smoker_list=NULL;
pthread_mutex_t * mutex_keeper=NULL;
pthread_cond_t * cond_1_smo=NULL;
pthread_cond_t * cond_2_sol=NULL;
pthread_mutex_t * mutex_keeper_for_sol=NULL;
pthread_mutex_t * mutex_keeper_for_smoker=NULL;
pthread_mutex_t orderer_mutex,fast_mutex;
pthread_mutex_t bekleyen,bekleten,cond_2,cond_1;
pthread_mutex_t kesisim,mutex_keep_unlock,mutex_keep_lock,mutex_keep_minus,mutex_keep_check,waiter_mut,mutex_unlock,mutex_keep_lock_smoker,waiter_mut_for_smoker,mutex_keep_unlock_for_smoker,mutex_keep_lock_for_smoker;
pthread_cond_t waiter_con,wait_for_fast;
pthread_cond_t checker_con;
pthread_cond_t break_con;
pthread_cond_t cont_con;
pthread_cond_t stop_con;
//Parser for int
int take_int(char* buffer,int* left){
    char * keeper=NULL;
    int result;
    keeper = (char *)malloc(bufsize * sizeof(char));
    int i;
    for(int j=0;j<bufsize;j++){
        keeper[j]=' ';
    }
    for(i=*left;i<bufsize;i++){
        if(buffer[i]==' ' || buffer[i]==10){
            break;
        }
        if(buffer[i]==EOF){
            return -2;
        }
        else{
            keeper[i-*left]=buffer[i];
        }

    }
    *left=i+1;
    result=atoi(keeper);
    free(keeper);
    return result;

}
//Parser for string
void take_string(char* buffer,int* left,char* result){
    int i;
    for(i=*left;i<bufsize;i++){
        if(buffer[i]==' ' || buffer[i]==10){

            break;
        }
        else{
            result[i-*left]=buffer[i];
        }

    }

    *left=i+1;
}
//Routine for part A
void* keep(void* arg){
    int index=*(int*)arg;
    hw2_notify(PROPER_PRIVATE_CREATED,private_list[index].pr_id,0,0);

    for(int k=0;k<private_list[index].area_number;k++){

        if(current_con==0){
            INTER:
            hw2_notify(PROPER_PRIVATE_TOOK_BREAK, private_list[index].pr_id, 0, 0);
            counter_for_fast++;
            if(counter_for_fast==sol_number){
                pthread_cond_broadcast(&wait_for_fast);
            }
            pthread_mutex_lock(&orderer_mutex);
            while(current_con==0) pthread_cond_wait(&cont_con,&orderer_mutex);
            counter_for_fast=0;
            if(current_con==1){
                hw2_notify(PROPER_PRIVATE_CONTINUED, private_list[index].pr_id, 0, 0);
                pthread_mutex_unlock(&orderer_mutex);
            }
            else if (current_con==2){

                hw2_notify(PROPER_PRIVATE_STOPPED, private_list[index].pr_id, 0, 0);
                pthread_mutex_unlock(&orderer_mutex);

                goto end;
            }

        }
        int i_location=*(private_list[index].gathering_areas+(2*(private_list[index].left_at)));
        int j_location=*(private_list[index].gathering_areas+1+(2*(private_list[index].left_at)));
        pthread_mutex_lock(&bekleyen);
        if(flag==1){
            pthread_mutex_unlock(&bekleyen);
            goto gel;
        }
        for(int i=i_location;i<(private_list[index].si+i_location);i++){ // Set Locked Areas
            int i_times_j=i*col;
            for(int j=j_location;j<(private_list[index].sj+j_location);j++){
                if(*(priv+i_times_j+j)==1){
                    pthread_mutex_unlock(&bekleyen);

                    pthread_mutex_lock(&mutex_keeper[i_times_j+j]);
                    pthread_mutex_unlock(&bekleyen); //BURAYA DİKKAT
                    pthread_mutex_lock(&bekleyen);
                    if(flag==1){
                        pthread_mutex_unlock(&bekleyen);
                        goto gel;
                    }
                }
                if(*(priv+i_times_j+j)==0){
                    pthread_mutex_unlock(&mutex_keeper[i_times_j+j]);
                }

            }
        }
        for(int i=i_location;i<(private_list[index].si+i_location);i++){ // Set Locked Areas
            int i_times_j=i*col;
            for(int j=j_location;j<(private_list[index].sj+j_location);j++){
                int gecici=*(priv_sol+i_times_j+j);
                if(gecici!=0){
                    pthread_mutex_unlock(&bekleyen);
                    pthread_mutex_lock(&mutex_keeper_for_sol[i_times_j+j]);
                    pthread_mutex_lock(&mutex_keeper_for_sol[i_times_j+j]);
                    pthread_mutex_unlock(&bekleyen);
                    pthread_mutex_lock(&bekleyen);
                    if(flag==1){
                        pthread_mutex_unlock(&mutex_keeper_for_sol[i_times_j+j]);
                        pthread_mutex_unlock(&mutex_keeper_for_sol[i_times_j+j]);
                        pthread_mutex_unlock(&bekleyen);
                        goto gel;
                    }
                }
                if(*(priv_sol+i_times_j+j)==0){
                    pthread_mutex_unlock(&mutex_keeper_for_sol[i_times_j+j]);
                    if(flag==1){
                        goto gel;
                    }
                }

            }
        }

        for(int i=i_location;i<(private_list[index].si+i_location);i++){ // Set Locked Areas
            int i_times_j=i*col;
            for(int j=j_location;j<(private_list[index].sj+j_location);j++){
                pthread_mutex_lock(&mutex_keep_lock);
                if(flag==1){
                    pthread_mutex_unlock(&mutex_keep_lock);
                    goto gel;
                }
                *(priv+i_times_j+j)=1;

                pthread_mutex_unlock(&mutex_keep_lock);
                pthread_mutex_lock(&mutex_keeper[i_times_j+j]);

                if(flag==1){
                    pthread_mutex_unlock(&mutex_keep_lock);
                    pthread_mutex_unlock(&mutex_keeper[i_times_j+j]);
                    goto gel;
                }
            }
        }
        pthread_mutex_unlock(&bekleyen);
        if(flag==1){
            goto gel;
        }
        hw2_notify(PROPER_PRIVATE_ARRIVED, private_list[index].pr_id, i_location, j_location);
        struct timespec abs_ns;
        struct timeval tp_1;


        for(int i=i_location;i<(private_list[index].si+i_location);i++){
            int i_times_j=i*col;
            for(int j=j_location;j<(private_list[index].sj+j_location);j++){
                int tut=*(area+i_times_j+j);
                for(int u=0;u<tut;u++){
                    pthread_mutex_lock(&waiter_mut);
                    gettimeofday(&tp_1, NULL);
                    abs_ns.tv_sec=tp_1.tv_sec;
                    abs_ns.tv_nsec=tp_1.tv_usec*1000;
                    if(abs_ns.tv_nsec + (private_list[index].wait_time%1000)*1000000 > 999999999){
                        abs_ns.tv_sec++;
                        abs_ns.tv_nsec = abs_ns.tv_nsec + (private_list[index].wait_time%1000)*1000000 - 999999999;
                    }

                    else{
                        abs_ns.tv_nsec += (private_list[index].wait_time%1000)*1000000;
                    }
                    abs_ns.tv_sec+=(private_list[index].wait_time/1000);
                    if(flag==1){
                        pthread_mutex_unlock(&waiter_mut);
                        goto gel;
                    }
                    if( pthread_cond_timedwait(&break_con ,&waiter_mut,&abs_ns)==0  ){ //Sinyal

                        pthread_mutex_unlock(&waiter_mut);
                        gel:
                        for(int t=0;t<row;t++){
                            int t_times_col=t*col;
                            for(int l=0;l<col;l++){
                                *(priv+t_times_col+l)=0;
                                pthread_mutex_unlock(&mutex_keeper[t_times_col+l]);
                                pthread_mutex_unlock(&mutex_keeper_for_sol[t_times_col+l]);

                            }
                        }
                        /* for(int sol_num=sol_number;sol_num>0;sol_num--){
                             pthread_mutex_unlock(&mutex_keep_lock);
                             pthread_mutex_unlock(&bekleyen);
                             pthread_mutex_unlock(&mutex_keep_unlock);
                         }*/
                        if(current_con==0){
                            goto INTER;
                        }
                        else if(current_con==2){
                            hw2_notify(PROPER_PRIVATE_STOPPED, private_list[index].pr_id, 0, 0);
                            goto end;
                        }
                    }
                    else{ //Süre bitti
                        pthread_mutex_unlock(&waiter_mut);
                    }
                    pthread_mutex_lock(&mutex_keep_unlock);
                    if(flag==1){
                        pthread_mutex_unlock(&mutex_keep_unlock);
                        goto gel;
                    }
                    *(area+i_times_j+j)-=1;
                    pthread_mutex_unlock(&mutex_keep_unlock);
                    hw2_notify(PROPER_PRIVATE_GATHERED, private_list[index].pr_id,i , j);

                }

            }

        }

        for(int i=i_location;i<(private_list[index].si+i_location);i++){ //Clear Locked Areas
            int i_times_j=i*col;
            for(int j=j_location;j<(private_list[index].sj+j_location);j++){
                pthread_mutex_lock(&mutex_keep_lock);
                *(priv+i_times_j+j)=0;

                pthread_mutex_unlock(&mutex_keep_lock);
                pthread_mutex_unlock(&mutex_keeper[i_times_j+j]);
                //pthread_cond_broadcast(&cond_1_smo[i_times_j+j]);
            }
        }

        private_list[index].left_at+=1;
        hw2_notify(PROPER_PRIVATE_CLEARED, private_list[index].pr_id, 0, 0);
    }
    hw2_notify(PROPER_PRIVATE_EXITED, private_list[index].pr_id, 0, 0);
    end:

    free(arg);
}
//Routine for part B
void* order(void* arg) {
    int index=*(int*)arg;
    for(int i=0;i<index;i++){
        long waited_time=call_list[i].sec*1000;
        if(i!=0){
            waited_time=(call_list[i].sec*1000)-(call_list[i-1].sec*1000);
        }
        if(waited_time<(500*1000) && !strcmp(call_list[i].command,"continue")){
            pthread_mutex_lock(&fast_mutex);
            while(counter_for_fast<sol_number) pthread_cond_wait(&wait_for_fast,&fast_mutex);
            pthread_mutex_unlock(&fast_mutex);
        }

        //pthread_mutex_lock(&orderer_mutex);

        usleep(waited_time);

        if(!strcmp(call_list[i].command,"break")){

            if(current_con!=0){




                current_con=0;
                flag=1;
                hw2_notify(ORDER_BREAK,0, 0, 0);
                for(int t=0;t<row;t++){
                    int t_times_col=t*col;
                    for(int l=0;l<col;l++){
                        *(priv+t_times_col+l)=0;
                        pthread_mutex_unlock(&mutex_keeper[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keeper_for_sol[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keep_lock);
                        pthread_mutex_unlock(&cond_2);
                        pthread_mutex_unlock(&cond_1);
                        pthread_mutex_unlock(&bekleyen);
                        pthread_mutex_unlock(&bekleten);

                        pthread_mutex_unlock(&mutex_keep_minus);
                        pthread_mutex_unlock(&mutex_keep_unlock);
                        pthread_mutex_unlock(&mutex_keep_check);
                        pthread_mutex_unlock(&kesisim);
                        pthread_mutex_unlock(&waiter_mut);
                        pthread_mutex_unlock(&mutex_unlock);


                    }
                }

                pthread_cond_broadcast(&break_con);
                //pthread_mutex_unlock(&orderer_mutex);

            }
            else{
                hw2_notify(ORDER_BREAK,0, 0, 0);

                //pthread_mutex_unlock(&orderer_mutex);
            }

        }
        else if(!strcmp(call_list[i].command,"continue")){

            if(current_con!=1){



                hw2_notify(ORDER_CONTINUE,0, 0, 0);
                current_con=1;
                flag=0;
                pthread_cond_broadcast(&cont_con);
                //pthread_mutex_unlock(&orderer_mutex);

            }
            else{
                hw2_notify(ORDER_CONTINUE,0, 0, 0);
                //pthread_mutex_unlock(&orderer_mutex);
            }
        }
        else if(!strcmp(call_list[i].command,"stop")){

            if(current_con!=2){

                hw2_notify(ORDER_STOP,0, 0, 0);
                current_con=2;
                flag=1;
                for(int t=0;t<row;t++){
                    int t_times_col=t*col;
                    for(int l=0;l<col;l++){
                        *(priv+t_times_col+l)=0;
                        pthread_mutex_unlock(&mutex_keeper[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keeper_for_sol[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keeper_for_smoker[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keep_lock);
                        pthread_mutex_unlock(&cond_2);
                        pthread_mutex_unlock(&cond_1);
                        pthread_mutex_unlock(&bekleyen);
                        pthread_mutex_unlock(&bekleten);
                        pthread_mutex_unlock(&mutex_keep_lock_smoker);
                        pthread_mutex_unlock(&waiter_mut_for_smoker);
                        pthread_mutex_unlock(&mutex_keep_unlock_for_smoker);
                        pthread_mutex_unlock(&mutex_keep_lock_for_smoker);
                        pthread_mutex_unlock(&mutex_keep_minus);
                        pthread_mutex_unlock(&mutex_keep_unlock);
                        pthread_mutex_unlock(&mutex_keep_check);
                        pthread_mutex_unlock(&kesisim);
                        pthread_mutex_unlock(&orderer_mutex);
                        pthread_mutex_unlock(&waiter_mut);
                        pthread_mutex_unlock(&mutex_unlock);
                        pthread_mutex_unlock(&fast_mutex);

                    }
                }
                pthread_cond_broadcast(&break_con);
                pthread_cond_broadcast(&cont_con);
                pthread_cond_broadcast(&stop_con);

                // pthread_mutex_unlock(&orderer_mutex);
                for(int j=0;j<row;j++){
                    int j_times_col=j*col;
                    for(int k=0;k<col;k++){
                        *(priv+j_times_col+k)=0;
                    }
                }


            }
            else{
                hw2_notify(ORDER_STOP,0, 0, 0);
                //pthread_mutex_unlock(&orderer_mutex);
            }
        }
        else{

            //pthread_mutex_unlock(&orderer_mutex);
        }

    }

    free(arg);
}
//Routine for part C
void* smoke(void* arg) {
    int index=*(int*)arg;
    hw2_notify(SNEAKY_SMOKER_CREATED,smoker_list[index].sm_id,0,0);
    for(int k=0;k<smoker_list[index].cell_count;k++){

        int i_location=*(smoker_list[index].smoking_areas+(3*(smoker_list[index].lefted)));
        int j_location=*(smoker_list[index].smoking_areas+1+(3*(smoker_list[index].lefted)));
        int cig_number=*(smoker_list[index].smoking_areas+2+(3*(smoker_list[index].lefted)));
        pthread_mutex_lock(&bekleyen);
        if(*(priv_smoke+(i_location*col)+j_location)==1){
            pthread_mutex_unlock(&bekleyen);
            pthread_mutex_lock(&mutex_keeper_for_smoker[(i_location*col)+j_location]);
            pthread_mutex_unlock(&bekleyen);
            pthread_mutex_lock(&bekleyen);
            if(flag==1 && current_con==2){
                pthread_mutex_unlock(&bekleyen);
                goto gel_smo;
            }

        }
        if(*(priv_smoke+(i_location*col)+j_location)==0){
            pthread_mutex_unlock(&mutex_keeper_for_smoker[(i_location*col)+j_location]);
        }
        *(priv_smoke+(i_location*col)+j_location)=1;
        pthread_mutex_lock(&mutex_keeper_for_smoker[(i_location*col)+j_location]);
        for(int i=i_location-1;i<(2+i_location);i++){ // Set Locked Areas
            int i_times_j=i*col;
            for(int j=j_location-1;j<(2+j_location);j++){
                if(*(priv+i_times_j+j)==1){
                    pthread_mutex_unlock(&bekleyen);
                    pthread_mutex_lock(&mutex_keeper[i_times_j+j]);
                    pthread_mutex_unlock(&bekleyen);
                    pthread_mutex_lock(&bekleyen);
                }
                if(*(priv+i_times_j+j)==0){

                    pthread_mutex_unlock(&mutex_keeper[i_times_j+j]);

                }
            }
        }




        for(int i=i_location-1;i<(2+i_location);i++){ // Set Locked Areas
            int i_times_j=i*col;
            for(int j=j_location-1;j<(2+j_location);j++){
                pthread_mutex_lock(&mutex_keep_lock_smoker);
                *(priv_sol+i_times_j+j)+=1;
                pthread_mutex_unlock(&mutex_keep_lock_smoker);


               // pthread_mutex_lock(&mutex_keeper_for_sol[i_times_j+j]);
            }
        }
        pthread_mutex_unlock(&bekleyen);
        if(flag==1 && current_con==2){
            goto gel_smo;
        }
        hw2_notify(SNEAKY_SMOKER_ARRIVED, smoker_list[index].sm_id, i_location, j_location);

        struct timespec abs_s;
        struct timeval tp;

        for(int u=0;u<cig_number;u++){
            pthread_mutex_lock(&waiter_mut_for_smoker);
            gettimeofday(&tp, NULL);
            abs_s.tv_sec=tp.tv_sec;
            abs_s.tv_nsec=tp.tv_usec*1000;
            if(abs_s.tv_nsec + (smoker_list[index].smoke_time%1000)*1000000 > 999999999){
                abs_s.tv_sec++;
                abs_s.tv_nsec = abs_s.tv_nsec + (smoker_list[index].smoke_time%1000)*1000000 - 999999999;
            }

            else{
                abs_s.tv_nsec += (smoker_list[index].smoke_time%1000)*1000000;
            }
            abs_s.tv_sec+=(smoker_list[index].smoke_time/1000);
            if( pthread_cond_timedwait(&stop_con ,&waiter_mut_for_smoker,&abs_s)==0  ){ //Sinyal
                pthread_mutex_unlock(&waiter_mut_for_smoker);
                gel_smo:
                for(int t=0;t<row;t++){
                    int t_times_col=t*col;
                    for(int l=0;l<col;l++){
                        *(priv_smoke+t_times_col+l)=0;
                        *(priv_sol+t_times_col+l)=0;
                        *(priv+t_times_col+l)=0;
                        pthread_mutex_unlock(&mutex_keeper[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keeper_for_smoker[t_times_col+l]);
                        pthread_mutex_unlock(&mutex_keeper_for_sol[t_times_col+l]);
                    }
                }

                hw2_notify(SNEAKY_SMOKER_STOPPED, smoker_list[index].sm_id, 0, 0);
                goto end;
            }
            else{ //Süre bitti
                pthread_mutex_unlock(&waiter_mut_for_smoker);
            }
            pthread_mutex_lock(&mutex_keep_unlock_for_smoker);
            if(flag==1 && current_con==2){
                pthread_mutex_unlock(&mutex_keep_unlock_for_smoker);
                goto gel_smo;
            }
            switch (u%9) {
                case 0:
                    *(area + ((i_location - 1) * col) + j_location - 1) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, (i_location - 1), j_location - 1);
                    break;
                case 1:
                    *(area + ((i_location - 1) * col) + j_location) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, (i_location - 1), j_location);
                    break;
                case 2:
                    *(area + ((i_location - 1) * col) + j_location + 1) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, i_location - 1, j_location + 1);
                    break;
                case 3:
                    *(area + ((i_location) * col) + j_location + 1) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, i_location, j_location + 1);
                    break;
                case 4:
                    *(area + ((i_location + 1) * col) + j_location + 1) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, i_location + 1, j_location + 1);
                    break;
                case 5:
                    *(area + ((i_location + 1) * col) + j_location) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, i_location + 1, j_location);
                    break;
                case 6:
                    *(area + ((i_location + 1) * col) + j_location - 1) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, i_location + 1, j_location - 1);
                    break;
                case 7:
                    *(area + ((i_location) * col) + j_location - 1) += 1;
                    hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_list[index].sm_id, i_location, j_location - 1);
                    break;
                default:
                    break;
            }
            pthread_mutex_unlock(&mutex_keep_unlock_for_smoker);

        }



        hw2_notify(SNEAKY_SMOKER_LEFT, smoker_list[index].sm_id, 0, 0);
        for(int i=i_location-1;i<(2+i_location);i++){ //Clear Locked Areas
            int i_times_j=i*col;
            for(int j=j_location-1;j<(2+j_location);j++){
                pthread_mutex_lock(&mutex_keep_lock_smoker);
                *(priv_sol+i_times_j+j)-=1;

                pthread_mutex_unlock(&mutex_keep_lock_smoker);
                if(*(priv_sol+i_times_j+j)==0){
                    pthread_mutex_unlock(&mutex_keeper_for_sol[i_times_j+j]);
                }

            }
        }

        *(priv_smoke+(i_location*col)+j_location)=0;
        pthread_mutex_unlock(&mutex_keeper_for_smoker[(i_location*col)+j_location]);
        smoker_list[index].lefted+=1;
    }
    hw2_notify(SNEAKY_SMOKER_EXITED, smoker_list[index].sm_id, 0, 0);
    end:
    free(arg);
}

int main(int argc, char * argv[]){

    hw2_init_notifier();


    int left[1];
    *left =0;
    //Input Taking For Part A -------------------------------------------------------------------------------------------
    char *buffer;

    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));

    if( buffer == NULL)
    {

        exit(-1);
    }

    characters = getline(&buffer,&bufsize,stdin);

    row=take_int(buffer,left);
    col=take_int(buffer,left);

    area = (int*)malloc((row*col) * sizeof(int));
    priv = (int*)malloc((row*col) * sizeof(int));
    priv_smoke = (int*)malloc((row*col) * sizeof(int));
    priv_sol= (int*)malloc((row*col) * sizeof(int));
    for(int i=0;i<row;i++) {
        *left = 0;
       // getchar();
        characters = getline(&buffer, &bufsize, stdin);
        for (int j = 0; j < col; j++) {
            *(area+i*col+j) = take_int(buffer,left);
            *(priv+i*col+j) = 0;
            *(priv_smoke+i*col+j) = 0;
            *(priv_sol+i*col+j) = 0;
        }

    }
    *left=0;
    //getchar();
    characters = getline(&buffer,&bufsize,stdin);
    sol_number=take_int(buffer,left);


    private_list =(struct proper_private *)malloc(sol_number * sizeof(struct proper_private));
    //getchar();
    for(int i=0;i<sol_number;i++){
        *left=0;
        //if(i!=0) getchar();
        characters = getline(&buffer,&bufsize,stdin);
        private_list[i].pr_id=take_int(buffer,left);
        private_list[i].si=take_int(buffer,left);
        private_list[i].sj=take_int(buffer,left);
        private_list[i].wait_time=take_int(buffer,left);
        private_list[i].area_number=take_int(buffer,left);
        private_list[i].left_at=0;
        private_list[i].gathering_areas= (int*)malloc(private_list[i].area_number*2*sizeof(int));
        for(int j=0;j<private_list[i].area_number;j++){
            *left=0;
          //  getchar();
            characters = getline(&buffer,&bufsize,stdin);
            *(private_list[i].gathering_areas+(2*j))=take_int(buffer,left);
            *(private_list[i].gathering_areas+(2*j)+1)=take_int(buffer,left);
        }

    }

    //Input Taking For Part A  -------------------------------------------------------------------------------------------
    char a;
    a=getchar();
    if(a!=EOF){
        *left=0;
        characters = getline(&(buffer),&bufsize,stdin);
        call_number= take_int(buffer,left);
        if(call_number!=0){
            if(call_number/100>0){
                call_number+=((((int)a)-48)*1000);
            }
            else if(call_number/10>0){
                call_number+=((((int)a)-48)*100);
            }
            else if(call_number/1>0){
                call_number+=((((int)a)-48)*10);
            }

        }
        else {

            call_number=((int)a)-48;
        }
        call_list =(struct calls *)malloc(call_number * sizeof(struct calls));
        for(int i=0;i<call_number;i++){
            *left=0;
            call_list[i].command=(char*) malloc(bufsize*sizeof(char ));
            for (int al = 0; al < bufsize; ++al) {
                call_list[i].command[al]=NULL;
            }
            // getchar();
            characters = getline(&buffer,&bufsize,stdin);
            call_list[i].sec= take_int(buffer,left);
            take_string(buffer,left,call_list[i].command);

        }
        //Input Taking For Part B   -------------------------------------------------------------------------------------------
        a=getchar();
        if(a!=EOF){
            *left=0;
            characters = getline(&(buffer),&bufsize,stdin);
            smoker_number= take_int(buffer,left);
            if(smoker_number!=0){
                if(smoker_number/100>0){
                    smoker_number+=((((int)a)-48)*1000);
                }
                else if(smoker_number/10>0){
                    smoker_number+=((((int)a)-48)*100);
                }
                else if(smoker_number/1>0){
                    smoker_number+=((((int)a)-48)*10);
                }

            }
            else smoker_number=((int)a)-48;
            smoker_list =(struct sneaky_smoker *)malloc(smoker_number * sizeof(struct sneaky_smoker));
            for(int i=0;i<smoker_number;i++){
                *left=0;
                //getchar();
                characters = getline(&buffer,&bufsize,stdin);
                smoker_list[i].sm_id= take_int(buffer,left);
                (*(smoker_list+i)).smoke_time= take_int(buffer,left);
                (*(smoker_list+i)).cell_count= take_int(buffer,left);
                (*(smoker_list+i)).lefted=0;
                (*(smoker_list+i)).smoking_areas= (int*)malloc(smoker_list[i].cell_count*3*sizeof(int));
                for(int j=0;j<smoker_list[i].cell_count;j++){
                    *left=0;
                    //
                    // getchar();
                    characters = getline(&buffer,&bufsize,stdin);
                    *(smoker_list[i].smoking_areas+(3*j))=take_int(buffer,left);
                    *(smoker_list[i].smoking_areas+(3*j)+1)=take_int(buffer,left);
                    *(smoker_list[i].smoking_areas+(3*j)+2)=take_int(buffer,left);
                }

            }
        }

    }





    //Input Taking For Part C   -------------------------------------------------------------------------------------------

    pthread_t th[sol_number];
    mutex_keeper=(pthread_mutex_t *) malloc((row*col)*sizeof(pthread_mutex_t));
    mutex_keeper_for_sol=(pthread_mutex_t *) malloc((row*col)*sizeof(pthread_mutex_t));
    mutex_keeper_for_smoker=(pthread_mutex_t *) malloc((row*col)*sizeof(pthread_mutex_t));
   // cond_2_sol=(pthread_mutex_t *) malloc((row*col)*sizeof(pthread_cond_t));;
   // cond_1_smo=(pthread_mutex_t *) malloc((row*col)*sizeof(pthread_cond_t));;
    for(int i=0;i<row*col;i++){
        pthread_mutex_init(&mutex_keeper[i],NULL);
        pthread_mutex_init(&mutex_keeper_for_smoker[i],NULL);
        pthread_mutex_init(&mutex_keeper_for_sol[i],NULL);
       // pthread_cond_init(&cond_2_sol[i],NULL);
       // pthread_cond_init(&cond_1_smo[i],NULL);
    }
    pthread_mutex_init(&mutex_keep_check,NULL);
    pthread_mutex_init(&kesisim,NULL);
    pthread_mutex_init(&cond_1,NULL);
    pthread_mutex_init(&cond_2,NULL);
    pthread_mutex_init(&waiter_mut_for_smoker,NULL);
    pthread_mutex_init(&mutex_keep_unlock_for_smoker,NULL);
    pthread_mutex_init(&mutex_keep_lock_for_smoker,NULL);
    pthread_mutex_init(&mutex_unlock,NULL);
    pthread_mutex_init(&mutex_keep_lock,NULL);
    pthread_mutex_init(&mutex_keep_lock_smoker,NULL);
    pthread_mutex_init(&mutex_keep_minus,NULL);
    pthread_mutex_init(&mutex_keep_unlock,NULL);
    pthread_mutex_init(&orderer_mutex, NULL);
    pthread_mutex_init(&waiter_mut,NULL);
    pthread_mutex_init(&bekleyen,NULL);
    pthread_mutex_init(&fast_mutex,NULL);
    pthread_mutex_init(&bekleten,NULL);
    pthread_cond_init(&waiter_con,NULL);
    pthread_cond_init(&checker_con,NULL);
    pthread_cond_init(&break_con,NULL);
    pthread_cond_init(&cont_con,NULL);
    pthread_cond_init(&stop_con,NULL);
    pthread_cond_init(&wait_for_fast,NULL);
    for (int i = 0; i < sol_number; ++i) {
        int *keeper=(int *)malloc(sizeof(int));
        *keeper=i;
        if(pthread_create(&th[i],NULL,&keep,keeper)!=0){
            perror("Failed to create thread");
        }
    }


    // -------------------------------------------------------------------------------- ROUTINE FOR PART B
    pthread_t th_sol;
    if(call_number>=1){

        int *keeper=(int *)malloc(sizeof(int));
        *keeper=call_number;
        if(pthread_create(&th_sol,NULL,&order,keeper)!=0){
            perror("Failed to create thread");
        }
    }

    // -------------------------------------------------------------------------------- ROUTINE FOR PART C
    pthread_t th_smoker[smoker_number];
    for (int i = 0; i < smoker_number; ++i) {
        int *keeper=(int *)malloc(sizeof(int));
        *keeper=i;
        if(pthread_create(&th_smoker[i],NULL,&smoke,keeper)!=0){
            perror("Failed to create thread");
        }
    }


    //Deallocation

    for (int  i= 0;  i< sol_number; ++i) {
        if(pthread_join(th[i],NULL)!=0){
            perror("Failed to join thread");
        }
    }
    if(call_number>=1){
        if(pthread_join(th_sol,NULL)!=0){
            perror("Failed to join thread");
        }
    }

    for (int  i= 0;  i< smoker_number; ++i) {
        if(pthread_join(th_smoker[i],NULL)!=0){
            perror("Failed to join thread");
        }
    }
    for(int i=0;i<row*col;i++){
        pthread_mutex_destroy(&mutex_keeper[i]);
        pthread_mutex_destroy(&mutex_keeper_for_smoker[i]);
        pthread_mutex_destroy(&mutex_keeper_for_sol[i]);
       // pthread_cond_destroy(&cond_2_sol[i]);
       // pthread_cond_destroy(&cond_1_smo[i]);
    }
    pthread_mutex_destroy(&mutex_keep_lock);
    pthread_mutex_destroy(&cond_2);
    pthread_mutex_destroy(&cond_1);
    pthread_mutex_destroy(&bekleyen);
    pthread_mutex_destroy(&bekleten);
    pthread_mutex_destroy(&mutex_keep_lock_smoker);
    pthread_mutex_destroy(&waiter_mut_for_smoker);
    pthread_mutex_destroy(&mutex_keep_unlock_for_smoker);
    pthread_mutex_destroy(&mutex_keep_lock_for_smoker);
    pthread_mutex_destroy(&mutex_keep_minus);
    pthread_mutex_destroy(&mutex_keep_unlock);
    pthread_mutex_destroy(&mutex_keep_check);
    pthread_mutex_destroy(&kesisim);
    pthread_mutex_destroy(&orderer_mutex);
    pthread_mutex_destroy(&waiter_mut);
    pthread_cond_destroy(&waiter_con);
    pthread_cond_destroy(&break_con);
    pthread_cond_destroy(&cont_con);
    pthread_cond_destroy(&stop_con);
    pthread_cond_destroy(&wait_for_fast);
    pthread_cond_destroy(&checker_con);
    pthread_mutex_destroy(&mutex_unlock);
    pthread_mutex_destroy(&fast_mutex);
    free(area);
    free(priv);
    free(priv_smoke);
    free(priv_sol);
    free(buffer);
    free(mutex_keeper);
    free(mutex_keeper_for_smoker);
    free(mutex_keeper_for_sol);
    for(int i=0;i<sol_number;i++){
        free(private_list[i].gathering_areas);
    }
    free(private_list);
    for(int i=0;i<call_number;i++){
        free(call_list[i].command);
    }
    free(call_list);

    for(int i=0;i<smoker_number;i++){
        free(smoker_list[i].smoking_areas);

    }
    free(smoker_list);
    return 0;
}