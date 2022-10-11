SRC=data/crc-custom.c
ADF=data/riscv_custom_ops.adf
CC=../../../../openasip/src/bintools/Compiler/oacc-riscv
ELF=crc.elf
LOG=compileLog.txt

$CC -O0 -a $ADF -o $ELF $SRC &> $LOG

if [ -s $LOG ]; then
    echo "failure"
    exit 1;
elif ! [ -f $ELF ]; then
    echo "failure"
    exit 1;
elif ! [ -s $ELF ]; then
    echo "failure"
    exit 1;
fi

rm $LOG
rm $ELF

exit 0;
