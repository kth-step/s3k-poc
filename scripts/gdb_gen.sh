#!/bin/sh

SCRIPT=$1
shift
ELFS=$@ 

SYMBOL_FILES=""
for elf in $ELFS; do
	SYMBOL_FILES+="-ex 'add-symbol-file $elf' "
done

cat <<EOF > $SCRIPT
#!/bin/sh
cd \$(dirname \$0)
exec riscv64-unknown-elf-gdb -ex 'target extended-remote :1234' $SYMBOL_FILES
EOF

chmod +x $SCRIPT
