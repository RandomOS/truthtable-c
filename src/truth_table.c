/*
 * 2011-07-07
 * ��ɢ��ѧ��ֵ��
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
 * �����ջΪ�գ�����TRUE�����򷵻�FALSE
 */
int isEmpty(StackNode *stack)
{
    return stack == NULL;
}

/*
 * push
 * ��һ����ֵѹ����ջ�С����Ĳ�������Ҫ��ѹ���ֵ��
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
 * �Ӷ�ջ�е���һ��ֵ�������䶪����
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
 * ���ض�ջ����Ԫ�ص�ֵ�������Զ�ջ�����޸ġ�
 */
STACK_TYPE getTop(StackNode *stack)
{
    assert(!isEmpty(stack));
    return stack->value;
}

/*
 * destroyStack
 * ���ٶ�ջ
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
 * ��StdIn��ȡһ������
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
 * ���ļ���ȡһ������
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
 * �ж��Ƿ��ǺϷ��Ĳ�����
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
 * �ж��Ƿ���˫Ŀ������
 */
int isbinaryoperator(char c)
{
    return c != '!' && isoperator(c);
}

/*
 * priority
 * �������ʵ����ȼ�
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
 * �жϱ��ʽ�Ƿ�Ϸ�
 */
int is_valid_expression(const char *exp)
{
    int i, len, count;
    int lbracket, rbracket;
    int lbracketpos, rbracketpos;
    len = strlen(exp);
    /* ��ͷ���ַ�Ҫ��  */
    if (!isupper(exp[0]) && exp[0] != '(' && exp[0] != '!')
    {
        printf("��ͷ�ַ�����!\n");
        return 0;
    }
    /* ��β���ַ�Ҫ��  */
    if (!isupper(exp[len - 1]) && exp[len - 1] != ')')
    {
        printf("��β�ַ�����!\n");
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
            printf("�������Ϸ��ַ�!\n");
            return 0;
        }
    }
    /* ���ʽ�б�Ԫ�ĸ�������Ϊ1  */
    if (count == 0)
    {
        printf("û�б�Ԫ!\n");
        return 0;
    }
    /* ����������Ҫ���  */
    if (lbracket != rbracket)
    {
        printf("����������!\n");
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
                printf("����λ�ò�ƥ��!\n");
                return 0;
            }
        }
    }
    /* ÿ���ַ�����Ӧ���������  */
    for (i = 0; i < len - 1; i++)
    {
        if (isupper(exp[i]))
        {
            if (exp[i + 1] != ')' && !isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'�ұ�����!����: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else if (exp[i] == '(')
        {
            if (exp[i + 1] == ')' || isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'�ұ�����!����: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else if (exp[i] == ')')
        {
            if (exp[i + 1] != ')' && !isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'�ұ�����!����: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else if (exp[i] == '!')
        {
            if (exp[i + 1] == ')' || isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'�ұ�����!����: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
        else
        {
            if (exp[i + 1] == ')' || isbinaryoperator(exp[i + 1]))
            {
                printf("'%c'�ұ�����!����: %c%c\n", exp[i], exp[i], exp[i + 1]);
                return 0;
            }
        }
    }
    return 1;
}

/*
 * ��ȡ���ʽ��Ϣ
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
 * ��׺ת��׺
 * �㷨
 * 1)����������һԪ�ء�
 * 2)�����Ǹ��������������
 * 3)�����Ǹ������ţ�����ѹջ��
 * 4)�����Ǹ����������
 *	  i) ����ջΪ�գ����������ѹջ��
 * 	 ii) ����ջ���ǿ����ţ����������ѹջ��
 * 	iii) ������������ջ����������ȼ��ߣ����������ѹ��ջ�С�
 * 	 iv) ����ջ���������ջ��������ظ�����4��
 * 5)�����Ǹ������ţ�ջ������������ջ�������ֱ�����������š������ų�ջ��������
 * 6)�������뻹δ��ϣ���ת������1��
 * 7)����������ϣ�ջ��ʣ������в�������ջ��������ǡ�
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
 * ������ʽ
 * row : ÿ�б�Ԫ��Ӧ��ֵ������
 * exp : ��׺��ʾ�ı��ʽ
 * info: �洢���ʽ��Ϣ�Ľṹ��
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
     * ��ջ�д洢���Ǳ�Ԫ,ͨ�������Ԫ��elem_val������е�ƫ����
     * �õ���Ԫ��Ӧ��ֵ
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
            /* ������Ľ����ѹ�ض�ջ��  */
            push(&stack, temp);
        }
    }

    ret = getTop(stack);
    pop(&stack);
    return ret;
}

/*
 * ��ת�ַ���
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
 * ��ӡ��ֵ��
 * bPrint : �Ƿ������StdOut
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
        /* ͨ����ʮ����ת���ɶ�����������ÿ�е�����  */
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
 * ��ӡ��ֵ����
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
 * �ж�����ָ���Ķ�����λΪ1����0
 */
int test_bit(unsigned val, unsigned bit)
{
    /* return !(~(val | ~(1 << bit))); */
    return val & 1 << bit;
}

/*
 * �ж��Ƿ�������ʽ(����ʽ)
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
 * �ж��Ƿ���ì��ʽ(����ʽ)
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
 * ����ȡ��ʽ ������ʽ �����ȡ����
 */
void print_CNF(const int *result, ExpInfo *info)
{
    char *letter;
    int i, j, count = 0, *a;

    /* ����ʽû�к�ȡ��ʽ  */
    if (is_tautology(result, info))
        return;

    letter = (char *)calloc(info->cols, sizeof(char));
    a = (int *)calloc(info->rows, sizeof(int));

    /* letterΪ��ű�Ԫ���ַ�����  */
    for (i = j = 0; i < MAX_LETTERS; i++)
    {
        if (info->elem[i])
            letter[j++] = i + 'A';
    }

    /* aΪ�����ֵ������Ϊ0����  */
    for (i = 0; i < info->rows; i++)
    {
        if (result[i] == 0)
            a[count++] = i;
    }

    printf("����ȡ��ʽ\n");
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
                printf("��%c", letter[j]);
            else
                printf("%c", letter[j]);

            if (j < info->cols - 1)
                printf("��");
        }
        if (i < count - 1)
            printf(")��");
        else
            printf(")\n");
    }
    free(letter);
    free(a);
}

/*
 * ����ȡ��ʽ �����ʽ С����ȡ����
 */
void print_DNF(const int *result, ExpInfo *info)
{
    char *letter;
    int i, j, count = 0, *a;

    /* ����ʽû����ȡ��ʽ  */
    if (is_contradiction(result, info))
        return;

    letter = (char *)calloc(info->cols, sizeof(char));
    a = (int *)calloc(info->rows, sizeof(int));

    /* letterΪ��ű�Ԫ���ַ�����  */
    for (i = j = 0; i < MAX_LETTERS; i++)
    {
        if (info->elem[i])
            letter[j++] = i + 'A';
    }

    /* aΪ�����ֵ������Ϊ1����  */
    for (i = 0; i < info->rows; i++)
    {
        if (result[i] == 1)
            a[count++] = i;
    }

    printf("����ȡ��ʽ\n");
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
                printf("��%c", letter[j]);

            if (j < info->cols - 1)
                printf("��");
        }
        if (i < count - 1)
            printf(")��");
        else
            printf(")\n");
    }
    free(letter);
    free(a);
}

/*
 * �������
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
 * ���ն�����ͣ
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
    printf("||====��ɢ��ֵ��������====||\n");
    printf("||     '!' �������     ||\n");
    printf("||     '&' ��ȡ�����     ||\n");
    printf("||     '|' ��ȡ�����     ||\n");
    printf("||     '>' �̺������     ||\n");
    printf("||     '#' ��ֵ�����     ||\n");
    printf("||     'q' �˳�����       ||\n");
    printf("||========================||\n");
    printf("������ʽ:");
}

int main(int argc, char *argv[])
{
    char *exp;
    int *result;
    ExpInfo *info;
    FILE *fp;

#ifdef _WIN32
    system("title ��ɢ��ѧ��ֵ��");
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
            printf("���Ϸ��ı��ʽ!\n");
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
