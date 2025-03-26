#!/bin/sh

SCRIPT=$1
shift
ELFS=$@ 

FILES=""
for elf in $ELFS; do
	FILES+="-ex 'load $elf' -ex 'add-symbol-file $elf' "
done

cat <<EOF > $SCRIPT
#!/bin/sh
cd \$(dirname \$0)
exec riscv64-unknown-elf-gdb -ex 'target extended-remote :3333' -ex 'set confirm off' \
	$FILES
EOF

chmod +x $SCRIPT
