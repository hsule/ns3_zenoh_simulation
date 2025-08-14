#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/tap-bridge-module.h"

#include <fstream>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TapCsmaVirtualMachineExample");

int main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // 實時模擬 + 啟用檢查碼
    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));
    GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));

    // 建立 3 個 ghost node
    NodeContainer n;
    n.Create(3); // r1, r2, r3

    // --- LAN 10.0.1.* (n0 <-> n1) ---
    CsmaHelper csma12;
    csma12.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma12.SetChannelAttribute("Delay", TimeValue(MilliSeconds(1)));
    NetDeviceContainer d12 = csma12.Install(NodeContainer(n.Get(0), n.Get(1)));

    // --- LAN 10.0.2.* (n0 <-> n2) ---
    CsmaHelper csma13;
    csma13.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma13.SetChannelAttribute("Delay", TimeValue(MilliSeconds(5)));
    NetDeviceContainer d13 = csma13.Install(NodeContainer(n.Get(0), n.Get(2)));

    // --- LAN 10.0.3.* (n1 <-> n2) ---
    CsmaHelper csma23;
    csma23.SetChannelAttribute("DataRate", StringValue("50Mbps"));
    csma23.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    NetDeviceContainer d23 = csma23.Install(NodeContainer(n.Get(1), n.Get(2)));

    // 設定 TapBridge
    TapBridgeHelper tb;
    tb.SetAttribute("Mode", StringValue("UseBridge"));

    // r1
    tb.SetAttribute("DeviceName", StringValue("tap_1_0")); tb.Install(n.Get(0), d12.Get(0));
    tb.SetAttribute("DeviceName", StringValue("tap_1_1")); tb.Install(n.Get(0), d13.Get(0));
    // r2
    tb.SetAttribute("DeviceName", StringValue("tap_2_0")); tb.Install(n.Get(1), d12.Get(1));
    tb.SetAttribute("DeviceName", StringValue("tap_2_1")); tb.Install(n.Get(1), d23.Get(0));
    // r3
    tb.SetAttribute("DeviceName", StringValue("tap_3_0")); tb.Install(n.Get(2), d13.Get(1));
    tb.SetAttribute("DeviceName", StringValue("tap_3_1")); tb.Install(n.Get(2), d23.Get(1));

    // 模擬 10 分鐘
    Simulator::Stop(Seconds(600.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
