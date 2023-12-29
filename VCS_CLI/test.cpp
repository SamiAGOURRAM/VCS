#include "vcs.h"
#include "cli.h"


int main(){


VCS vcs = VCS();

CLI cli = CLI(vcs);

cli.start();


}