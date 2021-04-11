#!/bin/sh

TOP=$HOME
OS_BASE=/afs/inf.ed.ac.uk/group/teaching/cs3/os

echo "Preparing coursework directory for OS..."

if [ ! -d $TOP ]; then
	echo "ERROR: Home directory does not exist!"
	exit 1
fi

TARGET=$TOP/os-coursework

if [ -d $TARGET ]; then
	echo "ERROR: Target directory $TARGET already exists... ABORTING!"
	exit 1
fi

echo "Creating target directory $TARGET..."
mkdir $TARGET

echo "Checking out infos repository..."
git clone https://github.com/tspink/infos $TARGET/infos
#git clone $OS_BASE/infos.git $TARGET/infos

echo "Checking out infos-user repository..."
git clone https://github.com/tspink/infos-user $TARGET/infos-user
#git clone $OS_BASE/infos-user.git $TARGET/infos-user

echo "Copying coursework skeleton files..."
cp -r $OS_BASE/coursework-skeleton $TARGET/coursework-skeletons
chmod -R a-w $TARGET/coursework-skeletons
cp -r $OS_BASE/coursework-skeleton $TARGET/coursework

echo "Connecting coursework directory to master source..."
ln -s $TARGET/coursework $TARGET/infos/oot

echo "Copying utility scripts..."
cp $OS_BASE/utility-scripts/* $TARGET/

echo
echo "**********"
echo "   DONE   "
echo "**********"
echo

echo "  The infos coursework development environment has been created in:"
echo
echo "    $TARGET"

echo
echo "  From that directory, the following script can be used to build"
echo "  and run infos:"
echo
echo "    ./build-and-run.sh"
echo
echo "  Your coursework solutions (in the coursework directory) will be"
echo "  AUTOMATICALLY compiled in."
echo
echo "  In order for infos to use your implementations, you MUST specify"
echo "  your modules on the command-line, e.g.:"
echo
echo "    ./build-and-run.sh pgalloc.algorithm=buddy"
echo
echo "  See the coursework specification document for further information,"
echo "  and what command-line options are available for debugging."
echo
