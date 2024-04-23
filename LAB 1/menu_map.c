#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct fun_desc {
  char *name;
  char (*fun)(char);
};

char* map(char *array, int array_length, char (*f) (char));
char my_get(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);
char cprt(char c);

struct fun_desc menu[] ={{"Get String",my_get},
                         {"Print String",cprt},
                         {"Encrypt",encrypt},
                         {"Decrypt",decrypt},
                         {"Print Hex",xprt},
                         { NULL, NULL }};

const size_t MENU_SIZE = sizeof(menu)/sizeof(menu[0]);
const size_t ARRAY_SIZE = 5;

int main(int argc, char **argv){
  char* carray = malloc(ARRAY_SIZE); 
  char receive[1];
  int i, functionNum;

  while(1){
    puts("Please choose a function: (ctrl^D for exit): ");
    for(i = 0 ; i < MENU_SIZE - 1 ; i++){
      printf("%d) %s\n",i,menu[i].name);
    }
    printf("Option: ");
    char choice;
    int j=0;
      if(fgets(receive,100,stdin)==NULL){break;} 
    functionNum= atoi(receive);
    sscanf(receive , "%d\n" , &functionNum);
    if(functionNum >= 0 && functionNum <= MENU_SIZE -2)
      puts("Within bounds");
    else{
      puts("Not within bounds");
      break;
    }

    
    carray = map(carray,ARRAY_SIZE,menu[functionNum].fun);
    puts("DONE.\n");
  }

  return 0;
}


char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
   for(int i = 0; i < array_length; i++){
    mapped_array[i] = f(array[i]); //apply function f on each element in array
   }


  return mapped_array;
}
 
//Ignores c, reads and returns a character 
//from stdin using fgetc.(this comment from hw) 
char my_get(char c) {
    return fgetc(stdin);
}

//If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, 
//cprt returns the value of c unchanged. (this comment from hw) 
char cprt(char c) {
    if (c >= 0x20 && c <= 0x7E) {
        printf("%c\n", c);
    } else {
        printf(".\n");
    }
    return c;
}

//Gets a char c and returns its encrypted form by adding 1 to its value. 
//If c is not between 0x20 and 0x7E it is returned unchanged (this comment from hw) 
char encrypt(char c) {
    if (c >= 0x20 && c <= 0x7E) {
        return c + 1;
    } else {
        return c;
    }
}

// Gets a char c and returns its decrypted form by reducing 1 from its value. 
//If c is not between 0x20 and 0x7E it is returned unchanged (this comment from hw) 
char decrypt(char c) {
    if (c >= 0x20 && c <= 0x7E) {
        return c - 1;
    } else {
        return c;
    }
}

//xprt prints the value of c in a hexadecimal representation followed by a new line,
//and returns c unchanged. (this comment from hw) 
char xprt(char c) {

    if (c >= 0x20 && c <= 0x7E) {
         printf("%x\n", c);
    } else {
        printf(".\n");
    }
    return c;
}