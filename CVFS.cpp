#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>  //For macros
#include<iostream>

#define MAXINODE 5

#define READ 1
#define WRITE 2

#define MAXFILESIZE 100

#define REGULAR 1 //.txt, .c .cpp
#define SPECIAL 2 //drive link etc

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock //second block in hard disk
{
    int TotalInodes;
    int FreeInode;
}SUPERBLOCK,*PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int INodeNumber;
    int FileSize;              //depends on macro maxfilesize
    int FileActualSize;
    int FileType;              //regular
    char *Buffer;
    int LinkCount;             //always 1
    int ReferenceCount;        //always 1
    int permission;            //3 read+write
    struct inode *next;        //indicates SinglyLL
}INODE,*PINODE,**PPINODE;

typedef struct fileTable
{
    int readoffset;            //from where to start reading
    int writeoffset;           //from where to start writing
    int count;                 //always 1
    int mode;                  //either 1 or 2 or 3
    PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt            //ufdt->filetable->inode(part of iit)
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[MAXINODE];              //creating UFDT array 
SUPERBLOCK SUPERBLOCKobj;      //global object for superblock
PINODE head=NULL;

void man(char* name)
{
    if(name==NULL) return;

    if(strcmp(name,"create")==0)
    {
        printf("Description: Used to create new regular file\n");
        printf("Usage: create File_name Permission\n");
    }
    else if(strcmp(name,"read")==0)
    {
        printf("Description: Used to read data from regular file\n");
        printf("Usage :read File_Name No_Of_Bytes_To_Read\n");
    }
    else if(strcmp(name,"write")==0)
    {
        printf("Description: Used to write into regular file\n");
        printf("Usage :read File_Name\n After this enter the data that we want to write in the file\n");
    }
    else if(strcmp(name,"ls")==0)
    {
        printf("Description: Used to list all information of files\n");
        printf("Usage :ls\n");
    }
    else if(strcmp(name,"stat")==0)
    {
        printf("Description: Used to display information of file\n");
        printf("Usage :stat File_name\n");
    }
    else if(strcmp(name,"fstat")==0)
    {
        printf("Description: Used to display information of file\n");
        printf("Usage :stat File_Descriptor\n");
    }
    else if(strcmp(name,"truncate")==0)
    {
        printf("Description: Used to remove data from file\n");
        printf("Usage :truncate File_Name\n");
    }
    else if(strcmp(name,"open")==0)
    {
        printf("Description: Used to open existing file\n");
        printf("Usage :open File_Name mode\n");
    }
    else if(strcmp(name,"close")==0)
    {
        printf("Description: Used to close opened file\n");
        printf("Usage :close File_Name\n");
    }
    else if(strcmp(name,"closeall")==0)
    {
        printf("Description: Used to close all opened file\n");
        printf("Usage :closeall\n");
    }
    else if(strcmp(name,"lseek")==0)
    {
        printf("Description: Used to change file offset\n");
        printf("Usage :lseek File_Name ChangeInOffset StartPoint\n");
    }
    else if(strcmp(name,"rm")==0)
    {
        printf("Description: Used to delete the file\n");
        printf("Usage :rm File_Name\n");
    }
    else
    {
        printf("ERROR: No manual entry available\n");
    } 
}

void DisplayHelp()
{
    printf("ls: To List out all files\n");
    printf("clear:To clear console\n");
    printf("open:To open the file\n");
    printf("close: To close the file\n");
    printf("closeall: To close all opened file\n");
    printf("read:To read all contents from files\n");
    printf("write:To write comtents in the file\n");
    printf("exit:To terminate the file system\n");
    printf("stat: To display information of file using name\n");
    printf("fstat:To display information of file using file descriptor\n");
    printf("truncate:To remove all data from file\n");
    printf("rm: To dlete the file\n");
}
int GetFDFromName(char *name)
{
    int i=0;
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable!=NULL)
        {
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
            {
                break;
            }
        }
        i++;
    }
    if(i==50)
    {
        return -1;
    }
    else
    {
        return i;
    }
}
PINODE Get_Inode(char *name)
{
    PINODE temp=head;
    int i=0;

    if(name==NULL)
    {
        return NULL;
    }
    while(temp!=NULL)
    {
        if(strcmp(name,temp->FileName)==0)
        {
            break;
        }
        temp=temp->next;
    }
    return temp;
}
void CraeteDILB()                  //Like InsertLast but it is only called once and creates LL with 5 inodes
{
    int i=1;
    PINODE newn=NULL;
    PINODE temp=head;

    while(i<= MAXINODE)  //i<=5
    {
        newn=(PINODE)malloc(sizeof(INODE));
        newn->LinkCount=0;
        newn->ReferenceCount=0;
        newn->FileType=0;
        newn->FileSize=0;

        newn->Buffer=NULL;
        newn->next=NULL;

        newn->INodeNumber=i;          //creates 1 2 3 4 5 inode with number

        if(temp == NULL)
        {
            head=newn;
            temp=head;
        }
        else
        {
            temp->next=newn;
            temp=temp->next;
        }
        i++;
    }
    printf("DILB created successfully\n");
}

void IntializeSuperBlock()          //Putting NULL in each UFDTArr member
{
    int i=0;
    while(i<MAXINODE)
    {
        UFDTArr[i].ptrfiletable=NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes=MAXINODE;
    SUPERBLOCKobj.FreeInode=MAXINODE;
}

int CreateFile(char *name,int permission)
{
    int i=0;
    PINODE temp=head;

    if(name == NULL || (permission==0)||(permission >3))
    {
        return -1;
    }
    if(SUPERBLOCKobj.FreeInode==0)
    {
        return -2;
    }
    SUPERBLOCKobj.FreeInode--;

    if(Get_Inode(name)!=NULL)
    {
        return -3;
    }
    while(temp!=NULL)
    {
        if(temp->FileType==0)
        {
            break;
        }
        temp=temp->next;
    }
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=permission;
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;

    UFDTArr[i].ptrfiletable->ptrinode=temp;

    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
    UFDTArr[i].ptrfiletable->ptrinode->permission=permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char*)malloc(MAXFILESIZE);
    return i;
}

int rm_File(char *name)
{
    int fd=0;

    fd=GetFDFromName(name);
    if(fd==-1)
    {
        return -1;
    }

    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if((UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)==0)
    {
        (UFDTArr[fd].ptrfiletable->ptrinode->FileType)=0;
        free(UFDTArr[fd].ptrfiletable);
    }

    UFDTArr[fd].ptrfiletable=NULL;
    (SUPERBLOCKobj.FreeInode)++;
}

int ReadFile(int fd,char *arr,int isize)
{
    int read_size=0;
    if(UFDTArr[fd].ptrfiletable==NULL)
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable->mode!=READ && UFDTArr[fd].ptrfiletable->mode!=READ+WRITE)
    {
        return -2;
    }
    if(UFDTArr[fd].ptrfiletable->ptrinode->permission!=READ && UFDTArr[fd].ptrfiletable->ptrinode->permission!=READ+WRITE)
    {
        return -2;
    }
    if(UFDTArr[fd].ptrfiletable->readoffset==UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
    {
        return -3;
    }
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
    {
        return -4;
    }
    read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->readoffset);
    if(read_size<isize)
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);

        UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+read_size;
    }
    else
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+UFDTArr[fd].ptrfiletable->readoffset,isize);

        (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset)+isize;
    }
    return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode)!=WRITE)&& ((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE))
    {
        return -1;
    }
    if(((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=READ+WRITE))
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->writeoffset)==MAXFILESIZE)
    {
        return -2;
    }
    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR)
    {
        return -3;
    }

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

    (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+isize;
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;

    return isize;
}

int OpenFile(char *name,int mode)
{
    int i=0;
    PINODE temp=NULL;

    if(name==NULL || mode <0)
    {
        return -1;
    }
    temp=Get_Inode(name);
    if(temp == NULL)
    {
        return -2;
    }
    if(temp->permission< mode)
    {
        return -3;
    }
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=mode;

    if(mode == READ+WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
        UFDTArr[i].ptrfiletable->writeoffset=0;

    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }

    UFDTArr[i].ptrfiletable->ptrinode=temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    return i;
}

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;

    UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount--;
}
int CloseFileByName(char *name)
{
    int i=0;
    i=GetFDFromName(name);
    if(i==-1)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;

    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount--;

    return 0;
}

void CloseAllFile()
{
    int i=0;
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable!=NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset=0;
            UFDTArr[i].ptrfiletable->writeoffset=0;

            UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount--;
            break;
        }
        i++;
    }
}

int LseekFile(int fd,int size,int from)
{
    if((fd <0) ||(from >2))
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable==NULL)
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode==READ+WRITE))
    {
        if(from ==CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if((UFDTArr[fd].ptrfiletable->readoffset)+size < 0)
            {
                return -1;
            }
            UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if(size<0)
            {
                return -1;
            }
            UFDTArr[fd].ptrfiletable->readoffset=size;
        }
        else if(from == END)
        {
            if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size)>MAXFILESIZE)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size) <0)
            {
                return -1;
            }
            UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize+size;
        }
    }
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size) >MAXFILESIZE)
            {
                return -1;
            }
            if((UFDTArr[fd].ptrfiletable->writeoffset)+size <0)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size) >(UFDTArr[fd].ptrfiletable==NULL))
            {
                UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=UFDTArr[fd].ptrfiletable->writeoffset+size;
            }
            UFDTArr[fd].ptrfiletable->writeoffset=UFDTArr[fd].ptrfiletable->writeoffset+size;
        }
        else if(from == START)
        {
            if(size >MAXFILESIZE)
            {
                return -1;
            }
            if(size<0)
            {
                return -1;
            }
            if(size > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
            {
                UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=size;
            }
            UFDTArr[fd].ptrfiletable->writeoffset=size;
        }
        else if(from == END)
        {
            if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size) >MAXFILESIZE)
            {
                return -1;
            }
            if((UFDTArr[fd].ptrfiletable->writeoffset+size)<0)
            {
                return -1;
            }
            UFDTArr[fd].ptrfiletable->writeoffset=UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize+size;
        }
    }
}

void ls_file()
{
    int i=0;
    PINODE temp=head;
    if(SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("ERROR: There are no files\n");
        return ;
    }

    printf("\n FileNAme\tInodeNumber\tFileSize\tLinkCount\n");
    printf("-------------------------------------------\n");
    while(temp!=NULL)
    {
        if(temp->FileType!=0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->INodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp=temp->next;
    }
    printf("-------------------------------------------------\n");
}

int fstat_file(int fd)
{
    PINODE temp=head;
    int i=0;
    if(fd <0)
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable==NULL)
    {
        return -2;
    }
    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("-------Statistical Information about file------------\n");
    printf("FileName: %s\n",temp->FileName);
    printf("INode Number: %d\n",temp->INodeNumber);
    printf("FileSize:%d\n",temp->FileSize);
    printf("FileActualSize: %d\n",temp->FileActualSize);
    printf("LinkCount: %d\n",temp->LinkCount);
    printf("Rferenece count:%d \n",temp->ReferenceCount);

    if(temp->permission==1)
    {
        printf("File Permission:Read Only\n");
    }
    else if(temp->permission == 2)
    {
        printf("File Permission:Write\n");
    }
    else if(temp->permission==3)
    {
        printf("File Permission:Read & Write\n");
    }
    printf("-----------------------------------------------\n");

    return 0;
}

int stat_file(char *name)
{
    PINODE temp=head;
    int i=0;
    if(name == NULL)
    {
        return -1;
    }
    while(temp!=NULL)
    {
        if(strcmp(name,temp->FileName)==0)
        {
            break;
        }
        temp=temp->next;
    }
    if(temp==NULL)
    {
        return -2;
    }
    printf("-------Statistical Information about file------------\n");
    printf("FileNAme: %s\n",temp->FileName);
    printf("INode Number: %d\n",temp->INodeNumber);
    printf("FileSize:%d\n",temp->FileSize);
    printf("FileActualSize: %d\n",temp->FileActualSize);
    printf("LinkCount: %d\n",temp->LinkCount);
    printf("Rferenece count:%d \n",temp->ReferenceCount);

    if(temp->permission==1)
    {
        printf("File Permission:Read Only\n");
    }
    else if(temp->permission == 2)
    {
        printf("File Permission:Write\n");
    }
    else if(temp->permission==3)
    {
        printf("File Permission:Read & Write\n");
    }
    printf("-----------------------------------------------\n");

    return 0;
}

int truncate_File(char *name)
{
    int fd=GetFDFromName(name);
    if(fd == -1)
    {
        return -1;
    }
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;
}

int main()
{
    char *ptr=NULL;
    int ret=0,fd=0,count=0;
    char command[4][80],str[80],arr[1024];

    IntializeSuperBlock();
    CraeteDILB();

    while(1)                    //shell //unconditional loop
    {
        fflush(stdin);          //string method to use scanf properly
        strcpy(str,"");         //empty the string

        printf("Marvellous VFS: >"); 
        fgets(str,80,stdin);         //in termonal one line is of 80 letters   //like scanf f=file g=take s=string

        count=sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);   //splilting along spaces and return no. of words

        if(count==1)
        {
            if(strcmp(command[0],"ls")==0)
            {
                ls_file();
            }
            else if(strcmp(command[0],"closeall")==0)
            {
                CloseAllFile();
                printf("All files closed succesfully\n");
                continue;
            }
            else if(strcmp(command[0],"clear")==0)
            {
                system("cls");
                continue;
            }
            else if(strcmp(command[0],"help")==0)
            {
                DisplayHelp();
                continue;
            }
            else if(strcmp(command[0],"exit")==0)
            {
                printf("Terminating the Marvellous Virtual File Sytem\n");
                break;
            }
            else
            {
                printf("ERROR:Command not found\n");
                continue;
            }
        }
        else if(count==2)
        {
            if(strcmp(command[0],"stat")==0)   //statictics give file name as parameter
            {
                ret=stat_file(command[1]);
                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: There is no such file\n");
                }
                continue;
            }
            else if(strcmp(command[0],"fstat")==0)   //give fd as parameter  it is same as stat
            {
                ret=fstat_file(atoi(command[1]));
                if(ret == -1)
                {
                    printf("ERROR: Incorrect parameters\n");
                }
                if(ret == -2)
                {
                    printf("ERROR: There is no such file\n");
                }
                continue;
            }
            else if(strcmp(command[0],"close")==0)
            {
                ret=CloseFileByName(command[1]);
                if(ret == -1)
                {
                    printf("ERROE:No such File\n");
                }
                continue;
            }
            else if(strcmp(command[0],"rm")==0)   //remove to delete a file
            {
                ret=rm_File(command[1]);
                if(ret==-1)
                {
                    printf("ERROR:There is no such file\n");
                }
                continue;
            }
            else if(strcmp(command[0],"man")==0)   //manual of functions
            {
                man(command[1]);
            }
            else if(strcmp(command[0],"write")==0)   //
            {
                fd=GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR: Incorrect parameter\n");
                    continue;
                }
                printf("Enter the data\n");
                scanf("%[^\n]",arr);

                ret=strlen(arr);
                if(ret==0)
                {
                    printf("ERROR: Incorrect parameter\n");
                    continue;
                }
                ret=WriteFile(fd,arr,ret);
                if(ret==-1)
                {
                    printf("ERROR: Permission denied\n");
                }
                if(ret==-2)
                {
                    printf("ERROR:There is no sufficient memory to write\n");
                }
                if(ret==-3)
                {
                    printf("ERROR:It is not a regular file\n");
                }
            }
            else if(strcmp(command[0],"truncate")==0)
            {
                ret=truncate_File(command[1]);
                if(ret==-1)
                {
                    printf("ERROR:Incorrect parameter\n");
                }
                else 
                {
                    printf("ERROR: Command not found\n");
                }
                continue;
            }
        }
        else if(count==3)
        {
            if(strcmp(command[0],"create")==0)
            {
                ret=CreateFile(command[1],atoi(command[2]));
                if(ret>0)
                {
                    printf("File is successfully created with file descriptor: %d\n",ret);
                }
                if(ret==-1)
                {
                    printf("ERROR:Incorrect parameters\n");
                }
                if(ret==-2)
                {
                    printf("ERROR:There are no inodes\n");
                }
                if(ret==-3)
                {
                    printf("ERROR:File alrady exists\n");
                }
                if(ret==-4)
                {
                    printf("ERROR: Memory allocation failure\n");
                }
                continue;
            }
            else if(strcmp(command[0],"open")==0)
            {
                ret=OpenFile(command[1],atoi(command[2]));
                if(ret>0)
                {
                    printf("File is successfully created with file descriptor: %d\n",ret);
                }
                if(ret==-1)
                {
                    printf("ERROR:Incorrect parameters\n");
                }
                if(ret==-2)
                {
                    printf("ERROR:File  not present\n");
                }
                if(ret == -3)
                {
                    printf("Permission denied\n");
                }
                continue;
            }
            else if(strcmp(command[0],"read")==0)
            {
                fd=GetFDFromName(command[1]);
                if(fd==-1)
                {
                    printf("ERROR:Incorrect parameter\n");
                    continue;
                }
                ptr=(char *)malloc(sizeof(atoi(command[2]))+1);
                if(ptr==NULL)
                {
                    printf("ERROR:Memory allocation failure\n");
                }
                ret=ReadFile(fd,ptr,atoi(command[2]));
                if(ret==-1)
                {
                    printf("File Not existing\n");
                }
                if(ret ==-2)
                {
                    printf("Permission denied\n");
                }
                if(ret==-3)
                {
                    printf("ERROR:Reached at the end of file\n");
                }
                if(ret == -4)
                {
                    printf("It is not a regular file\n");
                }
                if(ret==0)
                {
                    printf("File Empty\n");
                }
                if(ret>0)
                {
                    write(2,ptr,ret);
                }
                continue;
            }
            else
            {
                printf("\nERROR:Command not found\n");
                continue;
            }
        }
        else if(count==4)
        {
            if(strcmp(command[0],"lseek")==0)
            {
                fd=GetFDFromName(command[1]);
                if(fd==-1)
                {
                    printf("ERROR:Incorrect parameter\n");
                    continue;
                }
                ret=LseekFile(fd,atoi(command[2]),atoi(command[3]));
                if(ret==-1)
                {
                    printf("ERROR:Unable to perform lseek\n");
                }
            }
            else
            {
                printf("ERROr: Command not found\n");
                continue;
            }
        }
        else
        {
            printf("Command Not Found\n");
            continue;
        }
    }
    return 0;
}
