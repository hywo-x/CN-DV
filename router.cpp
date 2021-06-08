#include "headers.h"

int main(int argc, char **argv)
{
    init(argv[1], argv[2], argv[3]); //启动程序的格式为"./router.exe a 50001 a.txt"，argv[1]表示路由名，argv[2]表示udp端口号，argv[3]表示初始化文件

    //新开一个线程，定时发送路由表

    //新开一个线程，负责接收路由表

    //新开一个线程监听键盘输入，包括暂停/恢复，更改距离

    return 0;
}

void init(char *name, char *udp_port, char *filename)
{
    //读取邻居路由文件
    std::ifstream file;
    char data[BUFSIZ] = {0}; //因为不能确定输入字符串的长度,所以用BUFSIZ
    file.open(filename, std::ios::in);
    file.read(data, BUFSIZ);
    file.close();
    file.clear();

    //字符串切割
    char *buf = data;
    char *outer_ptr = NULL;
    char *inner_ptr = NULL;
    char *tmp[3];
    int count = 0;
    while ((tmp[count] = strtok_r(buf, "\n", &outer_ptr)) != NULL)
    {
        buf = tmp[count];
        while ((tmp[count] = strtok_r(buf, " ", &inner_ptr)) != NULL)
        {
            count++;
            buf = NULL;
        }

        count = 0;

        Neighbor neighbor = Neighbor(atof(tmp[1]), atoi(tmp[2]));
        neighbors.insert(std::pair<char *, Neighbor>(tmp[0], neighbor));
        neighbor.~Neighbor();

        Node node = Node(tmp[0], atof(tmp[1]));
        nodes.insert(std::pair<char *, Node>(tmp[0], node));
        node.~Node();
    }

    //读取配置信息文件
}