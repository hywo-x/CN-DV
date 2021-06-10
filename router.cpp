#include "headers.h"

char router_name[router_name_size];
int port;                //本机udp端口号
bool pause_flag = false; // 标志是否暂停，false时程序正常执行，true时程序暂停执行。注意恢复的时候要重新初始化
int interval;
int max_distance;
int max_wait_time;

std::vector<Node> nodes;
std::vector<Neighbor> neighbors;

int main(int argc, char **argv)
{
    init(argv[1], argv[2], argv[3]); //启动程序的格式为"./router.exe a 50001 a.txt"，argv[1]表示路由名，argv[2]表示udp端口号，argv[3]表示初始化文件

    pthread_t tid1, tid2, tid3;

    //新开一个线程，定时发送路由表
    pthread_create(&tid1, NULL, send_thread, NULL);

    //新开一个线程，负责接收路由表
    pthread_create(&tid2, NULL, input_thread, NULL);

    //新开一个线程监听键盘输入，包括暂停/恢复，更改距离
    pthread_create(&tid3, NULL, listen_thread, NULL);

    //测试用
    std::cout << send_message() << std::endl;
    std::cout << nodes.size() << std::endl;
    std::cout << neighbors.size() << std::endl;
    return 0;
}

void init(char *cur_name, char *udp_port, char *filename)
{
    //读取邻居路由文件
    FILE *f = std::fopen(filename, "r");
    int count;
    char neighbor_name[router_name_size];
    float dist;
    int udp;
    fscanf(f, "%d", &count);
    for (int i = 0; i < count; ++i)
    {
        memset(neighbor_name, 0, router_name_size);
        fscanf(f, "%s %f %d", neighbor_name, &dist, &udp);

        Neighbor neighbor = Neighbor(dist, udp);
        neighbors.push_back(neighbor);
        Node node = Node(neighbor_name, neighbor_name, dist);
        nodes.push_back(node);
    }

    //系统文件配置
    strncpy(router_name, cur_name, router_name_size);
    f = std::fopen(config_path, "r");
    fscanf(f, "%d %d %d", &interval, &max_distance, &max_wait_time);

    fclose(f);
    return;

    //下面是用fstream的失败版本，用来凑字数

    //// 读取邻居路由文件
    //// std::ifstream file;
    //// char data[BUFSIZ] = {0}; //因为不能确定输入字符串的长度,所以用BUFSIZ
    //// file.open(filename, std::ios::in);
    //// file.read(data, BUFSIZ);
    //// file.close();
    //// file.clear();

    //// 字符串切割
    //// char *buf = data;
    //// char *outer_ptr = NULL;
    //// char *inner_ptr = NULL;
    //// char *tmp[3];
    //// int count = 0;
    //// while ((tmp[count] = strtok_r(buf, "\n", &outer_ptr)) != NULL)
    //// {
    ////     buf = tmp[count];
    ////     while ((tmp[count] = strtok_r(buf, " ", &inner_ptr)) != NULL)
    ////     {
    ////         count++;
    ////         buf = NULL;
    ////     }

    ////     count = 0;

    ////     atoi和atof有点问题，难受
    ////     Neighbor neighbor = Neighbor(atof(tmp[1]), atoi(tmp[2]));
    ////     neighbors.insert(std::pair<char *, Neighbor *>(tmp[0], &neighbor));
    ////     neighbor.~Neighbor();

    ////     Node node = Node(tmp[0], atof(tmp[1]));
    ////     nodes.insert(std::pair<char *, Node *>(tmp[0], &node));
    ////     node.~Node();
    //// }

    //// 读取配置信息文件
    //// port = atoi(udp_port);
    //// std::ifstream file2;
    //// std::memset(data, 0, BUFSIZ);
    //// file.open(config_path, std::ios::in);
    //// file.read(data, BUFSIZ);
    //// count = 0;
    //// while ((tmp[count] = strtok_r(buf, " ", &outer_ptr)) != NULL)
    //// {
    ////     count++;
    ////     buf = NULL;
    //// }

    //// std::cout << tmp[0] << " " << tmp[1] << " " << tmp[2] << " " << port << std::endl;
    //// interval = atoi(tmp[0]);
    //// max_distance = atoi(tmp[1]);
    //// max_wait_time = atoi(tmp[2]);
}

std::string send_message()
{
    std::string message;
    message += router_name;
    for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
    {
        message = message + " " + iter->get_name() + " " + std::to_string(iter->get_distance());
    }
    return message;
}

void send()
{
}

void *send_thread(void *args)
{
    while (true)
    {
        std::cout << "send thread" << std::endl;
        usleep(interval);
    }
    pthread_exit(NULL);
}

void listen()
{
}

void update_table()
{
}

void *input_thread(void *args)
{
    while (true)
    {
        std::cout << "input thread" << std::endl;
        usleep(interval);
    }
}

void listen_keyboard()
{
}

void restart_prog()
{
}

void exit_prog()
{
}

void *listen_thread(void *args)
{
    while (true)
    {
        std::cout << "listen thread" << std::endl;
        usleep(interval);
    }
}