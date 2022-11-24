#!/bin/sh


gcc -static $1 -o $1.c
    qemu-system-x86_64 \
                -m 64M \
                -kernel ./bzImage \
                -initrd ./init.cpio.gz \
                -cpu kvm64,+smep,+smap \
                -append "console=ttyS0 oops=panic panic=1 kpti=1 nokaslr quiet" \
                -monitor /dev/null \
                -serial mon:stdio \
                -nographic -s


# if [ -z "$1" ]
# then
#     echo "No exploit supplied"
#     # No exploit executable supplied
#     qemu-system-x86_64 \
#                 -m 64M \
#                 -kernel ./bzImage \
#                 -initrd ./init.cpio.gz  \
#                 -cpu kvm64,+smep,+smap \
#                 -append "console=ttyS0 oops=panic panic=1 kpti=1 nokaslr quiet" \
#                 -monitor /dev/null \
#                 -serial mon:stdio \
#                 -nographic -s
                
# else
#     echo "Exploit supplied :)"
#     # Exploit executable supplied as first argument
#     gcc -static $1 -o $1.c
#     qemu-system-x86_64 \
#                 -m 64M \
#                 -kernel ./bzImage \
#                 -initrd ./initramfs.cpio.gz  \
#                 -cpu kvm64,+smep,+smap \
#                 -append "console=ttyS0 oops=panic panic=1 kpti=1 nokaslr quiet" \
#                 -monitor /dev/null \
#                 -drive file=$1.c,format=raw,index=0,media=disk \
#                 -serial mon:stdio \
#                 -nographic -s
# fi
