#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
//pthread是只有linux能用吗？注意编译的时候要加-lpthread

#define router_name_size 10

char router_name[router_name_size];
int udp_port;       //udp端口号
bool pause = false; // 标志是否暂停，false时程序正常执行，true时程序暂停执行。注意恢复的时候要重新初始化

class Node
{
private:
    float distance;
    char exit_router[router_name_size]; //出口路由

public:
    Node(char *, float);
    ~Node();
};

Node::Node(char *exit_router, float distance)
{
    Node::distance = distance;
    strncpy(Node::exit_router, exit_router, router_name_size);
    std::cout << "Node " << Node::exit_router << " init, distance=" << Node::distance << std::endl;
}

Node::~Node()
{
}

class Neighbor
{
private:
    float distance;
    int udp_port;

public:
    Neighbor(float, int);
    ~Neighbor();
};

Neighbor::Neighbor(float distance, int udp_port)
{
    Neighbor::distance = distance;
    Neighbor::udp_port = udp_port;
}

Neighbor::~Neighbor()
{
}

std::map<char *, Node> nodes; //用map是因为方便查询
std::map<char *, Neighbor> neighbors;

void init(char *name, char *udp, char *filename);

void send_message(); //编辑要发送的消息

void send(); //通过socket通信发送消息

void send_thread(); //负责发送的线程的执行函数

void listen(); //通过socket通信接收消息，并将接收到的消息进行初步的切割

void update_router_table(); //更新路由表

void listen_thread(); //负责接收的线程的执行函数

void listen_keyboard(); //负责监听键盘输入的线程的执行函数。注意恢复的时候要重新初始化

void exit_prog(); //退出整个程序