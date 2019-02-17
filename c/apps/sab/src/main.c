#include <libgen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arraysize.h"
#include "commands.h"

static const char *progname;

struct builtin_cmd {
        const char *name;
        int (*exec)(int, char **);
};

static struct builtin_cmd cmds[] = {
        { "add", cmd_add },
        { "change", cmd_change },
        { "delete", cmd_delete },
        { "list", cmd_list },
};

static void usage(void) {
        static const char *usage_msg =
                "usage: %s COMMAND [ARGS]\n\n"
                "The commands are:\n"
                "   add       Add a new contact\n"
                "   change    Change a contact\n"
                "   delete    Delete the specified contact\n"
                "   list      List all contacts\n"
                "";
        fprintf(stderr, usage_msg, progname);
        exit(EXIT_FAILURE);
}

static struct builtin_cmd *get_builtin(const char *name) {
        size_t i;
        for (i = 0; i < ARRAY_SIZE(cmds); i++) {
                struct builtin_cmd *cmd = &cmds[i];
                if (!strcmp(name, cmd->name)) {
                        printf("Command name: %s\n", cmds[i].name);
                        return cmd;
                }
        }
        return NULL;
}

int main(int argc, char **argv) {
        struct builtin_cmd *cmd;

        progname = basename(argv[0]);

        if (argc < 2)
                usage();

        cmd = get_builtin(argv[1]);
        if (!cmd)
                usage();

        cmd->exec(argc - 1, argv + 1);

        return 0;
}