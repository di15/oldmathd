#!/usr/bin/perl -w

use warnings;
use strict;

print <<__EOF__;
/*
 * This file is part of PhysicsFS (http://icculus.org/physfs/)
 *
 * This data generated by physfs/extras/makecasefoldhashtable.pl ...
 * Do not manually edit this file!
 *
 * Please see the file LICENSE.txt in the source's root directory.
 */

#ifndef __PHYSICSFS_INTERNAL__
#error Do not include this header from your applications.
#endif

__EOF__


my @foldPairs;

for (my $i = 0; $i < 256; $i++) {
    $foldPairs[$i] = '';
}

open(FH,'<','casefolding.txt') or die("failed to open casefolding.txt: $!\n");
while (<FH>) {
    chomp;
    # strip comments from textfile...
    s/\#.*\Z//;

    # strip whitespace...
    s/\A\s+//;
    s/\s+\Z//;

    next if not /\A([a-fA-F0-9]+)\;\s*(.)\;\s*(.+)\;/;
    my ($code, $status, $mapping) = ($1, $2, $3);
    my $hexxed = hex($code);
    my $hashed = (($hexxed ^ ($hexxed >> 8)) & 0xFF);
    #print("// code '$code'   status '$status'   mapping '$mapping'\n");
    #print("// hexxed '$hexxed'  hashed '$hashed'\n");

    if (($status eq 'C') or ($status eq 'F')) {
        my ($map1, $map2, $map3) = ('0000', '0000', '0000');
        $map1 = $1 if $mapping =~ s/\A([a-fA-F0-9]+)(\s*|\Z)//;
        $map2 = $1 if $mapping =~ s/\A([a-fA-F0-9]+)(\s*|\Z)//;
        $map3 = $1 if $mapping =~ s/\A([a-fA-F0-9]+)(\s*|\Z)//;
        die("mapping space too small for '$code'\n") if ($mapping ne '');
        $foldPairs[$hashed] .= "    { 0x$code, 0x$map1, 0x$map2, 0x$map3 },\n";
    }
}
close(FH);

for (my $i = 0; $i < 256; $i++) {
    $foldPairs[$i] =~ s/,\n\Z//;
    my $str = $foldPairs[$i];
    next if $str eq '';
    my $num = '000' . $i;
    $num =~ s/\A.*?(\d\d\d)\Z/$1/;
    my $sym = "case_fold_${num}";
    print("static const CaseFoldMapping ${sym}[] = {\n$str\n};\n\n");
}

print("\nstatic const CaseFoldHashBucket case_fold_hash[256] = {\n");

for (my $i = 0; $i < 256; $i++) {
    my $str = $foldPairs[$i];
    if ($str eq '') {
        print("    { 0, NULL },\n");
    } else {
        my $num = '000' . $i;
        $num =~ s/\A.*?(\d\d\d)\Z/$1/;
        my $sym = "case_fold_${num}";
        print("    { __PHYSFS_ARRAYLEN($sym), $sym },\n");
    }
}
print("};\n\n");

exit 0;

# end of makecashfoldhashtable.pl ...

