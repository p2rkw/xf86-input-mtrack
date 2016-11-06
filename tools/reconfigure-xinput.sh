#! /bin/bash

# Script to parse configuration dumped by 'xinput --list-props <dev id>'
# and reapply it on host machine.
# It was created to help reconfigure mtrack switch easly between user's setup
# and mine. 

logExec(){
  echo $@
  eval "$@"
}

# $1 - device id
# $2 - file name to read new settings from
main(){
  local devId=$1
  local fileToApply=$2
  while read -r line || [[ -n "$line" ]]; do
#    echo line "$line"
    local propId=$(grep -Po '\(\d+\)' <<< $line)
    propId=$(grep -Po '\d+' <<< $propId)
    if [[ "$propId" == "" ]]; then
      continue
    fi
    
    local propName=$(grep -Po ".*\(" <<< $line)
    propName=${propName::-2}
#    echo $propName
    
    local values=$(grep -Po ':.*' <<< $line)
    values=${values:1} # skip colon
    values=${values//,/ } # remove commas
    
#    echo $propName $propId : ${values}
    logExec xinput --set-prop $devId \'$propName\' $values   
  done < $fileToApply
}

# $1 - device id
# $2 - file name to read new settings from
main "$@"