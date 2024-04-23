#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
  FILE *infile = stdin;
  FILE *outfile = stdout;
  char *encodekey ;
  int encodemode = 0;
  int index = 0;
  int encodekeyIndex = 2;
  int length ;
  int debugmode = 0;
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "+D") == 0)
    {
      debugmode = 1;
    }
    if (strcmp(argv[i], "-D") == 0)
    {
      debugmode = 0;
    }
    if (debugmode)
    {
      fprintf(stderr, "%s\n", argv[i]);
    }

    if (argv[i][0] == '+' && argv[i][1] == 'e')
    {
      encodekey = argv[i];
      encodemode = 1;
      length = 0;
      while (encodekey[encodekeyIndex] != '\0')
      {
        length = length + 1;
        encodekeyIndex++;
      }
    }

    if (argv[i][0] == '-' && argv[i][1] == 'e')
    {
      encodekey = argv[i];
      encodemode = -1;
      length = 0;
      while (encodekey[encodekeyIndex] != '\0')
      {
        length = length + 1;
        encodekeyIndex++;
      }
    }

    if (argv[i][0] == '-' && argv[i][1] == 'i')
    {
      infile = fopen(argv[i] + 2, "r");
      if (infile == NULL)
      {
        fprintf(stderr, "Failed to open file %s\n" , argv[i]+2);
        return 1;
      }
    }


    if (argv[i][0] == '-' && argv[i][1] == 'o')
    {
     
      outfile = fopen(argv[i] + 2, "w");
      if (outfile == NULL)
      {
        fprintf(stderr, "Failed to open file %s\n" , argv[i]+2);
        return 1;
      }
    }
  }

  char ch;
  while ((ch = fgetc(infile)) != EOF)
  {
    char encodedc = ch;
    int digit = encodekey[index + 2] - '0';
    index = (index + 1) % length;
    if (('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
    {

      if (encodemode == 1)
      {
        encodedc = ch + digit;
        if (('0' <= ch && ch <= '9') && encodedc > '9')
          encodedc = '0' + (digit - ('9' - ch + 1));
        if ('a' <= ch && ch <= 'z' && encodedc > 'z')
          encodedc = 'a' + (digit - ('z' - ch + 1));
        if ('A' <= ch && ch <= 'Z' && encodedc > 'Z')
          encodedc = 'A' + (digit - ('Z' - ch + 1));
      }
      else if (encodemode == -1)
      {

        encodedc = ch - digit;
        if ('0' <= ch && ch <= '9' && encodedc < '0')
          encodedc = '9' - (digit - (ch + 1 - '0'));
        if ('a' <= ch && ch <= 'z' && encodedc < 'a')
          encodedc = 'z' - (digit - (ch + 1 - 'a'));
        if ('A' <= ch && ch <= 'Z' && encodedc < 'A')
          encodedc = 'Z' - (digit - (ch + 1 - 'A'));
      }
    }
    fputc(encodedc, outfile);
  }
  fclose(infile);
  fclose(outfile);

  return 0;
}
