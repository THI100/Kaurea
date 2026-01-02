F1: ΩFUSE(a, b, c, d):
    a += b ^ 0x6A09E667
    d ^= ROTL(a, 9)
    c += d
    b ^= ROTR(c, 11)
    return (a, b, c, d)

F2: ΦMIX(x, y, z):
    t  = x + y + 0x9E3779B9
    t ^= ROTL(z, 11)
    t ^= (x & y) ^ (~x & z)
    return ROTR(t, 7)

F3: πPERM(x, r):
    k = (r * 0x243F6A88) & 31
    return ROTL(x, k) ^ ROTR(x, 32 - k)

F4: ARXL(x, c):
    x += c
    x ^= ROTR(x, 11)
    return ROTL(x, 3)

F5: SRPERM(x):
    x = (x << 16) | (x >> 16)
    return ROTL(x, 9)

F6: MINIMIX(a, b):
    a += b
    b ^= ROTL(a, 7)
    return (a, b)