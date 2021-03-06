#!/bin/sh

# Should typically be called as:
# ./find-lib.sh $(HW_OS) $(LIB) <paths to search in order of preference>

HW_OS=$1
shift
TARGET=$1
shift
OBJECTS=$1
shift
SOURCES=$1
shift

LIB1=lib/$TARGET
LIB2=app/nano/lib/$TARGET

SRC1=src/$LIB1
OBJ1=obj/$HW_OS/$LIB1

SRC2=src/$LIB2
OBJ2=obj/$HW_OS/$LIB2

if test -d "$SOURCES/$SRC1/"; then
   echo 1>&2 "Found $TARGET in $SOURCES/$SRC1";
   echo "-I$SOURCES/$SRC1 -L$OBJECTS/$LIB1"
   exit 0;
fi

if test -d "$SOURCES/$SRC2/"; then
   echo 1>&2 "Found $TARGET in $SOURCES/$SRC2";
   echo "-I$SOURCES/$SRC2 -L$OBJECTS/$LIB2"
   exit 0;
fi

for path in $* ; do
   if test -d "$path/$TARGET/" ; then
      echo 1>&2 "Found $TARGET in $path/$TARGET";
      echo "-I$path/$TARGET -L$path/$TARGET/$HW_OS"
      exit 0;
   fi

   if test -d "$path/$SRC1/" ; then
      echo 1>&2 "Found $TARGET in $path/$SRC1";
      echo "-I$path/$SRC1 -L$path/$OBJ1"
      exit 0;
   fi

   if test -d "$path/$SRC2/" ; then
      echo 1>&2 "Found $TARGET in $path/$SRC2";
      echo "-I$path/$SRC2 -L$path/$OBJ2"
      exit 0;
   fi
done

echo 1>&2 "Failed to locate $TARGET ... perhaps it has not been checked-out?"

exit 1
