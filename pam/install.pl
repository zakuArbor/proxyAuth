#!/usr/bin/perl

use strict;
use warnings;

sub check_dependency {
	my ($dep) = @_;
	my $cmd = "ldconfig -p | grep $dep | wc -l";
    return `$cmd`;
}

{
	my $install_manager_cmd = "sudo apt-get install";

	my %dependencies = (
		'libpam' => 'libpam0g-dev',
		'libglib' => 'libglib2.0-dev',
		'libbluetooth' => 'bluez'
	);

	my @directories = ('/lib/security/', '/etc/proxy_auth/');

	#install missing dependencies
	for my $dep (keys %dependencies) {
		if (check_dependency($dep) == 0) {
			`$install_manager_cmd $dependencies{$dep}`
		}
	}

	#create missing directories
	for my $dir (@directories) {
		if (not -d $dir) {
			`sudo mkdir $dir` 
		}
	}
}