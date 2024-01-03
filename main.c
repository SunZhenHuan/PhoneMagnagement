#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "string.h"
#include "pthread.h"
//动态分配字符空间
char* allocateSpace(int size){
    return (char*) malloc(size * sizeof (char));
}
typedef struct borrowAndReturn{
    //记录开始借手机的时间
    time_t checkInTime;//登记时间
    struct tm *Time_info;//用于时间格式
}BorrowAndReturn;
BorrowAndReturn borrowAndReturn;//借还时间结构体

FILE *borrowFile;//保存借手机人的数据
FILE *returnFile;//保存还手机人的数据
FILE *useFile;//使用手机时间的数据
FILE *ini;//读取配置文件
FILE * timeout;

time_t end_time;//还的结束日期
double diff_t;//两者之差
time_t start_time;//借的开始时间
int start_flag; //选择标志
char *first_name; //记录输入的姓名
int boAndRe; //借或还标志
//函数声明
char *timeFormat_ret(time_t *,struct tm *,char *);
void timeFormat_bor(time_t *,struct tm *,char *);
void bor_re_method();
void check_isAdmin();
void read_borrowFile();//读借的文件
void read_returnFile();//读归还的文件
void read_useFile();//读计时的文件
void admin();//管理员权限
int phone_use;//手机使用情况数据
char *is_admin;
FILE *sjg;
int h,m,s;
int global_mill;//控制计时
// 窗口初始化
void init(){
    puts("*************************************************************");
    puts("************************欢迎进入手机管理系统*****************");
    puts("*************************Hello World !***********************");
    puts("*************************************************************");
    puts("************************系统功能菜单*************************");
    puts("-------------------------------------------------------------");
    init:puts("-1.手机使用登记  2.管理员权限  3.手机使用情况数据 4.退出系统-");
    puts("-------------------------------------------------------------");
    printf("请执行操作: ");
    scanf("%d",&start_flag);
    switch (start_flag) {
        case 1:
            puts("您选择了手机使用登记操作!");
            //判断时间
            time_t rawtime;
            struct tm *info;
            time( &rawtime );
            info = localtime( &rawtime );
            int hours = info->tm_hour;
            if (strcmp(is_admin,"admin") == 0){
                //判断是否是管理员
                check_isAdmin();//是的则不用验证时间限制
            }
            else{
                if (hours >=0 &&hours <= 5){
                    //现在时间不开放
                    puts("0-5点时间段不开放");
                    goto init;//返回界面
                }
                else{
                    check_isAdmin();
                }
            }

            break;
        case 2:
            puts("您选择了管理员权限操作!");
            admin();
            break;
        case 3:
            puts("您选择了手机使用情况操作!");
        use:puts("-1.借用时间查询  2.归还时间查询  3.正在使用情况数据 4.生成图形样式 5.返回上一步-");
            printf("输入选择标志:");
            scanf("%d",&phone_use);
            int return_flag;//内部使用的返回标志
            switch (phone_use) {
                case 1:read_borrowFile();//读取借用情况
                op_borrow:puts("-------------1.返回主界面-----2.返回上一步--------------");
                    scanf("%d",&return_flag);
                    if(return_flag == 1)
                        init();
                    else if (return_flag == 2)
                        goto use;
                    else{
                        puts("输入有误!重新输入");
                        goto op_borrow;
                    }
                    break;
                case 2:
                    read_returnFile();//读取归还情况
                op_return:puts("-------------1.返回主界面-----2.返回上一步--------------");
                    scanf("%d",&return_flag);
                    if(return_flag == 1)
                        init();
                    else if (return_flag == 2)
                        goto use;
                    else{
                        puts("输入有误!重新输入");
                        goto op_return;
                    }
                    break;
                case 3:
                    read_useFile();//读取借用情况
                op_use:puts("-------------1.返回主界面-----2.返回上一步--------------");
                    scanf("%d",&return_flag);
                    if(return_flag == 1)
                        init();
                    else if (return_flag == 2)
                        goto use;
                    else{
                        puts("输入有误!重新输入");
                        goto op_use;
                    }
                    break;
                case 4:
                    puts("暂无开发!");
                    break;
                case 5:
                    goto init;
                default:
                    puts("选择有误请重新选择!");
                    goto use;
            }
            break;
        case 4:
            puts("欢迎下次再来!2秒后自动关闭窗口!");
            Sleep(2000);
            exit(0);
        default:
            puts("操作错误,请重新操作!");
            goto init;
    }
}

_Noreturn void *printNumbers() {
    int seconds = h * 60 * 60 + m * 60 + s;
    while (1){
        if (global_mill >= seconds){
            //当时间超过了设置的时间则提醒
            fprintf(timeout,"%s手机使用超时!\n",first_name);
            fflush(timeout);
        }
        global_mill++;//每秒加1
        Sleep(1000);
    }
}
void admin(){
    char admin[255];
    fseek(ini,6,SEEK_SET);
    fscanf(ini,"%s",admin);
    int ad_flag;//标志
    char *pwd = allocateSpace(20);
    admm:puts("-1.输入管理员密码在非休息时间借手机  2.管理员权限  3.手机超时未归还提醒管理员 4.返回上一步 5.退出系统-");
    printf("请输入标志:");
    scanf("%d",&ad_flag);
    if (ad_flag == 1){
        do_adm:printf("输入密码后可在非休息时间借时间:");
        scanf("%s",pwd);
        char adm[] = {};
        fseek(ini,6,SEEK_SET);
        fscanf(ini,"%s",adm);
        if (strcmp(pwd,adm) == 0){
            //相等通过
            puts("验证成功!");
            is_admin = "admin";//改成admin
            init();
        }
        else{
            //重新输入
            puts("管理员密码错误,请重新输入！");
            goto do_adm;
        }
    }
    else if (ad_flag == 2){
        opp:puts("-1.设置手机柜信息  2.设置手机使用时间  3.查看单个人或者整体手机使用情况 4.对单个人进行禁用 5.返回上一步 6.退出系统-");
        int op_flag;
        printf("请输入标志:");
        scanf("%d",&op_flag);
        switch (op_flag) {
            case 1:
                sjg = fopen("./config/phoneGui.txt","a+");
                printf("请输入手机柜信息(姓名+设备)用逗号隔开:");
                char *na = allocateSpace(20);
                char *shebei = allocateSpace(20);
                scanf("%s,%s",na,shebei);
                fprintf(sjg,"名字%s--%s",na,shebei);
                fflush(sjg);
                fclose(sjg);
                printf("设置成功!");
                goto opp;
            case 2:
                printf("输入时分秒用逗号隔开:");
                scanf("%d,%d,%d",&h,&m,&s);
                printf("您设置了使用时间为:%d小时%d分钟%d秒",h,m,s);
                goto opp;
            case 3:
                read_useFile();
                goto opp;
            case 4:
                printf("对%s禁用成功!",first_name);
                goto opp;
            case 5:
                goto admm;
            case 6:
                puts("欢迎下次再来!2秒后自动关闭窗口!");
                Sleep(2000);
                exit(0);
            default:
                puts("操作有误请重试!");
                goto opp;
        }
    }
    else if (ad_flag == 3){
        //判断设置的时分秒是否开始计时
        if (h == 0 && m == 0 && s == 0){
            puts("您并没有设置使用时间!");
            int set;
            puts("--------1.前往设置---2.不设置返回上一步");
            scanf("%d",&set);
            if (set == 1){
                goto opp;
            }
            if (set == 2)
                goto admm;
        }
        else{
            //设置了计算秒数
            pthread_t thread;
            pthread_create(&thread, NULL, printNumbers, NULL);
            pthread_join(thread, NULL);
        }
    }
    else if (ad_flag == 4){
        init();
    }
    else if (ad_flag == 5){
        goto admm;
    }
    else{
        printf("请重新输入:\n");
        goto admm;
    }
}
/**
 * 检查普通用户还是管理员
 */
void check_isAdmin(){
    printf("请输入您的姓名:");
    first_name = allocateSpace(10);//分配空间
    scanf("%s",first_name);
    bor_re_method();
}
void bor_re_method(){
    error:puts("---------------------1.借-------2.还-------3.返回上一步--------------------");
    printf("请输入标志:");
    scanf("%d",&boAndRe);
    if (boAndRe == 1){
        //后台开始记录时间!
        puts("后台开始记录时间!!!");
        timeFormat_bor(&(borrowAndReturn.checkInTime),borrowAndReturn.Time_info,first_name);
    }
    else if (boAndRe == 2){
        //归还
        re:printf("请再次输入您的名字还手机:");
        char *confirmTheName = allocateSpace(10);
        scanf("%s",confirmTheName);
        if (strcmp(confirmTheName,first_name) == 0){
            //归还成功!
            printf("%s归还成功!时间为:%s\n",confirmTheName,timeFormat_ret(&(borrowAndReturn.checkInTime),borrowAndReturn.Time_info,confirmTheName));
            diff_t = difftime(end_time,start_time);
            useFile = fopen("./config/useFile.txt","a+");
            //写入文件
            fprintf(useFile,"用户-%s使用了%.2f秒\n",confirmTheName,diff_t);
            fflush(useFile);
            puts("记录成功!");
            //重新返回到借还界面
            bor_re_method();
            end_time = 0;
            start_time = 0;//重置!
        }
        else{
            puts("两次名字不一致!!!");
            goto re;
        }
    }
    else if (boAndRe == 3){
        init();//主界面
    }
    else{
        puts("请重新选择操作!!");
        goto error;
    }
}
/**
 *
 * @return "返回一个时间字符串"
 */
char *timeFormat_ret(time_t *t,struct tm *info,char * name){
    end_time = 0;//初始化
    char *buffer = allocateSpace(20);
    time(t);
    info = localtime(t);
    strftime(buffer,20,"%Y-%m-%d %H:%M:%S",info);//初始化
    //写入文件
    fprintf(returnFile,"还手机用户--%s | %s\n",name,buffer);
    fflush(returnFile);
    puts("记录成功!");
    time(&end_time);
    return buffer;
}


void timeFormat_bor(time_t *t,struct tm *info,char * name){
    start_time = 0;//初始化
    char *buffer = allocateSpace(20);
    time(t);
    info = localtime(t);
    strftime(buffer,20,"%Y-%m-%d %H:%M:%S",info);//初始化
    printf("%s借手机的时间 : %s\n",name,buffer);
    //写入文件
    fprintf(borrowFile,"借手机用户--%s | %s\n",name,buffer);
    //开始记录时间
    time(&start_time);
    fflush(borrowFile);
    puts("记录成功!");
    //重新返回到借还界面
    bor_re_method();
}
/**
 * 读取手机借用情况
 */
void read_borrowFile(){
    fseek(borrowFile,0,SEEK_END);//移动到文件尾
    int length = ftell(borrowFile);
    if (length == 0){
        //表示文件没有内容
        puts("当前并没有人借手机!");
        init();//回到开始界面
    }
    else{
        //展示数据
        puts("-----------------------手机使用情况如下--------------------------");
        rewind(borrowFile);//移到文件头
        char *buff;
        buff = allocateSpace(1024*10);
        while(fscanf(borrowFile, "%s", buff)!=EOF)
            printf("********************|%s|******************\n", buff );
        puts("---------------------------------------------------------------------");
    }
}

/**
 * 读取手机归还情况
 */
void read_returnFile(){
    fseek(returnFile,0,SEEK_END);//移动到文件尾
    int length = ftell(returnFile);
    if (length == 0){
        //表示文件没有内容
        puts("当前并没有人归还手机!");
        init();//回到开始界面
    }
    else{
        //展示数据
        puts("-----------------------手机归还情况如下-----------------------");
        rewind(returnFile);//移到文件头
        char *buff;
        buff = allocateSpace(1024);
        while(fscanf(returnFile, "%s", buff)!=EOF)
            printf("********************|%s|******************\n", buff );
        puts("---------------------------------------------------------------------");
    }
}


/**
 * 读取手机使用情况
 */
void read_useFile(){
    fseek(useFile,0,SEEK_END);//移动到文件尾
    int length = ftell(useFile);
    if (length == 0){
        //表示文件没有内容
        puts("查询无结果,请等待归还.");
        init();//回到开始界面
    }
    else{
        //展示数据
        puts("------------------------手机使用情况如下------------------------");
        rewind(useFile);//移到文件头
        char *buff;
        buff = allocateSpace(1024);
        while(fscanf(useFile, "%s", buff)!=EOF)
            printf("********************|%s|******************\n", buff );
        puts("----------------------------------------------------------");
    }
}




int main() {
    is_admin = allocateSpace(20);//分配
    //打开两个文件
    borrowFile = fopen("./config/borrowFile.txt","a+");//追加借手机的人
    returnFile = fopen("./config/returnFile.txt","a+");//追加还手机的人
    ini = fopen("./config/admin.ini","r");//读取配置文件
    timeout = fopen("./config/timeout.txt","a+");
    init();
    system("pause");
    fclose(borrowFile);
    fclose(returnFile);
    fclose(useFile);
    fclose(ini);
    fclose(timeout);
    return 0;
}