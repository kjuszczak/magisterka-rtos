#ifndef IP_STACK_H
#define IP_STACK_H

typedef void (* ApplicationIPNetwork) (void);

void initIpStack(ApplicationIPNetwork app);

#endif // IP_STACK_H