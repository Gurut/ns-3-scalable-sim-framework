#! /bin/bash
#################################################################################
##./runDCE-CoAP.sh 20 20 10 360.4 136 udp 0 5 10.1.0.1 0
#row=$1
#column=$2
#firstSTime=$3
#totalTime=$4
#GOOSEPktSize=$5    # 136 bytes
#protocol=$6
#gatewayID=$7
#nIter=$8
#gatewayIP=$9
#topoID=${10}
#
#let "macSeed=gatewayID+1"
#printf -v mac2 "%x" "$macSeed"
#size=${#mac2}
#mac=$mac2
#if [ $size -eq 1 ] ; then mac="0"$mac2 ;
#fi
#mac3="00:00:00:00:00:"
#./waf --run "dce-coap -row=$row -column=$column -shuffle=$topoID -iterations=$nIter -time=$totalTime -firstSending=$firstSTime -GWIP=$gatewayIP -GOOSEPktSize=$GOOSEPktSize -sink=$gatewayID -root=$mac3$mac -UdpTcp=$protocol -pearto=false -grid=true -arp-op=1" > DCE-CoAP-packetSize-$GOOSEPktSize-GID$gatewayID.$protocol 2>&1;
#################################################################################

################################################################################
#./runDCE-CoAP.sh 10 10 10 360.4 512 udp 0 5 10.1.0.1 0 true true /media/samettonyali/5a1a8839-e023-40e3-9acf-839568bc2148/samet/Desktop/1024Nodes/coap-standard-jitterOn-mixedOn
row=$1
column=$2
firstSTime=$3
totalTime=$4
GOOSEPktSize=$5    # 136 bytes
protocol=$6
gatewayID=$7
nIter=$8
gatewayIP=$9
topoID=${10}
jttr=${11}
mx=${12}
folder=${13}

let "macSeed=gatewayID+1"
printf -v mac2 "%x" "$macSeed"
size=${#mac2}
mac=$mac2
if [ $size -eq 1 ] ; then mac="0"$mac2 ;
fi
mac3="00:00:00:00:00:"

for rowcolumn in 32
do
    for tTime in 2 #20
    do
        for mPREQs in 5 #10 15 20
        do
            ./waf --run "dce-coap -jitter=$jttr -mix=$mx -folderPrefix=$folder -traversal-time=$tTime -max-PREQs=$mPREQs -row=$rowcolumn -column=$rowcolumn -shuffle=$topoID -iterations=$nIter -time=$totalTime -firstSending=$firstSTime -GWIP=$gatewayIP -GOOSEPktSize=$GOOSEPktSize -sink=$gatewayID -root=$mac3$mac -UdpTcp=$protocol -pearto=false -grid=true -arp-op=1" > $folder/DCE-CoAP-size-$rowcolumn-tTime-$tTime-mPREQs-$mPREQs-GID$gatewayID.$protocol 2>&1;
        done
    done
done
#/media/samettonyali/5a1a8839-e023-40e3-9acf-839568bc2148/samet/Desktop/1024Nodes/
################################################################################
