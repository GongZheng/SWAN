// #define DEBUG
int gcb(int m, int n)
{
    int temp, i;
    if (m < n) /*比较大小，使得m中存储大数，n中存储小数*/
    {          /*交换m和n的值*/
        temp = m;
        m = n;
        n = temp;
    }
    for (i = n; i > 0; i--) /*按照从大到小的顺序寻找满足条件的自然数*/
        if (m % i == 0 && n % i == 0)
        { /*输出满足条件的自然数并结束循环*/
            return i;
        }
    return 0;
}

int is_double_coprime(int m,int n,int level){

    if (level < 3)
    {
        return 1;
    }
    int p = gcb(m, n);
    if (p > 1)
    {
        return 0;
   }
   else{
       return 1;
   }
}
int is_triple_coprime(int a, int b, int c,int level)
{
    if(level < 2){
        return 1;
    }
    if (gcb(a, b) == 1 && gcb(a, c) == 1 && gcb(b, c) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void tobin(uint32_t x, int SIZE) //要和函数声明一致，所以后面也要填int x,int a[]
{
    int a[32] = {0};
    int n = 0, t, k;
    do
    {
        a[n] = x % 2;
        x = (unsigned int)x >> 1; //要考虑到参数int x有可能为负数所以填x=x/2是不行的。
        //如果x>=0的话填x=x/2没有问题，实际上我估计这里出题者的本意希望填x/2，但是如果当x为负数的时候
        //会出错的，要么填 x=(unsigned int)x/2也是可以的，不过 x=(unsigned int)x/2的运行效率远远低于x=(unsigned int)x>>1。这里牵涉的东西比较多，三言两语说不清
        //如果想了解原因，建议回去看谭浩强的强制类型转换、正数和负数的2进制表示、移位3个知识点
        n++;
    } while (x != 0);
    //do...while()这个功能就是把这个数的二进制的位存入这个数组中
    for (k = 0; k < n / 2; k++)
    {
        t = a[k];
        a[k] = a[n - k - 1]; //实现数组中2个数交换
        a[n - k - 1] = t;
        //for循环是为了交换顺序，比如x=11是的二进制码是1011这4个码一次存在a[3] a[2] a[1] a[0]中，而输出的时候是按a[0] a[1] a[2] a[3]这样输出的如果没有这个交换屏幕上看到的会是1101
    }
    for (k = SIZE; k > n; k--)
    {
        printf("%d-", a[k]);
    }
    for (k = 0; k < n; k++)
        printf("%d-", a[k]);
    int num = 0;
    for (int i = 0; i < 32; i++)
    {
        num += a[i];
    }
    // printf("%d\n", num);
    printf("\n");
}

int checkAverage32(int a, int b, int c,int level)
{
    if(level < 4){
        return 1;
    }
    int flag[4] = {0};
    int Rarray[3] = {a, b, c};
    for (int i = 0; i < 3; i++)
    {
        if (Rarray[i] >= 1 && Rarray[i] <= 7)
        {
            flag[0] = 1;
        }
        if (Rarray[i] >= 8 && Rarray[i] <= 15)
        {
            flag[1] = 1;
        }
        if (Rarray[i] >= 17 && Rarray[i] <= 23)
        {
            flag[2] = 1;
        }
        if (Rarray[i] >= 24 && Rarray[i] <= 31)
        {
            flag[3] = 1;
        }
    }
    if ((flag[0] + flag[1] + flag[2] + flag[3]) >= 3)
    {
        return 1;
    }
    else
        return 0;
}

int checkAverage8(int a, int b, int c,int level)
{
    if (level < 4)
    {
        return 1;
    }
    int flag[4] = {0};
    int Rarray[3] = {a, b, c};
    for (int i = 0; i < 3; i++)
    {
        if (Rarray[i] == 1)
        {
            flag[0] = 1;
        }
        if (Rarray[i] >= 2 && Rarray[i] <= 3)
        {
            flag[1] = 1;
        }
        if (Rarray[i] >= 4 && Rarray[i] <= 5)
        {
            flag[2] = 1;
        }
        if (Rarray[i] >= 6 && Rarray[i] <= 7)
        {
            flag[3] = 1;
        }
    }

    if ((flag[0] + flag[1] + flag[2] + flag[3]) >= 3)
    {
        return 1;
    }
    else{
        return 0;
    }
  
}

int checkAverage16(int a, int b, int c,int level)
{
    if (level < 4)
    {
        return 1;
    }
    int flag[4] = {0};
    int Rarray[3] = {a, b, c};
    for (int i = 0; i < 3; i++)
    {
        if (Rarray[i] >= 1 && Rarray[i] <= 3)
        {
            flag[0] = 1;
        }
        if (Rarray[i] >= 4 && Rarray[i] <= 7)
        {
            flag[1] = 1;
        }
        if (Rarray[i] >= 8 && Rarray[i] <= 11)
        {
            flag[2] = 1;
        }
        if (Rarray[i] >= 12 && Rarray[i] <= 15)
        {
            flag[3] = 1;
        }
    }
    if ((flag[0] + flag[1] + flag[2] + flag[3]) >= 3)
    {
        return 1;
    }
    else
        return 0;
}
