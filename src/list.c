#include "engine.h"

int main(int argc, char** argv) {
    struct aucont_list_args args;
    return aucont_list(&args) ? 1 : 0;
}
