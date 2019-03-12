#!/bin/sh
VERSION=${VERSION:=$BUILD_NUMBER}
VERSION=${VERSION:=trunk}
RPM_RELEASE=${RPM_RELEASE:=1}
SOURCE=`pwd`
BUILD=`pwd`/build

create_dir ()
{
	mkdir -p $1
	if [ $? -ne 0 ] ; then
		echo "ERROR: could not create: $1"
		exit 1
	fi
}

create_dir $BUILD/install/usr/bin

cat rpm.spec.template | \
	sed -e "s/@VERSION@/$VERSION/g" | \
	sed -e "s!@SOURCE@!$SOURCE!g" | \
	sed -e "s/@RPM_RELEASE@/$RPM_RELEASE/g" > $BUILD/rpm.spec
	
cd $BUILD
	
rpmbuild --buildroot=$BUILD/install --define "_rpmdir ." -bb rpm.spec
if [ $? -ne 0 ] ; then
	echo "ERROR: RPM build failed"
	exit 1
fi

exit 0
	


