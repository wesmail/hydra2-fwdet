#!/bin/bash

#-------------------------------------------------
# CONFIGURATION
serverDir=/home/hades-qa/online/apr12/server  # location of hadesonlineserver.exe
clientDir=/home/hades-qa/online/apr12/client   # location of hadesonlineclient.exe
setup=/home/hades-qa/hadessoftware/squeeze64/install/5.32.01/hydra-dev/defall.sh  # environment script of Hydra
clientConf=/home/hades-qa/online/apr12/client/ClientConfig.xml # contains server host/port
#-------------------------------------------------

#-------------------------------------------------
# get host and port from client config xml
declare -a params
ct=0
for par in $(perl ${serverDir}/analyzeConfig.pl $clientConf)
do
    params[$ct]=$par
    ((ct+=1))
done

serverHost=${params[0]}
serverPort=${params[1]}
#-------------------------------------------------

#-------------------------------------------------
echo "Server host = $serverHost, port = $serverPort , found in $clientConf"
echo "serverDir   = $serverDir"
echo "clientDir   = $clientDir"
echo "clientConf  = $clientConf"
echo "setup       = $setup"
#-------------------------------------------------

#-------------------------------------------------
# cleaning of eventually running processes

client=$(ps u | grep "hadesonlineclient.exe $clientConf" | grep -v grep)     
server=$(ps u | grep "hadesonlineserver.exe SUPER $serverHost $serverPort" | grep -v grep)

if [ "$client" != "" ]
then
   pid=$(echo $client | cut -d " " -f 2)
   kill -9 $pid
fi

if [ "$server" != "" ]
then
   pid=$(echo $server | cut -d " " -f 2)
   kill -9 $pid
fi


sleep 1
#-------------------------------------------------

#-------------------------------------------------
# Jurek's favoured ERROR Handling :-) (can be commented)
#xterm  -g 20x10-0+0 -T "OLM ERROR" -bg red -fn r24 -e 'echo "ERROR"; sleep 100000'&
#xterm  -g 20x10-0-0 -T "OLM ERROR" -bg red -fn r24 -e 'echo "ERROR"; sleep 100000'&
#-------------------------------------------------


#-------------------------------------------------
sleep 1
xterm  -g 110x30-1600+30 -T "OLM server" -bg white -e ${serverDir}/start_monServer $serverDir $serverHost $serverPort $setup &
sleep 3
xterm  -g 110x30+1600+30 -T "OLM client" -bg yellow -e ${serverDir}/start_monClient $clientDir $clientConf $setup &
sleep 1
#-------------------------------------------------
