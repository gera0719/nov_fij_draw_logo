#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "turtlesim/srv/set_pen.hpp"
#include "turtlesim/srv/teleport_absolute.hpp"
#include <chrono>
#include <cmath>

class DrawLogo : public rclcpp::Node{
    private:

    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr pen_on_off_c;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr velocity_pub;
    rclcpp::TimerBase::SharedPtr timer_;

    

    void pen_on_off(bool on){
        auto on_off = std::make_shared<turtlesim::srv::SetPen::Request>();
        on_off->b = 0;
        on_off->g = 0;
        on_off->r = 0;
        on_off->width = 8;
        on_off->off = !on;
        pen_on_off_c->async_send_request(on_off, [this](rclcpp::Client<turtlesim::srv::SetPen>::SharedFuture)
        {
            RCLCPP_INFO(this->get_logger(), "pen setting has been changed");
        });
    }

    void move_turtle(double distance, double speed = 1.0){
        geometry_msgs::msg::Twist msg;
        msg.linear.x = speed;
        rclcpp::Time start_time = this->now();
        double duration = distance / std::abs(speed);
        while ((this->now() - start_time).seconds() < duration) {
            velocity_pub->publish(msg);
            rclcpp::sleep_for(std::chrono::milliseconds(100));
        }
        // Stop the turtle
        msg.linear.x = 0.0;
        velocity_pub->publish(msg);
    }

    void control_turtle(){
        // 0.3 unit left
        pen_on_off(true);
        RCLCPP_INFO(this->get_logger(), "0.3 left, pen on");
        move_turtle(0.3);

        // pen off
        RCLCPP_INFO(this->get_logger(), "pen off");
        pen_on_off(false);
        

        
        RCLCPP_INFO(this->get_logger(), "0.3 left, pen off");
        move_turtle(0.3);

        
        RCLCPP_INFO(this->get_logger(), "pen on");
        pen_on_off(true);
        

        
        RCLCPP_INFO(this->get_logger(), "0.3 left, pen on");
        move_turtle(0.3);
    }

    public:

    DrawLogo() : Node("draw_logo"){
        
        velocity_pub = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        pen_on_off_c = this->create_client<turtlesim::srv::SetPen>("turtle1/set_pen");

        // cooldown after init
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1), std::bind(&DrawLogo::control_turtle, this));

    };
};

int main(int argc, char** argv){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DrawLogo>());
    rclcpp::shutdown();
    return 0;
}
