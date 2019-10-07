#include <chrono>

#include <rclcpp/rclcpp.hpp>
#include <fake_ar_publisher/msg/ar_marker.hpp>
#include <visualization_msgs/msg/marker.hpp>

rclcpp::Node::SharedPtr node;
rclcpp::Publisher<fake_ar_publisher::msg::ARMarker>::SharedPtr ar_pub;
rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr visual_pub;

static std::string& camera_frame_name()
{
  static std::string camera_frame;
  return camera_frame;
}

// Singleton Instance of Object Position
static geometry_msgs::msg::Pose& pose()
{
  static geometry_msgs::msg::Pose pose;
  return pose;
}

// Given a marker w/ pose data, publish an RViz visualization
// You'll need to add a "Marker" visualizer in RVIZ AND define
// the "camera_frame" TF frame somewhere to see it.
static void pubVisualMarker(const fake_ar_publisher::msg::ARMarker& m)
{
  const double width = 0.08;
  const double thickness = 0.005;

  visualization_msgs::msg::Marker marker;
  marker.header.frame_id = m.header.frame_id;
  marker.header.stamp = node->now();
  marker.ns = "ar_marker_visual";
  marker.id = 0;
  marker.type = visualization_msgs::msg::Marker::CUBE;
  marker.action = visualization_msgs::msg::Marker::ADD;
  marker.pose = m.pose.pose;
  marker.pose.position.z -= thickness / 2.0;
  marker.scale.x = width;
  marker.scale.y = width;
  marker.scale.z = thickness;
  marker.color.a = 1.0;
  marker.color.b = 1.0;

  visual_pub->publish(marker);
}

void pubCallback()
{
  geometry_msgs::msg::Pose p = pose();
  fake_ar_publisher::msg::ARMarker m;
  m.header.frame_id = camera_frame_name();
  m.header.stamp = node->now();
  m.pose.pose = p;

  ar_pub->publish(m);

  pubVisualMarker(m); // visualize the marker
}

int main(int argc, char **argv)
{
  using namespace std::chrono_literals;

  // Set up ROS.
  rclcpp::init(argc, argv);

  node = std::make_shared<rclcpp::Node>("fake_ar_publisher");

  ar_pub = node->create_publisher<fake_ar_publisher::msg::ARMarker>("ar_pose_marker", rclcpp::QoS(1));
  visual_pub = node->create_publisher<visualization_msgs::msg::Marker>("ar_pose_visual", rclcpp::QoS(1));

  node->declare_parameter("x_pos", -0.6);
  node->declare_parameter("y_pos", 0.2);
  node->declare_parameter("z_pos", 0.5);
  node->declare_parameter("camera_frame", "camera_frame");

  // init pose
  pose().orientation.w = 1.0; // facing straight up
  pose().position.x = node->get_parameter("x_pos").as_double();
  pose().position.y = node->get_parameter("y_pos").as_double();
  pose().position.z = node->get_parameter("z_pos").as_double();

  camera_frame_name() = node->get_parameter("camera_frame").as_string();

  RCLCPP_INFO(node->get_logger(), "Starting simulated ARMarker publisher");
  auto timer = node->create_wall_timer(100ms, pubCallback);

  rclcpp::spin(node);
}
