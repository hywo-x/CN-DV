#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <vector>
//pthread是只有linux能用吗？注意编译的时候要加-lpthread

#define router_name_size 10

const char *config_path = "config.txt";

class Node
{
private:
    char exit_router[router_name_size]; //出口路由
    float distance;
    char name[router_name_size];

public:
    Node(char *, char *, float);
    ~Node();
    float get_distance();
    char *get_exit();
    void alter_exit(char *);
    char *get_name();
};

Node::Node(char *name, char *exit_router, float distance)
{
    Node::distance = distance;
    strncpy(Node::name, name, router_name_size);
    strncpy(Node::exit_router, exit_router, router_name_size);
    std::cout << "Node " << name << " init, distance=" << Node::distance << std::endl;
}

Node::~Node()
{
}

float Node::get_distance()
{
    return Node::distance;
}

char *Node::get_exit()
{
    return Node::exit_router;
}

void Node::alter_exit(char *new_exit)
{
    std::strncpy(Node::exit_router, new_exit, router_name_size);
}

char *Node::get_name()
{
    return Node::name;
}

class Neighbor
{
private:
    float distance;
    int udp_port;

public:
    Neighbor(float, int);
    ~Neighbor();
    float get_distance();
    int get_port();
};

Neighbor::Neighbor(float distance, int udp_port)
{
    Neighbor::distance = distance;
    Neighbor::udp_port = udp_port;
}

Neighbor::~Neighbor()
{
}

void init(char *name, char *udp, char *filename);

std::string send_message(); //编辑要发送的消息

void send(); //通过socket通信发送消息

void send_thread(); //负责发送的线程的执行函数

void listen(); //通过socket通信接收消息，并将接收到的消息进行初步的切割

void update_router_table(); //更新路由表

void listen_thread(); //负责接收的线程的执行函数

void listen_keyboard(); //负责监听键盘输入的线程的执行函数。注意恢复的时候要重新初始化

void exit_prog(); //退出整个程序