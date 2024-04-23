#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct
{
    int debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int display_mode;
} state;

typedef struct fun_desc
{
    char *name;
    void (*fun)(state *s);
} fun_desc;

////////task0

void Toggle_Debug_Mode(state *s)
{
    if (s->debug_mode == 0)
    {
        s->debug_mode = 1;
        fprintf(stdout, "Debug flag now On\n");
    }
    else if (s->debug_mode == 1)
    {
        s->debug_mode = 0;
        fprintf(stdout, "Debug flag now off\n");
    }
}

void Set_File_Name(state *s)
{
    char filename[100];
    printf("enter file name:\n");
    if (fgets(filename, sizeof(filename), stdin) != NULL)
    {
        filename[strcspn(filename, "\n")] = '\0';
        strncpy(s->file_name, filename, sizeof(s->file_name));
        s->file_name[sizeof(s->file_name) - 1] = '\0';
        if (s->debug_mode == '1')
        {
            fprintf(stderr, "Debug: file name set to %s\n", filename);
        }
    }
}
void Set_Unit_Size(state *s)
{
    fprintf(stdout, "Enter File Size:\n");
    char input[100];
    int size;
    fgets(input, 100, stdin);
    sscanf(input, "%d", &size);
    if ((size == 1) || (size == 2) || (size == 4))
    {
        s->unit_size = size;
        if (s->debug_mode == 1)
            fprintf(stderr, "Debug:set size to %d\n", size);
    }
    else
    {
        printf("Invalid unit size");
    }
}

void Quit(state *s)
{
    if (s->debug_mode == 1)
        fprintf(stderr, "Quitting\n");
    exit(0);
}

//Task 1.a
void Load_Into_Memory(state *s)
{
    if (strcmp("", s->file_name) == 0)
    {
        printf("There's No File To Load Into Memory From");
        return;
    }
    FILE *file = fopen(s->file_name, "rb");
    if (file == NULL)
    {
        printf("Couldnt open file");
        return;
    }
    char input[256];
    int location;
    unsigned int length;
    printf("Please enter <location> <length>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %u", &location, &length);
    if (location < 0 || length <= 0)
    {
        printf("Error: Invalid input.\n");
        fclose(file);
        return;
    }
    if (s->debug_mode == '1')
    {
        fprintf(stderr, "file name: %s", s->file_name);
        fprintf(stderr, " location: %X", location);
        fprintf(stderr, " length: %d\n", length);
    }
    fseek(file, location, SEEK_SET);
    size_t units_read = fread(s->mem_buf, s->unit_size, length, file);
    //s->mem_count = units_read;
    fprintf(stderr, "Loaded %d units into memory\n", units_read);
    fclose(file);
}

//Task 1.b
void Toggle_Display_Mode(state *s)
{

    if (s->display_mode == 0)
    { //turn on the debug mode
        s->display_mode = 1;
        fprintf(stdout, "Display flag now on,hexadecimal representation\n");
    }
    else if (s->display_mode == 1)
    {
        s->display_mode = 0;
        fprintf(stdout, "Display flag now off,decimal representation\n");
    }
}

//Task 1.c
void Memory_Diplay(state *s)
{
    static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    static char *dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    char input[256];
    unsigned int u;
    unsigned int addr;
    printf("Enter address and length: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %u", &addr, &u);

    char *start_address = (char *)s->mem_buf + addr;
    char *end = start_address + s->unit_size * u;
    if (s->display_mode)
        printf("Hexadecimal\n===========\n");
    else
    {
        printf("Decimal\n=======\n");
    }

    while (start_address < end)
    {
        int var = *((int *)(start_address));
        if (s->display_mode)
            printf(hex_formats[s->unit_size - 1], var);
        else
            printf(dec_formats[s->unit_size - 1], var);
        start_address += s->unit_size;
    }
}

//Task 1.d
void Save_Into_File(state *s)
{
    //input
    char input[256];
    unsigned int source_address;
    unsigned int target_location;
    unsigned int length;
    printf("Please enter <source-address> <target-address> <length>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x %u", &source_address, &target_location, &length);
    void *offset;
    
    if (source_address == 0)
        offset = (s->mem_buf);
    else
        offset =(unsigned char*) (source_address);
    //end of input

    FILE *file = fopen(s->file_name, "r+b");
    if (file == NULL)
    {
        if (s->debug_mode == 1)
        {
            fprintf(stderr, "couldnt open file\n");
        }
        fprintf(stdout, "file name is %s", s->file_name);
        return;
    }

    //long size = ftell(file);
    // if (target_location < size)
    // {
    //     fprintf(stderr, "error, target lcoation is bigger than size of file\n");
    //     return;
    // }
    if(s->debug_mode){
        fprintf(stdout,"length = %d bytes from (virtual) memory, starting at address %x to the file %s ,starting from offset %p\n",length,source_address,s->file_name,offset);
    }
    //  end of check
    fseek(file, target_location, SEEK_SET);
    fwrite(offset,s->unit_size,length, file);
    //fwrite(offset, s->unit_size, length, file);
    fclose(file);
}

//Task 1.e
void Memory_Modify(state *s)
{
    char input[256];
    unsigned int loc;
    unsigned int val;
    printf("Please enter <location> <val>: ");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x", &loc, &val);
    //end of input
    if (s->debug_mode == 1)
    {
        fprintf(stderr, "location:%x value:%x\n", loc, val);
    }
    s->mem_buf[loc] = val;
    //~memmove(s->mem_buf + loc, &val, s->unit_size);
}

struct fun_desc fun_menu[] = {{"Toggle Debug Mode", Toggle_Debug_Mode},
                              {"Set File Name", Set_File_Name},
                              {"Set Unit Size", Set_Unit_Size},
                              {"Load Into Memory", Load_Into_Memory},
                              {"Toggle Display Mode", Toggle_Display_Mode},
                              {"Memory Display", Memory_Diplay},
                              {"Save Into File", Save_Into_File},
                              {"Memory Modify", Memory_Modify},
                              {"Quit", Quit},

                              {NULL, NULL}};

int main(int argc, char **argv)
{
    char user_input[100];
    int bound = (sizeof(fun_menu) / sizeof(struct fun_desc) - 1);
    state *s = malloc(sizeof(state));
    s->debug_mode = 0;
    s->display_mode = 0;
    while (1)

    {
        fprintf(stdout, "Please choose a function:\n");
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
        /*  if (func_num < 0 || func_num > bound - 1)
        {
            printf("Not within bounds\n");
            break;
        }*/
        fun_menu[func_num].fun(s);
    }
}