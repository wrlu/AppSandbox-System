#!/bin/bash
#
# Created by Yiwen Xu on 2022/6.
#

echo "Copy the specified files to aosp"

if [[ $# != 2 ]]; then
    echo "Usage: ./copyfiles.sh <copy_file_path> to <target_aosp_source_path>"
    echo "Example: ./copyfiles.sh /home/hssl/androidframework/external/okhttp /home/hssl/aosp/external/okhttp"
    exit 5
fi

fileset=()
read_recursive_dir(){
    for file in `ls -a $1`
    do
        if [ -d $1"/"$file ]
        then
            if [[ $file != '.' && $file != '..' ]]
            then
                read_recursive_dir $1"/"$file
            fi
        else
            #echo $1"/"$file
            fileset[${#fileset[@]}]=$1"/"$file
        fi
    done
}



COPYPATH=$1
AOSPPATH=$2

read_recursive_dir $COPYPATH 

echo "We will copy ${#fileset[@]} files to the related path of "$AOSPPATH

read -r -p "Do you confirm the copy [y/show/N]?" answer
case $answer in 
    Y|y) 
        for file in ${fileset[*]}
        do
            src=$file
            tgt=$AOSPPATH${file##$COPYPATH}
            prefix=${tgt%/*}"/"
            if [ ! -d $prefix ]; then
                mkdir -p $prefix
            fi
            cp $src $tgt
        done
        exit 0
        ;;
    show)
        for file in ${fileset[*]}
        do
            src=$file
            tgt=$AOSPPATH${file##$COPYPATH}
            prefix=${tgt%/*}"/"
            if [ ! -d $prefix ]; then
                echo "mkdir -p $prefix"
            fi
            echo "cp $src $tgt"
        done
        exit 0
        ;;
    N|n)
        echo "Nothing happens. Take care~"
        exit 6
        ;;
    *) 
        echo "Bad choice. Exiting...."
        exit 7
        ;;
esac
