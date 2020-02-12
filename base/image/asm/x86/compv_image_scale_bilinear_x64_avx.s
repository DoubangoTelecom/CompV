;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>   ;
; File author: Mamadou DIOP (Doubango Telecom, France).                 ;
; License: GPLv3. For commercial license please contact us.             ;
; Source code: https://github.com/DoubangoTelecom/compv                 ;
; WebSite: http://compv.org                                             ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "compv_common_x86.s"
%if COMPV_YASM_ABI_IS_64BIT
%include "compv_image_scale_bilinear_macros.s"

COMPV_YASM_DEFAULT_REL

global sym(CompVImageScaleBilinear_Asm_X64_AVX2)

section .data
	extern sym(kShuffleEpi8_Deinterleave8uL2_32s)
	extern sym(kShuffleEpi8_Deinterleave16uL2_32s)

section .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; arg(0) -> const uint8_t* inPtr
; arg(1) -> compv_uscalar_t inStride,
; arg(2) -> COMPV_ALIGNED(SSE) uint8_t* outPtr
; arg(3) -> compv_uscalar_t outWidth
; arg(4) -> compv_uscalar_t outYStart
; arg(5) -> compv_uscalar_t outYEnd
; arg(6) -> COMPV_ALIGNED(SSE) compv_uscalar_t outStride
; arg(7) -> compv_uscalar_t sf_x
; arg(8) -> compv_uscalar_t sf_y
sym(CompVImageScaleBilinear_Asm_X64_AVX2)
	vzeroupper
	push rbp
	mov rbp, rsp
	COMPV_YASM_SHADOW_ARGS_TO_STACK 9
	COMPV_YASM_SAVE_YMM 15
	push rsi
	push rdi
	push rbx
	push r12
	push r13
	;; end prolog ;;

	; align stack and alloc memory
	COMPV_YASM_ALIGN_STACK 32, rax
	sub rsp, (14*32)
	%define memNeighb0              rsp + 0
	%define memNeighb1              memNeighb0 + 32
	%define memNeighb2              memNeighb1 + 32
	%define memNeighb3              memNeighb2 + 32
	%define vecy0                   memNeighb3 + 32
	%define vecy1                   vecy0 + 32
	%define vec0xff_epi32           vecy1 + 32
	%define vec0xff_epi16           vec0xff_epi32 + 32
	%define vecSfxTimes32           vec0xff_epi16 + 32
	%define vecSFX0                 vecSfxTimes32 + 32
	%define vecSFX1                 vecSFX0 + 32
	%define vecSFX2                 vecSFX1 + 32
	%define vecSFX3                 vecSFX2 + 32
	%define vecStride				vecSFX3 + 32

	%define vecNeighb0              ymm4
	%define vecNeighb1              ymm5
	%define vecNeighb2              ymm6
	%define vecNeighb3              ymm7
	%define vecX0                   ymm8
	%define vecX1                   ymm9
	%define vecX2                   ymm10
	%define vecX3                   ymm11
	%define vec4                    ymm12
	%define vec5                    ymm13
	%define vec6                    ymm14
	%define vec7                    ymm15
	%define vecDeinterleave8u       sym(kShuffleEpi8_Deinterleave8uL2_32s)
	%define vecDeinterleave16u		sym(kShuffleEpi8_Deinterleave16uL2_32s)

	%define arg_inPtr               arg(0)
	%define arg_inStride            arg(1)
	%define arg_outPtr              arg(2)
	%define arg_outWidth            arg(3)
	%define arg_outYStart           arg(4)
	%define arg_outYEnd             arg(5)
	%define arg_outStride           arg(6)
	%define arg_sf_x                arg(7)
	%define arg_sf_y                arg(8)

	%define outWidth                r8
	%define outYStart               r9
	%define outPtr                  r10
	%define outStride               r11
	%define sf_y                    r12
	%define inPtr                   r13

	mov outWidth, arg_outWidth
	mov outYStart, arg_outYStart
	mov outPtr, arg_outPtr
	mov outStride, arg_outStride
	mov sf_y, arg_sf_y
	mov inPtr, arg_inPtr

	; compute vecStride
	mov rax, arg_inStride
	vmovd xmm0, eax
	vpbroadcastd ymm0, xmm0
	vmovdqa [vecStride], ymm0

	; compute vec0xff_epi32 and vec0xff_epi16
	vpcmpeqw ymm2, ymm2
	vpsrld ymm1, ymm2, 24
	vpsrlw ymm2, ymm2, 8
	vmovdqa [vec0xff_epi32], ymm1
	vmovdqa [vec0xff_epi16], ymm2

	; compute vecSfxTimes32
	mov rax, arg_sf_x
	shl rax, 5
	vmovd xmm0, eax
	vpbroadcastd ymm0, xmm0
	vmovdqa [vecSfxTimes32], ymm0

	; compute vecSFX0, vecSFX1, vecSFX2 and vecSFX3
	mov rsi, arg_sf_x ; sf_x_
	xor rax, rax ; sf_x_ * 0
	lea rbx, [rsi * 2] ; sf_x_ * 2
	lea rcx, [rbx + rsi] ; sf_x_ * 3
	mov [vecSFX0 + 0], dword eax ; sf_x_ * 0
	mov [vecSFX0 + 4], dword esi ; sf_x_ * 1
	mov [vecSFX0 + 8], dword ebx ; sf_x_ * 2
	mov [vecSFX0 + 12], dword ecx ; sf_x_ * 3
	lea rax, [rcx + rsi] ; sf_x_ * 4
	lea rbx, [rax + rsi] ; sf_x_ * 5
	lea rcx, [rbx + rsi] ; sf_x_ * 6
	lea rdx, [rcx + rsi] ; sf_x_ * 7
	mov [vecSFX0 + 16], dword eax ; sf_x_ * 4
	mov [vecSFX0 + 20], dword ebx ; sf_x_ * 5
	mov [vecSFX0 + 24], dword ecx ; sf_x_ * 6
	mov [vecSFX0 + 28], dword edx ; sf_x_ * 7
	lea rdx, [rdx + rsi] ; sf_x_ * 8
	vmovdqa ymm0, [vecSFX0] ; ymm0 = vecSFX0
	vmovd xmm1, edx
	vpbroadcastd ymm1, xmm1 ; ymm1 = vecSfxTimes8
	%assign sfxIndex 1
	%rep 3
		vpaddd ymm0, ymm1 ; ymm0 = vecSFXn = (vecSFX0 + vecSfxTimes8)
		vmovdqa [vecSFX %+ sfxIndex], ymm0
		%assign sfxIndex sfxIndex+1
	%endrep	

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; do
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.DoWhile:
		mov rax, outYStart
		vmovd xmm0, eax
		vpbroadcastd ymm0, xmm0 ; ymm0 = vecYStart
		shr rax, 8 ; rax = (outYStart >> 8) = nearestY 
		imul rax, arg_inStride ; rax = (nearestY * inStride)
		lea rbx, [inPtr + rax] ; rbx = inPtr_
		vmovdqa ymm1, [vec0xff_epi32]
		vpand ymm0, ymm1 ; ymm0 = vecy0
		vpsubd ymm1, ymm0 ; ymm1 = vecy1
		vpackssdw ymm0, ymm0
		vpackssdw ymm1, ymm1
		vmovdqa [vecy0], ymm0
		vmovdqa [vecy1], ymm1
		vmovdqa vecX0, [vecSFX0]
		vmovdqa vecX1, [vecSFX1]
		vmovdqa vecX2, [vecSFX2]
		vmovdqa vecX3, [vecSFX3]

		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		; for (i = 0; i < outWidth; i += 32)
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		xor rsi, rsi ; rsi = i = 0x0
		.LoopWidth:
			vpcmpeqb vec7, vec7, vec7 ; condition mask
			vmovdqa vec6, vec7
			vmovdqa vecNeighb2, vec7
			vmovdqa vecNeighb3, vec7
			vpsrld ymm0, vecX0, 8
			vpsrld ymm1, vecX1, 8
			vpgatherdd  vecNeighb0, dword ptr [rbx+ymm0], vec7
			vpgatherdd  vec4, dword ptr [rbx+ymm1], vec6
			vpsrld ymm2, vecX2, 8
			vpsrld ymm3, vecX3, 8
			vpgatherdd  vecNeighb1, dword ptr [rbx+ymm2], vecNeighb2
			vpgatherdd  vec5, dword ptr [rbx+ymm3], vecNeighb3

			vpaddd ymm0, ymm0, [vecStride]
			vpaddd ymm1, ymm1, [vecStride]
			vpcmpeqb vec6, vec6, vec6 ; condition mask
			vmovdqa vec7, vec6
			vpaddd ymm2, ymm2, [vecStride]
			vpgatherdd  vecNeighb2, dword ptr [rbx+ymm0], vec6
			vpgatherdd  vec6, dword ptr [rbx+ymm1], vec7
			vpaddd ymm3, ymm3, [vecStride]
			vpcmpeqb ymm0, ymm0, ymm0
			vpgatherdd  vecNeighb3, dword ptr [rbx+ymm2], ymm0
			vpcmpeqb ymm1, ymm1, ymm1
			vpgatherdd  vec7, dword ptr [rbx+ymm3], ymm1
			vpshufb vecNeighb0, vecNeighb0, [vecDeinterleave16u]
			vpshufb vec4, vec4, [vecDeinterleave16u]
			vpunpcklqdq vecNeighb0, vecNeighb0, vec4
			vpermq vecNeighb0, vecNeighb0, 0xD8
			vpshufb vecNeighb1, vecNeighb1, [vecDeinterleave16u]
			vpshufb vec5, vec5, [vecDeinterleave16u]
			vpunpcklqdq vecNeighb1, vecNeighb1, vec5
			vpermq vecNeighb1, vecNeighb1, 0xD8
			vpshufb vecNeighb2, vecNeighb2, [vecDeinterleave16u]
			vpshufb vec6, vec6, [vecDeinterleave16u]
			vpunpcklqdq vecNeighb2, vecNeighb2, vec6
			vpermq vecNeighb2, vecNeighb2, 0xD8
			vpshufb vecNeighb3, vecNeighb3, [vecDeinterleave16u]
			vpshufb vec7, vec7, [vecDeinterleave16u]
			vpunpcklqdq vecNeighb3, vecNeighb3, vec7			
			vpermq vecNeighb3, vecNeighb3, 0xD8

			;;; Deinterleave neighbs ;;;
			vpshufb vecNeighb0, vecNeighb0, [vecDeinterleave8u] ; 0,0,0,0,1,1,1,1
			vpshufb vecNeighb1, vecNeighb1, [vecDeinterleave8u] ; 0,0,0,0,1,1,1,1
			vpshufb vecNeighb2, vecNeighb2, [vecDeinterleave8u] ; 2,2,2,2,3,3,3,3
			vpshufb vecNeighb3, vecNeighb3, [vecDeinterleave8u] ; 2,2,2,2,3,3,3,3			
			vpunpckhqdq ymm0, vecNeighb0, vecNeighb1          ; 1,1,1,1,1,1
			vpunpckhqdq ymm2, vecNeighb2, vecNeighb3          ; 3,3,3,3,3,3
			vpunpcklqdq vecNeighb0, vecNeighb0, vecNeighb1    ; 0,0,0,0,0,0
			vpunpcklqdq vecNeighb2, vecNeighb2, vecNeighb3    ; 2,2,2,2,2,2
			vmovdqa vecNeighb1, ymm0
			vmovdqa vecNeighb3, ymm2

			; starting here ymm2 = vecZero
			%define vecZero ymm2
			vpxor vecZero, vecZero
			
			vpand ymm0, vecX0, [vec0xff_epi32]
			vpand ymm3, vecX1, [vec0xff_epi32]
			vpunpcklbw vec4, vecNeighb0, vecZero
			vpackusdw ymm0, ymm0, ymm3
			vpunpcklbw ymm3, vecNeighb1, vecZero
			vpermq ymm0, ymm0, 0xD8 ; ymm0 = vec0
			vpandn ymm1, ymm0, [vec0xff_epi16] ; ymm1 = vec1			
			vpmullw ymm3, ymm3, ymm0
			vpmullw vec4, vec4, ymm1
			vpaddd vecX0, vecX0, [vecSfxTimes32]
			vpunpcklbw vec5, vecNeighb2, vecZero
			vpaddusw vec4, vec4, ymm3
			vpunpcklbw ymm3, vecNeighb3, vecZero
			vpmullw vec5, vec5, ymm1
			vpmullw ymm3, ymm3, ymm0
			vpand ymm0, vecX2, [vec0xff_epi32]
			vpaddd vecX1, vecX1, [vecSfxTimes32]
			vpaddusw vec5, vec5, ymm3
			vpand ymm3, vecX3, [vec0xff_epi32]
			vpackusdw ymm0, ymm0, ymm3
			vpunpckhbw vec6, vecNeighb0, vecZero
			vpermq ymm0, ymm0, 0xD8 ; ymm0 = vec0
			vpunpckhbw ymm3, vecNeighb1, vecZero
			vpandn ymm1, ymm0, [vec0xff_epi16] ; ymm1 = vec1			
			vpmullw ymm3, ymm3, ymm0
			vpmullw vec6, vec6, ymm1
			vpaddd vecX2, vecX2, [vecSfxTimes32]
			vpunpckhbw vec7, vecNeighb2, vecZero
			vpmullw vec7, vec7, ymm1
			vpaddusw vec6, vec6, ymm3
			vpunpckhbw ymm3, vecNeighb3, vecZero
			vpmullw ymm3, ymm3, ymm0
			vpaddd vecX3, vecX3, [vecSfxTimes32]
			vpaddusw vec7, vec7, ymm3

			; after this line ymm2 is no longer equal to zero
			%undef vecZero

			; Let''s say:
			;		A = ((neighb0 * x1) + (neighb1 * x0))
			;		B = ((neighb2 * x1) + (neighb3 * x0))
			; Then:
			;		A = vec4, vec6
			;		B = vec5, vec7
			;
			; We cannot use pmaddwd to compute C and D because it operates on epi16 while A and B contain epu16 values

			; compute C = (y1 * A) >> 16
			vpmulhuw ymm0, vec4, [vecy1]
			vpmulhuw ymm1, vec6, [vecy1]

			; compute D = (y0 * B) >> 16
			vpmulhuw ymm2, vec5, [vecy0]
			vpmulhuw ymm3, vec7, [vecy0]

			; Compute R = (C + D)
			vpaddusw ymm0, ymm0, ymm2
			vpaddusw ymm1, ymm1, ymm3

			add rsi, 32

			; Store the result
			vpackuswb ymm0, ymm0, ymm1
			cmp rsi, outWidth
			vpermq ymm0, ymm0, 0xD8
			vmovdqa [outPtr + rsi - 32], ymm0
			
			;;
			jl .LoopWidth
			; end-of-LoopWidth

		;;
		add outYStart, sf_y
		cmp outYStart, arg_outYEnd
		lea outPtr, [outPtr + outStride]
		jl .DoWhile
		; end-of-DoWhile

	%undef memNeighb0            
	%undef memNeighb1              
	%undef memNeighb2              
	%undef memNeighb3              
	%undef vec4
	%undef vec5
	%undef vec6
	%undef vec7
	%undef vecy0
	%undef vecy1
	%undef vec0xff_epi32
	%undef vec0xff_epi16
	%undef vecSfxTimes32
	%undef vecSFX0
	%undef vecSFX1
	%undef vecSFX2
	%undef vecSFX3
	%undef vecStride

	%undef vecNeighb0
	%undef vecNeighb1
	%undef vecNeighb2
	%undef vecNeighb3
	%undef vecX0
	%undef vecX1
	%undef vecX2
	%undef vecX3
	%undef vec4                    
	%undef vec5                    
	%undef vec6                    
	%undef vec7                    
	%undef vecDeinterleave8u

	%undef arg_inPtr
	%undef arg_inStride
	%undef arg_outPtr
	%undef arg_outWidth
	%undef arg_outYStart
	%undef arg_outYEnd
	%undef arg_outStride
	%undef arg_sf_x
	%undef arg_sf_y

	%undef outWidth
	%undef outYStart
	%undef outPtr
	%undef outStride
	%undef sf_y
	%undef inPtr

	; free memory and unalign stack
	add rsp, (14*32)
	COMPV_YASM_UNALIGN_STACK

	;; begin epilog ;;
	pop r13
	pop r12
	pop rbx
	pop rdi
	pop rsi
	COMPV_YASM_RESTORE_YMM
	COMPV_YASM_UNSHADOW_ARGS
	mov rsp, rbp
	pop rbp
	vzeroupper
	ret

%endif ; COMPV_YASM_ABI_IS_64BIT