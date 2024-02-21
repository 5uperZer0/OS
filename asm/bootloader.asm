[org 0x7c00]

kernel_offset equ 0x1000

_start:
	mov bp, 0x8000		; Setup stack and frame pointers
	mov sp, bp
	call load_kernel	; Load the kernel
	call switch			; Switch to protected mode
	jmp $

%include "./asm/disk_load.asm"
%include "./asm/gdt.asm"
%include "./asm/switch.asm"

[bits 16]
load_kernel:
	mov bx, kernel_offset 
	mov dh, 18			; Unless we increase the number of accessible sectors, we'll run out of memory for our source code! (1024 bits is not much) 
	call disk_load		; Load the disk so we can properly start the kernel

	; Put your code here to disable the blinking cursor
	; The blinking cursor can only be disabled in real mode using BIOS interrupt int 0x10
	mov ah, 0x01
	mov cx, 0x2607
	int 0x10
	
	ret

[bits 32]
pmode:
	call kernel_offset
	jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa
