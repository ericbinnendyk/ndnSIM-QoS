/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

    /**
     * This scenario simulates a very simple network topology:
     *
     *
     *      +----------+     1Mbps      +--------+     1Mbps      +----------+
     *      | consumer | <------------> | router | <------------> | producer |
     *      +----------+         10ms   +--------+          10ms  +----------+
     *
     *
     * Consumer requests data from producer with frequency 10 interests per second
     * (interests contain constantly increasing sequence number).
     *
     * For every received interest, producer replies with a data packet, containing
     * 1024 bytes of virtual payload.
     *
     * To run scenario and see what is happening, use the following command:
     *
     *     NS_LOG=ndn.Consumer:ndn.Producer ./waf --run=ndn-simple
     */

    int
    main(int argc, char* argv[])
    {
       // setting default parameters for PointToPoint links and channels
       Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
       Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
       Config::SetDefault("ns3::QueueBase::MaxSize", StringValue("20p"));

       // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
       CommandLine cmd;
       cmd.Parse(argc, argv);

       // Creating nodes
       NodeContainer nodes;
       nodes.Create(5);

       // Connecting nodes using two links
       PointToPointHelper p2p;
       p2p.Install(nodes.Get(0), nodes.Get(3));
       p2p.Install(nodes.Get(1), nodes.Get(3));
       p2p.Install(nodes.Get(2), nodes.Get(3));
       p2p.Install(nodes.Get(3), nodes.Get(4));

       // Install NDN stack on all nodes
       ndn::StackHelper ndnHelper;
       ndnHelper.SetDefaultRoutes(true);
       ndnHelper.InstallAll();

       // Setup dynamic routing
       ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
       ndnGlobalRoutingHelper.Install(nodes);

       // Choosing forwarding strategy
       ndn::StrategyChoiceHelper::Install(nodes.Get(0), "/prefix/dscp", "/localhost/nfd/strategy/multicast");
       ndn::StrategyChoiceHelper::Install(nodes.Get(1), "/prefix/dscp", "/localhost/nfd/strategy/multicast");
       ndn::StrategyChoiceHelper::Install(nodes.Get(2), "/prefix/dscp", "/localhost/nfd/strategy/multicast");
       ndn::StrategyChoiceHelper::Install(nodes.Get(3), "/prefix/dscp", "/localhost/nfd/strategy/qos");
       ndn::StrategyChoiceHelper::Install(nodes.Get(4), "/prefix/dscp", "/localhost/nfd/strategy/multicast");


       // Installing applications

       // Consumer
       ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
       // Consumer will request /prefix/0, /prefix/1, ...
       consumerHelper.SetPrefix("/prefix/dscp/1");
       consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
       //consumerHelper.Install(nodes.Get(0));                        // first node
       auto apps = consumerHelper.Install(nodes.Get(0));
       apps.Stop(Seconds(0.5)); // stop the consumer app at 10 seconds mark

       // Consumer will request /prefix/0, /prefix/1, ...
       consumerHelper.SetPrefix("/prefix/dscp/21");
       consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
       //consumerHelper.Install(nodes.Get(0));                        // first node
       apps = consumerHelper.Install(nodes.Get(1));
       apps.Stop(Seconds(0.5)); // stop the consumer app at 10 seconds mark

       // Consumer will request /prefix/0, /prefix/1, ...
       consumerHelper.SetPrefix("/prefix/dscp/60");
       consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
       //consumerHelper.Install(nodes.Get(0));                        // first node
       apps = consumerHelper.Install(nodes.Get(2));
       apps.Stop(Seconds(0.5)); // stop the consumer app at 10 seconds mark

       ndn::AppHelper tokenHelper("ns3::ndn::TokenBucket");
       // Consumer will request /prefix/0, /prefix/1, ...
       tokenHelper.SetAttribute("FillRate1", StringValue("75")); // 10 interests a second
       tokenHelper.SetAttribute("Capacity1", StringValue("80")); // 10 interests a second
       tokenHelper.SetAttribute("FillRate2", StringValue("75")); // 10 interests a second
       tokenHelper.SetAttribute("Capacity2", StringValue("80")); // 10 interests a second
       tokenHelper.SetAttribute("FillRate3", StringValue("75")); // 10 interests a second
       tokenHelper.SetAttribute("Capacity3", StringValue("80")); // 10 interests a second
       //consumerHelper.Install(nodes.Get(0));                        // first node
       apps = tokenHelper.Install(nodes.Get(3));
       tokenHelper.SetAttribute("FillRate1", StringValue("105")); // 10 interests a second
       tokenHelper.SetAttribute("Capacity1", StringValue("120")); // 10 interests

       // Producer
       ndn::AppHelper producerHelper("ns3::ndn::Producer");
       // Producer will reply to all requests starting with /prefix
       producerHelper.SetPrefix("/prefix/dscp/");
       producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
       producerHelper.Install(nodes.Get(4)); // last node

       ndn::GlobalRoutingHelper::CalculateAllPossibleRoutes();

       Simulator::Stop(Seconds(1.0));
       Simulator::Run();
       Simulator::Destroy();

       return 0;
   }

} // namespace ns3

    int
main(int argc, char* argv[])
{
    return ns3::main(argc, argv);
}
