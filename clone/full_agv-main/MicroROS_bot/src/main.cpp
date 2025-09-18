#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rcl/error_handling.h>
#include <rcl/node.h>
#include <geometry_msgs/msg/twist.h>
#include <nav_msgs/msg/odometry.h>
#include <geometry_msgs/msg/transform_stamped.h>
#include "motor.h"
#include "imu.h"
#include <string.h> 
#include <rmw_microros/rmw_microros.h>
#include "timer.h"
//#include "../.pio/libdeps/esp32doit-devkit-v1/micro_ros_platformio/platform_code/arduino/wifi/micro_ros_transport.h"
//#include <rmw_microroxrcedds_c/rmw_c_macros.h>
#define RMW_UXRCE_DEFAULT_DOMAIN_ID 5
#define USE_HSPI_PORT

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){ printf("Failed status on line %d: %d. Aborting.\n", __LINE__, (int) temp_rc); return 1; }}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){ printf("Failed status on line %d: %d. Continuing.\n", __LINE__, (int) temp_rc); }}

rcl_subscription_t cmd_vel_sub;
geometry_msgs__msg__Twist cmd_vel_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

rcl_publisher_t state_pub;  
geometry_msgs__msg__Twist state_msg;

rcl_publisher_t odom_pub;   
nav_msgs__msg__Odometry odom_msg;

rcl_publisher_t tf_pub;     
geometry_msgs__msg__TransformStamped tf_msg;

 rcl_init_options_t init_options;

void cmd_vel_callback(const void *msg_recv) {
    const geometry_msgs__msg__Twist *recieved_data = (const geometry_msgs__msg__Twist *) msg_recv;
    float linear_vel = recieved_data->linear.x;
    float angular_vel = recieved_data->angular.z;

    Serial.print("Received: ");
    Serial.print(linear_vel); 
    Serial.print(" / "); 
    Serial.println(angular_vel);


    
    Motor_Set_Velocity(linear_vel);
    Motor_Set_Angular(angular_vel);
    Motor_Set();
    // else {
    //     if(linear_vel != 0){
    //         Motor_Translation_Set(linear_vel);
    //     }
    
    //     if(angular_vel != 0){
    //         Motor_Rotation_Set(angular_vel);
    //     }
    // }

    // Motor_Translation_Set(linear_vel);

}

void setup() {
    Serial.begin(115200);
    //IPAddress agent_ip(192, 168, 42, 13);//nha
    IPAddress agent_ip(192, 168, 31, 109);
    //IPAddress agent_ip(192, 168, 31, 216);//t9
    size_t agent_port = 8888;
    // char ssid[] = "Phong 402";//nha
    // char psk[] = "09128288566";//nha

    char ssid[] = "Rang Dong Lab_2G";
    //char ssid[] = "KTDT-Lab";//t9
    //char psk[] = "ktdt12345678";//t9
    char psk[] = "ktdt123456";
    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
    //set_microros_serial_transports(Serial);
    delay(2000);
    allocator = rcl_get_default_allocator();
    init_options = rcl_get_zero_initialized_init_options();//
    rcl_init_options_init(&init_options, allocator);//
    rcl_init_options_set_domain_id(&init_options, 20);//
    rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);//
    //rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "robot_state_node", "", &support);

    rclc_publisher_init_default(
        &state_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "robot_state"
    );

    rclc_publisher_init_default(
        &odom_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        "/odom"
    );

    rclc_publisher_init_default(
        &tf_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TransformStamped),
        "/tf"
    );

    rclc_subscription_init_default(
        &cmd_vel_sub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "/cmd_vel"
    );

    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor, &cmd_vel_sub, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA);

    Motor_Init();
    Timer_Init();
}

void loop() {
    Motor_Translation_Controller();
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1));
    
    float current_velocity = Motor_Velocity_Get();
    float current_omega = Motor_Omega_Get();
    // Serial.print("Velocity:");
    // Serial.println(current_velocity);
    state_msg.linear.x = current_velocity;
    state_msg.angular.z = current_omega;
    rcl_publish(&state_pub, &state_msg, NULL);

    odom_msg.header.stamp.sec = millis() / 1000;
    odom_msg.header.stamp.nanosec = (millis() % 1000) * 1000000;

    odom_msg.header.frame_id.data = (char*) malloc(5 * sizeof(char));
    strcpy(odom_msg.header.frame_id.data, "odom");
    odom_msg.header.frame_id.size = strlen(odom_msg.header.frame_id.data);
    odom_msg.header.frame_id.capacity = odom_msg.header.frame_id.size + 1;

    odom_msg.child_frame_id.data = (char*) malloc(10 * sizeof(char));
    strcpy(odom_msg.child_frame_id.data, "base_link");
    odom_msg.child_frame_id.size = strlen(odom_msg.child_frame_id.data);
    odom_msg.child_frame_id.capacity = odom_msg.child_frame_id.size + 1;

    odom_msg.pose.pose.position.x += current_velocity * 0.005;  
    odom_msg.pose.pose.orientation.z = sin(current_omega / 2);
    odom_msg.pose.pose.orientation.w = cos(current_omega / 2);
    odom_msg.twist.twist.linear.x = current_velocity;
    odom_msg.twist.twist.angular.z = current_omega;

    if (rcl_publish(&odom_pub, &odom_msg, NULL) != RCL_RET_OK) {
        Serial.println("Failed to publish odometry");
    }

    tf_msg.header.stamp = odom_msg.header.stamp;

    tf_msg.header.frame_id.data = (char*) malloc(5 * sizeof(char));
    strcpy(tf_msg.header.frame_id.data, "odom");
    tf_msg.header.frame_id.size = strlen(tf_msg.header.frame_id.data);
    tf_msg.header.frame_id.capacity = tf_msg.header.frame_id.size + 1;

    tf_msg.child_frame_id.data = (char*) malloc(10 * sizeof(char));
    strcpy(tf_msg.child_frame_id.data, "base_link");
    tf_msg.child_frame_id.size = strlen(tf_msg.child_frame_id.data);
    tf_msg.child_frame_id.capacity = tf_msg.child_frame_id.size + 1;

    tf_msg.transform.translation.x = odom_msg.pose.pose.position.x;
    tf_msg.transform.translation.y = odom_msg.pose.pose.position.y;
    tf_msg.transform.rotation = odom_msg.pose.pose.orientation;

    if (rcl_publish(&tf_pub, &tf_msg, NULL) != RCL_RET_OK) {
        Serial.println("Failed to publish transform");
    }

    delay(1);
}
