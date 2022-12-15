
const long task_mtype = 1;

struct Task{
    long mtype;
    pid_t sender;
    float a, b;
    char op;
};

struct Ans{
    long mtype;
    float result;
};