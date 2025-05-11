	.text
	.attribute	4, 16
	.attribute	5, "rv32i2p0"
	.file	"test_input.cpp"
	.globl	_Z3divii                        # -- Begin function _Z3divii
	.p2align	2
	.type	_Z3divii,@function
_Z3divii:                               # @_Z3divii
	.cfi_startproc
# %bb.0:
	addi	sp, sp, -32
	.cfi_def_cfa_offset 32
	sw	ra, 28(sp)                      # 4-byte Folded Spill
	sw	s0, 24(sp)                      # 4-byte Folded Spill
	.cfi_offset ra, -4
	.cfi_offset s0, -8
	addi	s0, sp, 32
	.cfi_def_cfa s0, 0
	sw	a0, -16(s0)
	sw	a1, -20(s0)
	j	.LBB0_1
.LBB0_1:
	lw	a0, -16(s0)
	li	a1, 2
	bne	a0, a1, .LBB0_3
	j	.LBB0_2
.LBB0_2:
	lw	a0, -16(s0)
	sw	a0, -12(s0)
	j	.LBB0_4
.LBB0_3:
	lw	a0, -20(s0)
	sw	a0, -12(s0)
	j	.LBB0_4
.LBB0_4:
	lw	a0, -12(s0)
	lw	ra, 28(sp)                      # 4-byte Folded Reload
	lw	s0, 24(sp)                      # 4-byte Folded Reload
	addi	sp, sp, 32
	ret
.Lfunc_end0:
	.size	_Z3divii, .Lfunc_end0-_Z3divii
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	addi	sp, sp, -16
	.cfi_def_cfa_offset 16
	sw	ra, 12(sp)                      # 4-byte Folded Spill
	sw	s0, 8(sp)                       # 4-byte Folded Spill
	.cfi_offset ra, -4
	.cfi_offset s0, -8
	addi	s0, sp, 16
	.cfi_def_cfa s0, 0
	sw	zero, -12(s0)
	li	a0, 0
	lw	ra, 12(sp)                      # 4-byte Folded Reload
	lw	s0, 8(sp)                       # 4-byte Folded Reload
	addi	sp, sp, 16
	ret
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
