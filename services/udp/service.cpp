// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <service>
#include <net/interfaces>
using namespace net;

void Service::start()
{
  extern void create_network_device(int N, const char* ip);
  create_network_device(0, "10.0.0.1/24");

  auto& inet = Interfaces::get(0);
  inet.network_config({  10,  0,  0, 55 },   // IP
                      { 255, 255, 0,  0 },   // Netmask
                      {  10,  0,  0,  1 } ); // Gateway
  const uint16_t port = 4242;
  auto& sock = inet.udp().bind(port);

  inet.add_addr(ip6::Addr{"fe80::4242"});
  auto& sock6 = inet.udp().bind6(port);

  sock.on_read(
    [&sock] (
      UDP::addr_t addr, UDP::port_t port,
      const char* data, size_t len)
  {
    CHECK(1, "UDP data from %s:%u -> %.*s",
         addr.to_string().c_str(), port, std::min((int) len, 16), data);
    // send the same thing right back!
    printf("Got %lu bytes\n", len);
    const size_t maxlen = len;
    while (len >= 1472)
    {
      printf("Sending %lu bytes\n", 1472);
      sock.sendto(addr, port, data, 1472);
      data += 1472;
      len  -= 1472;
    }
    if (len > 0) {
      printf("Sending %lu bytes\n", len);
      sock.sendto(addr, port, data, len);
    }
    if (maxlen == 9216) os::shutdown();
  });

  sock6.on_read(
    [&sock6] (
      UDP::addr_t addr, UDP::port_t port,
      const char* data, size_t len)
  {
    CHECK(1, "UDP data from %s:%u -> %.*s",
         addr.to_string().c_str(), port, std::min((int) len, 16), data);
    // send the same thing right back!
    printf("Got %lu bytes\n", len);
    if (len > 0) {
      sock6.sendto(addr, port, data, len);
    }
  });


  INFO("UDP test service", "Listening on port %d\n", port);
}