#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//pthread是只有linux能用吗？注意编译的时候要加-lpthread

#define ROUTER_NAME_SIZE 10
#define MESSAGE_SIZE 1024
#define SERVER_IP "127.0.0.1"

const char *config_path = "config.txt";

class Node
{
private:
    char exit_router[ROUTER_NAME_SIZE]; //出口路由
    float distance;
    char name[ROUTER_NAME_SIZE];

public:
    Node(char *, char *, float);
    ~Node();
    float get_distance();
    char *get_exit();
    void alter_distance(float);
    void alter_exit(char *);
    char *get_name();
};

Node::Node(char *name, char *exit_router, float distance)
{
    Node::distance = distance;
    strncpy(Node::name, name, ROUTER_NAME_SIZE);
    strncpy(Node::exit_router, exit_router, ROUTER_NAME_SIZE);
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

void Node::alter_distance(float new_dist)
{
    Node::distance = new_dist;
}

void Node::alter_exit(char *new_exit)
{
    std::strncpy(Node::exit_router, new_exit, ROUTER_NAME_SIZE);
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
    char name[ROUTER_NAME_SIZE];

public:
    Neighbor(float, int, char *);
    ~Neighbor();
    float get_distance();
    int get_port();
    const char *get_name();
};

Neighbor::Neighbor(float distance, int udp_port, char *name)
{
    Neighbor::distance = distance;
    Neighbor::udp_port = udp_port;
    strncpy(Neighbor::name, name, ROUTER_NAME_SIZE);
}

Neighbor::~Neighbor()
{
}

float Neighbor::get_distance()
{
    return Neighbor::distance;
}

int Neighbor::get_port()
{
    return Neighbor::udp_port;
}

const char *Neighbor::get_name()
{
    return (const char *)Neighbor::name;
}

void init(char *name, char *udp, char *filename);

std::string send_message(); //编辑要发送的消息

void *send_thread(void *args); //负责发送的线程的执行函数

void update_table(char *msg); //更新路由表

void print_table(); //输出路由表

void *recv_thread(void *arg); //负责接收的线程的执行函数

void listen_keyboard(); //负责监听键盘输入的线程的执行函数。注意恢复的时候要重新初始化

void restart_prog(); //暂停后的重启。监听键盘输入的线程还没想好怎么操作

void exit_prog(); //退出

void *listen_thread(void *args); //监听线程