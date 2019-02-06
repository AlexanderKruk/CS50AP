.LC0:
	.string	"%i\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp            ; организация стека
	.cfi_def_cfa_offset 16  ;
	.cfi_offset 6, -16      ;
	movq	%rsp, %rbp      ;
	.cfi_def_cfa_register 6 ;
	subq	$16, %rsp       ; выделяет память
	movl	$1, -4(%rbp)    ; поместь 1 в rbp
	jmp	.L2                 ; перемещаемся в L2
.L3:
	movl	-4(%rbp), %eax  ; поместить rbp в eax
	movl	%eax, %esi      ; поместить eax в esi
	movl	$.LC0, %edi     ; поместить в edi значени .LC0
	movl	$0, %eax        ; помещаем 0 в eax
	call	printf          ; вызываем printf выводим "%i\n"
	addl	$1, -4(%rbp)    ; прибавляем 1
.L2:
	cmpl	$50, -4(%rbp)   ; сравнение 50 и rbp
	jle	.L3                 ; если меньше или равно в L3
	leave                   ; копирует sp в bp
	.cfi_def_cfa 7, 8
	ret                     ; return
	.cfi_endproc