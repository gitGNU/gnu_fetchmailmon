#!/bin/sh

previous_email=""

if [ -z "$TXTFETCHMAILMON" ]
then
  TXTFETCHMAILMON="txtfetchmailmon"
fi

INFO=number
case $1 in
  -size) INFO=size ;;
  -number) INFO=number ;;
esac

$TXTFETCHMAILMON |\
  sed 's/^\(.*\): .*(\([0-9]*\)%), \([0-9]*\)%.*$/\1 \2 \3/' |\
  while read email number size
  do
    if [ "$previous_email" != "$email" ]
    then
      echo "XXX"
      echo "$email"
      echo "XXX"
    fi
    eval echo \$$INFO
    sleep 1
  done | dialog --begin 0 0 --gauge test 10 70
