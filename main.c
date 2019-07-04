#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>   // open 
#include <unistd.h>  // close
#include <string.h>
#include <errno.h>

int run_cmd(char *cmd, ...)
{
  va_list ap;
  char buf[100];

  va_start(ap, cmd);
  vsnprintf(buf, 100, cmd, ap);
  va_end(ap);

  printf("%s\n", buf);
  return system(buf); 
}

int set_if_up(char *dev)
{
  // enable tun dev
  return run_cmd("ip link set dev %s up", dev);
}

int set_route(char *dev, char *cidr)
{
  return run_cmd("ip route add dev %s %s", dev, cidr);
}


int tun_alloc(char *dev)
{
  struct ifreq ifr;
  int fd, err;
  
  if ( (fd = open("/dev/net/tun", O_RDWR)) < 0 ){
    printf("Cannnot open TUN/TAP dev");
    exit(1);
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN;
  if( *dev ) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }


  if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
    printf("ERR: Could not ioctl tun: %s\n", strerror(errno));
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);
  return fd;
}

int main(int argc, char **argv)
{
  int tun_fd = -1;
  char buf[100];
  char *dev = calloc(10, 1);
  tun_alloc(dev);

  if (set_if_up(dev) != 0)
  {
    printf("ERROR when setting up if\n");
  }

  if( set_route(dev, "10.0.0.0/24") != 0 )
  {
    printf("ERROR when setting route for if\n");
  }

  read(tun_fd, buf, 100);
  free(dev);
  return 0;
}
