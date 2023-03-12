mx-snapshot
===================
[![latest packaged version(s)](https://repology.org/badge/latest-versions/mx-snapshot.svg)](https://repology.org/project/mx-snapshot/versions)

Program for creating a live-CD from MX Linux and antiX running system

JUST TO CLARIFY, this program is meant for MX Linux and antiX it won't work on another other system without considerable modifications because other systems don't have the infrastructure needed to run this program. Don't try to install the deb it won't work and might ruin your system.

# SYNOPSIS

mx-snapshot \[options\]

# DESCRIPTION

Program used for creating functional ISO images from the running system.
You can later on install/burn the ISO image on a USB flashdrive or a DVD
(depending on the size of the image). Use MX Live USB Maker (GUI) or
live-usb-maker (CLI tool) to burn the resulting image.

The tool has basically two different modes: - Preserving accounts --
used for full backups and reinstallations. Takes a full image, includes
user accounts and documents in /home folder (By default, to save space,
it excludes \~/.VirtualBox folder). - Reset accounts -- for ISOs meant
to be distributed to others. It doesn't save user account info, network
settings, and doesn't save any files in /home folder. The resulting ISO
is as close as possible to the official release with the difference that
you can customize it by updating, installing, or removing programs and
changing global settings (typically files in /etc/ folder).

By default the program starts with a GUI (program window), it can run in
terminal (CLI) if launched with -c or --cli option.

# WARNING

This tool is not actively tested on any other systems, it might or might
not work, it might or might not break things. Basically, we don't
recommend it for anything other than MX or antiX compatible systems.

# OPTIONS

  - **-h**, **--help**  
    Displays this help.

  - **-v**, **--version**  
    Displays version information.

  - **-c**, **--cli**  
    Use CLI only

  - **-d**, **--directory** \<path\>  
    Output directory

  - **-f**, **--file** \<name\>  
    Output filename

  - **-k**, **--kernel** \<ver, or path\>  
    Name a different kernel to use other than the default running
    kernel, use format returned by 'uname **-r**' Or the full path:
    */boot/vmlinuz-x.xx.x*...

  - **-l**, **--compression-level** \<"option"\>  
    Compression level options. Use quotes: "-Xcompression-level
    \<level\>", or "-Xalgorithm \<algorithm\>", or "-Xhc", see
    mksquashfs man page

  - **-m**, **--month**  
    Create a monthly snapshot, add 'Month' name in the ISO name, skip
    used space calculation This option sets reset-accounts and
    compression to defaults, arguments changing those items will be
    ignored

  - **-n**, **--no-checksums**  
    Don't calculate checksums for resulting ISO file

  - **-p**, **--preempt**  
    Option to fix issue with calculating checksums on preempt\_rt
    kernels

  - **-r**, **--reset**  
    Resetting accounts (for distribution to others)

  - **-s**, **--checksums**  
    Calculate checksums for resulting ISO file

  - **-o**, **--override-size**  
    Skip calculating free space to see if the resulting ISO will fit

  - **-x**, **--exclude** \<one item\>  
    Exclude main folders, valid choices: Desktop, Documents, Downloads,
    Music, Networks, Pictures, Steam, Videos, VirtualBox. Use the option
    one time for each item you want to exclude

  - **-z**, **--compression** \<format\>  
    Compression format, valid choices: lz4, lzo, gzip, xz, zstd

# SEE ALSO

mx-live-usb-maker -- writes created ISOs to USB flashdrives
