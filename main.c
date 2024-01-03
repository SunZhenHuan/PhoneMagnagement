#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "string.h"
#include "pthread.h"
//��̬�����ַ��ռ�
char* allocateSpace(int size){
    return (char*) malloc(size * sizeof (char));
}
typedef struct borrowAndReturn{
    //��¼��ʼ���ֻ���ʱ��
    time_t checkInTime;//�Ǽ�ʱ��
    struct tm *Time_info;//����ʱ���ʽ
}BorrowAndReturn;
BorrowAndReturn borrowAndReturn;//�軹ʱ��ṹ��

FILE *borrowFile;//������ֻ��˵�����
FILE *returnFile;//���滹�ֻ��˵�����
FILE *useFile;//ʹ���ֻ�ʱ�������
FILE *ini;//��ȡ�����ļ�
FILE * timeout;

time_t end_time;//���Ľ�������
double diff_t;//����֮��
time_t start_time;//��Ŀ�ʼʱ��
int start_flag; //ѡ���־
char *first_name; //��¼���������
int boAndRe; //��򻹱�־
//��������
char *timeFormat_ret(time_t *,struct tm *,char *);
void timeFormat_bor(time_t *,struct tm *,char *);
void bor_re_method();
void check_isAdmin();
void read_borrowFile();//������ļ�
void read_returnFile();//���黹���ļ�
void read_useFile();//����ʱ���ļ�
void admin();//����ԱȨ��
int phone_use;//�ֻ�ʹ���������
char *is_admin;
FILE *sjg;
int h,m,s;
int global_mill;//���Ƽ�ʱ
// ���ڳ�ʼ��
void init(){
    puts("*************************************************************");
    puts("************************��ӭ�����ֻ�����ϵͳ*****************");
    puts("*************************Hello World !***********************");
    puts("*************************************************************");
    puts("************************ϵͳ���ܲ˵�*************************");
    puts("-------------------------------------------------------------");
    init:puts("-1.�ֻ�ʹ�õǼ�  2.����ԱȨ��  3.�ֻ�ʹ��������� 4.�˳�ϵͳ-");
    puts("-------------------------------------------------------------");
    printf("��ִ�в���: ");
    scanf("%d",&start_flag);
    switch (start_flag) {
        case 1:
            puts("��ѡ�����ֻ�ʹ�õǼǲ���!");
            //�ж�ʱ��
            time_t rawtime;
            struct tm *info;
            time( &rawtime );
            info = localtime( &rawtime );
            int hours = info->tm_hour;
            if (strcmp(is_admin,"admin") == 0){
                //�ж��Ƿ��ǹ���Ա
                check_isAdmin();//�ǵ�������֤ʱ������
            }
            else{
                if (hours >=0 &&hours <= 5){
                    //����ʱ�䲻����
                    puts("0-5��ʱ��β�����");
                    goto init;//���ؽ���
                }
                else{
                    check_isAdmin();
                }
            }

            break;
        case 2:
            puts("��ѡ���˹���ԱȨ�޲���!");
            admin();
            break;
        case 3:
            puts("��ѡ�����ֻ�ʹ���������!");
        use:puts("-1.����ʱ���ѯ  2.�黹ʱ���ѯ  3.����ʹ��������� 4.����ͼ����ʽ 5.������һ��-");
            printf("����ѡ���־:");
            scanf("%d",&phone_use);
            int return_flag;//�ڲ�ʹ�õķ��ر�־
            switch (phone_use) {
                case 1:read_borrowFile();//��ȡ�������
                op_borrow:puts("-------------1.����������-----2.������һ��--------------");
                    scanf("%d",&return_flag);
                    if(return_flag == 1)
                        init();
                    else if (return_flag == 2)
                        goto use;
                    else{
                        puts("��������!��������");
                        goto op_borrow;
                    }
                    break;
                case 2:
                    read_returnFile();//��ȡ�黹���
                op_return:puts("-------------1.����������-----2.������һ��--------------");
                    scanf("%d",&return_flag);
                    if(return_flag == 1)
                        init();
                    else if (return_flag == 2)
                        goto use;
                    else{
                        puts("��������!��������");
                        goto op_return;
                    }
                    break;
                case 3:
                    read_useFile();//��ȡ�������
                op_use:puts("-------------1.����������-----2.������һ��--------------");
                    scanf("%d",&return_flag);
                    if(return_flag == 1)
                        init();
                    else if (return_flag == 2)
                        goto use;
                    else{
                        puts("��������!��������");
                        goto op_use;
                    }
                    break;
                case 4:
                    puts("���޿���!");
                    break;
                case 5:
                    goto init;
                default:
                    puts("ѡ������������ѡ��!");
                    goto use;
            }
            break;
        case 4:
            puts("��ӭ�´�����!2����Զ��رմ���!");
            Sleep(2000);
            exit(0);
        default:
            puts("��������,�����²���!");
            goto init;
    }
}

_Noreturn void *printNumbers() {
    int seconds = h * 60 * 60 + m * 60 + s;
    while (1){
        if (global_mill >= seconds){
            //��ʱ�䳬�������õ�ʱ��������
            fprintf(timeout,"%s�ֻ�ʹ�ó�ʱ!\n",first_name);
            fflush(timeout);
        }
        global_mill++;//ÿ���1
        Sleep(1000);
    }
}
void admin(){
    char admin[255];
    fseek(ini,6,SEEK_SET);
    fscanf(ini,"%s",admin);
    int ad_flag;//��־
    char *pwd = allocateSpace(20);
    admm:puts("-1.�������Ա�����ڷ���Ϣʱ����ֻ�  2.����ԱȨ��  3.�ֻ���ʱδ�黹���ѹ���Ա 4.������һ�� 5.�˳�ϵͳ-");
    printf("�������־:");
    scanf("%d",&ad_flag);
    if (ad_flag == 1){
        do_adm:printf("�����������ڷ���Ϣʱ���ʱ��:");
        scanf("%s",pwd);
        char adm[] = {};
        fseek(ini,6,SEEK_SET);
        fscanf(ini,"%s",adm);
        if (strcmp(pwd,adm) == 0){
            //���ͨ��
            puts("��֤�ɹ�!");
            is_admin = "admin";//�ĳ�admin
            init();
        }
        else{
            //��������
            puts("����Ա�������,���������룡");
            goto do_adm;
        }
    }
    else if (ad_flag == 2){
        opp:puts("-1.�����ֻ�����Ϣ  2.�����ֻ�ʹ��ʱ��  3.�鿴�����˻��������ֻ�ʹ����� 4.�Ե����˽��н��� 5.������һ�� 6.�˳�ϵͳ-");
        int op_flag;
        printf("�������־:");
        scanf("%d",&op_flag);
        switch (op_flag) {
            case 1:
                sjg = fopen("./config/phoneGui.txt","a+");
                printf("�������ֻ�����Ϣ(����+�豸)�ö��Ÿ���:");
                char *na = allocateSpace(20);
                char *shebei = allocateSpace(20);
                scanf("%s,%s",na,shebei);
                fprintf(sjg,"����%s--%s",na,shebei);
                fflush(sjg);
                fclose(sjg);
                printf("���óɹ�!");
                goto opp;
            case 2:
                printf("����ʱ�����ö��Ÿ���:");
                scanf("%d,%d,%d",&h,&m,&s);
                printf("��������ʹ��ʱ��Ϊ:%dСʱ%d����%d��",h,m,s);
                goto opp;
            case 3:
                read_useFile();
                goto opp;
            case 4:
                printf("��%s���óɹ�!",first_name);
                goto opp;
            case 5:
                goto admm;
            case 6:
                puts("��ӭ�´�����!2����Զ��رմ���!");
                Sleep(2000);
                exit(0);
            default:
                puts("��������������!");
                goto opp;
        }
    }
    else if (ad_flag == 3){
        //�ж����õ�ʱ�����Ƿ�ʼ��ʱ
        if (h == 0 && m == 0 && s == 0){
            puts("����û������ʹ��ʱ��!");
            int set;
            puts("--------1.ǰ������---2.�����÷�����һ��");
            scanf("%d",&set);
            if (set == 1){
                goto opp;
            }
            if (set == 2)
                goto admm;
        }
        else{
            //�����˼�������
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
        printf("����������:\n");
        goto admm;
    }
}
/**
 * �����ͨ�û����ǹ���Ա
 */
void check_isAdmin(){
    printf("��������������:");
    first_name = allocateSpace(10);//����ռ�
    scanf("%s",first_name);
    bor_re_method();
}
void bor_re_method(){
    error:puts("---------------------1.��-------2.��-------3.������һ��--------------------");
    printf("�������־:");
    scanf("%d",&boAndRe);
    if (boAndRe == 1){
        //��̨��ʼ��¼ʱ��!
        puts("��̨��ʼ��¼ʱ��!!!");
        timeFormat_bor(&(borrowAndReturn.checkInTime),borrowAndReturn.Time_info,first_name);
    }
    else if (boAndRe == 2){
        //�黹
        re:printf("���ٴ������������ֻ��ֻ�:");
        char *confirmTheName = allocateSpace(10);
        scanf("%s",confirmTheName);
        if (strcmp(confirmTheName,first_name) == 0){
            //�黹�ɹ�!
            printf("%s�黹�ɹ�!ʱ��Ϊ:%s\n",confirmTheName,timeFormat_ret(&(borrowAndReturn.checkInTime),borrowAndReturn.Time_info,confirmTheName));
            diff_t = difftime(end_time,start_time);
            useFile = fopen("./config/useFile.txt","a+");
            //д���ļ�
            fprintf(useFile,"�û�-%sʹ����%.2f��\n",confirmTheName,diff_t);
            fflush(useFile);
            puts("��¼�ɹ�!");
            //���·��ص��軹����
            bor_re_method();
            end_time = 0;
            start_time = 0;//����!
        }
        else{
            puts("�������ֲ�һ��!!!");
            goto re;
        }
    }
    else if (boAndRe == 3){
        init();//������
    }
    else{
        puts("������ѡ�����!!");
        goto error;
    }
}
/**
 *
 * @return "����һ��ʱ���ַ���"
 */
char *timeFormat_ret(time_t *t,struct tm *info,char * name){
    end_time = 0;//��ʼ��
    char *buffer = allocateSpace(20);
    time(t);
    info = localtime(t);
    strftime(buffer,20,"%Y-%m-%d %H:%M:%S",info);//��ʼ��
    //д���ļ�
    fprintf(returnFile,"���ֻ��û�--%s | %s\n",name,buffer);
    fflush(returnFile);
    puts("��¼�ɹ�!");
    time(&end_time);
    return buffer;
}


void timeFormat_bor(time_t *t,struct tm *info,char * name){
    start_time = 0;//��ʼ��
    char *buffer = allocateSpace(20);
    time(t);
    info = localtime(t);
    strftime(buffer,20,"%Y-%m-%d %H:%M:%S",info);//��ʼ��
    printf("%s���ֻ���ʱ�� : %s\n",name,buffer);
    //д���ļ�
    fprintf(borrowFile,"���ֻ��û�--%s | %s\n",name,buffer);
    //��ʼ��¼ʱ��
    time(&start_time);
    fflush(borrowFile);
    puts("��¼�ɹ�!");
    //���·��ص��軹����
    bor_re_method();
}
/**
 * ��ȡ�ֻ��������
 */
void read_borrowFile(){
    fseek(borrowFile,0,SEEK_END);//�ƶ����ļ�β
    int length = ftell(borrowFile);
    if (length == 0){
        //��ʾ�ļ�û������
        puts("��ǰ��û���˽��ֻ�!");
        init();//�ص���ʼ����
    }
    else{
        //չʾ����
        puts("-----------------------�ֻ�ʹ���������--------------------------");
        rewind(borrowFile);//�Ƶ��ļ�ͷ
        char *buff;
        buff = allocateSpace(1024*10);
        while(fscanf(borrowFile, "%s", buff)!=EOF)
            printf("********************|%s|******************\n", buff );
        puts("---------------------------------------------------------------------");
    }
}

/**
 * ��ȡ�ֻ��黹���
 */
void read_returnFile(){
    fseek(returnFile,0,SEEK_END);//�ƶ����ļ�β
    int length = ftell(returnFile);
    if (length == 0){
        //��ʾ�ļ�û������
        puts("��ǰ��û���˹黹�ֻ�!");
        init();//�ص���ʼ����
    }
    else{
        //չʾ����
        puts("-----------------------�ֻ��黹�������-----------------------");
        rewind(returnFile);//�Ƶ��ļ�ͷ
        char *buff;
        buff = allocateSpace(1024);
        while(fscanf(returnFile, "%s", buff)!=EOF)
            printf("********************|%s|******************\n", buff );
        puts("---------------------------------------------------------------------");
    }
}


/**
 * ��ȡ�ֻ�ʹ�����
 */
void read_useFile(){
    fseek(useFile,0,SEEK_END);//�ƶ����ļ�β
    int length = ftell(useFile);
    if (length == 0){
        //��ʾ�ļ�û������
        puts("��ѯ�޽��,��ȴ��黹.");
        init();//�ص���ʼ����
    }
    else{
        //չʾ����
        puts("------------------------�ֻ�ʹ���������------------------------");
        rewind(useFile);//�Ƶ��ļ�ͷ
        char *buff;
        buff = allocateSpace(1024);
        while(fscanf(useFile, "%s", buff)!=EOF)
            printf("********************|%s|******************\n", buff );
        puts("----------------------------------------------------------");
    }
}




int main() {
    is_admin = allocateSpace(20);//����
    //�������ļ�
    borrowFile = fopen("./config/borrowFile.txt","a+");//׷�ӽ��ֻ�����
    returnFile = fopen("./config/returnFile.txt","a+");//׷�ӻ��ֻ�����
    ini = fopen("./config/admin.ini","r");//��ȡ�����ļ�
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