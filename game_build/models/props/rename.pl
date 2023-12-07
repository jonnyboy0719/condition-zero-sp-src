#
#

use File::Copy;

open(NAMEFILE, "names.txt");

while(<NAMEFILE>) {
	if(/^\s+/) {
		next;
	}
	chomp;
	($oldname,$newname) = split /,/;
	$oldname = $oldname . ".mdl";
	$newname = $newname . ".mdl";
	if( -f "../$oldname") {
		if( !-f "$newname") {
			print "Creating $newname from ..\\$oldname\n";
			copy ("../$oldname", $newname);	
		}
	} else {
		print "CANNOT FIND $oldname!\n";
	}
}

close NAMEFILE;

	