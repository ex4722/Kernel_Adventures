#!/bin/sh


gcc -static $1 -o $1.c -lpthread

# qemu-system-x86_64 \
#     -m 512M \
#     -kernel ./bzImage \
#     -initrd ./rootfs.cpio \
#     -append "root=/dev/ram rw console=ttyS0 oops=panic panic=1 kaslr quiet" \
#     -cpu kvm64,+smep \
#     -net user -net nic -device e1000 \
#     -drive file=$1.c,format=raw,index=0,media=disk \
#     -monitor /dev/null \
#     -nographic


# gcc -static $1 -o $1.c
#     qemu-system-x86_64 \
#                 -m 64M \
#                 -kernel ./bzImage \
#                 -initrd /tmp/object.cpio \
#                 -cpu kvm64,+smep,+smap \
#                 -append "console=ttyS0 oops=panic panic=1 kpti=1 nokaslr quiet" \
#                 -drive file=$1.c,format=raw \
#                 -monitor /dev/null \
#                 -serial mon:stdio \
#                 -nographic -s


qemu-system-x86_64 \
    -m 512M \
    -kernel ./bzImage \
    -initrd ./rootfs.cpio \
    -append "root=/dev/ram rw console=ttyS0 oops=panic panic=1 nokaslr quiet nopti" \
    -cpu kvm64,+smep \
    -drive file=$1.c,format=raw \
    -net user -net nic -device e1000 \
    -monitor /dev/null \
    -nographic 
