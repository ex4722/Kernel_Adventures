set $base=0xffffffffc0000000
set $head=0xffffffffc00022c0

add-symbol-file kstack.ko (0xffffffffc0000000)

define boff
    x/10i $base-0x30 + $arg0

define uboff
    print $arg0-$base+0x30 
