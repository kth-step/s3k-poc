#!/bin/sh

SCRIPT=$1
shift
ELFS=$@ 

LOADERS=""
for elf in $ELFS; do
	LOADERS+="-device loader,file='$(realpath $elf)' "
done

cat <<EOF > $SCRIPT
#!/bin/sh
qemu-system-riscv64 -M virt -smp 1 -m 128M -nographic -bios none $LOADERS
EOF

chmod +x $SCRIPT
