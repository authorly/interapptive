#include "DeviceType.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>

bool DeviceType::isIphone4S()
{
  size_t size;
  sysctlbyname("hw.machine", NULL, &size, NULL, 0);
  char *machine = (char*)malloc(size);
  sysctlbyname("hw.machine", machine, &size, NULL, 0);
  NSString *platform = [NSString stringWithUTF8String:machine];
  free(machine);
  
  return [platform isEqualToString:@"iPhone4,1"];
}

bool DeviceType::DeviceType::isIpad1()
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithUTF8String:machine];
    free(machine);
    
    return [platform isEqualToString:@"iPad1,1"] || [platform isEqualToString:@"iPad1,2"];
}
