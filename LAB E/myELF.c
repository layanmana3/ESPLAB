#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include "sys/stat.h"
#include <unistd.h> // for close
#include <string.h>
int debug_mode = 0;
typedef struct
{

    void *mapped_file1;
    void *mapped_file2;
    int fd1;
    int fd2;
    int size1;
    int size2;
    char file1_name[100];
    char file2_name[100];
    int counterFiles;
} ELFInfo;
ELFInfo *info;
typedef struct fun_desc
{
    char *name;
    void (*fun)();
} fun_desc;
//Task 0
void Toggle_Debug_Mode()
{
    if (debug_mode == 0)
    {
        debug_mode = 1;
        fprintf(stdout, "Debug flag now On\n");
    }
    else if (debug_mode == 1)
    {
        debug_mode = 0;
        fprintf(stdout, "Debug flag now off\n");
    }
}
void Examine_ELF_File()
{
    if (info->counterFiles == 2)
    {
        fprintf(stderr, "Two files are already opened!\n");
        return;
    }
    char currentFilename[100];
    printf("Enter file name:\n");
    fgets(currentFilename, 100, stdin);
    currentFilename[strlen(currentFilename) - 1] = '\0';
    int fd = open(currentFilename, O_RDONLY);
    if (fd == -1)
    {
        perror("Error while opening the file");
        exit(EXIT_FAILURE);
    }
    if (debug_mode)
    {
        fprintf(stdout, "Examine_ELF_File: opened the ELF successfully for the examination!\n");
    }
    //mapping the ELF into memory:1.gettint the file size 2.using mmap
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1)
    {
        perror("Error while getting the file size!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (debug_mode)
    {
        fprintf(stdout, "Examine_ELF_File: got the file size successfully\n");
    }
    void *file_data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED)
    {
        perror("Error mapping file into memory\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    //checking that the file is an ELF file -> by checking the first 4 bytes
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)file_data;
    if (elf_header->e_ident[EI_MAG0] != ELFMAG0 ||
        elf_header->e_ident[EI_MAG1] != ELFMAG1 ||
        elf_header->e_ident[EI_MAG2] != ELFMAG2 ||
        elf_header->e_ident[EI_MAG3] != ELFMAG3)
    {
        fprintf(stderr, "File is not an ELF file\n");
        munmap(file_data, file_stat.st_size);
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (info->counterFiles == 0)
    {
        strcpy(info->file1_name, currentFilename);
        info->fd1 = fd;
        info->mapped_file1 = file_data;
        info->size1 = file_stat.st_size;
    }
    else
    {
        strcpy(info->file2_name, currentFilename);
        info->fd2 = fd;
        info->mapped_file2 = file_data;
        info->size2 = file_stat.st_size;
    }
    info->counterFiles++;
    if (debug_mode)
    {
        fprintf(stdout, "Examine_ELF_File: an acual ELF file!\n");
    }
    //all the prints:
    printf("\n\nELF Header:\n\n");
    printf("Magic: %c%c%c\n", elf_header->e_ident[EI_MAG1],
           elf_header->e_ident[EI_MAG2],
           elf_header->e_ident[EI_MAG3]);
    printf("Data: ");
    switch (elf_header->e_ident[EI_DATA])
    {
    case ELFDATA2LSB:
        printf("2's complement, little endian\n");
        break;
    case ELFDATA2MSB:
        printf("2's complement, big endian\n");
        break;
    default:
        printf("Unknown data encoding scheme\n");
    }
    printf("Entry point address: 0x%x\n", elf_header->e_entry);
    printf("Section header table offset: %u\n", elf_header->e_shoff);
    printf("Number of section headers: %u\n", elf_header->e_shnum);
    printf("Size of section headers: %u (bytes)\n", elf_header->e_shentsize);
    printf("Program header table offset: %u (bytes into file)\n", elf_header->e_phoff);
    printf("Number of program headers: %u\n", elf_header->e_phnum);
    printf("Size of program headers: %u (bytes)\n", elf_header->e_phentsize);

    //must do unmap? maybe in quitting is enough?

    close(fd);
}
void Print_Section_Names()
{

    if (info->counterFiles == 0)
    { //no files have been loaded
        fprintf(stderr, "should examine the file first\n");
        exit(EXIT_FAILURE);
    }
    //fprintf(stdout, "index\tsection_name\t\tsection_address\t\tsection_offset\t\tsection_size\t\tsection_type\n");
    if (info->counterFiles > 0)
    { //one OR tow files have been loaded
        printf("File: %s\n", info->file1_name);
        Elf32_Ehdr *header1 = (Elf32_Ehdr *)info->mapped_file1;
        int NumberOfSections1 = header1->e_shnum;
        Elf32_Shdr *shdr = (Elf32_Shdr *)(info->mapped_file1 + header1->e_shoff);
        char *shstrtab = (char *)(info->mapped_file1 + shdr[header1->e_shstrndx].sh_offset);
        for (int i = 0; i < NumberOfSections1; i++)
        {
            fprintf(stdout, "[%d]%s\t%-20x\t%-20x\t%-20d\t%-20d\n", i, &shstrtab[shdr[i].sh_name], shdr[i].sh_addr, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_type);
        }
    }
    printf("\n");
    if (info->counterFiles == 2)
    {
        //fprintf(stdout, "index\tsection_name\t\tsection_address\t\tsection_offset\t\tsection_size\t\tsection_type\n");
        printf("File: %s\n", info->file2_name);
        Elf32_Ehdr *header2 = (Elf32_Ehdr *)info->mapped_file2;
        int NumberOfSections2 = header2->e_shnum;
        Elf32_Shdr *shdr2 = (Elf32_Shdr *)(info->mapped_file2 + header2->e_shoff);
        char *shstrtab2 = (char *)(info->mapped_file2 + shdr2[header2->e_shstrndx].sh_offset);
        for (int i = 0; i < NumberOfSections2; i++)
        {
            fprintf(stdout, "[%d]%s\t%-20x\t%-20x\t%-20d\t%-20d\n", i, &shstrtab2[shdr2[i].sh_name], shdr2[i].sh_addr, shdr2[i].sh_offset, shdr2[i].sh_size, shdr2[i].sh_type);

            //fprintf(stdout, "[%d]\t%s\t\t\t%x\t\t\t%x\t\t%d\t\t%d\n", i, &shstrtab2[shdr2[i].sh_name], shdr2[i].sh_addr, shdr2[i].sh_offset, shdr2[i].sh_size, shdr2[i].sh_type);
        } //%-20s\t%-20s\t%-20s\t%-20s\t%-20s\n
    }
}

void print_symbols_helper(char *file_name, void *mapped_file)
{
    printf("\nFile: %s\n", file_name);
    Elf32_Ehdr *header = (Elf32_Ehdr *)mapped_file;
    int NumberOfSections = header->e_shnum;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(mapped_file + header->e_shoff);
    char *shstrtab = (char *)(mapped_file + shdr[header->e_shstrndx].sh_offset);
    for (int i = 0; i < NumberOfSections; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM)
        {
            Elf32_Sym *symbols = (Elf32_Sym *)(mapped_file + shdr[i].sh_offset);
            char *symbols_names = (char *)(mapped_file + shdr[shdr[i].sh_link].sh_offset);
            for (int j = 0; j < shdr[i].sh_size / shdr[i].sh_entsize; j++)
            {
                if (symbols[j].st_shndx == SHN_ABS)
                {
                    printf("%d\t%x\t\tABS\t%s\t\t%s\n",
                           j, symbols[j].st_value, "", &symbols_names[symbols[j].st_name]);
                }
                else if (symbols[j].st_shndx == SHN_UNDEF)
                {
                    printf("%d\t%x\t\tUND\t%s\t\t%s\n",
                           j, symbols[j].st_value, "", &symbols_names[symbols[j].st_name]);
                }
                else
                {
                    printf("%d\t%x\t\t%d\t%s\t\t%s\n",
                           j, symbols[j].st_value, symbols[j].st_shndx,
                           &shstrtab[shdr[symbols[j].st_shndx].sh_name], &symbols_names[symbols[j].st_name]);
                }
            }
        }
    }
}

void Print_Symbols()
{
    if (info->counterFiles == 0)
    {
        fprintf(stdout, "Got no files!");
        exit(EXIT_FAILURE);
    }
    if (info->counterFiles == 1)
    {
        if (debug_mode)
        {
            fprintf(stdout, "Print symbols: about to print -> file1!\n");
        }
        print_symbols_helper(info->file1_name, info->mapped_file1);
        if (debug_mode)
        {
            fprintf(stdout, "Print symbols:printed !! -> file1!\n");
        }
    }
    if (info->counterFiles == 2)
    {
        if (debug_mode)
        {
            fprintf(stdout, "Print symbols: about to print ->file1 & file2!\n");
        }
        print_symbols_helper(info->file1_name, info->mapped_file1);
        print_symbols_helper(info->file2_name, info->mapped_file2);
        if (debug_mode)
        {
            fprintf(stdout, "Print symbols: about to print ->file1 & file2!\n");
        }
    }
}
//---------------------------------------------------------------------------------------

void Check_Files_for_Merge()
{
    if (info->counterFiles != 2)
    {
        fprintf(stderr, "You need 2 files for merging!\n");
        return;
    }
    //for the first file
    Elf32_Ehdr *header1 = (Elf32_Ehdr *)info->mapped_file1;                    //header table
    Elf32_Shdr *shdr1 = (Elf32_Shdr *)(info->mapped_file1 + header1->e_shoff); //sections headers
    int NumberOfSections1 = header1->e_shnum;
    char *symbols_names1;
    int size1TEMP;
    Elf32_Sym *symbols1 = NULL; //symbol table

    //for the second file
    Elf32_Ehdr *header2 = (Elf32_Ehdr *)info->mapped_file2;                    //header table
    Elf32_Shdr *shdr2 = (Elf32_Shdr *)(info->mapped_file2 + header2->e_shoff); //sections headers
    int NumberOfSections2 = header2->e_shnum;
    char *symbols_names2;
    int size2TEMP;
    Elf32_Sym *symbols2 = NULL; //symbol table
    //-----------------checking------------------------
    if (NumberOfSections1 == 0 || NumberOfSections2 == 0)
    {
        fprintf(stdout, "sections table not found!\n");
        return;
    }
    //counts the number of symbol tables and Checks if each file has exactly one symbol table
    int symtab_count1 = 0, symtab_count2 = 0;
    for (int i = 0; i < NumberOfSections1; i++)
    {
        if (shdr1[i].sh_type == SHT_SYMTAB)
        {
            symtab_count1++;
            symbols1 = (Elf32_Sym *)(info->mapped_file1 + shdr1[i].sh_offset);
            symbols_names1 = (char*) (info->mapped_file1 + shdr1[shdr1[i].sh_link].sh_offset);
            size1TEMP = shdr1[i].sh_size / sizeof(Elf32_Sym);
        }
    }
    //for the second file
    for (int i = 0; i < NumberOfSections2; i++)
    {
        if (shdr2[i].sh_type == SHT_SYMTAB)
        {
            symtab_count2++;
            symbols2 = (Elf32_Sym *)(info->mapped_file2 + shdr2[i].sh_offset);
            symbols_names2 = (char*) (info->mapped_file2 + shdr2[shdr2[i].sh_link].sh_offset);
            size2TEMP = shdr2[i].sh_size / sizeof(Elf32_Sym);
        }
    }

    if (symtab_count1 != 1 || symtab_count2 != 1)
    {
        fprintf(stdout, "feature not supported!\n");
        return;
    }
    if(debug_mode){
        fprintf(stdout,"done for the first part of checking\n");
    }
    //-----------now checks defined and undefined-------------------
    //----------------undefined:

    for(int i = 1; i < size1TEMP; i++){
        if(symbols1[i].st_shndx == SHN_UNDEF){       
            char* name1=&symbols_names1[symbols1[i].st_name];
             //CHECK IN SYMBOL2
            for (int j = 1; j < size2TEMP; j++) {        
                char* name2=&symbols_names2[symbols2[j].st_name];
                if (strcmp(name1, name2) == 0 && symbols2[j].st_shndx == SHN_UNDEF) {
                    fprintf(stdout,"Symbol undefined in both tables\n");
                    break;
                }
            }       
        }
    }
    if(debug_mode){
        fprintf(stdout,"done for undefined checking\n");
    }
    //----------------defined:
    for(int i = 1; i < size1TEMP; i++){
        if(symbols1[i].st_shndx != SHN_UNDEF){
            //CHECK IN SYMBOL2
            char* name1=&symbols_names1[symbols1[i].st_name];
            for (int j = 1; j < size2TEMP; j++) {        
                char* name2=&symbols_names2[symbols2[j].st_name];
                if (strcmp(name1, name2) == 0 && symbols2[j].st_shndx != SHN_UNDEF) {
                    fprintf(stdout,"Symbol multiply defined in both tables\n");
                    break;
                }
            }
        }
    }
    if(debug_mode){
        fprintf(stdout,"done for defined checking\n");
    }


}

void Merge_ELF_Files() {
    printf(stdout,"is a bonus assignment, not required for a full grade.!");
}

void Quit()
{
    if (info->fd1 != -1)
    {
        munmap(info->mapped_file1, info->size1);
        close(info->fd1);
        if (debug_mode == 1)
            fprintf(stdout, "Quit: umpaped - file1!\n");
    }
    if (info->fd2 != -1)
    {
        munmap(info->mapped_file2, info->size2);
        close(info->fd2);
        if (debug_mode == 1)
            fprintf(stdout, "Quit: umpaped-file2!\n");
    }
    if (debug_mode == 1)
        fprintf(stderr, "Quitting\n");
    //should unmap and close any mapped or open files
    exit(0);
}

struct fun_desc fun_menu[] = {{"Toggle Debug Mode", Toggle_Debug_Mode},
                              {"Examine ELF File", Examine_ELF_File},
                              {"Print Section Names", Print_Section_Names},
                              {"Print Symbols", Print_Symbols},
                              {"Check Files for Merge", Check_Files_for_Merge},
                              {"Merge ELF Files", Merge_ELF_Files},
                              {"Quit", Quit},

                              {NULL, NULL}};

int main(int argc, char **argv)
{

    char user_input[100];
    int bound = (sizeof(fun_menu) / sizeof(struct fun_desc) - 1);
    info = malloc(sizeof(ELFInfo));
    info->fd1 = -1;
    info->fd2 = -1;
    //info->debug_mode=0;
    info->counterFiles = 0;
    while (1)

    {

        fprintf(stdout, "\nPlease choose a function:\n");
        for (int i = 0; i < bound; i++)
        {
            printf("%d)%s\n", i, fun_menu[i].name);
        }
        printf("option: ");
        if (fgets(user_input, 100, stdin) == NULL)
            break;
        int func_num;
        sscanf(user_input, "%d\n", &func_num);
        func_num = func_num;
          if (func_num < 0 || func_num > bound - 1)
        {
            printf("Not within bounds\n");
            break;
        }
        fun_menu[func_num].fun();
    }
}