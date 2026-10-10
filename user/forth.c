#include "ulib.h"

#define LINE_MAX 128
#define STACK_MAX 256
#define VAR_MAX 64
#define NAME_MAX 32
#define POOL_MAX 4096

static uint32_t stack[STACK_MAX];
static int sp;
static uint32_t cells[VAR_MAX];
static char var_names[VAR_MAX][NAME_MAX];
static int var_count;
static char pool[POOL_MAX];
static uint32_t pool_used;
static int running = 1;

static char upper(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return (char)(c - 32);
    }

    return c;
}

static int word_is(const char *w, const char *name)
{
    while (*w != '\0' && *name != '\0' && upper(*w) == *name)
    {
        w++;
        name++;
    }

    return upper(*w) == *name;
}

static int push(uint32_t v)
{
    if (sp >= STACK_MAX)
    {
        print("stack overflow\n");
        return 0;
    }

    stack[sp++] = v;
    return 1;
}

static int pop(uint32_t *v)
{
    if (sp <= 0)
    {
        print("stack underflow\n");
        return 0;
    }

    *v = stack[--sp];
    return 1;
}

static int find_var(const char *name)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(var_names[i], name) == 0)
        {
            return i;
        }
    }

    return -1;
}

static int parse_number(const char *s, uint32_t *out)
{
    int negative = 0;
    uint32_t value = 0;

    if (*s == '-')
    {
        negative = 1;
        s++;
    }

    if (*s == '\0')
    {
        return 0;
    }

    while (*s != '\0')
    {
        if (*s < '0' || *s > '9')
        {
            return 0;
        }

        value = value * 10u + (uint32_t)(*s - '0');
        s++;
    }

    *out = negative ? 0u - value : value;
    return 1;
}

static void declare_variable(const char *name)
{
    if (var_count >= VAR_MAX)
    {
        print("too many variables\n");
        return;
    }

    int i = 0;

    while (name[i] != '\0' && i < NAME_MAX - 1)
    {
        var_names[var_count][i] = name[i];
        i++;
    }

    var_names[var_count][i] = '\0';
    cells[var_count] = 0;
    var_count++;
}

static void print_top(void)
{
    uint32_t v;

    if (pop(&v))
    {
        print_int((int)v);
        put_char('\n');
    }
}

static void type_string(void)
{
    uint32_t len;
    uint32_t idx;

    if (!pop(&len) || !pop(&idx))
    {
        return;
    }

    if (idx > pool_used || len > pool_used - idx)
    {
        print("bad string\n");
        return;
    }

    sys_write(pool + idx, len);
}

static void fetch(void)
{
    uint32_t addr;

    if (!pop(&addr))
    {
        return;
    }

    if (addr >= (uint32_t)var_count)
    {
        print("bad address\n");
        return;
    }

    push(cells[addr]);
}

static void store(void)
{
    uint32_t addr;
    uint32_t value;

    if (!pop(&addr) || !pop(&value))
    {
        return;
    }

    if (addr >= (uint32_t)var_count)
    {
        print("bad address\n");
        return;
    }

    cells[addr] = value;
}

static void binary(char op)
{
    uint32_t a;
    uint32_t b;

    if (!pop(&b) || !pop(&a))
    {
        return;
    }

    if (op == '+')
    {
        push(a + b);
    }
    else if (op == '-')
    {
        push(a - b);
    }
    else if (op == '*')
    {
        push(a * b);
    }
    else
    {
        if (b == 0)
        {
            print("division by zero\n");
            return;
        }

        push((uint32_t)((int)a / (int)b));
    }
}

static void read_string(char **pp)
{
    char *p = *pp;
    uint32_t start = pool_used;

    p++;

    while (*p != '\0' && *p != '"')
    {
        if (pool_used >= POOL_MAX)
        {
            print("string pool full\n");
            break;
        }

        pool[pool_used++] = *p;
        p++;
    }

    if (*p == '"')
    {
        p++;
    }

    push(start);
    push(pool_used - start);
    *pp = p;
}

static void execute(const char *w, int *declaring)
{
    uint32_t n;
    int v;

    if (*declaring)
    {
        declare_variable(w);
        *declaring = 0;
    }
    else if (word_is(w, "TYPE"))
    {
        type_string();
    }
    else if (word_is(w, "VARIABLE"))
    {
        *declaring = 1;
    }
    else if (word_is(w, "BYE"))
    {
        running = 0;
    }
    else if (w[0] == '.' && w[1] == '\0')
    {
        print_top();
    }
    else if (w[0] == '@' && w[1] == '\0')
    {
        fetch();
    }
    else if (w[0] == '!' && w[1] == '\0')
    {
        store();
    }
    else if (w[0] == '?' && w[1] == '\0')
    {
        fetch();
        print_top();
    }
    else if (w[1] == '\0' && (w[0] == '+' || w[0] == '-' || w[0] == '*' || w[0] == '/'))
    {
        binary(w[0]);
    }
    else if ((v = find_var(w)) >= 0)
    {
        push((uint32_t)v);
    }
    else if (parse_number(w, &n))
    {
        push(n);
    }
    else
    {
        print("unknown word: ");
        print(w);
        put_char('\n');
    }
}

static void run_line(char *line)
{
    char *p = line;
    char word[NAME_MAX];
    int declaring = 0;

    while (running)
    {
        while (*p == ' ' || *p == '\t')
        {
            p++;
        }

        if (*p == '\0')
        {
            break;
        }

        if (*p == '"')
        {
            read_string(&p);
            continue;
        }

        int len = 0;

        while (*p != '\0' && *p != ' ' && *p != '\t')
        {
            if (len < NAME_MAX - 1)
            {
                word[len++] = *p;
            }

            p++;
        }

        word[len] = '\0';
        execute(word, &declaring);
    }
}

int main(void)
{
    char line[LINE_MAX];

    print("FORTH INTERPRETER\n");

    while (running)
    {
        print("> ");
        read_line(line, LINE_MAX);
        run_line(line);
    }

    print("== Variable Checkout ==\n");

    for (int i = 0; i < var_count; i++)
    {
        print(var_names[i]);
        print(" : ");
        print_int((int)cells[i]);
        put_char('\n');
    }

    return 0;
}