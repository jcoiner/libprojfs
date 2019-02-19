source /proj/verif_release_ro/cbwa_initscript/current/cbwa_init.csh

module load gcc/8.2.0
module load binutils/2.30
module load git/2.16.2
module load ccache/3.3.3
module load automake/1.15.1
module load libtool/2.4.6
module load python/3.7.0
module load libfuse/3.3.0

setenv CC "ccache gcc"
setenv CXX "ccache g++"

#  THEN:
#
#  ./configure --prefix=/home/jcoiner/work3/libprojfs-install --with-libfusepkg=/tool/pandora64/.package/libfuse-3.3.0/lib64/pkgconfig/fuse3.pc --enable-vfs-api

