%define WRITE 4
%define STDOUT 1
%define OPEN 5
%define READ 3
%define EXIT 1

section .data
counter: dd 0
argc: dd 0
argv: dd 0
char: db 0
arg_len: dd 0
newline: db 10
infile: dd 0
outfile: dd 1


section .text
extern strlen
global main


main:              
    mov eax, dword[esp+4] ;bringing argc
    mov dword[argc],eax ;storing argc in pointer
    mov eax, dword[esp+8] ;bringing argv
    mov dword[argv],eax ;storing a pointer to argvs


print:
  while_loop:
; Load current argv[i] into ecx
  mov ecx, dword [argv]            ; ecx = char** argv
  mov edi, dword [arg_len]         ; edi = current index
  mov ecx, dword [ecx + edi * 4]   ; ecx = argv[i]
; check length
  push ecx ;to check length
  call strlen ;length is now in eax
  add esp,4 ;default pointer to stack
  mov dword[arg_len],eax
  ;print
  mov eax, WRITE            
  mov ebx, STDOUT
  mov edx, dword[arg_len]   ;edx = strlen(argv[i])
  int 0x80                 ;system call write
  ;print new line
  mov eax, WRITE
  mov ebx,STDOUT
  mov ecx,newline
  mov edx,1
  int 0x80
  ;get input and output file
  mov ecx, dword[argv]       ;ecx=char**argv
  mov edi, dword[counter]    ;index of the argument
  mov ecx, dword[ecx + edi * 4]  ;ecx= argv[i]
  cmp word[ecx], "-i"          ; comparing the first 2 bytes in the arg with -i
  je open_input
  mov ecx, dword[argv] 
  mov ecx, dword[ecx + edi * 4]
  cmp word[ecx], "-o"
  je open_output
continue:
  add dword[counter], 1     ;increment the counter by 1
  mov edi, dword[counter]   ;increment the index register
  cmp edi, dword[argc]      ;check if done
  je encode
  jmp while_loop

open_input:
mov eax, OPEN
mov ebx, ecx                ;ebx = argv[i]
add ebx, 2                  ;ebx = argv[i]+2 to ignore the "-i"
mov ecx, 0                  ;read only
int 0x80 
cmp eax, -1                 ;check if error exists
jle exit_fail
mov dword[infile],eax
jmp continue


open_output:
  mov eax, OPEN                    ; Prepare system call: sys_open
  mov ebx, ecx                    ; In check_argc, ecx had the argument, now ebx has it
  add ebx, 2                      ; ebx = argv[i] + 2 (ignore the -o)
  mov ecx, 101o                   ; Open for writing, create new file if it doesn't exist
  mov edx, 777o                   ; Create new file with read/write/execute permissions
  int 0x80                        ; Call system call
  cmp eax, -1                     ; Check for error
  jle exit                       ; Jump if equal to -1 (open failed)
  mov dword [outfile], eax        ; Save opened file in global variable
  jmp continue                   ; Continue the loop



encode:
;get a char from the user
mov eax, READ
mov ebx, dword[infile]
mov ecx, char
mov edx, 1
int 0x80
;check errors =-1 or EOF=0
cmp eax, 0
jle exit
;check range 
cmp byte[char], 'A'     ; check if char < 'A' 
jl print_char           ; jump to print it without encoding
cmp byte[char], 'z'     ; check if char > 'z'
jg print_char           ; jump to print it without encoding
add byte[char],1        ; encode by adding 1

print_char:
mov eax, WRITE
mov ebx, dword[outfile]
mov ecx, char
mov edx, 1
int 0x80
; loop
jmp encode 


error:
    mov eax, 0x4                    ; Prepare system call: sys_write
    mov ebx, 2                      ; stderr
    mov edx, 17                     ; Buffer length
    int 0x80                        ; Call system call
    jmp exit_fail                   ; Exit because of error

exit_fail:
    mov eax, 0x1                    ; sys_exit
    mov ebx, 1                      ; Exit code for failure
    int 0x80

exit:
mov eax, EXIT               ;  system exit
mov ebx, 0                  ; exit status 0 success
int 0x80