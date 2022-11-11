set $base=0xffffffffc0000000
add-symbol-file chall.ko (0xffffffffc0000000)

define boff
    x/10i $base-0x60 + $arg0

define uboff
    print $arg0-$base+0x60 
