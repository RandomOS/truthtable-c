/*
 * 2011-07-07
 * 离散数学真值表
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#define STACK_TYPE char
#define MAX_LETTERS 26

typedef enum BOOL { FALSE, TRUE } Bool;

typedef struct INFO
{
    int cols;
    int rows;
    char elem[MAX_LETTERS];
} ExpInfo;

typedef struct STACKNODE
{
    STACK_TYPE value;
    struct STACKNODE *next;
} StackNode;

int isEmpty(StackNode *stack);
void push(StackNode **stackp, STACK_TYPE value);
void pop(StackNode **stackp);
STACK_TYPE getTop(StackNode *stack);
void destroyStack(StackNode **stackp);
char *getstr(void);
char *fReadLine(FILE *stream);
int priority(char opr);
int isoperator(char c);
int isbinaryoperator(char c);
int is_valid_expression(const char *exp);
ExpInfo *get_info(const char *exp);
char *infix_to_suffix(const char *exp);
int eval(const char *row, const char *exp, ExpInfo *info);
void strReverse(char *str, int n);
int *print_table(const char *exp, ExpInfo *info, Bool bPrint);
void print_result(const int *result, ExpInfo *info);
int test_bit(unsigned val, unsigned bit);
int is_tautology(const int *result, ExpInfo *info);
int is_contradiction(const int *result, ExpInfo *info);
void print_CNF(const int *result, ExpInfo *info);
void print_DNF(const int *result, ExpInfo *info);
void cls(void);
void pause(void);
void instruction(void);

/*
 * isEmpty
 * 如果堆栈为空，返回TRUE，否则返回FALSE
 */
int isEmpty(StackNode *stack)
{
    return stack == NULL;
}

/*
 * push
 * 把一个新值压到堆栈中。它的参数是需要被压入的值。
 */
void push(StackNode **stackp, STACK_TYPE value)
{
    StackNode *new_node;
    new_node = (StackNode *)malloc(sizeof(StackNode));
    assert(new_node != NULL);
    new_node->value = value;
    new_node->next = *stackp;
    *stackp = new_node;
}

/*
 * pop
 * 从堆栈中弹出一个值，并将其丢弃。
 */
void pop(StackNode **stackp)
{
    StackNode *first;
    assert(!isEmpty(*stackp));
    first = *stackp;
    *stackp = first->next;
    free(first);
}

/*
 * getTop
 * 返回堆栈顶部元素的值，但不对堆栈进行修改。
 */
STACK_TYPE getTop(StackNode *stack)
{
    assert(!isEmpty(stack));
    return stack->value;
}

/*
 * destroyStack
 * 销毁堆栈
 */
void destroyStack(StackNode **stackp)
{
    while (!isEmpty(*stackp))
    {
        pop(stackp);
    }
}

/*
 * getstr
 * 从StdIn获取一行输入
 */
char *getstr(void)
{
    char *s, *tmp;
    int i, j, ch;
    const int bufsize = 10;

    s = (char *)calloc(bufsize, sizeof(char));

    for (i = 0, j = 2; (ch = getchar()) != '\n'; ++i)
    {
        s[i] = ch;
        s[i + 1] = '\0';
        if ((i + 2) % bufsize == 0)
        {
            tmp = (char *)realloc(s, j * bufsize * sizeof(char));
            assert(tmp != NULL);
            s = tmp;
            ++j;
        }
    }
    return s;
}

/*
 * fReadLine
 * 从文件获取一行输入
 */
char *fReadLine(FILE *stream)
{
    char *s, *tmp;
    int i, j, ch;
    const int bufsize = 100;

    s = (char *)calloc(bufsize, sizeof(char));

    for (i = 0, j = 2; (ch = fgetc(stream)) != '\n'; ++i)
    {
        if (ch == EOF)
        {
            break;
        }

        s[i] = ch;
        s[i + 1] = '\0';
        if ((i + 2) % bufsize == 0)
        {
            tmp = (char *)realloc(s, j * bufsize * sizeof(char));
            assert(tmp != NULL);
            s = tmp;
            ++j;
        }
    }
    return s;
}

/*
 * isoperator
 * 判断是否是合法的操作符
 */
int isoperator(char c)
{
    switch (c)
    {
        case '!':
        case '&':
        case '|':
        case '>':
        case '#':
            return 1;
    }
    return 0;
}

/*
 * 判断是否是双目操作符
 */
int isbinaryoperator(char c)
{
    return c != '!' && isoperator(c);
}

/*
 * priority
 * 输出联结词的优先级
 */
int priority(char opr)
{
    switch (opr)
    {
        case '!':
            return 5;
        case '&':
            return 4;
        case '|':
            return 3;
        case '>':
            return 2;
        case '#':
            return 1;
        default:
            return 0;
    }
}

/*
 * 判断表达式是否合法
 */
int is_valid_expression(const char *exp)
{
    int i, len, count;
    int lbracket, rbracket;
    int lbracketpos, rbracketpos;
    len = strlen(exp);
    /* 开头的字符要求  */
    if (!isupper(exp[0]) && exp[0] != '(' && exp[0] != '!')
    {
        printf("开头字符错误!\n");
        return 0;
    }
    /* 结尾的字符要求  */
    if (!isupper(exp[len - 1]) && exp[len - 1] != ')')
    {
        printf("结尾字符错误!\n");
        return 0;
    }

    count = lbracket = rbracket = 0;
    for (i = 0; exp[i] != '\0'; i++)
    {
        if (isupper(exp[i]))
            count++;
        else if (exp[i] == '(')
            lbracket++;
        else if (exp[i] == ')')
            rbracket++;
        else if (!isoperator(exp[i]))
        {
            printf("包含不合法字符!\n");
            return 0;
        }
    }
    /* 表达式中变元的个数至少为1  */
    if (count == 0)
    {
        printf("没有变元!\n");
        return 0;
    }
    /* 左右括号数要相等  */
    if (lbracket != rbracket)
    {
        printf("括号数不等!\n");
        return 0;
    }
    else if (lbracket > 0)
    {
        lbracketpos = 0;
        rbracketpos = len - 1;
        lbracket = rbracket = 0;
        for (i = 0; exp[i] != '\0'; i++)
        {
            if (exp[i] == '(')
            {
                lbracket++;
                lbracketpos = i;
            }
            else if (exp[i] == ')')
            {
                rbracket++;
                rbracketpos = i;
            }
            if (lbracket == rbracket && lbracketpos > rbracketpos)
            {
                printf("括号位置不匹配!\n");
                return 0;
            }
        }
    }
    /* 每个字符左右应满足的条件  */
    for (i = 0; i < len - 1; i++)
    {
        if (isupper(exp[i]))
        {
            if (exp[i + 1] != ')' && !isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'右边有误!错误: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else if (exp[i] == '(')
        {
            if (exp[i + 1] == ')' || isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'右边有误!错误: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else if (exp[i] == ')')
        {
            if (exp[i + 1] != ')' && !isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'右边有误!错误: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else if (exp[i] == '!')
        {
            if (exp[i + 1] == ')' || isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'右边有误!错误: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else
        {
            if (exp[i + 1] == ')' || isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'右边有误!错误: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
    }
    return 1;
}

/*
 * 获取表达式信息
 */
ExpInfo *get_info(const char *exp)
{
    int i;
    ExpInfo *info;
    info = (ExpInfo *)calloc(1, sizeof(ExpInfo));
    assert(info != NULL);

    for (i = 0; exp[i] != '\0'; i++)
    {
        if (isupper(exp[i]) && info->elem[exp[i] - 'A'] == 0)
        {
            info->elem[exp[i] - 'A'] = 1;
            info->cols += 1;
        }
    }
    info->rows = pow(2, info->cols);
    return info;
}

/*
 * 中缀转后缀
 * 算法
 * 1)检查输入的下一元素。
 * 2)假如是个操作数，输出。
 * 3)假如是个开括号，将其压栈。
 * 4)假如是个运算符，则
 *       i) 假如栈为空，将此运算符压栈。
 *      ii) 假如栈顶是开括号，将此运算符压栈。
 *     iii) 假如此运算符比栈顶运算符优先级高，将此运算符压入栈中。
 *      iv) 否则栈顶运算符出栈并输出，重复步骤4。
 * 5)假如是个闭括号，栈中运算符逐个出栈并输出，直到遇到开括号。开括号出栈并丢弃。
 * 6)假如输入还未完毕，跳转到步骤1。
 * 7)假如输入完毕，栈中剩余的所有操作符出栈并输出它们。
 */
char *infix_to_suffix(const char *exp)
{
    int i, j;
    char *suffix, top;
    StackNode *stack = NULL;
    suffix = (char *)calloc(1, strlen(exp) + 1);
    assert(suffix != NULL);

    for (i = j = 0; exp[i] != '\0'; i++)
    {
        if (isupper(exp[i]))
            suffix[j++] = exp[i];
        else if (exp[i] == '(')
            push(&stack, exp[i]);
        else if (isoperator(exp[i]))
        {
            while (1)
            {
                if (isEmpty(stack) || getTop(stack) == '(')
                {
                    push(&stack, exp[i]);
                    break;
                }
                else
                {
                    top = getTop(stack);
                    if (priority(exp[i]) > priority(top) || (exp[i] == '!' && top == '!'))
                    {
                        push(&stack, exp[i]);
                        break;
                    }
                    else
                    {
                        suffix[j++] = getTop(stack);
                        pop(&stack);
                    }
                }
            }
        }
        else if (exp[i] == ')')
        {
            while (!isEmpty(stack) && (top = getTop(stack)) != '(')
            {
                suffix[j++] = top;
                pop(&stack);
            }
            pop(&stack);
        }
    }

    while (!isEmpty(stack))
    {
        suffix[j++] = getTop(stack);
        pop(&stack);
    }
    return suffix;
}

/*
 * eval
 * 计算表达式
 * row : 每行变元对应的值的序列
 * exp : 后缀表示的表达式
 * info: 存储表达式信息的结构体
 */
int eval(const char *row, const char *exp, ExpInfo *info)
{
    char temp = 0;
    char elem_val[MAX_LETTERS] = {0};
    StackNode *stack = NULL;
    int i, j, p, q, ret;

    p = q = 0;
    for (i = j = 0; i < MAX_LETTERS; i++)
    {
        if (info->elem[i])
        {
            elem_val[i] = row[j++];
        }
    }
    /*
     * 堆栈中存储的是变元,通过计算变元在elem_val这个表中的偏移量
     * 得到变元对应的值
     */
    for (i = 0; exp[i] != '\0'; i++)
    {
        if (isupper(exp[i]))
            push(&stack, elem_val[exp[i] - 'A']);
        else if (isoperator(exp[i]))
        {
            q = getTop(stack);
            pop(&stack);

            if (exp[i] != '!' && !isEmpty(stack))
            {
                p = getTop(stack);
                pop(&stack);
            }

            switch (exp[i])
            {
                case '!':
                    temp = !q;
                    break;
                case '&':
                    temp = p && q;
                    break;
                case '|':
                    temp = p || q;
                    break;
                case '>':
                    temp = !p || q;
                    break;
                case '#':
                    temp = (!p || q) && (!q || p);
                    break;
            }
            /* 把运算的结果再压回堆栈中  */
            push(&stack, temp);
        }
    }

    ret = getTop(stack);
    pop(&stack);
    return ret;
}

/*
 * 翻转字符串
 */
void strReverse(char *str, int n)
{
    int i;
    char temp;
    for (i = 0; i < n / 2; i++)
    {
        temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

/*
 * 打印真值表
 * bPrint : 是否输出到StdOut
 */
int *print_table(const char *exp, ExpInfo *info, Bool bPrint)
{
    int *result;
    int i, j, cols, rows, tmp, count;
    char *suffix, row[MAX_LETTERS];

    cols = info->cols;
    rows = info->rows;
    result = (int *)calloc(rows , sizeof(int));

    if (bPrint)
    {
        for (i = 0; i < MAX_LETTERS; ++i)
        {
            if (info->elem[i])
            {
                printf("\t%c", i + 'A');
            }
        }
        printf("\t%s\n", exp);
    }

    suffix = infix_to_suffix(exp);

    for (i = 0; i < rows; i++)
    {
        count = 0;
        tmp = i;

        memset(row, 0, sizeof(row));
        /* 通过将十进制转换成二进制来生成每行的序列  */
        do
        {
            row[count++] = tmp % 2;
            tmp /= 2;
        }
        while (tmp != 0);

        strReverse(row, cols);

        result[i] = eval(row, suffix, info);

        if (bPrint)
        {
            for (j = 0; j < cols; j++)
            {
                printf("\t%d", row[j]);
            }
            printf("\t%d\n", result[i]);
        }
    }
    free(suffix);
    return result;
}

/*
 * 打印真值表结果
 */
void print_result(const int *result, ExpInfo *info)
{
    int i;
    for (i = 0; i < info->rows; i++)
    {
        printf("%d", result[i]);
    }
    printf("\n");
}

/*
 * 判断整数指定的二进制位为1还是0
 */
int test_bit(unsigned val, unsigned bit)
{
    /* return !(~(val | ~(1 << bit))); */
    return val & 1 << bit;
}

/*
 * 判断是否是重言式(永真式)
 */
int is_tautology(const int *result, ExpInfo *info)
{
    int i;
    for (i = 0; i < info->rows; i++)
    {
        if (result[i] == 0)
            return 0;
    }
    return 1;
}

/*
 * 判断是否是矛盾式(永假式)
 */
int is_contradiction(const int *result, ExpInfo *info)
{
    int i;
    for (i = 0; i < info->rows; i++)
    {
        if (result[i] == 1)
            return 0;
    }
    return 1;
}

/*
 * 主合取范式 或与表达式 大项合取而成
 */
void print_CNF(const int *result, ExpInfo *info)
{
    char *letter;
    int i, j, count = 0, *a;

    /* 永真式没有合取范式  */
    if (is_tautology(result, info))
        return;

    letter = (char *)calloc(info->cols, sizeof(char));
    a = (int *)calloc(info->rows, sizeof(int));

    /* letter为存放变元的字符数组  */
    for (i = j = 0; i < MAX_LETTERS; i++)
    {
        if (info->elem[i])
            letter[j++] = i + 'A';
    }

    /* a为存放真值表结果中为0的项  */
    for (i = 0; i < info->rows; i++)
    {
        if (result[i] == 0)
            a[count++] = i;
    }

    printf("主合取范式\n");
    printf("M(");
    for (i = 0; i < count; i++)
    {
        if (i < count - 1)
            printf("%d, ", a[i]);
        else
            printf("%d", a[i]);
    }
    printf(")\n");

    for (i = 0; i < count; i++)
    {
        printf("(");
        for (j = 0; j < info->cols ; j++)
        {
            if (test_bit(a[i], info->cols - j - 1))
                printf("┐%c", letter[j]);
            else
                printf("%c", letter[j]);

            if (j < info->cols - 1)
                printf("∨");
        }
        if (i < count - 1)
            printf(")∧");
        else
            printf(")\n");
    }
    free(letter);
    free(a);
}

/*
 * 主析取范式 与或表达式 小项析取而成
 */
void print_DNF(const int *result, ExpInfo *info)
{
    char *letter;
    int i, j, count = 0, *a;

    /* 永假式没有析取范式  */
    if (is_contradiction(result, info))
        return;

    letter = (char *)calloc(info->cols, sizeof(char));
    a = (int *)calloc(info->rows, sizeof(int));

    /* letter为存放变元的字符数组  */
    for (i = j = 0; i < MAX_LETTERS; i++)
    {
        if (info->elem[i])
            letter[j++] = i + 'A';
    }

    /* a为存放真值表结果中为1的项  */
    for (i = 0; i < info->rows; i++)
    {
        if (result[i] == 1)
            a[count++] = i;
    }

    printf("主析取范式\n");
    printf("M(");
    for (i = 0; i < count; i++)
    {
        if (i < count - 1)
            printf("%d, ", a[i]);
        else
            printf("%d", a[i]);
    }
    printf(")\n");

    for (i = 0; i < count; i++)
    {
        printf("(");
        for (j = 0; j < info->cols ; j++)
        {
            if (test_bit(a[i], info->cols - j - 1))
                printf("%c", letter[j]);
            else
                printf("┐%c", letter[j]);

            if (j < info->cols - 1)
                printf("∧");
        }
        if (i < count - 1)
            printf(")∨");
        else
            printf(")\n");
    }
    free(letter);
    free(a);
}

/*
 * 清空屏显
 */
void cls(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/*
 * 在终端上暂停
 */
void pause(void)
{
#ifdef _WIN32
    system("pause");
#else
    system("read -p \"Press any key to continue...\" tmp");
#endif
}

void instruction(void)
{
    printf("||====离散真值表生成器====||\n");
    printf("||     '!' 否定联结词     ||\n");
    printf("||     '&' 合取联结词     ||\n");
    printf("||     '|' 析取联结词     ||\n");
    printf("||     '>' 蕴含联结词     ||\n");
    printf("||     '#' 等值联结词     ||\n");
    printf("||     'q' 退出程序       ||\n");
    printf("||========================||\n");
    printf("输入表达式:");
}

int main(int argc, char *argv[])
{
    char *exp;
    int *result;
    ExpInfo *info;
    FILE *fp;

#ifdef _WIN32
    system("title 离散数学真值表");
#endif

    if (argc == 2)
    {
        fp = fopen(argv[1], "r");

        if (fp == NULL)
        {
            printf("file read error!");
            return 0;
        }

        while (!feof(fp))
        {
            exp = fReadLine(fp);
            printf("%s\n", exp);
            if (!is_valid_expression(exp))
            {
                free(exp);
                continue;
            }
            info = get_info(exp);
            result = print_table(exp, info, FALSE);
            print_result(result, info);
            free(exp);
            free(info);
            free(result);
        }
        fclose(fp);
        return 0;
    }

    while (1)
    {
        cls();
        instruction();
        exp = getstr();

        if (strlen(exp) == 1 && exp[0] == 'q')
        {
            free(exp);
            break;
        }
        if (!is_valid_expression(exp))
        {
            free(exp);
            printf("不合法的表达式!\n");
            pause();
            continue;
        }

        info = get_info(exp);
        /* printf("%s", infix_to_suffix(exp));  */
        result = print_table(exp, info, TRUE);
        print_CNF(result, info);
        print_DNF(result, info);
        free(exp);
        free(info);
        free(result);
        pause();
    }
    return 0;
}
