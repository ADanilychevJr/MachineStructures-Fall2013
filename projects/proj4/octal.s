OCTAL:
lui $r2 0x00 #overwrite first
lui $r1 0x00 # give all 0s
lui $r0 0x00 # overwrite the registers first yo
lw $r0 0($r1) #load first thing in memory
ori $r3 $r3 0x7
and $r1 $r3 $r0
addi $r2 $r2 3
sllv $r3 $r3 $r2
and $r2 $r3 $r0
lui $r3 0x00
addi $r3 $r3 1
sllv $r2 $r2 $r3
or $r1 $r2 $r1
lui $r2 0x00
addi $r3 $r3 6
addi $r2 $r2 6
sllv $r3 $r3 $r2
and $r2 $r0 $r3
lui $r3 0x00
addi $r3 $r3 2
sllv $r2 $r2 $r3
or $r1 $r1 $r2
disp $r1 0 #display!