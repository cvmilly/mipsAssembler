	.text
	addu	$s0,$zero,$zero		# s0 = 0
	addu	$s1,$zero,$zero		# s1 = 0
	addiu	$v0,$zero,5		# read first value
	addu	$t0,$v0,$zero		# $t0 = A
	syscall
	addiu	$v0,$zero,5		# read second value
	addu	$t1,$v0,$zero		# $t1 = B 
	syscall
	addu	$s0,$t0,$t1		# s0 = A + B
	addu	$a0,$s0,$zero		# a0 = s0
	addiu	$v0,$zero,1		# print a0
	syscall
	addiu	$v0,$zero,10		# exit
	syscall
	.data
