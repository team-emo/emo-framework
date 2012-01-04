#!/bin/bash

echo 'emo-framework template installer for Xcode 4'

EMO_VER='emo-framework 0.2.0'
TEMPLATE_DIR="$HOME/Library/Developer/Xcode/Templates/emo-framework/"
SCRIPT_DIR=$(dirname $0)

force=

usage(){
cat << EOF
usage: $0 [options]
 
Install / update templates for ${EMO_VER}
 
OPTIONS:
   -f	force overwrite if directories exist
   -h	this help
EOF
}

while getopts "fhu" OPTION; do
	case "$OPTION" in
		f)
			force=1
			;;
		h)
			usage
			exit 0
			;;
	esac
done

copy_files(){
	SRC_DIR="${SCRIPT_DIR}/${1}"
	rsync -r --exclude=.svn "$SRC_DIR" "$2"
}

check_dst_dir(){
	if [[ -d $DST_DIR ]];  then
		if [[ $force ]]; then
			echo "removing old libraries: ${DST_DIR}"
			rm -rf $DST_DIR
		else
			echo "templates already installed. To force a re-install use the '-f' parameter"
			exit 1
		fi
	fi
	
	echo ...creating destination directory: $DST_DIR
	mkdir -p "$DST_DIR"
}

# copies project-based templates
copy_project_templates(){

	if [[ ! -d "$TEMPLATE_DIR" ]]; then
		echo '...creating emo-framework template directory'
		echo ''
		mkdir -p "$TEMPLATE_DIR"
	fi

	echo 'Installing emo-framework template'

	DST_DIR="$TEMPLATE_DIR"libemo.xctemplate/
	RESOURCES_DIR="$TEMPLATE_DIR""An Application.xctemplate/Resources/"

	check_dst_dir

	echo ...copying template files
	copy_files iOS-Templates/emo_app4/ "$TEMPLATE_DIR"

	echo ...copying runtime scripts
	copy_files Emo-iOS/Resources/ "$RESOURCES_DIR"

	echo ...copying framework files

	DST_DIR="$TEMPLATE_DIR"libemo.xctemplate/
	LIBS_DIR="$DST_DIR"libs

	copy_files Emo-iOS/libs/emo "$LIBS_DIR"

	echo ...copying box2d files

	DST_DIR="$TEMPLATE_DIR"libbox2d.xctemplate/
	LIBS_DIR="$DST_DIR"libs

	copy_files Emo-iOS/libs/box2d "$LIBS_DIR"

	echo ...copying squirrel files

	DST_DIR="$TEMPLATE_DIR"libsquirrel.xctemplate/
	LIBS_DIR="$DST_DIR"libs

	copy_files Emo-iOS/libs/squirrel "$LIBS_DIR"

	echo ...copying libpng files

	DST_DIR="$TEMPLATE_DIR"libpng.xctemplate/
	LIBS_DIR="$DST_DIR"libs

	copy_files Emo-iOS/libs/libpng "$LIBS_DIR"

	echo done!
}

copy_project_templates
