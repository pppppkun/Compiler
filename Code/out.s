.data
v2: .word 0
v1: .space 40
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
main:
  li $t2, 12
  la $t1, v1
  add $t0, $t1, $t2
  la $t3, v2
  sw $t0, 0($t3)
  li $t4, 5
  la $t5, v2
  sw $t4, 0($t5)
