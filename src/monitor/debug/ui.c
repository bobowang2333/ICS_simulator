#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <regex.h>
#include <ctype.h>
#include <elf.h>
void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int is_number(char *s)
{
    for(;*s ;s++ )
        if((*s < '0') || (*s > '9'))
                return 0;
    return 1;
}
static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}
/* BOBO Added */
static int cmd_si(char *args)
{
   if(!args)
        cpu_exec(1);
   else if(is_number(args))
        cpu_exec(atoi(args));
   else
        printf("Invalid Number\n");
   return 0;
}

static int cmd_info(char *args)
{
    if(args)
    {
        if(*args == 'r')
        {
            int i;
            for(i = 0; i < 4; i++)
                printf("%s\t0x%x\t\t%d\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
            for(i = 4; i < 8; i++)
                printf("%s\t0x%x\t\t0x%x\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
            printf("eip\t0x%x\n", cpu.eip);
            printf("CF:%d   PF:%d   AF:%d   ZF:%d   SF:%d   TF:%d   IF:%d   DF:%d   OF:%d\n", cpu.eflags.CF, cpu.eflags.PF, cpu.eflags.AF, cpu.eflags.ZF, cpu.eflags.SF, cpu.eflags.TF, cpu.eflags.IF, cpu.eflags.DF, cpu.eflags.OF);
        }
    }
    else
        printf("Error: No Argument!\n");
    return 0;
}

static int cmd_x(char *args)
{
    /*TODO: calculate the expression*/
    char *nstr = strtok(args, " ");
    char *expr = nstr + strlen(nstr) + 1; /* pointer to the next expression */
    int i;
    int n = atoi(nstr); /* the number to show (address)*/
    cpu.current_sreg = 3;
    swaddr_t addr = cal_str(expr) & 0xfffffffc;
    // do some simplicity
    /*
    long int temp;
    temp = strtol(expr, NULL, 16);
    swaddr_t addr = temp & 0xffffffff;
    */
    printf("0x%x:",addr);
    for(i = 0; i < n;i++)
        printf("\t0x%08x",swaddr_read(addr + i*4, 4));
    putchar('\n');
    return 0;
}

static int cmd_p(char *args)
{
    printf("%d\n",cal_str(args));
    return 0;
}

static int cmd_w(char *args)
{
    if(0)
        printf("Can not watch constant\n");
    else
    {
        uint32_t old_value = cal_str(args);
        add_WP(args,old_value);
    }
    return 0;
}

static int cmd_d(char *args)
{
    int succ = 0;
    int n = atoi(args);
    if(is_number(args))
    {
        succ = del_WP(n);
    }else{
        printf("Invalid argument!");
    }
    if(!succ)
        printf("Unable to delete watch point number %d\n",n);
    return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
    /* BOBO Add */
    {"si", "Execute the next N command", cmd_si},
    {"info", "'r' means printing register info, 'w' means printing watch point info ", cmd_info},
	{"x","Print N 4bits from address starting with the value of EXPRESSION", cmd_x},
    {"p", "Calculate and print the value of Expression", cmd_p}, 
    {"w", "Add a watchpoint to watch the value of the Expression", cmd_w},
    {"d", "Delete the watchpoint NUM", cmd_d},
 /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
