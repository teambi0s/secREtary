#include <sys/ptrace.h>
#include <stdio.h>
int main()
{
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	return 0;
}
