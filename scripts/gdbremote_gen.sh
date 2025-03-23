#!/bin/sh

SCRIPT=$1
shift
ELFS=$@ 

FILES=""
for elf in $ELFS; do
	FILES+="-ex 'load $elf' "
done

cat <<EOF > $SCRIPT
#!/bin/sh
cd \$(dirname \$0)
exec riscv64-unknown-elf-gdb -ex 'target extended-remote :3333' $FILES
EOF

chmod +x $SCRIPT
