/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2019
 *  Evocortex GmbH
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Nuremberg Institute of Technology
 *     Georg Simon Ohm nor the authors names may be used to endorse
 *     or promote products derived from this software without specific
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Stefan May
 *********************************************************************/

#include <sys/stat.h>
#include "OptrisImager.h"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  if(argc<2)
  {
    std::cerr << "usage: ros2  run optris_drivers2 optris_imager_node <xmlConfig>" << std::endl;
    return -1;
  }

  std::string xmlConfig = argv[1];

  // A specific configuration file for each imager device is needed (cf. config directory)
  struct stat s;
  if(stat(xmlConfig.c_str(), &s) != 0)
  {
    std::cerr << "usage: ros2  run optris_drivers2 " << argv[0] << " <xmlConfig>" << std::endl;
    std::cerr << " verify that <xmlConfig> exists" << std::endl;
    return -1;
  }
  else
  {
    std::cout << "Found configuration file " << argv[1] << std::endl;
  }
  

  // Read parameters from xml file
  evo::IRDeviceParams params;
  if(!evo::IRDeviceParamsReader::readXML(xmlConfig.c_str(), params))
    return -1;

  // Find valid device
  evo::IRDevice* dev = evo::IRDevice::IRCreateDevice(params);
  if(!dev)
  {
    std::cout << "Error: UVC device with serial " << params.serial << " could not be found" << std::endl;
    return -1;
  }

  // Give control to class instance
  rclcpp::Node::SharedPtr node = std::make_shared<optris_drivers2::OptrisImager>(dev, params);

  rclcpp::spin(node);
  rclcpp::shutdown();

  return 0;
}
