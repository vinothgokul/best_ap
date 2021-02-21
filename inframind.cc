#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"


using namespace ns3;

int 
main (int argc, char *argv[])
{
  bool verbose = false;
  uint32_t nCsma = 5;

  if (verbose)
    {
      LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;


  NodeContainer csmaNodes;
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);
         
  NodeContainer stationNode1;
  stationNode1.Create (1);
  NodeContainer stationNode2;
  stationNode2.Create (2);
  NodeContainer stationNode3;
  stationNode3.Create (2);
  NodeContainer stationNode4;
  stationNode4.Create (2);
  NodeContainer stationNode5;
  stationNode5.Create (2);

  NodeContainer wifiApNode1 = csmaNodes.Get (1);
  NodeContainer wifiApNode2 = csmaNodes.Get (2);
  NodeContainer wifiApNode3 = csmaNodes.Get (3);
  NodeContainer wifiApNode4 = csmaNodes.Get (4);
  
  
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());
  
  WifiHelper wifi1;
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");
  
  
  WifiMacHelper mac1;
  
  Ssid ssid1 = Ssid ("ns-3-ssid");

  mac1.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid1),
                "ActiveProbing", BooleanValue (true));

  NetDeviceContainer staDevices1;
  phy.SetChannel (channel.Create ());
  staDevices1 = wifi1.Install (phy, mac1, stationNode1);
  
  NetDeviceContainer staDevices2;
  staDevices2 = wifi1.Install (phy, mac1, stationNode2);
  
  NetDeviceContainer staDevices3;
  staDevices3 = wifi1.Install (phy, mac1, stationNode3);
  
  NetDeviceContainer staDevices4;
  staDevices4 = wifi1.Install (phy, mac1, stationNode4);
  
  NetDeviceContainer staDevices5;
  staDevices5 = wifi1.Install (phy, mac1, stationNode5);

  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));

 
                         
  NetDeviceContainer apDevices1;
  apDevices1 = wifi1.Install (phy, mac1, wifiApNode1);

  NetDeviceContainer apDevices2;
  apDevices2 = wifi1.Install (phy, mac1, wifiApNode2);
  
  NetDeviceContainer apDevices3;
  apDevices3 = wifi1.Install (phy, mac1, wifiApNode3);
  
  NetDeviceContainer apDevices4;
  apDevices4 = wifi1.Install (phy, mac1, wifiApNode4);

  InternetStackHelper stack;
  
  stack.Install (csmaNodes);
  stack.Install (stationNode1);
  stack.Install (stationNode2);
  stack.Install (stationNode3);
  stack.Install (stationNode4);
  stack.Install (stationNode5);
  
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (staDevices1);
  address.Assign (staDevices2);
  address.Assign (staDevices3);
  address.Assign (staDevices4);
  address.Assign (staDevices5);
  address.Assign (csmaDevices);
  

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

	mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
	
  	mobility.Install (stationNode1);
	
  

        Ptr<ConstantVelocityMobilityModel> mobile;
	mobile = (stationNode1.Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(0.0,115.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	//mobile = (stationNode1.Get(1))->GetObject<ConstantVelocityMobilityModel>();
	//mobile->SetPosition(Vector(0.0,110.0,0.0));
	//mobile->SetVelocity(Vector(15.0,0.0,0.0));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (csmaNodes.Get(0));
  mobility.Install (wifiApNode1);
  mobility.Install (wifiApNode2);
  mobility.Install (wifiApNode3);
  mobility.Install (wifiApNode4);
  mobility.Install (stationNode2);
  mobility.Install (stationNode3);
  mobility.Install (stationNode4);
  mobility.Install (stationNode5);
  
  //mobility.Install (p2pNodes);

 	UdpServerHelper srv(9);  
  	ApplicationContainer srv_apps = srv.Install (stationNode1.Get(0)); //server = UDP recv 
  	srv_apps.Start (Seconds (0.5));
  	srv_apps.Stop (Seconds (15.0));

  	UdpClientHelper client(csmaInterfaces.GetAddress (0), 9); // dest: IP,port
  	client.SetAttribute("MaxPackets",UintegerValue (64707202));
  	client.SetAttribute("Interval",TimeValue (Time ("0.01"))); 
  	client.SetAttribute("PacketSize",UintegerValue (1450));    
  	ApplicationContainer cln_apps = client.Install (csmaNodes.Get(0)); //cli = UDP send
  	cln_apps.Start (Seconds (0.5));
  	cln_apps.Stop (Seconds (60.0));
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  	AnimationInterface anim("inframind.xml");
        anim.UpdateNodeColor(wifiApNode1.Get(0),255,255,0);
	anim.UpdateNodeColor(wifiApNode2.Get(0),255,255,0);
	anim.UpdateNodeColor(wifiApNode3.Get(0),255,255,0);
	anim.UpdateNodeColor(wifiApNode4.Get(0),255,255,0);
	//anim.UpdateNodeColor(apNode.Get(2),255,255,0);
	//anim.UpdateNodeColor(p2pNodes.Get (1),0,255,0);
	//anim.UpdateNodeColor(p2pNodes.Get (0),0,255,0);
	
	anim.UpdateNodeColor(stationNode1.Get(0),0,255,255);
	//anim.UpdateNodeColor(stationNode1.Get(1),0,255,255);
	anim.UpdateNodeColor(stationNode2.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode2.Get(1),0,255,255);
	anim.UpdateNodeColor(stationNode3.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode3.Get(1),0,255,255);
	anim.UpdateNodeColor(stationNode4.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode4.Get(1),0,255,255);
	anim.UpdateNodeColor(stationNode5.Get(0),0,255,255);
	anim.UpdateNodeColor(stationNode5.Get(1),0,255,255);
	anim.UpdateNodeDescription(wifiApNode1.Get(0),"Ap-1");
	anim.UpdateNodeDescription(wifiApNode2.Get(0),"Ap-2");
	anim.UpdateNodeDescription(wifiApNode3.Get(0),"Ap-3");
	anim.UpdateNodeDescription(wifiApNode4.Get(0),"Ap-4");
        anim.SetConstantPosition(wifiApNode1.Get(0),10.0,100.0);
	anim.SetConstantPosition(wifiApNode2.Get(0),100.0,100.0);
	anim.SetConstantPosition(wifiApNode3.Get(0),200.0,100.0);
	anim.SetConstantPosition(wifiApNode4.Get(0),300.0,100.0);
	//anim.UpdateNodeDescription(apNode.Get(2),"Ap-3");
	//anim.SetConstantPosition(apNode.Get(2),200.0,100.0);
	
        anim.UpdateNodeDescription(csmaNodes.Get (0),"Server Node");
	anim.SetConstantPosition(csmaNodes.Get (0),150.0,50.0);

	//anim.UpdateNodeDescription(p2pNodes.Get (0),"Server Node");
	//anim.SetConstantPosition(p2pNodes.Get (0),100.0,0.0);
	
	anim.UpdateNodeDescription(stationNode3.Get(0),"S1");
	anim.SetConstantPosition(stationNode3.Get(0),90.0,130.0);
	anim.UpdateNodeDescription(stationNode3.Get(1),"S2");
	anim.SetConstantPosition(stationNode3.Get(1),110.0,130.0);
	
	anim.UpdateNodeDescription(stationNode4.Get(0),"S1");
	anim.SetConstantPosition(stationNode4.Get(0),190.0,130.0);
	anim.UpdateNodeDescription(stationNode4.Get(1),"S2");
	anim.SetConstantPosition(stationNode4.Get(1),210.0,130.0);
	
	anim.UpdateNodeDescription(stationNode5.Get(0),"S1");
	anim.SetConstantPosition(stationNode5.Get(0),290.0,130.0);
	anim.UpdateNodeDescription(stationNode5.Get(1),"S2");
	anim.SetConstantPosition(stationNode5.Get(1),310.0,130.0);
	
	anim.UpdateNodeDescription(stationNode2.Get(0),"S1");
	anim.SetConstantPosition(stationNode2.Get(0),0.0,130.0);
	anim.UpdateNodeDescription(stationNode2.Get(1),"S2");
	anim.SetConstantPosition(stationNode2.Get(1),20.0,130.0);
	
	anim.UpdateNodeDescription(stationNode1.Get(0),"Mobile");

  Simulator::Stop(Seconds(30.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
