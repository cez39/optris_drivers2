#ifndef _OPTRISIMAGER2_H_
#define _OPTRISIMAGER2_H_

#include "libirimager/IRDevice.h"
#include "libirimager/IRImager.h"
#include "libirimager/IRImagerClient.h"
#include <image_transport/image_transport.hpp>
#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include <thread>
#include <image_transport/image_transport.h>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/time_reference.hpp>
#include <std_srvs/srv/empty.hpp>
#include "optris_drivers2/msg/temperature.hpp"
#include "optris_drivers2/msg/flag.hpp"
#include "optris_drivers2/srv/auto_flag.hpp"
#include "optris_drivers2/srv/temperature_range.hpp"
#include "optris_drivers2/srv/radiation_parameters.hpp"

namespace optris_drivers2
{

/**
 * @class OptrisImager
 * @brief Node management class
 */
class OptrisImager : public rclcpp::Node, public evo::IRImagerClient
{
public:
  /**
   * Constructor
   * @param[in] dev UVC device instance
   * @param[in] params device parameters
   */
  OptrisImager(evo::IRDevice* dev, evo::IRDeviceParams params);

  /**
   * Destructor
   */
  virtual ~OptrisImager();

  /**
   * RCL Timer callback for image grabbing event
   */
  void timer_callback();

  /**
   * Raw frame callback (not needed in Linux implementations, since timer instance takes care of data acquisition => method is never called).
   * @param[in] data raw data
   * @param[in] size size of raw data
   */
  virtual void onRawFrame(unsigned char* data, int size) {(void)data; (void)size;};

  /**
   * Thermal frame callback
   * @param[in] image thermal image (for information about the format see IRImager class of libirimager)
   * @param[in] w width of image
   * @param[in] h height of image
   * @param[in] meta metadata of the image
   * @param[in] arg user arguments (passed to process method of IRImager class)
   */
  virtual void onThermalFrame(unsigned short* image, unsigned int w, unsigned int h, evo::IRFrameMetadata meta, void* arg);

  /**
   * Visible frame callback
   * @param[in] image visible image
   * @param[in] w width of image
   * @param[in] h height of image
   * @param[in] meta metadata of the image
   * @param[in] arg user arguments (passed to process method of IRImager class)
   */
  virtual void onVisibleFrame(unsigned char* image, unsigned int w, unsigned int h, evo::IRFrameMetadata meta, void* arg);

  /**
   * Flag state change callback
   * @param[in] flagstate new flag state
   * @param[in] arg user arguments (passed to process method of IRImager class)
   */
  virtual void onFlagStateChange(evo::EnumFlagState flagstate, void* arg);

  /**
   * Callback method for synchronizing data. This is the very last method to be called for each raw data set.
   * @param[in] arg user arguments (passed to process method of IRImager class)
   */
  virtual void onProcessExit(void* arg);

  /**
   * ROS service callback changing automatic flag control
   */
  void onAutoFlag(const std::shared_ptr<rmw_request_id_t> request_header,
                  const std::shared_ptr<optris_drivers2::srv::AutoFlag::Request> req,
                  const std::shared_ptr<optris_drivers2::srv::AutoFlag::Response> res);

  /**
   * ROS service callback forcing a flag cycle
   */
  void onForceFlag(const std::shared_ptr<rmw_request_id_t> request_header,
                   const std::shared_ptr<std_srvs::srv::Empty::Request> req,
                   const std::shared_ptr<std_srvs::srv::Empty::Response> res);

  /**
   * ROS service callback changing the temperature range
   */
  void onSetTemperatureRange(const std::shared_ptr<rmw_request_id_t> request_header,
                             const std::shared_ptr<optris_drivers2::srv::TemperatureRange::Request> req,
                             const std::shared_ptr<optris_drivers2::srv::TemperatureRange::Response> res);

  /**
   * Set radiation parameters for the thermal imager
   * @param camID Camera instance id from init to apply this function 
   * @param emissivity The emissivity of the observed object [0;1]
   * @param transmissivity The transmissivity of the observed object [0;1]
   * @param tAmbient The ambient temperature, setting invalid values (below -273.15 degrees) forces the library to take its own measurement values
   */
  void onSetRadiationParameters(const std::shared_ptr<rmw_request_id_t> request_header,
                                const std::shared_ptr<optris_drivers2::srv::RadiationParameters::Request> req,
                                const std::shared_ptr<optris_drivers2::srv::RadiationParameters::Response> res);

private:
  bool _run;
  std::thread* _th;
  evo::IRImager _imager;
  evo::IRDevice* _dev;
  unsigned int _img_cnt;
  unsigned char* _bufferRaw;
  sensor_msgs::msg::Image _thermal_image;
  sensor_msgs::msg::Image _visible_image;
  sensor_msgs::msg::TimeReference _device_timer;
  optris_drivers2::msg::Temperature _internal_temperature;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr _thermal_pub;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr _visible_pub;
  rclcpp::Publisher<optris_drivers2::msg::Temperature>::SharedPtr _temp_pub;
  rclcpp::Publisher<sensor_msgs::msg::TimeReference>::SharedPtr _timer_pub;
  rclcpp::Publisher<optris_drivers2::msg::Flag>::SharedPtr _flag_pub;
  rclcpp::Service<optris_drivers2::srv::AutoFlag>::SharedPtr _sAuto;
  rclcpp::Service<std_srvs::srv::Empty>::SharedPtr _sForce;
  rclcpp::Service<optris_drivers2::srv::TemperatureRange>::SharedPtr _sTemp;
  rclcpp::Service<optris_drivers2::srv::RadiationParameters>::SharedPtr _sSetRadiationParams;
};

} //namespace

#endif // _OPTRISIMAGER_H_


