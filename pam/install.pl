#!/usr/bin/perl

=head1 NAME

install.pl - Installs all the dependencies needed to compile the PAM module and 
             the deauthentication server
          
=head1 SYNOPSIS

Default Usage: 
rsh_hack.pl

Silent Usage: 
rsh_hack.pl -silent


Note:
Supports Ubuntu and Fedora distributions only.

=head1 DESCRIPTION
Installs the following packages:

=over

=item * pam development pkg

=item * glib2 development pkg

=item * bluez - the official Bluetooth communication lib

=item * bluez-libs-devel

=back

=head1 OPTIONS

-silent    suppresses output to STDOUT


=head1 AUTHORS

zakuarbor (Ju Hong Kim)

=cut
################################################################################

use strict;
use warnings;
use Getopt::Long;
use Data::Dumper;

my $silent = 0;
my $help = 0;

my $PROG;
($PROG = $0) =~ s/.*[\/\\]//g;

my $usage = 
"Usage: $PROG [-silent]

-silent    suppress all output to STDOUT unless there's an error
-help      display the usage instruction
";

my %packages = (
  'ubuntu' => {
    'deps'     => ['libpam0g-dev','libglib2.0-dev','bluez','libbluetooth-dev'],
    'checker'  => 'dpkg -s',
    'installer'=> 'apt-get install',
    'dirs'     => ['/lib/security/', '/etc/proxy_auth/'],
  },
  'fedora' => {
    'deps'     => [
                     'pam-devel', 'glib2-devel', 'bluez', 'bluez-libs-devel',
                     'libasan', 'libubsan'
                  ],
    'checker'  => 'rpm -q',
    'installer'=> 'dnf install',
    'dirs'     => ['/lib64/security/', '/etc/proxy_auth/']
  },
);

GetOptions ( 
  "silent"  =>  \$silent,
  "help"    =>  \$help
) or die("Error in command line arguments\n");

if ($help) {
   print $usage;
   exit 0;
}

################################################################################
=head1 SUBROUTINES

=head2 get_distro

Returns the OS distribution of the target machine (fedora or ubuntu).
If invalid OS, will return an empty string

=cut

################################################################################
sub get_distro {
	my $os="";
	if (-e '/etc/os-release') {
		$os=`grep -E "^ID=" /etc/os-release | grep ID | cut -d = -f 2`;
    chomp($os);
    $os =~ s/"//g;
	}
	$os=lc($os);
	if ($os =~ /fedora|ubuntu/) {
		return $os;
	}
	return "";
}

################################################################################
=head2 print_s

Print message if not silent

@param msg: the message to print
=cut
################################################################################
sub print_s {
  my $msg = shift;
  print "  " . $msg . "\n" if !$silent;
}

################################################################################
=head2 check_dependency

Checks if the given package exist on the system

@param dep: the name of the package

@param os: the os running on the system

@return: return non-zero if the package exists
=cut
################################################################################
sub check_dependency {
	my ($pkg, $os) = @_;
	my $cmd = $packages{$os}{'checker'} . " $pkg 2>&1 1>/dev/null";
  print_s("cmd: $cmd");
  `$cmd`;
  return not $?;
}

################################################################################
=head2 install_pkg
Install the package onto the machine

@param pkg: the name of the package
@param os: the os running on the system
=cut
################################################################################
sub install_pkg {
  my ($pkg, $os) =@_;
  print_s("$pkg not installed");
  print_s("Installing $pkg");
  my $cmd = "sudo " . $packages{$os}{'installer'} . " $pkg -y";
  if ($silent) {
    $cmd .= " > /dev/null";
  }
  print_s("$cmd");
  `$cmd`;
}

################################################################################
# MAIN 
################################################################################
my $os = get_distro();
if ($os eq "") {
  print STDERR "Error: $os is not a supported. Please use Fedora or Ubuntu\n";
  exit 1;
}
print Dumper(@{$packages{$os}{'deps'}});

#install missing dependencies
for my $dep (@{$packages{$os}{'deps'}}) {
	print_s("Checking if $dep is installed");
	if (check_dependency($dep, $os) == 0) {
		install_pkg($dep, $os);
	}
}
print_s("All dependencies have been or already been installed");

#create missing directories
for my $dir (@{$packages{$os}{'dirs'}}) {
	if (not -d $dir) {
    print_s("$dir does not exist. Proceeding to create the dir.");
		`sudo mkdir $dir`;
	}
}
