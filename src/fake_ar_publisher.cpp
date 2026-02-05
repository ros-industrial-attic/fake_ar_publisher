#include <chrono>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker.hpp>

#include <fake_ar_publisher/msg/ar_marker.hpp>

class FakeArPublisher
{
public:
  FakeArPublisher(const rclcpp::Node::SharedPtr& node)
    : node_(node)
    , ar_pub_(nullptr)
    , visual_pub_(nullptr)
    , marker_width_(0.08)
    , marker_thickness_(0.005)
  {
    // Declare node parameters
    node_->declare_parameter("x_pos", -0.6);
    node_->declare_parameter("y_pos", 0.2);
    node_->declare_parameter("z_pos", 0.5);
    node_->declare_parameter("camera_frame", "camera_frame");
    
    // Load node parameters
    pose_.orientation.w = 1.0; // facing straight up
    pose_.position.x = node_->get_parameter("x_pos").as_double();
    pose_.position.y = node_->get_parameter("y_pos").as_double();
    pose_.position.z = node_->get_parameter("z_pos").as_double();

    camera_frame_ = node_->get_parameter("camera_frame").as_string();
  
    // Construct the publishers
    ar_pub_ = node_->create_publisher<fake_ar_publisher::msg::ARMarker>("ar_pose_marker", rclcpp::QoS(1));
    visual_pub_ = node_->create_publisher<visualization_msgs::msg::Marker>("ar_pose_visual", rclcpp::QoS(1));
  
    // Prepare the parts of the visualization_msgs::msg::Marker that will remain constant
    marker_.ns = "ar_marker_visual";
    marker_.id = 0;
    marker_.type = visualization_msgs::msg::Marker::CUBE;
    marker_.action = visualization_msgs::msg::Marker::ADD;
    marker_.scale.x = marker_width_;
    marker_.scale.y = marker_width_;
    marker_.scale.z = marker_thickness_;
    marker_.color.a = 1.0;
    marker_.color.b = 1.0;
    return;
  }
  
  void pubCallback()
  {
    // Fill out and publish the ARMarker
    fake_ar_publisher::msg::ARMarker m;
    m.header.frame_id = camera_frame_;
    m.header.stamp = node_->now();
    m.pose.pose = pose_;

    ar_pub_->publish(m);

    // publish an RViz visualization. You'll need to add a "Marker" visualizer
    // in RVIZ AND define the "camera_frame" TF frame somewhere to see it.
    marker_.header.frame_id = m.header.frame_id;
    marker_.header.stamp = m.header.stamp;
    marker_.pose = m.pose.pose;
    marker_.pose.position.z -= marker_thickness_ / 2.0;

    visual_pub_->publish(marker_);
    return;
  }

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Publisher<fake_ar_publisher::msg::ARMarker>::SharedPtr ar_pub_;
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr visual_pub_;
  
  std::string camera_frame_;
  geometry_msgs::msg::Pose pose_;
  visualization_msgs::msg::Marker marker_;
  const double marker_width_;
  const double marker_thickness_;
};

int main(int argc, char** argv)
{
  using namespace std::chrono_literals;

  // Set up ROS.
  rclcpp::init(argc, argv);

  rclcpp::Node::SharedPtr node = std::make_shared<rclcpp::Node>("fake_ar_publisher");
  FakeArPublisher f(node);

  RCLCPP_INFO(node->get_logger(), "Starting simulated ARMarker publisher");
  auto timer = node->create_wall_timer(100ms, std::bind(&FakeArPublisher::pubCallback, &f));

  rclcpp::spin(node);
  
  return 0;
}
