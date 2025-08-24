#!/bin/env perl
sub asm {
    open PIPE, "|m68k-linux-gnu-as - -o /tmp/asm.o" or die $!;
    for my $line (@_) {
        print PIPE "$line;\n"
    }
#    print PIPE ".word 0044117;\n";
    close PIPE or die  $!;
    if( ! -e "/tmp/asm.o") {
        die "asm failed!";
    }
    system ("/usr/bin/m68k-linux-gnu-objcopy", "-O", "binary", "/tmp/asm.o",  "/tmp/asm2.o") == 0 or die $? ;
    unlink "/tmp/asm.o" or die  $!;
    open ASM, "</tmp/asm2.o" or die  $!;
    local $\;
    my $data = <ASM>;
    close ASM;
    unlink "/tmp/asm2.o" or die $!;
    return unpack "n*", $data;    
}
print "ASM={\n";
while (my $file = <../test/lua/*/*.lua>) {
    open FILE, $file or next;
    while (my $line = <FILE>) {
        if( $line =~ /ASM\((.+)\)/ ) {
            my $asm = $1;
            my @ret = asm($asm);
            my $oct = join ",", map { sprintf qq[OCT "%06o" ], $_ } @ret;
            print qq["$asm" = {$oct},\n];
        }
    }
    close FILE;
}
print "}\n"