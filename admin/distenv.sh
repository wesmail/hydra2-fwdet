#!/bin/bash

#DEFHOSTS='@lxi001 @lxi002 @lxi003 @lxi004 @lxi005 @lxi006 @lxi007 @lxi008'
DEFHOSTS='@lxg0423 @lxg0424 @lxg0425 @lxg0426 @lxg0427 @lxg0428 @lxg0430 @lxg0414 @lxg0411'

DISTCC_PATH="/misc/wuestenf/distcc-2.18.3"

undo=FALSE
usage=FALSE
doscan=FALSE

while [ $# -gt 0 ]
do
		Option=$1
		case $Option in
				-H) DEFHOSTS=$2
						shift
						shift
						;;
				-u) undo=TRUE
						shift
						;;
				-s) doscan=TRUE
						shift
						;;
				-h) usage=TRUE
						shift
						;;
		esac
done

if [ "$undo" = "TRUE" ]
then
		tmp=
		path=`echo ${PATH} | sed -e 's/:/ /g'`
		for item in ${path}
		do
				state=`echo ${item} | grep distcc`
				if [ ${?} -ne 0 ]
				then
						tmp=${tmp}':'${item}
				fi
		done
		PATH=${tmp#:}
		export PATH
		unset DISTCC_HOSTS
		unset DISTCC
else
		if [ "$usage" = "TRUE" ]
		then
				echo -e "Usage: . `basename $0` (Huh)"
				echo -e "\t-H list of hosts to be used eg \"lxi001 lxi002\""
				echo -e "\t-u undo setup of distcc"
				echo -e "\t-s scan hosts for ssh key"
				echo -e "\t-h print this help"
				return
		else
				if [ `basename $0` = distenv.sh ]
				then 
						echo
						echo use a leading " . " to call the `basename $0` script!
						echo -e "exiting ...\n"
						exit
				fi

				echo $DEFHOSTS | grep $(hostname) >/dev/null 2>&1

				if [ $? -ne 0 ]
				then
						DEFHOSTS="@$(hostname) $DEFHOSTS"
				fi

				echo $PATH | grep $DISTCC_PATH >/dev/null 2>&1

				if [ $? -ne 0 ]
				then
						PATH=$DISTCC_PATH:$PATH
						export PATH
				fi

				# get SSH keys from all hosts in $DEFHOSTS and check if already in known_hosts list
				if [ "$doscan" = "TRUE" ]
				then
						for item in $DEFHOSTS
						do
								#chech rsa key
								cmd="ssh-keyscan -t rsa ${item#@} 2>/dev/null"
								tmp=`eval $cmd`
								cmd="grep \"$tmp\" ~/.ssh/known_hosts | cut -f 1 -d \" \""
								tmp1=`eval $cmd`
								if [ -z "$tmp1" ]
								then
										# check if host in ~/.ssh/known_hosts
										cmd="grep \"${item#@}\" ~/.ssh/known_hosts"
										tmp2=`eval $cmd`
										if [ -z "$tmp2" ]
										then
												echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
												echo "@ ${item#@} is not found in ~/.ssh/known_hosts            @"
												echo "@ adding ssh key of $item to ~/.ssh/known_hosts           @"
												echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
												echo $tmp>>~/.ssh/known_hosts
										else
												# host is in ~/.ssh/known_hosts but has other key, so check DSA key
												cmd="ssh-keyscan -t dsa ${item#@} 2>/dev/null"
												tmp=`eval $cmd`
												cmd="grep \"$tmp\" ~/.ssh/known_hosts | cut -f 1 -d \" \""
												tmp1=`eval $cmd`
												if [ -z "$tmp1" ]
												then
														# host has completly other key!
														echo "${item#@} keyscan failed:"
														echo
														ssh ${item#@} /bin/true
														DEFHOSTS=`echo $DEFHOSTS | sed -e "s/$item//g"`
												# else
														# host has dsa key, nothing to do!
												
												fi		
										fi
								fi
						done
				fi

				DISTCC_HOSTS="$DEFHOSTS"
				export DISTCC_HOSTS
				DISTCC=distcc
				export DISTCC
		fi
fi
