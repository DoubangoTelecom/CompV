;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 2016-2019 Doubango Telecom <https://www.doubango.org>   ;
; File author: Mamadou DIOP (Doubango Telecom, France).                 ;
; License: GPLv3. For commercial license please contact us.             ;
; Source code: https://github.com/DoubangoTelecom/compv                 ;
; WebSite: http://compv.org                                             ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "compv_common_x86.s"

%if COMPV_YASM_ABI_IS_64BIT

COMPV_YASM_DEFAULT_REL

global sym(CompVMathDotDotSub_64f64f_Asm_X64_AVX)
global sym(CompVMathDotDotSub_64f64f_Asm_X64_FMA3_AVX)
global sym(CompVMathDotDot_64f64f_Asm_X64_AVX)
global sym(CompVMathDotDot_64f64f_Asm_X64_FMA3_AVX)

section .data

section .text

%define DOT_TYPE_DOT		0
%define DOT_TYPE_DOTSUB		1

%define DOT_FMA_OFF			0
%define DOT_FMA_ON			1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; arg(0) -> const compv_float64_t* ptrA
; arg(1) -> const compv_float64_t* ptrB
; arg(2) -> const compv_uscalar_t width
; arg(3) -> const compv_uscalar_t height
; arg(4) -> const compv_uscalar_t strideA
; arg(5) -> const compv_uscalar_t strideB
; arg(6) -> compv_float64_t* sum
; %1 -> DOT_TYPE
; %2 -> DOT_FMA
%macro CompVMathDot_64f64f_Macro_X64_AVX	2
	vzeroupper
	push rbp
	mov rbp, rsp
	COMPV_YASM_SHADOW_ARGS_TO_STACK 7
	COMPV_YASM_SAVE_YMM 7
	push rsi
	push rdi
	push rbx
	;; end prolog ;;
	
	%define ptrA		rax
	%define ptrB		rcx
	%define width		rdx
	%define height		rbx
	%define strideA		rsi
	%define strideB		rdi
	%define sum			r8

	%define width16		r9
	%define width2		r10
	%define i			r11

	mov ptrA, arg(0)
	mov ptrB, arg(1)
	mov width, arg(2)
	mov height, arg(3)
	mov strideA, arg(4)
	mov strideB, arg(5)
	mov sum, arg(6)

	mov width16, width
	mov width2, width
	and width16, -16
	and width2, -2

	shl strideA, COMPV_YASM_FLOAT64_SHIFT_BYTES
	shl strideB, COMPV_YASM_FLOAT64_SHIFT_BYTES

	vpxor ymm4, ymm4, ymm4
	%if %2 == DOT_FMA_ON
		vpxor ymm5, ymm5, ymm5
		vpxor ymm6, ymm6, ymm6
		vpxor ymm7, ymm7, ymm7
	%endif

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; for (compv_uscalar_t j = 0; j < height; ++j)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.LoopHeight:
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		; for (i = 0; i < width16; i += 16)
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		xor i, i
		test width16, width16
		jz .EndOf_LoopWidth16
		.LoopWidth16:
			vmovupd ymm0, [ptrA + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
			vmovupd ymm1, [ptrA + ((i + 4) * COMPV_YASM_FLOAT64_SZ_BYTES)]
			vmovupd ymm2, [ptrA + ((i + 8) * COMPV_YASM_FLOAT64_SZ_BYTES)]
			vmovupd ymm3, [ptrA + ((i + 12) * COMPV_YASM_FLOAT64_SZ_BYTES)]
			%if %1 == DOT_TYPE_DOTSUB
				vsubpd ymm0, ymm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				vsubpd ymm1, ymm1, [ptrB + ((i + 4) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				vsubpd ymm2, ymm2, [ptrB + ((i + 8) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				vsubpd ymm3, ymm3, [ptrB + ((i + 12) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%if %2 == DOT_FMA_ON
					vfmadd231pd ymm4, ymm0, ymm0
					vfmadd231pd ymm5, ymm1, ymm1
					vfmadd231pd ymm6, ymm2, ymm2
					vfmadd231pd ymm7, ymm3, ymm3
				%else
					vmulpd ymm0, ymm0, ymm0
					vmulpd ymm1, ymm1, ymm1
					vmulpd ymm2, ymm2, ymm2
					vmulpd ymm3, ymm3, ymm3
				%endif
			%else
				%if %2 == DOT_FMA_ON
					vfmadd231pd ymm4, ymm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
					vfmadd231pd ymm5, ymm1, [ptrB + ((i + 4) * COMPV_YASM_FLOAT64_SZ_BYTES)]
					vfmadd231pd ymm6, ymm2, [ptrB + ((i + 8) * COMPV_YASM_FLOAT64_SZ_BYTES)]
					vfmadd231pd ymm7, ymm3, [ptrB + ((i + 12) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%else
					vmulpd ymm0, ymm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
					vmulpd ymm1, ymm1, [ptrB + ((i + 4) * COMPV_YASM_FLOAT64_SZ_BYTES)]
					vmulpd ymm2, ymm2, [ptrB + ((i + 8) * COMPV_YASM_FLOAT64_SZ_BYTES)]
					vmulpd ymm3, ymm3, [ptrB + ((i + 12) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%endif
			%endif
			add i, 16
			cmp i, width16
			%if %2 == DOT_FMA_OFF
				vaddpd ymm0, ymm0, ymm1
				vaddpd ymm2, ymm2, ymm3
				vaddpd ymm0, ymm0, ymm2
				vaddpd ymm4, ymm4, ymm0
			%endif
			jl .LoopWidth16
		.EndOf_LoopWidth16:
		
		vmovapd xmm1, xmm4
		
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		; for (; i < width2; i += 2)
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		cmp i, width2
		jge .EndOf_LoopWidth2
		.LoopWidth2:
			vmovupd xmm0, [ptrA + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
			%if %1 == DOT_TYPE_DOTSUB
				vsubpd xmm0, xmm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%if %2 == DOT_FMA_ON
					vfmadd231pd xmm1, xmm0, xmm0
				%else
					vmulpd xmm0, xmm0, xmm0
				%endif
			%else
				%if %2 == DOT_FMA_ON
					vfmadd231pd xmm1, xmm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%else
					vmulpd xmm0, xmm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%endif
			%endif
			add i, 2
			cmp i, width2
			%if %2 == DOT_FMA_OFF
				vaddpd xmm1, xmm1, xmm0
			%endif
			jl .LoopWidth2
		.EndOf_LoopWidth2:

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		; for (; i < width; i += 1)
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		cmp i, width
		jge .EndOf_LoopWidth1
		.LoopWidth1:
			vmovsd xmm0, [ptrA + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
			%if %1 == DOT_TYPE_DOTSUB
				vsubsd xmm0, xmm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%if %2 == DOT_FMA_ON
					vfmadd231sd xmm1, xmm0, xmm0
				%else
					vmulsd xmm0, xmm0, xmm0
				%endif
			%else
				%if %2 == DOT_FMA_ON
					vfmadd231sd xmm1, xmm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%else
					vmulsd xmm0, xmm0, [ptrB + ((i + 0) * COMPV_YASM_FLOAT64_SZ_BYTES)]
				%endif
			%endif
			inc i
			cmp i, width
			%if %2 == DOT_FMA_OFF
				vaddsd xmm1, xmm1, xmm0
			%endif
			jl .LoopWidth1
		.EndOf_LoopWidth1:
		
		vinsertf128 ymm4, ymm4, xmm1, 0x0
		
		dec height
		lea ptrA, [ptrA + strideA]
		lea ptrB, [ptrB + strideB]
		jnz .LoopHeight
	.EndOf_LoopHeight:

	%if %2 == DOT_FMA_ON
		vaddpd ymm4, ymm4, ymm5
		vaddpd ymm6, ymm6, ymm7
		vaddpd ymm4, ymm4, ymm6
	%endif

	vextractf128 xmm0, ymm4, 0x1
	vaddpd xmm4, xmm4, xmm0	
	vshufpd xmm0, xmm4, xmm4, 0xff
	vaddsd xmm4, xmm4, xmm0
	vmovsd [sum], xmm4

	%undef ptrA
	%undef ptrB
	%undef width
	%undef height
	%undef strideA
	%undef strideB
	%undef sum
	%undef width16
	%undef width2
	%undef i

	;; begin epilog ;;
	pop rbx
	pop rdi
	pop rsi
	COMPV_YASM_RESTORE_YMM
	COMPV_YASM_UNSHADOW_ARGS
	mov rsp, rbp
	pop rbp
	vzeroupper
	ret
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sym(CompVMathDotDotSub_64f64f_Asm_X64_AVX):
	CompVMathDot_64f64f_Macro_X64_AVX DOT_TYPE_DOTSUB, DOT_FMA_OFF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sym(CompVMathDotDotSub_64f64f_Asm_X64_FMA3_AVX):
	CompVMathDot_64f64f_Macro_X64_AVX DOT_TYPE_DOTSUB, DOT_FMA_ON

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sym(CompVMathDotDot_64f64f_Asm_X64_AVX):
	CompVMathDot_64f64f_Macro_X64_AVX DOT_TYPE_DOT, DOT_FMA_OFF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sym(CompVMathDotDot_64f64f_Asm_X64_FMA3_AVX):
	CompVMathDot_64f64f_Macro_X64_AVX DOT_TYPE_DOT, DOT_FMA_ON

%endif ; COMPV_YASM_ABI_IS_64BIT
