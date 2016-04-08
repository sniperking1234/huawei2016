#include "route.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    char *file1 = "topo.csv";
    char *file2 = "demand.csv";
    char *file3 = "result.csv";
    print_time("Begin");
    char *topo[5000];
    int edge_num;
    char *demand;
    int demand_num;

    char *topo_file = file1;
    edge_num = read_file(topo, 5000, topo_file);
    if (edge_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }
    char *demand_file = file2;
    demand_num = read_file(&demand, 1, demand_file);
    if (demand_num != 1)
    {
        printf("Please input valid demand file.\n");
        return -1;
    }

    search_route(topo, edge_num, demand);

    char *result_file = file3;
    write_result(result_file);
    release_buff(topo, edge_num);
    release_buff(&demand, 1);

    print_time("End");

    return 0;
}

