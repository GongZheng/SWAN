import sys
import math

class InputError(Exception):
    def __str__(self):
        return repr('Invalid size of sbox!')


def components(x):
    return [i for i in range(x + 1) if x & i == i]


def sbox2anf(sbox):
    num = len(sbox)
    dim = int(math.log(num, 2))
    if 2 ** dim != num:
        raise InputError

    ls = []
    for d in range(dim):
        dc = []
        for n in range(num):
            sign = 0
            for i in components(n):
                sign = sign ^ ((sbox[i] >> d) & 0x1)
            dc.append(sign)
        ls.append(dc)
    return ls


def print_anf(sbox):
    anf = sbox2anf(sbox)
    for i in range(len(anf)):
        #print anf[i]
        #continue
        sys.stdout.write('y%u = ' % i)

        first_time = True
        ls = anf[i]
        for j in range(len(ls)):
            if ls[j] == 1:
                if first_time:
                    first_time = False
                else:
                    sys.stdout.write(' + ')

                if j == 0:
                    sys.stdout.write('1')
                else:
                    mask = 0x1
                    while mask <= j:
                        if mask & j > 0 :
                            sys.stdout.write('x%u' % int(math.log(mask, 2)))
                        mask = mask << 1
        if first_time: # if still be True
            sys.stdout.write('0')
        print()


if __name__ == '__main__':
    #zhangwt_sbox1 = (0x03, 0x0D, 0x09, 0x00, 0x06, 0x0C, 0x04, 0x0B, 0x0F, 0x02, 0x0E, 0x07, 0x05, 0x01, 0x0A, 0x08)
    #zhangwt_sbox2 = (0x09, 0x00, 0x0A, 0x07, 0x03, 0x0F, 0x0C, 0x02, 0x05, 0x0B, 0x06, 0x0D, 0x0E, 0x04, 0x01, 0x08)

    #ideaSbox = (0x0C, 0x02, 0x0A, 0x04, 0x01, 0x0F, 0x07, 0x09, 0x06, 0x08, 0x0E, 0x00, 0x0B, 0x05, 0x03, 0x0D)
    #ideaSbox = (0x00, 0x0A, 0x05, 0x0F, 0x08, 0x03, 0x0D, 0x06, 0x01, 0x0B, 0x04, 0x0E, 0x09, 0x02, 0x0C, 0x07)

    KLEIN_sbox = (0x7, 0x4, 0xA, 0x9, 0x1, 0xF, 0xB, 0x0, 0xC, 0x3, 0x2, 0x6, 0x8, 0xE, 0xD, 0x5)

    %Piccolo_sbox = (0xe, 0x4, 0xb, 0x2, 0x3, 0x8, 0x0, 0x9, 0x1, 0xa, 0x7, 0xf, 0x6, 0xc, 0x5, 0xd)

    SWAN_sbox = (0x01, 0x02, 0x0C, 0x05, 0x07, 0x08, 0x0A, 0x0F, 0x04, 0x0D, 0x0B, 0x0E, 0x09, 0x06, 0x00, 0x03)
    
    print_anf(test_sbox)

