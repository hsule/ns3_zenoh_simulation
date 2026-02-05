#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/tap-bridge-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NewYorkTopology");

int main(int argc, char* argv[]) {
  CommandLine cmd(__FILE__);
  cmd.Parse(argc, argv);

  GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));

  // SNDlib newyork: 16 nodes (N1..N16 -> 0..15), 49 links
  // + 2 added nodes: pub (16), sub (17)
  NodeContainer n;
  n.Create(18);

  // All links: 100Mbps, 1ms delay
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("3Mbps"));
  csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(1)));

  // --- 49 CSMA links ---
  // L1:  N1(0)  <-> N2(1)
  NetDeviceContainer dL1  = csma.Install(NodeContainer(n.Get(0),  n.Get(1)));
  // L2:  N1(0)  <-> N5(4)
  NetDeviceContainer dL2  = csma.Install(NodeContainer(n.Get(0),  n.Get(4)));
  // L3:  N1(0)  <-> N6(5)
  NetDeviceContainer dL3  = csma.Install(NodeContainer(n.Get(0),  n.Get(5)));
  // L4:  N1(0)  <-> N7(6)
  NetDeviceContainer dL4  = csma.Install(NodeContainer(n.Get(0),  n.Get(6)));
  // L5:  N1(0)  <-> N8(7)
  NetDeviceContainer dL5  = csma.Install(NodeContainer(n.Get(0),  n.Get(7)));
  // L6:  N1(0)  <-> N12(11)
  NetDeviceContainer dL6  = csma.Install(NodeContainer(n.Get(0),  n.Get(11)));
  // L7:  N1(0)  <-> N13(12)
  NetDeviceContainer dL7  = csma.Install(NodeContainer(n.Get(0),  n.Get(12)));
  // L8:  N1(0)  <-> N15(14)
  NetDeviceContainer dL8  = csma.Install(NodeContainer(n.Get(0),  n.Get(14)));
  // L9:  N2(1)  <-> N5(4)
  NetDeviceContainer dL9  = csma.Install(NodeContainer(n.Get(1),  n.Get(4)));
  // L10: N2(1)  <-> N6(5)
  NetDeviceContainer dL10 = csma.Install(NodeContainer(n.Get(1),  n.Get(5)));
  // L11: N2(1)  <-> N12(11)
  NetDeviceContainer dL11 = csma.Install(NodeContainer(n.Get(1),  n.Get(11)));
  // L12: N2(1)  <-> N13(12)
  NetDeviceContainer dL12 = csma.Install(NodeContainer(n.Get(1),  n.Get(12)));
  // L13: N2(1)  <-> N15(14)
  NetDeviceContainer dL13 = csma.Install(NodeContainer(n.Get(1),  n.Get(14)));
  // L14: N3(2)  <-> N4(3)
  NetDeviceContainer dL14 = csma.Install(NodeContainer(n.Get(2),  n.Get(3)));
  // L15: N3(2)  <-> N6(5)
  NetDeviceContainer dL15 = csma.Install(NodeContainer(n.Get(2),  n.Get(5)));
  // L16: N3(2)  <-> N7(6)
  NetDeviceContainer dL16 = csma.Install(NodeContainer(n.Get(2),  n.Get(6)));
  // L17: N3(2)  <-> N8(7)
  NetDeviceContainer dL17 = csma.Install(NodeContainer(n.Get(2),  n.Get(7)));
  // L18: N3(2)  <-> N11(10)
  NetDeviceContainer dL18 = csma.Install(NodeContainer(n.Get(2),  n.Get(10)));
  // L19: N4(3)  <-> N7(6)
  NetDeviceContainer dL19 = csma.Install(NodeContainer(n.Get(3),  n.Get(6)));
  // L20: N4(3)  <-> N8(7)
  NetDeviceContainer dL20 = csma.Install(NodeContainer(n.Get(3),  n.Get(7)));
  // L21: N4(3)  <-> N9(8)
  NetDeviceContainer dL21 = csma.Install(NodeContainer(n.Get(3),  n.Get(8)));
  // L22: N4(3)  <-> N10(9)
  NetDeviceContainer dL22 = csma.Install(NodeContainer(n.Get(3),  n.Get(9)));
  // L23: N4(3)  <-> N11(10)
  NetDeviceContainer dL23 = csma.Install(NodeContainer(n.Get(3),  n.Get(10)));
  // L24: N5(4)  <-> N12(11)
  NetDeviceContainer dL24 = csma.Install(NodeContainer(n.Get(4),  n.Get(11)));
  // L25: N5(4)  <-> N13(12)
  NetDeviceContainer dL25 = csma.Install(NodeContainer(n.Get(4),  n.Get(12)));
  // L26: N5(4)  <-> N15(14)
  NetDeviceContainer dL26 = csma.Install(NodeContainer(n.Get(4),  n.Get(14)));
  // L27: N6(5)  <-> N7(6)
  NetDeviceContainer dL27 = csma.Install(NodeContainer(n.Get(5),  n.Get(6)));
  // L28: N6(5)  <-> N8(7)
  NetDeviceContainer dL28 = csma.Install(NodeContainer(n.Get(5),  n.Get(7)));
  // L29: N7(6)  <-> N8(7)
  NetDeviceContainer dL29 = csma.Install(NodeContainer(n.Get(6),  n.Get(7)));
  // L30: N7(6)  <-> N9(8)
  NetDeviceContainer dL30 = csma.Install(NodeContainer(n.Get(6),  n.Get(8)));
  // L31: N7(6)  <-> N10(9)
  NetDeviceContainer dL31 = csma.Install(NodeContainer(n.Get(6),  n.Get(9)));
  // L32: N7(6)  <-> N12(11)
  NetDeviceContainer dL32 = csma.Install(NodeContainer(n.Get(6),  n.Get(11)));
  // L33: N7(6)  <-> N13(12)
  NetDeviceContainer dL33 = csma.Install(NodeContainer(n.Get(6),  n.Get(12)));
  // L34: N7(6)  <-> N14(13)
  NetDeviceContainer dL34 = csma.Install(NodeContainer(n.Get(6),  n.Get(13)));
  // L35: N7(6)  <-> N15(14)
  NetDeviceContainer dL35 = csma.Install(NodeContainer(n.Get(6),  n.Get(14)));
  // L36: N9(8)  <-> N10(9)
  NetDeviceContainer dL36 = csma.Install(NodeContainer(n.Get(8),  n.Get(9)));
  // L37: N9(8)  <-> N11(10)
  NetDeviceContainer dL37 = csma.Install(NodeContainer(n.Get(8),  n.Get(10)));
  // L38: N9(8)  <-> N14(13)
  NetDeviceContainer dL38 = csma.Install(NodeContainer(n.Get(8),  n.Get(13)));
  // L39: N9(8)  <-> N15(14)
  NetDeviceContainer dL39 = csma.Install(NodeContainer(n.Get(8),  n.Get(14)));
  // L40: N9(8)  <-> N16(15)
  NetDeviceContainer dL40 = csma.Install(NodeContainer(n.Get(8),  n.Get(15)));
  // L41: N10(9) <-> N11(10)
  NetDeviceContainer dL41 = csma.Install(NodeContainer(n.Get(9),  n.Get(10)));
  // L42: N10(9) <-> N14(13)
  NetDeviceContainer dL42 = csma.Install(NodeContainer(n.Get(9),  n.Get(13)));
  // L43: N10(9) <-> N15(14)
  NetDeviceContainer dL43 = csma.Install(NodeContainer(n.Get(9),  n.Get(14)));
  // L44: N12(11)<-> N13(12)
  NetDeviceContainer dL44 = csma.Install(NodeContainer(n.Get(11), n.Get(12)));
  // L45: N12(11)<-> N15(14)
  NetDeviceContainer dL45 = csma.Install(NodeContainer(n.Get(11), n.Get(14)));
  // L46: N13(12)<-> N14(13)
  NetDeviceContainer dL46 = csma.Install(NodeContainer(n.Get(12), n.Get(13)));
  // L47: N13(12)<-> N15(14)
  NetDeviceContainer dL47 = csma.Install(NodeContainer(n.Get(12), n.Get(14)));
  // L48: N14(13)<-> N15(14)
  NetDeviceContainer dL48 = csma.Install(NodeContainer(n.Get(13), n.Get(14)));
  // L49: N14(13)<-> N16(15)
  NetDeviceContainer dL49 = csma.Install(NodeContainer(n.Get(13), n.Get(15)));

  // --- Added links (not from SNDlib) ---
  CsmaHelper csma100;
  csma100.SetChannelAttribute("DataRate", StringValue("100Mbps"));
  csma100.SetChannelAttribute("Delay", TimeValue(MilliSeconds(1)));
  // L50: pub(16) <-> N14(13)
  NetDeviceContainer dL50 = csma100.Install(NodeContainer(n.Get(16), n.Get(13)));
  // L51: sub(17) <-> N7(6)
  NetDeviceContainer dL51 = csma100.Install(NodeContainer(n.Get(17), n.Get(6)));

  // --- TAP bridges ---
  TapBridgeHelper tb;
  tb.SetAttribute("Mode", StringValue("UseBridge"));

  // Node 0 (N1) — 8 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_0_0"));  tb.Install(n.Get(0),  dL1.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_1"));  tb.Install(n.Get(0),  dL2.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_2"));  tb.Install(n.Get(0),  dL3.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_3"));  tb.Install(n.Get(0),  dL4.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_4"));  tb.Install(n.Get(0),  dL5.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_5"));  tb.Install(n.Get(0),  dL6.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_6"));  tb.Install(n.Get(0),  dL7.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_0_7"));  tb.Install(n.Get(0),  dL8.Get(0));

  // Node 1 (N2) — 6 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_1_0"));  tb.Install(n.Get(1),  dL1.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_1_1"));  tb.Install(n.Get(1),  dL9.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_1_2"));  tb.Install(n.Get(1),  dL10.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_1_3"));  tb.Install(n.Get(1),  dL11.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_1_4"));  tb.Install(n.Get(1),  dL12.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_1_5"));  tb.Install(n.Get(1),  dL13.Get(0));

  // Node 2 (N3) — 5 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_2_0"));  tb.Install(n.Get(2),  dL14.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_2_1"));  tb.Install(n.Get(2),  dL15.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_2_2"));  tb.Install(n.Get(2),  dL16.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_2_3"));  tb.Install(n.Get(2),  dL17.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_2_4"));  tb.Install(n.Get(2),  dL18.Get(0));

  // Node 3 (N4) — 6 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_3_0"));  tb.Install(n.Get(3),  dL14.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_3_1"));  tb.Install(n.Get(3),  dL19.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_3_2"));  tb.Install(n.Get(3),  dL20.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_3_3"));  tb.Install(n.Get(3),  dL21.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_3_4"));  tb.Install(n.Get(3),  dL22.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_3_5"));  tb.Install(n.Get(3),  dL23.Get(0));

  // Node 4 (N5) — 5 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_4_0"));  tb.Install(n.Get(4),  dL2.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_4_1"));  tb.Install(n.Get(4),  dL9.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_4_2"));  tb.Install(n.Get(4),  dL24.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_4_3"));  tb.Install(n.Get(4),  dL25.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_4_4"));  tb.Install(n.Get(4),  dL26.Get(0));

  // Node 5 (N6) — 5 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_5_0"));  tb.Install(n.Get(5),  dL3.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_5_1"));  tb.Install(n.Get(5),  dL10.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_5_2"));  tb.Install(n.Get(5),  dL15.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_5_3"));  tb.Install(n.Get(5),  dL27.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_5_4"));  tb.Install(n.Get(5),  dL28.Get(0));

  // Node 6 (N7) — 11 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_6_0"));  tb.Install(n.Get(6),  dL4.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_6_1"));  tb.Install(n.Get(6),  dL16.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_6_2"));  tb.Install(n.Get(6),  dL19.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_6_3"));  tb.Install(n.Get(6),  dL27.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_6_4"));  tb.Install(n.Get(6),  dL29.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_5"));  tb.Install(n.Get(6),  dL30.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_6"));  tb.Install(n.Get(6),  dL31.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_7"));  tb.Install(n.Get(6),  dL32.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_8"));  tb.Install(n.Get(6),  dL33.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_9"));  tb.Install(n.Get(6),  dL34.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_10")); tb.Install(n.Get(6),  dL35.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_6_11")); tb.Install(n.Get(6),  dL51.Get(1)); // added: to sub

  // Node 7 (N8) — 5 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_7_0"));  tb.Install(n.Get(7),  dL5.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_7_1"));  tb.Install(n.Get(7),  dL17.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_7_2"));  tb.Install(n.Get(7),  dL20.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_7_3"));  tb.Install(n.Get(7),  dL28.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_7_4"));  tb.Install(n.Get(7),  dL29.Get(1));

  // Node 8 (N9) — 7 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_8_0"));  tb.Install(n.Get(8),  dL21.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_8_1"));  tb.Install(n.Get(8),  dL30.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_8_2"));  tb.Install(n.Get(8),  dL36.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_8_3"));  tb.Install(n.Get(8),  dL37.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_8_4"));  tb.Install(n.Get(8),  dL38.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_8_5"));  tb.Install(n.Get(8),  dL39.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_8_6"));  tb.Install(n.Get(8),  dL40.Get(0));

  // Node 9 (N10) — 6 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_9_0"));  tb.Install(n.Get(9),  dL22.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_9_1"));  tb.Install(n.Get(9),  dL31.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_9_2"));  tb.Install(n.Get(9),  dL36.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_9_3"));  tb.Install(n.Get(9),  dL41.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_9_4"));  tb.Install(n.Get(9),  dL42.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_9_5"));  tb.Install(n.Get(9),  dL43.Get(0));

  // Node 10 (N11) — 4 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_10_0")); tb.Install(n.Get(10), dL18.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_10_1")); tb.Install(n.Get(10), dL23.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_10_2")); tb.Install(n.Get(10), dL37.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_10_3")); tb.Install(n.Get(10), dL41.Get(1));

  // Node 11 (N12) — 6 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_11_0")); tb.Install(n.Get(11), dL6.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_11_1")); tb.Install(n.Get(11), dL11.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_11_2")); tb.Install(n.Get(11), dL24.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_11_3")); tb.Install(n.Get(11), dL32.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_11_4")); tb.Install(n.Get(11), dL44.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_11_5")); tb.Install(n.Get(11), dL45.Get(0));

  // Node 12 (N13) — 7 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_12_0")); tb.Install(n.Get(12), dL7.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_12_1")); tb.Install(n.Get(12), dL12.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_12_2")); tb.Install(n.Get(12), dL25.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_12_3")); tb.Install(n.Get(12), dL33.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_12_4")); tb.Install(n.Get(12), dL44.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_12_5")); tb.Install(n.Get(12), dL46.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_12_6")); tb.Install(n.Get(12), dL47.Get(0));

  // Node 13 (N14) — 6 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_13_0")); tb.Install(n.Get(13), dL34.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_13_1")); tb.Install(n.Get(13), dL38.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_13_2")); tb.Install(n.Get(13), dL42.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_13_3")); tb.Install(n.Get(13), dL46.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_13_4")); tb.Install(n.Get(13), dL48.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_13_5")); tb.Install(n.Get(13), dL49.Get(0));
  tb.SetAttribute("DeviceName", StringValue("tap_13_6")); tb.Install(n.Get(13), dL50.Get(1)); // added: to pub

  // Node 14 (N15) — 9 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_14_0")); tb.Install(n.Get(14), dL8.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_1")); tb.Install(n.Get(14), dL13.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_2")); tb.Install(n.Get(14), dL26.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_3")); tb.Install(n.Get(14), dL35.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_4")); tb.Install(n.Get(14), dL39.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_5")); tb.Install(n.Get(14), dL43.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_6")); tb.Install(n.Get(14), dL45.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_7")); tb.Install(n.Get(14), dL47.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_14_8")); tb.Install(n.Get(14), dL48.Get(1));

  // Node 15 (N16) — 2 interfaces
  tb.SetAttribute("DeviceName", StringValue("tap_15_0")); tb.Install(n.Get(15), dL40.Get(1));
  tb.SetAttribute("DeviceName", StringValue("tap_15_1")); tb.Install(n.Get(15), dL49.Get(1));

  // --- Added nodes (not from SNDlib) ---
  // Node 16 (pub) — 1 interface
  tb.SetAttribute("DeviceName", StringValue("tap_16_0")); tb.Install(n.Get(16), dL50.Get(0));

  // Node 17 (sub) — 1 interface
  tb.SetAttribute("DeviceName", StringValue("tap_17_0")); tb.Install(n.Get(17), dL51.Get(0));

  Simulator::Stop(Seconds(600.0));
  Simulator::Run();
  Simulator::Destroy();
  return 0;
}
