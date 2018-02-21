#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mesh-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include "ns3/secure-arp.h"
#include "ns3/arp-l3-protocol.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "n_eq_coord.h"
#include "n_eq_25.h"
#include "n_eq_36.h"
#include "n_eq_49.h"
#include "n_eq_64.h"
#include "n_eq_81.h"
#include "n_eq_100.h"
#include "n_eq_121.h"
#include "n_eq_144.h"
#include "n_eq_169.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("GOOSEScript");

class GOOSE{
    public:
        /// Init test
        GOOSE ();
        /// Configure test from command line arguments
        void Configure (int argc, char ** argv);
        void ReadTargetList();
        /// Run test
        int Run ();
        uint32_t m_privacy;
        uint32_t m_Mode;
        
    private:
        int m_row;
        int m_column;
        double m_step;
        double m_randomStart;
        double m_totalTime;
        double m_firstSendingTime;
        double m_packetInterval;
        uint16_t m_GOOSEPktSize;
        uint32_t m_numOfLeafMeters;
        uint32_t m_nIfaces;
        uint32_t m_fileID;
        uint32_t m_nodeSize;
        uint32_t m_aesEncrDelay;
        uint32_t m_aesDecrDelay;
        uint32_t m_ecdsaSignDelay;
        uint32_t m_ecdsaVerifyDelay;
        uint32_t m_nIterations;
        bool m_chan;
        bool m_pcap;
        bool m_jitterActivated;
        bool m_mixActivated;
        std::string m_stack;
        std::string m_root;
        std::string m_txrate;
        std::string m_input;
        std::string m_folderPrefix;
        int m_node_num;
        int m_conn;
        int m_shuffle;
        double m_initstart;
        int m_sink;
        std::string m_sinkIpAddress;
        bool m_ActivateSecurityModule;
        std::string m_filename;
        bool m_gridtopology;
        std::string m_UdpTcpMode;
        std::string m_gatewayIP;
        int m_arpOp;
        double m_arpwait;
        bool m_randomAppStart;
        int  m_ArpMinIntervalFactor;
        bool m_ActivatePeriodicPiggybackArp;
        double m_ArpPiggybackScheduledTime;
        double m_ArpPiggybackDurationTime;
        int m_maxDepth;
        int m_strategy;
        double m_delta;
        bool m_peartoActivated;
        uint32_t m_consumerId;
        int m_connectionType;
        int m_portId;
        int m_sensor, m_aggregator;
        uint32_t child_count;
        int m_traversalTime;
        int m_nMaxPREQs;

        vector< coordinates > nodeCoords;
        vector< int > child;
        vector< int > parent;
        vector< int > aggnode;
        vector< int > parent2;
        vector< int > count;
        vector< int > psize;
        vector< int > m_targetList;

        //to calculate the length of the simulation
        float m_timeTotal, m_timeStart, m_timeEnd;
        // List of network nodes
        NodeContainer nodes;
        // List of all mesh point devices
        NetDeviceContainer meshDevices;
        //Addresses of interfaces:
        Ipv4InterfaceContainer interfaces;
        // MeshHelper. Report is not static methods
        MeshHelper mesh;
  
    private:
        // Create nodes and setup their mobility
        void CreateNodes ();
        // Install internet m_stack on nodes
        void InstallInternetStack ();
        void InstallSecureArp();
  
        // Install applications
        void InstallApplication ();

        // Print mesh devices diagnostics
        void Report ();
};

GOOSE::GOOSE () :
    m_privacy (0),
    m_Mode (0),
    m_row (5),
    m_column (5),
    m_step (100.0),
    m_randomStart (0.1),
    m_totalTime (70.4),
    m_firstSendingTime (10.0),
    m_packetInterval (60),
    m_GOOSEPktSize (512),
    m_nIfaces (1),
    m_fileID (1),
    m_nodeSize (25),
    m_aesEncrDelay (337477),
    m_aesDecrDelay (362341),
    m_ecdsaSignDelay (1842080),
    m_ecdsaVerifyDelay (2413828),
    m_nIterations (5),
    m_chan (true),
    m_pcap (true),
    m_jitterActivated (true),
    m_mixActivated (true),
    m_stack ("ns3::Dot11sStack"),
    m_root ("00:00:00:00:00:01"),
    m_txrate ("150kbps"),
    m_input("MST-7-0-0.mst"),
    m_node_num (0),
    m_conn (0),
    m_shuffle (0),
    m_initstart (35.4),
    m_sink (0),
    m_sinkIpAddress ("10.1.0.1"),
    m_ActivateSecurityModule (false),
    m_gridtopology (true),
    m_UdpTcpMode ("udp"),
    m_gatewayIP ("10.1.0.1"),
    m_arpOp (1),
    m_arpwait (4), // default 1 s, 4s better since no failed node
    m_randomAppStart (false),
    m_ArpMinIntervalFactor (1), // default 2.048 seconds or 1024 * 2000 microseconds
    m_ActivatePeriodicPiggybackArp (false),
    m_ArpPiggybackScheduledTime (100),
    m_ArpPiggybackDurationTime (10),
    m_maxDepth (0),
    m_strategy (3),
    m_delta (0.03),
    m_peartoActivated (false),
    m_consumerId (15),
    m_connectionType (0),
    m_portId (8100),
    m_traversalTime (2),
    m_nMaxPREQs (5)
{
}

void GOOSE::Configure (int argc, char *argv[]){
    CommandLine cmd;
    cmd.AddValue ("row", "Number of nodes in a row grid. [10]", m_row);
    cmd.AddValue ("column", "Number of rows in a grid. [10]", m_column);
    cmd.AddValue ("step",   "Size of edge in our grid, meters. [100 m]", m_step);
    /*
     * As soon as starting node means that it sends a beacon,
     * simultaneous start is not good.
     */
    cmd.AddValue ("start",  "Maximum random start delay, seconds. [0.1 s]", m_randomStart);
    cmd.AddValue ("mode",  "Aggregation Mode - [0] -> HbyH, [1] -> EtoE", m_Mode);
    cmd.AddValue ("privacy",  "[0] -> Plaintext, [1] -> PHE", m_privacy);
    cmd.AddValue ("time",  "Simulation time, seconds [100 s]", m_totalTime);
    cmd.AddValue ("firstSending",  "Simulation time, seconds [100 s]", m_firstSendingTime);
    cmd.AddValue ("packetInterval",  "Interval between packets in UDP ping, seconds [0.001 s]", m_packetInterval);
    cmd.AddValue ("GOOSEPktSize", "Size of the second packets", m_GOOSEPktSize);
    cmd.AddValue ("interfaces", "Number of radio interfaces used by each mesh point. [1]", m_nIfaces);
    cmd.AddValue ("channels",   "Use different frequency channels for different interfaces. [0]", m_chan);
    cmd.AddValue ("pcap",   "Enable PCAP traces on interfaces. [0]", m_pcap);
    cmd.AddValue ("jitter",   "Enable PCAP traces on interfaces. [0]", m_jitterActivated);
    cmd.AddValue ("mix",   "Enable PCAP traces on interfaces. [0]", m_mixActivated);
    cmd.AddValue ("stack",  "Type of protocol stack. ns3::Dot11sStack by default", m_stack);
    cmd.AddValue ("root", "Mac address of root mesh point in HWMP", m_root);
    cmd.AddValue ("node", "Node sink", m_node_num);
    cmd.AddValue ("conn", "Number of sending nodes [1]", m_conn); 
    cmd.AddValue ("shuffle", "Number of random shuffle [2]", m_shuffle);
    cmd.AddValue ("init", "Initial Starting time [5.4]", m_initstart);
    cmd.AddValue ("input", "Topology file to read in node positions", m_input);
    cmd.AddValue ("fileId", "Topology file to read in node positions", m_fileID);
    cmd.AddValue ("nodeSize", "Topology file to read in node positions", m_nodeSize);
    cmd.AddValue ("iterations", "The number of iterations", m_nIterations);
    cmd.AddValue ("sink", "Sink node ID [0]", m_sink);
    cmd.AddValue ("sink-ip", "IP address of the default entry in ARP table", m_sinkIpAddress);
    cmd.AddValue ("security","Activate Security Module [false]", m_ActivateSecurityModule);
    cmd.AddValue ("grid", "Choice whether grid or random topology [true]", m_gridtopology);
    cmd.AddValue ("UdpTcp", "UDP or TCP mode [udp]", m_UdpTcpMode);
    cmd.AddValue ("GWIP", "UDP or TCP mode [udp]", m_gatewayIP);
    cmd.AddValue ("arp-op", "ARP operations : 1. Normal [default], 2. Creation only, 3. Maintenance ony, 4. All pre-install arp table", m_arpOp);
    cmd.AddValue ("wait-arp", "When this timeout expires, the cache entries will be scanned and entries in WaitReply state will resend ArpRequest unless MaxRetries has been exceeded, in which case the entry is marked dead [1s]", m_arpwait);
    cmd.AddValue ("random-start", "Random start of the application [false]", m_randomAppStart);
    cmd.AddValue ("strategy", "Type of scheduling strategies. 1. Nearest nodes first, 2. Farthest node first, 3. Random [3]", m_strategy);
    cmd.AddValue ("delta", "The additional time for scheduling strategy [0.03]", m_delta);
    cmd.AddValue ("pearto", "PEA RTO Activated ? [false]", m_peartoActivated);
    cmd.AddValue ("connection-type", "Type of connection for connection establishment [0] = created first, 1 = created when data is sent", m_connectionType);
    cmd.AddValue ("port-num", "The port number of the remote host, it can determine the QoS support, 8100 = tcp_default, 9100 = gbr_gamming", m_portId);
    cmd.AddValue ("piggyback-interval", "The interval of piggyback ARP in PREQ-PREP  [1s]", m_ArpMinIntervalFactor);
    cmd.AddValue ("periodic-piggyback", "Activate Periodic Piggyback ARP in PREQ-PREP [false]", m_ActivatePeriodicPiggybackArp);
    cmd.AddValue ("pp-interval", "Periodic piggyback ARP interval [100s]", m_ArpPiggybackScheduledTime);
    cmd.AddValue ("pp-duration", "Periodic piggyback ARP interval [10s]", m_ArpPiggybackDurationTime);
    cmd.AddValue ("traversal-time", "Periodic piggyback ARP interval [100s]", m_traversalTime);
    cmd.AddValue ("max-PREQs", "Periodic piggyback ARP interval [10s]", m_nMaxPREQs);
    cmd.AddValue ("folderPrefix", "Periodic piggyback ARP interval [10s]", m_folderPrefix);

    cmd.Parse (argc, argv);
    NS_LOG_DEBUG ("Grid:" << m_row << "*" << m_column);
    m_nodeSize = m_row * m_column;
    NS_LOG_DEBUG ("Simulation time: " << m_totalTime << " s");
}

void GOOSE::CreateNodes (){
    nodes.Create (m_nodeSize);
    
    double m_txpower = 18.0; // dbm
    // Configure YansWifiChannel
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();

    wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-89.0) );
    wifiPhy.Set ("CcaMode1Threshold", DoubleValue (-62.0) );
    wifiPhy.Set ("TxGain", DoubleValue (1.0) );
    wifiPhy.Set ("RxGain", DoubleValue (1.0) );
    wifiPhy.Set ("TxPowerLevels", UintegerValue (1) );
    wifiPhy.Set ("TxPowerEnd", DoubleValue (m_txpower) );
    wifiPhy.Set ("TxPowerStart", DoubleValue (m_txpower) );
    wifiPhy.Set ("RxNoiseFigure", DoubleValue (7.0) );

    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
    wifiPhy.SetChannel (wifiChannel.Create ());
    wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

    // Configure the parameters of the Peer Link
    Config::SetDefault ("ns3::dot11s::PeerLink::MaxBeaconLoss", UintegerValue (20));
    Config::SetDefault ("ns3::dot11s::PeerLink::MaxRetries", UintegerValue (10));    //changed from 4 to 10 (S.T.)
    Config::SetDefault ("ns3::dot11s::PeerLink::MaxPacketFailure", UintegerValue (10));  //changed from 5 to 10 (S.T.)

    // Configure the parameters of the HWMP
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::Dot11MeshHWMPnetDiameterTraversalTime", TimeValue (Seconds (m_traversalTime)));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::Dot11MeshHWMPactivePathTimeout", TimeValue (Seconds (100)));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::Dot11MeshHWMPactiveRootTimeout", TimeValue (Seconds (100)));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::Dot11MeshHWMPmaxPREQretries", UintegerValue (m_nMaxPREQs));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::UnicastPreqThreshold",UintegerValue (10));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::UnicastDataThreshold",UintegerValue (5));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::DoFlag", BooleanValue (true));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::RfFlag", BooleanValue (false));

    // Configure the default entry of ARP table
    //  Config::SetDefault ("ns3::dot11s::HwmpProtocol::ArpIP", Ipv4AddressValue (Ipv4Address (m_sinkIpAddress.c_str())));
    //  Config::SetDefault ("ns3::dot11s::HwmpProtocol::ArpMac", Mac48AddressValue (Mac48Address (m_root.c_str ())));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::ArpMinInterval", TimeValue (MicroSeconds (m_ArpMinIntervalFactor * 1024 * 2000)));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::ActivatePeriodicPiggybackArp", BooleanValue(m_ActivatePeriodicPiggybackArp));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::ArpPiggybackScheduledTime", TimeValue (Seconds(m_ArpPiggybackScheduledTime)));
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::ArpPiggybackDurationTime", TimeValue (Seconds(m_ArpPiggybackDurationTime)));
    //
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::ActivateArpPiggybacking", BooleanValue (true));
    
    //Added by Samet Tonyali
    Config::SetDefault ("ns3::dot11s::HwmpProtocol::MaxTtl", UintegerValue (70));
    
    if (m_arpwait != 1.0) {
        Config::SetDefault ("ns3::ArpCache::WaitReplyTimeout", TimeValue (Seconds (m_arpwait)));
    }
   
    /*
     * Create mesh helper and set stack installer to it
     * Stack installer creates all needed protocols and install them to
     * mesh point device
     */
    mesh = MeshHelper::Default ();
    if (!Mac48Address (m_root.c_str ()).IsBroadcast ()){
        mesh.SetStackInstaller (m_stack, "Root", Mac48AddressValue (Mac48Address (m_root.c_str ())));
    }
    else{
        //If root is not set, we do not use "Root" attribute, because it
        //is specified only for 11s
        mesh.SetStackInstaller (m_stack);
    }
    
    if (m_chan){
        mesh.SetSpreadInterfaceChannels (MeshHelper::SPREAD_CHANNELS);
    }
    else{
        mesh.SetSpreadInterfaceChannels (MeshHelper::ZERO_CHANNEL);
    }
    
    mesh.SetStandard (WIFI_PHY_STANDARD_80211g);
    mesh.SetMacType ("RandomStart", TimeValue (Seconds(m_randomStart)));
    mesh.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("ErpOfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (2500));
 
    // Set number of interfaces - default is single-interface mesh point
    mesh.SetNumberOfInterfaces (m_nIfaces);
  
    // Install protocols and return container if MeshPointDevices
    meshDevices = mesh.Install (wifiPhy, nodes);
    
    // Setup mobility - static grid topology
    MobilityHelper mobility;
    if(m_gridtopology){
        mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0),
                                   "MinY", DoubleValue (0.0),
                                   "DeltaX", DoubleValue (m_step),
                                   "DeltaY", DoubleValue (m_step),
                                   "GridWidth", UintegerValue (m_column),
                                   "LayoutType", StringValue ("RowFirst"));
        for (unsigned int i = 0; i < (unsigned)m_row; i++){
            for (unsigned int j = 0; j < (unsigned)m_column; j++){
                nodeCoords.push_back ({(j*m_step), (i*m_step)});
            }
        }

        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        for (vector< coordinates >::iterator j = nodeCoords.begin (); j != nodeCoords.end (); j++){
            positionAlloc->Add (Vector ((*j).X, (*j).Y, 0));
        }
        mobility.SetPositionAllocator (positionAlloc);
    }
    else{
        switch (m_row) {
            case 5:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_25[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_25[m_shuffle][i]);
                break;
            case 6:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_36[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_36[m_shuffle][i]);
                break;
            case 7:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_49[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_49[m_shuffle][i]);
                break;
            case 8:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_64[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_64[m_shuffle][i]);
                break;
            case 9:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_81[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_81[m_shuffle][i]);
                break;
            case 10:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_100[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_100[m_shuffle][i]);
                break;
            case 11:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_121[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_121[m_shuffle][i]);
                break;
            case 12:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_144[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_144[m_shuffle][i]);
                break;
            case 13:
                for (unsigned int i = 0; i < sizeof makeArray(n_eq_169[m_shuffle]); i++)
                    nodeCoords.push_back (n_eq_169[m_shuffle][i]);
                break;
        }
        
        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        for (vector< coordinates >::iterator j = nodeCoords.begin (); j != nodeCoords.end (); j++){
            positionAlloc->Add (Vector ((*j).X, (*j).Y, 0));
        }
        mobility.SetPositionAllocator (positionAlloc);
    }
    
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);

//    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    for (NodeContainer::Iterator j = nodes.Begin(); j != nodes.End(); ++j) {
        Ptr<Node> object = *j;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
        Vector pos = position->GetPosition();

//        positionAlloc->Add (Vector (pos.x, pos.y, 0.0));

        NS_LOG_INFO("Node " << object->GetId() << " x=" << pos.x << ", y=" << pos.y);
    }

//    mobility.SetPositionAllocator (positionAlloc);
//    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//    mobility.Install (nodes);
    
    std::string folderDir;
    std::string folder = "folder";  /*//media/samettonyali/5a1a8839-e023-40e3-9acf-839568bc2148/samet/Desktop/1024Nodes/*/
    std::string dash = "-";
    
    std::stringstream folderName;
    folderName << m_folderPrefix << "/" << folder << dash << m_row << dash << m_traversalTime << dash << m_nMaxPREQs;
    folderDir = folderName.str();
    mkdir(folderDir.c_str(), 0777);
    
    folderName << "/dce-coap-";
    
    if (m_pcap)
        wifiPhy.EnablePcapAll (folderName.str().c_str());
    //wifiPhy.EnablePcapAll (std::string ("dce-coap-"));
}

void GOOSE::InstallInternetStack (){
    Config::SetDefault ("ns3::ArpCache::AliveTimeout", TimeValue (Seconds (365)));
    
/*
    DceManagerHelper dceManager;
    InternetStackHelper internetStack;
    internetStack.Install (nodes);
    Ipv4AddressHelper address;
    address.SetBase ("127.0.0.0", "255.255.255.0");
    interfaces = address.Assign (meshDevices);

    dceManager.Install (nodes);
*/
    DceManagerHelper dceManager;
    dceManager.SetTaskManagerAttribute ("FiberManagerType", StringValue ("UcontextFiberManager"));
//    dceManager.SetTaskManagerAttribute ("FiberManagerType", StringValue ("PthreadFiberManager"));
//    dceManager.SetNetworkStack ("ns3::LinuxSocketFdFactory", "Library", StringValue ("liblinux.so"));
    InternetStackHelper stack;
//    LinuxStackHelper stack;
    stack.Install (nodes);

    Ipv4AddressHelper address;
    address.SetBase ("10.1.0.0", "255.255.0.0");
    interfaces = address.Assign (meshDevices);

    dceManager.Install (nodes);
}

void
GOOSE::InstallSecureArp ()
{
   int x = 0;
   // for now, we generate 2 keys pair, first key pair is used for PREQ
   // second key pair is used for PREP, therefore : 
   // at the sink : store private key of PREQ and public key of PREP
   // at nodes : store private key of PREP and public key of PREQ
   for (NetDeviceContainer::Iterator i = meshDevices.Begin (); i != meshDevices.End (); ++i)
    {   
       Ptr<NetDevice> nd = *i;
       Ptr<Node> node = nd->GetNode ();
       Ptr<ArpL3Protocol> arpl3 = node->GetObject<ArpL3Protocol> ();
       Ptr<SecureArp> sa = nd->GetObject<SecureArp> ();
    
       sa->SetArpL3ProtocolCallback(MakeCallback (&ArpL3Protocol::AcceptArpFromHwmp, PeekPointer (arpl3)));
       sa->SetArpMacAddress (Mac48Address::ConvertFrom (nd->GetAddress ()) );
       sa->SetArpIpAddress (interfaces.GetAddress (x));  
       x++;
     }
}

void GOOSE::InstallApplication (){
    std::string filesID, fileDir;
    std::string files = "files-";
    
    std::stringstream flsID;
    FILE *from = NULL, *to = NULL;
    char ch;
    for (int i = 0; i < (int)(m_nodeSize); i++){
        if(i == m_sink) continue;
        
        flsID << files << i;    //<< m_folderPrefix
        filesID = flsID.str();
        mkdir(filesID.c_str(), 0777);
        
        flsID << "/tmp";
        filesID = flsID.str();
        mkdir(filesID.c_str(), 0777);
        
        /* open source file */
        if((from = fopen("togo", "rb"))==NULL) {
            printf("Cannot open source file.\n");
            exit(1);
        }
        
        flsID << "/togo";
        fileDir = flsID.str();
        
        /* open destination file */
        if((to = fopen(fileDir.c_str(), "wb"))==NULL) {
            printf("Cannot open destination file: %s.\n", fileDir.c_str());
            exit(1);
        }
        
        /* copy the file */
        while(!feof(from)) {
            ch = fgetc(from);
            if(ferror(from)) {
                printf("Error reading source file.\n");
                exit(1);
            }
            if(!feof(from)) fputc(ch, to);
            if(ferror(to)) {
                printf("Error writing destination file.\n");
                exit(1);
            }
        }
        
        if(fclose(from)==EOF) {
            printf("Error closing source file.\n");
            exit(1);
        }

        if(fclose(to)==EOF) {
            printf("Error closing destination file.\n");
            exit(1);
        }
        
        flsID.str("");
    }
    
    //ApplicationContainer apps [m_nodeSize];
    DceApplicationHelper dce;
    ApplicationContainer serverApp;
    ApplicationContainer clientApps[m_nodeSize];

    dce.SetBinary ("coap-server");
    dce.SetStackSize (1 << 20);
    dce.ResetArguments ();
    dce.AddArgument ("-A");
//    dce.AddArgument ("10.1.1.25");
    dce.AddArgument (m_gatewayIP);
    dce.AddArgument ("-v");
    dce.AddArgument ("7");
//    dce.AddArgument ("-p");
//    dce.AddArgument ("6931");
    serverApp = dce.Install (nodes.Get (m_sink));
    serverApp.Start (Seconds (55.4));
    serverApp.Stop (Seconds (m_totalTime));

/*  
    std::string m_protocol;
    if (m_UdpTcpMode=="udp") m_protocol = "ns3::UdpSocketFactory";
    else m_protocol = "ns3::TcpSocketFactory";
    
    // Listener for the gateway: Listens at port 8000
    Ptr<GOOSEApplication> gateway = CreateObject<GOOSEApplication> ();
    gateway->SetAttribute ("Local", AddressValue (InetSocketAddress (interfaces.GetAddress (m_sink), 8000)));
    gateway->SetAttribute ("Remote", AddressValue (InetSocketAddress (interfaces.GetAddress (m_targetList[m_sink]), 8000)));
    gateway->SetAttribute ("Protocol", StringValue(m_protocol));
    gateway->SetAttribute ("PacketSize", UintegerValue(m_GOOSEPktSize));
    gateway->SetAttribute ("DefaultRxSize", UintegerValue(m_GOOSEPktSize));
    gateway->SetAttribute ("MeterType", UintegerValue(0));
    gateway->SetAttribute ("Child", UintegerValue (m_nodeSize-1));
    gateway->SetAttribute ("LeafMeters", UintegerValue(m_nodeSize-1));
    gateway->SetAttribute ("FirstSendingTime", TimeValue (Seconds(m_firstSendingTime)));
    gateway->SetAttribute ("Iterations", UintegerValue(m_nIterations));
    gateway->SetNode(nodes.Get(m_sink));
    
    gateway->SetStartTime(Seconds (0.1));
    gateway->SetStopTime(Seconds (m_totalTime+20));
    nodes.Get (m_sink)->AddApplication(gateway);
*/

    dce.SetBinary ("coap-client");
    dce.SetStackSize (1 << 20);
    dce.ResetArguments ();
    dce.ResetEnvironment ();
//    dce.AddEnvironment ("HOME", "/home/dce");
    dce.AddArgument ("-m");
    dce.AddArgument ("post");
    dce.AddArgument ("-P");
    dce.AddArgument (m_gatewayIP);
//    dce.AddArgument ("10.1.1.25:6931");
    dce.AddArgument ("coap://[::1]/time");
    dce.AddArgument ("-v");
    dce.AddArgument ("7");
    dce.AddArgument ("-f");
//    dce.AddArgument ("togo");
    dce.AddArgument ("/tmp/togo");

//    Ipv4StaticRoutingHelper ipv4RoutingHelper;
//    Ptr<Node> meshNode;
//    Ptr<Ipv4StaticRouting> meshStaticRouting;
    
    if(m_mixActivated){
        int displacement = 0;
        int *array = new int[m_nodeSize];
        for (int i = 0; i < m_nodeSize-1; i++) {
           if (i == m_sink) {
              displacement++;
           }
           array[i] = i+displacement;
        }
        // shuffle twice, to make it more random
        for (int i = 0; i < m_shuffle; i++) {
          std::random_shuffle(array,array+(m_nodeSize-1));
        }
        
        int m_source;
        for (int i = 0; i < (int)(m_nodeSize-1); i++){
            m_source = array[i];
            clientApps[i] = dce.Install (nodes.Get (m_source));

            if(m_jitterActivated)
                clientApps[i].Start (Seconds (60.4+(i*0.001)));
            else
                clientApps[i].Start (Seconds (60.4));
        }
    }
    else{
        for (int i = 0; i < (int)(m_nodeSize); i++){
            if(i == m_sink) continue;

            clientApps[i] = dce.Install (nodes.Get (i));

            if(m_jitterActivated)
                clientApps[i].Start (Seconds (60.4+(i*0.001)));
            else
                clientApps[i].Start (Seconds (60.4));
            //apps.Stop (Seconds (90.4));
            
            clientApps[i].Stop(Seconds (m_totalTime));

    /*        
            Ptr<GOOSEApplication> node = CreateObject<GOOSEApplication> ();
            node->SetAttribute ("Local", AddressValue (InetSocketAddress (interfaces.GetAddress (m_source), 8000)));
            node->SetAttribute ("Remote", AddressValue (InetSocketAddress (interfaces.GetAddress (m_targetList[m_source]), 8000)));
            node->SetAttribute ("Protocol", StringValue(m_protocol));
            node->SetAttribute ("PacketSize", UintegerValue (m_GOOSEPktSize));
            node->SetAttribute ("DefaultRxSize", UintegerValue(m_GOOSEPktSize));
            node->SetAttribute ("Iterations", UintegerValue(m_nIterations));
            node->SetAttribute ("MeterType", UintegerValue(1));

            NS_LOG_INFO(interfaces.GetAddress (m_source) << " will send to " << interfaces.GetAddress (m_targetList[m_source]));

            node->SetNode(nodes.Get(m_source));
            node->SetStartTime(Seconds (0.1));
            node->SetStopTime(Seconds (m_totalTime+20));
            nodes.Get (m_source)->AddApplication(node);
    */
            // Static Routing from non-gateway nodes to the gateway
            /*meshNode = nodes.Get (m_source);
            meshStaticRouting = ipv4RoutingHelper.GetStaticRouting (meshNode->GetObject<Ipv4> ());
            NS_ASSERT(meshStaticRouting != 0);
            meshStaticRouting->SetDefaultRoute (interfaces.GetAddress (m_sink), 1);*/
        }
    }
}

int GOOSE::Run (){
    CreateNodes();
    InstallInternetStack ();
    InstallSecureArp ();

    // Create a mesh gateway pointer
    /*Ptr<Node> meshGateway = nodes.Get (m_sink);
   
    Ipv4InterfaceAddress iaddr = meshGateway->GetObject<Ipv4>()->GetAddress (1,0);
    Ipv4Address addri = iaddr.GetLocal (); 
    std::cout << "Mesh Gateway IP address : " << addri << " " << meshGateway->GetObject<Ipv4>()->GetInterfaceForAddress (addri) << " Node ID : " << meshGateway->GetId () << std::endl;*/

    InstallApplication();
 
    m_timeStart=clock();
    
//    Simulator::Schedule (Seconds (m_totalTime), &GOOSE::Report, this);
    Simulator::Stop (Seconds (m_totalTime));
//    AnimationInterface anim ("GOOSEOutput.xml");
    Simulator::Run ();
    Simulator::Destroy ();
    
    m_timeEnd=clock();
    m_timeTotal=(m_timeEnd - m_timeStart)/(double) CLOCKS_PER_SEC;
    
    std::cout << "\n*** Simulation time: " << m_timeTotal << "s\n\n";

    return 0;
}

int main (int argc, char *argv[]){
    LogComponentEnable ("GOOSEScript", LOG_LEVEL_ALL);
//    LogComponentEnable ("UdpL4Protocol", LOG_LEVEL_ALL);
//    LogComponentEnable ("UdpSocketImpl", LOG_LEVEL_ALL);
//    LogComponentEnable ("IpL4Protocol", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4AddressGenerator", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4EndPointDemux", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4EndPoint", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4GlobalRouting", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4Header", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4InterfaceAddress", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4ListRouting", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4PacketFilter", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4PacketInfoTag", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4PacketProbe", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4QueueDiscItem", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4RawSocketImpl", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4Route", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4RoutingTableEntry", LOG_LEVEL_ALL);
//    LogComponentEnable ("Ipv4StaticRouting", LOG_LEVEL_ALL);
    LogComponentEnable ("MeshPointDevice", LOG_LEVEL_ALL);
    LogComponentEnable ("MeshWifiInterfaceMac", LOG_LEVEL_ALL);
    LogComponentEnable ("RegularWifiMac", LOG_LEVEL_ALL);
    LogComponentEnable ("HwmpProtocol", LOG_LEVEL_ALL);
    LogComponentEnable ("WifiNetDevice", LOG_LEVEL_ALL);
    LogComponentEnable ("HwmpProtocolMac", LOG_LEVEL_ALL);
    LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
//    LogComponentEnable ("ArpHeader", LOG_LEVEL_ALL);
    LogComponentEnable ("ArpL3Protocol", LOG_LEVEL_ALL);
    LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
//    LogComponentEnable ("TrafficControlLayer", LOG_LEVEL_ALL);
    LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
//    LogComponentEnable ("HwmpRtable", LOG_LEVEL_ALL);
    LogComponentEnable ("SecureArp", LOG_LEVEL_ALL);
    LogComponentEnable ("Dot11sPeerManagementProtocol", LOG_LEVEL_ALL);
    LogComponentEnable ("Dot11sPeerManagementProtocolMac", LOG_LEVEL_ALL);
    LogComponentEnable ("PeerManagementProtocol", LOG_LEVEL_ALL);
//    LogComponentEnable ("DcaTxop", LOG_LEVEL_ALL);
//    LogComponentEnable ("EdcaTxopN", LOG_LEVEL_ALL);
//    LogComponentEnable ("BlockAckManager", LOG_LEVEL_ALL);
//    LogComponentEnable ("WifiRemoteStationManager", LOG_LEVEL_ALL);
//    LogComponentEnable ("MacLow", LOG_LEVEL_ALL);
//    LogComponentEnable ("WifiPhy", LOG_LEVEL_ALL);
//    LogComponentEnable ("YansWifiPhy", LOG_LEVEL_ALL);
//    LogComponentEnable ("YansWifiChannel", LOG_LEVEL_ALL);
  
    
//    LogComponentEnable ("DcfManager", LOG_LEVEL_ALL);
//    LogComponentEnable ("DcfState", LOG_LEVEL_ALL);
//    LogComponentEnable ("WifiRemoteStationManager", LOG_LEVEL_ALL);
//    LogComponentEnable ("WifiMacQueue", LOG_LEVEL_ALL);

    GOOSE g;
    g.Configure (argc, argv);
    g.Run();

    return 0;
}
