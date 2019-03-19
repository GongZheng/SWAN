#include <stdint.h>
#include <stdio.h>
#include <wbswan/wbswan.h>
#include <time.h>

int wbswan128_test(int weak_or_strong, enum swan_cipher_config_t cfg);

int main()
{
    int weak_or_strong = 0;
   
    wbswan128_test(weak_or_strong, swan_cfg_B128_K128);
    wbswan128_test(!weak_or_strong, swan_cfg_B128_K128);

    wbswan128_test(weak_or_strong, swan_cfg_B128_K256);
    wbswan128_test(!weak_or_strong, swan_cfg_B128_K256);



    return 0;
}