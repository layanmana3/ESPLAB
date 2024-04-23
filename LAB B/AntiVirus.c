#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

typedef struct link
{
    struct link *nextVirus;
    virus *vir;
} link;
typedef struct menuOP
{
    struct fun_desc *options;
    link *virus_list;
    char *file;
} menuOP;

struct fun_desc
{
    char *name;
    char (*fun)(menuOP *);
} fun_desc;

void PrintHex(unsigned char *buffer, size_t length, FILE *output)
{
    for (size_t i = 0; i < length; i++)
    {
        fprintf(output, "%02X ", buffer[i]);
    }
}

virus *readVirus(FILE *file, int bigEnd)
{
    virus *tempV = (virus *)malloc(sizeof(struct virus));
    if (tempV == NULL)
    {
        perror("error while malloc-ing memory");
        free(tempV);
        return NULL;
    }
    if (fread(&tempV->SigSize, sizeof(unsigned short), 1, file) != 1)
    {
        free(tempV);
        return NULL;
    }
    if (fread(tempV->virusName, sizeof(char), 16, file) != 16)
    {
        free(tempV);
        return NULL;
    }
    if (bigEnd)
    {
        tempV->SigSize = ((unsigned char *)tempV)[0] * 16 * 16 + ((unsigned char *)tempV)[1];
    }
    tempV->sig = (unsigned char *)malloc(tempV->SigSize);
    if (fread(tempV->sig, sizeof(char), tempV->SigSize, file) != tempV->SigSize)
    {
        free(tempV->sig);
        free(tempV);
        return NULL;
    }
    return tempV;
}

void printVirus(virus *virus, FILE *output)
{
    fprintf(output, "Virus name: %s", virus->virusName);
    fprintf(output, "\n");
    fprintf(output, "Virus size: %d", virus->SigSize);
    fprintf(output, "\n");
    fprintf(output, "signature: \n");
    PrintHex(virus->sig, virus->SigSize, output);
    fprintf(output, "\n");
}
//-------------1b----------------------
void list_print(link *virus_list, FILE *output)
{
    printf("in list print");
    link *curr = virus_list;
    while (curr != NULL)
    {
        printf("printing...");
        printVirus(curr->vir, output);
        curr = curr->nextVirus;
    }
}

link *list_append(link *virus_list, virus *data)
{
    //printf("in list append");
    link *newlink = (struct link *)malloc(sizeof(struct link));
    newlink->vir = data;
    newlink->nextVirus = NULL;
    if (virus_list == NULL)
    {
        return newlink;
    }
    link *curr = virus_list;
    while (curr->nextVirus != NULL)
    {
        curr = curr->nextVirus;
    }
    curr->nextVirus = newlink;
    return virus_list;
}

void list_free(link *virus_list)
{
    if (virus_list != NULL)
    {
        free(virus_list->vir->sig);
        free(virus_list->vir);
        list_free(virus_list->nextVirus);
        free(virus_list);
    }
}
//------1b for the map:
link *loadSignatures(menuOP *menu)
{
    char input[100];
    printf("Please enter the file name: \n");
    if (fgets(input, sizeof(input), stdin) == NULL)
        return NULL;
    input[strcspn(input,"\n")]='\0';
    FILE *file = fopen(input, "rb");
    if (file == NULL)
    {
        perror("Error opening the file!");
        exit(EXIT_FAILURE);
    }
    char magic[5]; // 4 bytes for magic number + 1 byte for null terminator
    fread(magic, sizeof(char), 4, file);
    magic[4] = '\0'; // Null terminate the string
    int bigEnd = 0;
    if (strcmp(magic, "VIRB") == 0)
    {
        bigEnd = 1;
    }
    else if (strcmp(magic, "VIRL") == 0)
    {
        bigEnd = 0;
    }
    else
    {
        perror("Error: Invalid magic number\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    virus *next;
    while ((next = readVirus(file, bigEnd)) != NULL)
    {
        menu->virus_list = list_append(menu->virus_list, next);
    }
    fclose(file);
    //list_print(virList,stdout);
    return ;
}

link *quit(menuOP *menu)
{
    list_free(menu->virus_list);
    exit(1);
}
link *printSignatures(menuOP *menu)
{
    if (menu->virus_list == NULL){
        perror("error while printing null list!");
        return menu->virus_list;
    }
    printf("in print sig");
    list_print(menu->virus_list, stdout);
    return menu->virus_list;
}
//--------------------1C--------------------
void detect_virus(char *buffer, unsigned int size, link *virus_list,int fixing,char* fileName)
{
    link *current = virus_list;
    while (current != NULL)
    {
        for (int i = 0; i < size - (current->vir->SigSize); i++)
        {
            if(memcmp(buffer+i,current->vir->sig,current->vir->SigSize)==0){
                if(fixing){
                    neutralize_virus(fileName,i);
                }else
                {
                printf("starting byte location in the suspected file: %d\n",i);
                printf("virus name: %s\n",current->vir->virusName);
                printf("size of the virus signature: %d\n",current->vir->SigSize);
                }
            
            }
        }
        current=current->nextVirus;
    }
}
void *DetectViruses(menuOP *menu)
{
    FILE *suspected = fopen(menu->file, "rb"); //input is argv[1]
    if(suspected==NULL){
        perror("Error while opening suspected File!");
        exit(EXIT_FAILURE);
    }
    char buffer[10000];
    int size = fread(buffer, sizeof(unsigned char), 10000, suspected);
    if (size <= 0)
    {
        perror("Error while fread-detect vir");
        fclose(suspected);
        exit(EXIT_FAILURE);
    }
    detect_virus(buffer, size, menu->virus_list,0,menu->file);
    fclose(suspected);
    
}
//--------------------2-----------------------------
void *fix_file(menuOP *menu)
{
    char buffer[10000];
    FILE *infected = fopen(menu->file, "rb");
     if(infected==NULL){
        perror("Error while opening infected File!");
        exit(EXIT_FAILURE);
    }
    int size = fread(buffer, 1, 10000, infected);
     if (size <= 0)
    {
        perror("Error while fread-infected vir");
        fclose(infected);
        exit(EXIT_FAILURE);
    }
    fclose(infected);
    link *virus = menu->virus_list;
    while (virus != NULL)
    {
        unsigned int curr = 0;
        while (curr <= (size - (virus->vir->SigSize)))
        {
            if (memcmp(buffer + curr, virus->vir->sig, virus->vir->SigSize) == 0)
            {
                neutralize_virus(menu->file, curr);
            }
            curr++;
        }

        virus = virus->nextVirus;
    }
    

}
void neutralize_virus(char *fileName, int signatureOffset){
    FILE* infectedFile=fopen(fileName, "rb+");
    if(infectedFile == NULL){
        perror("error opening the infected file");
        exit(EXIT_FAILURE);
    }
    unsigned char RTE = 0xC3;
    fseek(infectedFile,signatureOffset,SEEK_SET);
    fwrite(&RTE,sizeof(unsigned char),1,infectedFile);
    fclose(infectedFile);
}

//////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    link *virList=NULL;
    char input[100];
    int option;
    struct fun_desc menu[] = {
        {"Load signatures", loadSignatures}, {"Print signatures", printSignatures}, {"Detect viruses", DetectViruses}, {"Fix file", fix_file}, {"Quit", quit}, {NULL, NULL}};

    int menuSize = sizeof(menu) / sizeof(struct fun_desc) - 1;
    struct menuOP menuOG = {menu, 0, NULL, NULL};
    while (1)
    {
        printf("Select operation from the following menu:\n");
        for (int i = 0; i < menuSize; i++)
            printf("%d) %s\n", i, menu[i].name);
        printf("Option : ");
        if (fgets(input, 100, stdin) == NULL)
            break;
        if (sscanf(input, "%d\n", &option) != 1)
            break;
        if ((option >= 0) & (option < menuSize))
        {
            printf("Within bounds\n");
            //virList = menu[option].fun(virList," "); //must do something for detect the " " it's argv[1]
            menu[option].fun(&menuOG);
            printf("DONE.\n\n");
        }
        else
        {
            printf("Not within bounds\n");
            break;
        }
    }
    if(virList!=NULL){
        list_free(virList);
    }
    return 1;
}