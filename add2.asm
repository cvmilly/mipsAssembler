	.text
	lw	$t0,n($gp)              # t0 = M[n]  which is 10
	lw	$t1,n($gp)              # t1 = M[m]  which is 100	
	addu	$s0,$t0,$t1		# s0 = 10 + 100 
	addu	$a0,$s0,$zero		# a0 = s0
	addiu	$v0,$zero,1		# print a0
	syscall
	addiu	$v0,$zero,10		# exit
	syscall
	.data
n:	.word  10
m:	.word  100
