
#include <elf.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

extern int startup(int argc, char **argv, void (*start)());
// task1a
int foreach_phdr(void *mapping_S, void (*func)(Elf32_Phdr *, int), int arg)
{
    Elf32_Ehdr *header = (Elf32_Ehdr *)mapping_S;
    Elf32_Phdr *program_header = (Elf32_Phdr *)((char *)mapping_S + (header->e_phoff));
    int num_of_headers = header->e_phnum;
    for (int i = 1; i <= num_of_headers; i++)
    {
        func(program_header, arg);
        program_header = (Elf32_Phdr *)((header->e_phentsize) + (char *)program_header);
    }
    return 0;
}
void print_mapping_flags(Elf32_Phdr *phdr)
{
    int prot_flags = 0;
    int map_flags = MAP_FIXED;
    if (phdr->p_flags & PF_R)
        prot_flags |= PROT_READ;
    if (phdr->p_flags & PF_W)
        prot_flags |= PROT_WRITE;
    if (phdr->p_flags & PF_X)
        prot_flags |= PROT_EXEC;
    if (phdr->p_flags & PF_R)
        map_flags |= MAP_PRIVATE;
    if (phdr->p_flags & PF_W)
        map_flags |= MAP_PRIVATE;
    if (phdr->p_flags & PF_X)
        map_flags |= MAP_PRIVATE;

    printf("port flags: %d\n", prot_flags);
    printf("map flags: %d\n",map_flags);
}
void print_phdr_info(Elf32_Phdr *phdr, int dummy)
{
    char flg[3];
    if (phdr->p_flags & PF_X)
        flg[2] = 'E';
    else
        flg[2] = ' ';
    if (phdr->p_flags & PF_W)
        flg[1] = 'W';
    else
        flg[1] = ' ';
    if (phdr->p_flags & PF_R)
        flg[0] = 'R';
    else
        flg[0] = ' ';
    const char *seg_types[] = {
        "NULL", "LOAD", "DYNAMIC", "INTERP", "NOTE", "SHLIB", "PHDR"};
    fprintf(stdout, "%s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %s 0x%x\n",
            seg_types[phdr->p_type], phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, flg, phdr->p_align);
    print_mapping_flags(phdr);
    //read: 1 
    //write: 2
    //execute: 4

    //private: 2
    //fixed 16
}

// task2b
void load_phdr(Elf32_Phdr *phdr, int fd)
{
    if (phdr->p_type != PT_LOAD)
    {
        fprintf(stderr, "can not load this program headers due to their type!\n");
        exit(1);
    }
    //--------------------------------------
    int APPROPRIATE_PERMISSION_FLAGS = 0;
    if (phdr->p_flags & PF_X)
        APPROPRIATE_PERMISSION_FLAGS |= PROT_EXEC;
    if (phdr->p_flags & PF_R)
        APPROPRIATE_PERMISSION_FLAGS |= PROT_READ;
    if (phdr->p_flags & PF_W)
        APPROPRIATE_PERMISSION_FLAGS |= PROT_WRITE;
    void *addr = (void *)(uintptr_t)(phdr->p_vaddr & 0xfffff000);
    off_t offset = phdr->p_offset & 0xfffff000;
    size_t padding = phdr->p_vaddr & 0xfff;
    void *mapped_address = mmap(addr, phdr->p_memsz + padding, APPROPRIATE_PERMISSION_FLAGS, MAP_PRIVATE | MAP_FIXED, fd, offset);
    if (mapped_address == MAP_FAILED)
    {
        fprintf(stderr, "Error while mapping the program header into memory!\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No File Name Provided In Arguments\n");
        return 1;
    }
    int fd = open(argv[1], O_RDONLY); // argv[1]=file name - file directory
    if (fd == -1)
    {
        fprintf(stdout, "Error while opening file\n");
        exit(EXIT_FAILURE);
    }
    struct stat _size;
    if (fstat(fd, &_size) == -1)
    {
        perror("Error while getting the size of the file!");
        close(fd);
        exit(EXIT_FAILURE);
    }
    void *map_start = mmap(NULL, _size.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED)
    {
        munmap(map_start, _size.st_size);
        fprintf(stdout, "Error while mapping the file <%s> into memory", argv[1]);
        close(fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Type Offset   VirtAddr   PhysAddr   FileSiz  MemSiz Flg Align\n");
    foreach_phdr(map_start, print_phdr_info, fd);
    foreach_phdr(map_start, load_phdr, fd);
    startup(argc - 1, argv + 1, (void *)((Elf32_Ehdr *)map_start)->e_entry);
}