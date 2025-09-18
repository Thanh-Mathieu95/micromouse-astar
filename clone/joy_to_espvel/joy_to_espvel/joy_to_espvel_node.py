#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import String
import json

class JoyToEspVel(Node):
    def __init__(self):
        super().__init__('joy_to_espvel')

        # Subscribed to /joy
        self.subscription = self.create_subscription(
            Joy,
            'joy',
            self.joy_callback,
            2)

        # Publish to /esp_vel
        self.publisher = self.create_publisher(String, 'esp_vel', 10)

        # Scale coefficients
        self.max_vx = 0.3  # m/s
        self.max_vy = 0.15  # m/s
        self.max_w  = 0.5  # rad/s
        self.max_x  = 200  # PWM hoặc tốc độ tùy bạn
        self.max_y  = 200
        self.max_k  = 80
	
        self.get_logger().info("🎮 JoyToEspVel node started")

    def joy_callback(self, msg: Joy):
        vx = self.max_vx * msg.axes[0]  # Lên/xuống (L-stick dọc)
        vy = self.max_vy * msg.axes[1]  # Trái/phải (L-stick ngang)

        # Điều khiển w bằng nút:
        if msg.buttons[1]: 
            w = self.max_w
        elif msg.buttons[3]:
            w = -self.max_w
        else:
            w = 0.0
        if msg.buttons[4]:  
            x = self.max_x
        else:
            x = 0;
        if msg.buttons[5]:  
            y = self.max_y
        else:
            y = 0;
        if msg.buttons[6]:  
            k = self.max_k
        else:
            k = 120
        doc = {"vx": vx, "vy": vy, "w": w,"x": x,"y": y,"k": k}
        msg_out = String()
        msg_out.data = json.dumps(doc)
        self.publisher.publish(msg_out)

        self.get_logger().info(f"📤 Sent vx={vx:.2f}, vy={vy:.2f}, w={w:.2f}, x={x:.2f}, y={y:.2f}, k={k:.2f}")

def main(args=None):
    rclpy.init(args=args)
    node = JoyToEspVel()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()

