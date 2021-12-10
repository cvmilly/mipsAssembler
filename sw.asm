	.text
	sw      $v0,n($gp)
	addiu	$v0,$zero,10		# exit
	syscall
	.data
n:	.word	0
