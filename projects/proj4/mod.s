MOD:
   lui $r0 0x00 #zero out r0
   lui $r1 0x00 #zero out r1
   lui $r2 0x00 #zero out r2
   lui $r0 0xFF #put all 1s 
   ori $r0 $r0 0xFF 
   sw $r3 0($r0)
   lui $r3 0 
   lw $r0 0($r3)
   lw $r1 1($r3)
looper: 
   slt $r2 $r0 $r1
   bne $r2 $r3 done   #branch if r2 != r3
   sub $r0 $r0 $r1
   j looper  #keep looping because r2 = r3
done:
   sw $r0 2($r3)
   lui $r0 0xFF
   ori $r0 $r0 0xFF
   lw $r3 0($r0)
   jr $r3 #return!
   
	