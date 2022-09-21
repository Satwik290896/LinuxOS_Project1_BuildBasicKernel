void main(void) {
    __asm__("mov $0xb800, %ax");
    __asm__("mov %ax, %ds");
    
    char *base = 0;

    *(base+1988) = 0x68;
    *(base+1989) = 0xf2;
    *(base+1990) = 0x65;
    *(base+1991) = 0xf2;
    *(base+1992) = 0x6c;
    *(base+1993) = 0xf2;
    *(base+1994) = 0x6c;
    *(base+1995) = 0xf2;
    *(base+1996) = 0x6f;
    *(base+1997) = 0xf2;
    *(base+1998) = 0x2c;
    *(base+1999) = 0xf2;
    *(base+2000) = 0x20;
    *(base+2001) = 0xf2;
    *(base+2002) = 0x77;
    *(base+2003) = 0xf2;
    *(base+2004) = 0x6f;
    *(base+2005) = 0xf2;
    *(base+2006) = 0x72;
    *(base+2007) = 0xf2;
    *(base+2008) = 0x6c;
    *(base+2009) = 0xf2;
    *(base+2010) = 0x64;
    *(base+2011) = 0xf2;
    
    while(1){
    
    }
}
