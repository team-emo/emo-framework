#!/bin/bash

echo 'emo-framework template installer'

EMO_VER='emo-framework 0.1 alpha'
BASE_TEMPLATE_DIR="/Library/Application Support/Developer/Shared/Xcode"
BASE_TEMPLATE_USER_DIR="$HOME/Library/Application Support/Developer/Shared/Xcode"

force=
user_dir=

usage(){
cat << EOF
usage: $0 [options]
 
Install / update templates for ${EMO_VER}
 
OPTIONS:
   -f	force overwrite if directories exist
   -h	this help
   -u	install in user's Library directory instead of global directory
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
		u)
			user_dir=1
			;;
	esac
done

# Make sure only root can run our script
if [[ ! $user_dir  && "$(id -u)" != "0" ]]; then
	echo ""
	echo "Error: This script must be run as root in order to copy templates to ${BASE_TEMPLATE_DIR}" 1>&2
	echo ""
	echo "Try running it with 'sudo', or with '-u' to install it only you:" 1>&2
	echo "   sudo $0" 1>&2
	echo "or:" 1>&2
	echo "   $0 -u" 1>&2   
exit 1
fi

copy_files(){
	rsync -r --exclude=.svn "$1" "$2"
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
	if [[ $user_dir ]]; then
		TEMPLATE_DIR="${BASE_TEMPLATE_USER_DIR}/Project Templates/${EMO_VER}/"
	else
		TEMPLATE_DIR="${BASE_TEMPLATE_DIR}/Project Templates/${EMO_VER}/"
	fi

	if [[ ! -d "$TEMPLATE_DIR" ]]; then
		echo '...creating emo-framework template directory'
		echo ''
		mkdir -p "$TEMPLATE_DIR"
	fi

	echo 'Installing emo-framework template'

	DST_DIR="$TEMPLATE_DIR""An Application/"
	LIBS_DIR="$DST_DIR"libs
	RESOURCES_DIR="$DST_DIR"Resources

	check_dst_dir

	echo ...copying template files
	copy_files iOS-Templates/emo_app/ "$DST_DIR"

	echo ...copying runtime scripts
	copy_files Emo-iOS/Resources/ "$RESOURCES_DIR"

	echo ...copying framework files
	copy_files Emo-iOS/libs/ "$LIBS_DIR"

	echo done!
}

copy_project_templates
