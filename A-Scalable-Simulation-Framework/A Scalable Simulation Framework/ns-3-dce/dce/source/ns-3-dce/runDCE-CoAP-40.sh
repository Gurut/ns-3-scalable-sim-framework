#! /bin/bash
################################################################################
#./runDCE-CoAP-40.sh 40 40 10 115.4 512 udp 0 5 10.1.0.1 0
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

let "macSeed=gatewayID+1"
printf -v mac2 "%x" "$macSeed"
size=${#mac2}
mac=$mac2
if [ $size -eq 1 ] ; then mac="0"$mac2 ;
fi
mac3="00:00:00:00:00:"

for rowcolumn in 40
do
    for tTime in 2 4 6 8 10
    do
        for mPREQs in 5 10 15 20
        do
            ./waf --run "dce-coap -traversal-time=$tTime -max-PREQs=$mPREQs -row=$rowcolumn -column=$rowcolumn -shuffle=$topoID -iterations=$nIter -time=$totalTime -firstSending=$firstSTime -GWIP=$gatewayIP -GOOSEPktSize=$GOOSEPktSize -sink=$gatewayID -root=$mac3$mac -UdpTcp=$protocol -pearto=false -grid=true -arp-op=1" > /media/samettonyali/5a1a8839-e023-40e3-9acf-839568bc2148/samet/Desktop/TestResults/DCE-CoAP-size-$rowcolumn-tTime-$tTime-mPREQs-$mPREQs-GID$gatewayID.$protocol 2>&1;
        done
    done
done
################################################################################
