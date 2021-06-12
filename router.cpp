#include "headers.h"

char my_name[ROUTER_NAME_SIZE];
int my_port; //本机udp端口号
int interval;
int max_distance;
int max_wait_time;

std::vector<Node> nodes;
std::vector<Neighbor> neighbors;

std::map<int, bool> if_msg;

int send_sock, recv_sock;
bool pause_flag = false; // 标志是否暂停，false时程序正常执行，true时程序暂停执行。注意恢复的时候要重新初始化

int main(int argc, char **argv)
{
    //std::cout << argv[1] << argv[2] << argv[3] << std::endl;
    if (argc < 4)
    {
        printf("Usage: ./router name port filename\n");
        exit(0);
    }

restart:
    init(argv[1], argv[2], argv[3]); //启动程序的true格式为"./router.exe a 50001 a.txt"，argv[1]表示路由名，argv[2]表示udp端口号，argv[3]表示初始化文件

    pthread_t tid_send, tid_recv, tid_clock;

    //新开一个线程，定时发送路由表
    pthread_create(&tid_send, NULL, send_thread, NULL);

    //新开一个线程，负责接收路由表，具体数量由邻居节点数量确定
    pthread_create(&tid_recv, NULL, recv_thread, NULL);

    //检测是否超时的线程
    pthread_create(&tid_clock, NULL, clock_thread, NULL);

    //目前的想法是，在主进程进行控制，暂停就直接杀死线程，恢复就从头开始，这样可以实现距离的改变；退出就直接exit
    char command[5];
    while (true)
    {
        scanf("%s", command);
        //printf("%s\n", command);
        if ((strcmp(command, "P") == 0) && !pause_flag)
        {
            //shutdown(send_sock, SHUT_RDWR);
            //close(send_sock);
            //pthread_cancel(tid_send);

            shutdown(recv_sock, SHUT_RDWR);
            close(recv_sock);
            pthread_cancel(tid_recv);

            pthread_cancel(tid_clock); //这个直接退出应该没问题
            pause_flag = true;
            std::cout << "pause" << std::endl;
        }
        else if ((strcmp(command, "C") == 0) && pause_flag) //加一个pause_flag，使得正常运行时直接C是无效的
        {
            nodes.clear();
            neighbors.clear();
            std::cout << "continue" << std::endl;
            pause_flag = false;
            goto restart;
        }
        else if (strcmp(command, "Q") == 0)
        {
            std::cout << "quit" << std::endl;
            exit(0);
        }
        else if (strcmp(command, "T") == 0)
        {
            std::cout << "test" << std::endl;
        }
    }
    return 0;
}

void init(char *cur_name, char *udp_port, char *filename)
{
    //读取邻居路由文件
    FILE *f = fopen(filename, "r");
    int count;
    char neighbor_name[ROUTER_NAME_SIZE];
    float dist;
    int neighbor_port;
    fscanf(f, "%d\n", &count);
    for (int i = 0; i < count; ++i)
    {
        memset(neighbor_name, 0, ROUTER_NAME_SIZE);
        fscanf(f, "%s %f %d\n", neighbor_name, &dist, &neighbor_port);

        Neighbor neighbor = Neighbor(dist, neighbor_port, neighbor_name);
        neighbors.push_back(neighbor);
        Node node = Node(neighbor_name, neighbor_name, dist);
        nodes.push_back(node);
        if_msg.insert(std::pair<int, bool>(neighbor_port, false));
    }

    //系统文件配置
    strncpy(my_name, cur_name, ROUTER_NAME_SIZE);
    my_port = atoi(udp_port);
    f = fopen(config_path, "r");
    fscanf(f, "%d %d %d", &interval, &max_distance, &max_wait_time);
    fclose(f);
    Node node = Node(my_name, my_name, 0);
    nodes.push_back(node);

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
    message += my_name;
    for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
    {
        if (!iter->get_state())
        {
            continue;
        }
        message = message + " " + iter->get_name() + " " + std::to_string(iter->get_distance());
    }
    return message;
}

void *send_thread(void *args)
{
    //建立连接
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cout << "Can't create socket" << std::endl;
        exit(-1);
    }
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    char msg[MESSAGE_SIZE];
    int seq = 1;

    //先将日志文件清空
    std::string filename = my_name;
    filename += "_log.txt";
    FILE *f = fopen(filename.c_str(), "w");
    fclose(f);

    while (!pause_flag)
    {
        //std::cout << "send thread" << std::endl;
        std::string tmp_msg = send_message();
        std::string log = generate_log(seq);
        memset(msg, 0, MESSAGE_SIZE);
        for (int i = 0; i < tmp_msg.length(); ++i)
        {
            msg[i] = tmp_msg[i];
        }
        for (auto iter = neighbors.begin(); iter != neighbors.end(); iter++)
        {
            if (!iter->get_state())
            {
                continue;
            }
            int neighbor_port = iter->get_port();
            client_addr.sin_port = htons(neighbor_port);
            int len;
            if ((len = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&client_addr, sizeof(client_addr))) < 0)
            {
                std::cout << "Send message error" << std::endl;
                std::cout << len << std::endl;
            }
            else
            {
                std::cout << "Send message to " << neighbor_port << " successfully" << std::endl;
            }

            //应该是这么写吧
        }
        std::cout << log.c_str();
        std::string filename = my_name;
        filename += "_log.txt";
        FILE *f = fopen(filename.c_str(), "a");
        fprintf(f, "%s", log.c_str());
        fclose(f);
        seq += 1;
        sleep(interval);
    }
    pthread_exit(NULL);
}

void update_table(char *msg)
{
    char *tmp = msg;
    char *rest = NULL;
    int count = 0;
    char router_name[ROUTER_NAME_SIZE];
    float dist;
    float neighbor_dist;
    char neighbor_name[ROUTER_NAME_SIZE];
    bool flag = false;
    bool in_table = false;
    while ((tmp = strtok_r(tmp, " ", &rest)) != NULL)
    {
        switch (count)
        {
        case 0:
            for (auto iter = neighbors.begin(); iter != neighbors.end(); iter++)
            {
                if (strcmp(tmp, iter->get_name()) == 0)
                {
                    neighbor_dist = iter->get_distance();
                    strncpy(neighbor_name, iter->get_name(), ROUTER_NAME_SIZE);
                    std::cout << "receive message from " << neighbor_name << std::endl;
                    if (!iter->get_state())
                    {
                        iter->alter_state();
                    }
                    if_msg[iter->get_port()] = true;
                    break;
                }
            }
            count++;
            tmp = NULL;
            break;

        case 1:
            strncpy(router_name, tmp, ROUTER_NAME_SIZE);
            count++;
            tmp = NULL;
            break;

        case 2:
            count--;
            dist = atof(tmp);
            dist += neighbor_dist;
            if (dist >= max_distance)
            {
                break; //如果超出最大距离则直接跳过（无论该节点是否在路由表中都不需要更新）
            }
            in_table = false;
            for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                if (strcmp(iter->get_name(), router_name) == 0)
                {
                    in_table = true;
                    float old_dist = iter->get_distance();
                    if (old_dist > dist)
                    {
                        iter->alter_distance(dist);
                        iter->alter_exit(neighbor_name);
                        flag = true;
                    }
                    if (!iter->get_state())
                    {
                        iter->alter_state();
                    }
                }
            }
            if (!in_table)
            {
                Node node = Node(router_name, neighbor_name, dist);
                nodes.push_back(node);
            }
            tmp = NULL;
            break;
        }
    }
    if (flag)
    {
        print_table();
    }
    return;
}

void print_table()
{
    printf("Start print router table:\n");
    for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
    {
        printf("router name:%s   router distance: %.1f   router exit:%s\n", iter->get_name(), iter->get_distance(), iter->get_exit());
    }
    printf("Finish print router table\n");
    return;
}

void *recv_thread(void *arg)
{
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(my_port);
    socklen_t len = sizeof(server_addr);

    recv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (bind(recv_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "bind failed" << std::endl;
        close(recv_sock);
        exit(-1);
    }
    std::cout << "listening port " << my_port << std::endl;

    ssize_t msg_size;
    char msg[MESSAGE_SIZE];
    while (true)
    {
        //std::cout << "recv thread" << std::endl;
        if ((msg_size = recvfrom(recv_sock, (void *)msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, &len)) < 0)
        {
            std::cout << "receive message error" << std::endl;
        }
        else
        {
            update_table(msg);
        }
        sleep(interval);
    }
}

std::string generate_log(int seq)
{
    char buf[BUFSIZ];
    sprintf(buf, "%d", seq);
    std::string log;
    log = log + "## Sent. Source node name = " + my_name + "; Sequence number = " + buf + "\n";
    for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
    {
        if (!iter->get_state())
        {
            log = log + "节点 " + iter->get_name() + " 超时\n";
            continue;
        }
        sprintf(buf, "%.1f", iter->get_distance());
        log = log + "Dest node = " + iter->get_name() + "; Distance = " + buf + "; Neighbor = " + iter->get_exit() + "\n";
    }
    return log;
}

void *clock_thread(void *args)
{
    while (true)
    {
        sleep(max_wait_time);
        for (auto iter = if_msg.begin(); iter != if_msg.end(); iter++)
        {
            if (iter->second == true)
            {
                iter->second = false;
            }
            else
            {
                char exit[ROUTER_NAME_SIZE];
                for (auto iter1 = neighbors.begin(); iter1 != neighbors.end(); iter1++)
                {
                    if (iter1->get_port() == iter->first)
                    {
                        std::cout << "邻居节点 " << iter1->get_name() << " 超时" << std::endl;
                        iter1->alter_state();
                        strncpy(exit, iter1->get_name(), ROUTER_NAME_SIZE);
                        break;
                    }
                }
                for (auto iter1 = nodes.begin(); iter1 != nodes.end(); iter1++)
                {
                    if (strcmp(iter1->get_exit(), exit) == 0)
                    {
                        iter1->alter_state();
                    }
                }
            }
        }
    }
}