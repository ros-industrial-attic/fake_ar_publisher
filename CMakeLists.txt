cmake_minimum_required(VERSION 3.5)
project(fake_ar_publisher)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

# Custom interface generation
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
    msg/ARMarker.msg
  DEPENDENCIES
    geometry_msgs
    std_msgs
)

# Build and install targets
add_executable(${PROJECT_NAME}_node src/fake_ar_publisher.cpp)
ament_target_dependencies(${PROJECT_NAME}_node rclcpp geometry_msgs visualization_msgs)
rosidl_target_interfaces(${PROJECT_NAME}_node ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS ${PROJECT_NAME}_node
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# Ament package settings
ament_export_dependencies(rclcpp geometry_msgs visualization_msgs)
ament_package()
