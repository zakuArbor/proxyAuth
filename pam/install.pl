#!/usr/bin/perl

use strict;
use warnings;

sub check_dependency {
	my ($dep) = @_;
	my $cmd = "dpkg -s $dep 2>&1 1>/dev/null";
    `$cmd`;
    return not $?;
}

{
	my $install_manager_cmd = "sudo apt-get -y install";

	my @dependencies = ('libpam0g-dev', 'libglib2.0-dev', 'bluez', 'libbluetooth-dev');

	my @directories = ('/lib/security/', '/etc/proxy_auth/');

	#install missing dependencies
	for my $dep (@dependencies) {
		print "Checking if $dep is installed\n";
		if (check_dependency($dep) == 0) {
			print "$dep not installed\n";
			print "Installing $dep\n";
			print "$install_manager_cmd $dep\n";
			`$install_manager_cmd $dep`;
		}
	}
	
	#create missing directories
	for my $dir (@directories) {
		if (not -d $dir) {
			`sudo mkdir $dir`;
		}
	}
}