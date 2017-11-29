#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define MAXPATH 1024
#define CMDLEN  300

int is_end_with(const char *,char *);


int main(){

     
    /*获取当前目录*/
    char buffer[MAXPATH];
    getcwd(buffer,MAXPATH);
    printf("current work directory is %s\n",buffer);


   /*开始检测子文件夹和文件的大小*/
   DIR *currDir;
   struct dirent *entry;

   if((currDir=opendir(buffer))==NULL){
       printf("open directory fail!");
       exit(1);
   }


   //获取当日凌晨的时间
   time_t currDay = time(NULL);   
   struct tm *currTm;
   currTm = localtime(&currDay);
   currTm->tm_hour = 0;
   currTm->tm_min  = 0;
   currTm->tm_sec  = 0;

   currDay = mktime(currTm);
 

   struct stat tstat;

   char cmd[CMDLEN];

   /*如果目录成功打开了，则开始获取下面的所有文件名字*/
   while(entry=readdir(currDir)){

       /*不能把.和..目录也算进去了,而且如果已经是gz或者zip结尾的文件，也不需要处理*/
       int nlength = strlen(entry->d_name);
       if(entry->d_name[0] != '.' && is_end_with(entry->d_name,"gz") != 1 && is_end_with(entry->d_name,"zip") !=1 ){
           char rname[MAXPATH];
           realpath(entry->d_name,rname);

           if(lstat(rname,&tstat)<0){
       		printf("get flie info failed!\n");
                continue;
           }

           //判断时间要在今天之前的才压缩
           if(tstat.st_mtime > currDay){

            printf("file %s  created today.ignore!\n",entry->d_name);
               continue;
           }

           if(S_ISREG(tstat.st_mode)){
               printf("regual file:%s,size is:%ld\n",entry->d_name,tstat.st_size);
               memset(cmd,0,CMDLEN);
               strcpy(cmd,"gzip  ");
               strcat(cmd,entry->d_name);
               system(cmd);
               printf("file:%s zip success!\n",entry->d_name);
           }

           if(S_ISDIR(tstat.st_mode)){
               printf("directory file:%s,size is:%ld\n",entry->d_name,tstat.st_size);
               memset(cmd,0,CMDLEN);
               strcpy(cmd,"tar -czvf ");
               strcat(cmd,entry->d_name);
               strcat(cmd,".tar.gz  ");
               strcat(cmd,entry->d_name); 
               strcat(cmd,"  &&  rm -r  ");
               strcat(cmd,entry->d_name);
               system(cmd);
               printf("execute command:%s!\n",cmd);
               printf("directory:%s zip success!\n",entry->d_name);  
           }
	}

   }

   closedir(currDir);
    


    exit(0);

}



int is_end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

