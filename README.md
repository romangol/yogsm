# YogSM 性能测试表

---

### SM2 Performance

| 项目                  | CPU                   | 线程数    | Sign/s                   | Verify/s               |
| --------              | :-----:               | :----:    |  :----:                 |  :----:                |
| SM2 x64               | i7 8809 @ 3.1 GHz     | 1         | 34722                   |7016                    |
| SM2 x64               | i5 7200U @ 2.5GHz     | 1         | 28000                   |5600                    |
| SM2 x64               | i3 6100 @ 2.3 GHz     | 1         | 18315                   |3743                    |
| SM2 x64               | 树莓派3B+              | 1         | 2913                   |828                     |
| SM2 x64               | 一加5T                 | 1         | 6289                   |2174                     |

### SM3 Performance

| 项目                  | CPU                   | 线程数    | 速度（MBps）                   |
| --------              | :-----:               | :----:    |  :----:                 |
| SM3 x64               | i7 8809 @ 3.1 GHz     | 1         | 286                   |
| SM3 x64               | i5 7200U @ 2.5GHz     | 1         | 234                   |
| SM3 x64               | i3 6100 @ 2.3 GHz     | 1         | 162                   |

### SM4 Performance

| 项目                  | CPU                   | 线程数    | 速度（MBps）                   |
| --------              | :-----:               | :----:    |  :----:                 |
| SM4 x64               | i7 8809 @ 3.1 GHz     | 1         | 273                   |
| SM4 x64               | i5 7200U @ 2.5GHz     | 1         | 219                   |
| SM4 x64               | i3 6100 @ 2.3 GHz     | 1         | 162                   |


### SM4 Performance

| 项目                  | CPU               | 线程数    | 速度（MBps）      |
| --------              | :-----:           | :----:    |  :----:           |
| SM4Fast OpenMP        | i7 8809 @ 3.1 GHz | 16        | 640               |
| SM4Fast OpenMP        | i7 6700 @ 3.4 GHz | 8         | 446               |
| SM4Fast OpenMP        | i3 6100 @ 2.3 GHz | 4         | 213               |
| SM4Fast               | 一加5T            | 1         | 105               |
| SM4Fast               | 三星S6            | 1         | 80                |
| SM4Fast               | Moto XT1085       | 1         | 64                |
| SM4Fast               | 树莓派3B+         | 1         | 40                |
| SM4Fast               | 树莓派2           | 1         | 20                |


### SM4 Performance

| Implementation        | CPU               | Threads   | Speed（MBps）    |
| --------              | :-----:           | :----:    |  :----:          |
| SM4 Fast              | Oneplus 5T (2.45 GHz ARM64 Qualcomm Snapdragon 835) | 1         | 105              |
| SM4 Fast              | Samsung S6 (2.1 GHz ARM64 Exynos 7420)           | 1         | 80               |
| SM4 Fast              | Moto XT1085 (2.5 GHz ARM Qualcomm Snapdragon 801)       | 1         | 64               |
| SM4 Fast              | Raspberry Pi 3B+ (1.4GHz ARM64 Broadcom Cortex-A53)         | 1         | 40               |
| SM4 Fast              | Raspberry Pi 2  (900MHz ARM Cortex-A7)     | 1         | 20               |


| Implementation        | CPU               | Threads   | Speed（MBps）    |
| --------              | :-----:           | :----:    |  :----:          |
| Crypto++ SM4/CTR      | Skylake Core-i5 2.7 GHz     | 1         | 88               |
| sm4ni (SM4 with AES-NI)*       | Unknown Intel CPU with AES-NI     | 4         | 160.6  |

[*] https://github.com/mjosaarinen/sm4ni


```
i7 6700 @ 3.4 GHz
C:\Users\roman\Desktop\openssl-1.1.1-pre9-win64>openssl speed ecdsap256
Doing 256 bits sign ecdsa's for 10s: 463886 256 bits ECDSA signs in 10.00s
Doing 256 bits verify ecdsa's for 10s: 162890 256 bits ECDSA verify in 10.00s
OpenSSL 1.1.1-pre9 (beta) 21 Aug 2018
built on: Tue Aug 21 13:27:43 2018 UTC
options:bn(64,64) rc4(16x,int) des(long) aes(partial) idea(int) blowfish(ptr)
compiler: cl /Zi /Fdossl_static.pdb /Gs0 /GF /Gy /MT /W3 /wd4090 /nologo /O2 -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_CPUID_OBJ -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DKECCAK1600_ASM -DRC4_ASM -DMD5_ASM -DAES_ASM -DVPAES_ASM -DBSAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DX25519_ASM -DPADLOCK_ASM -DPOLY1305_ASM
                              sign    verify    sign/s verify/s
 256 bits ecdsa (nistp256)   0.0000s   0.0001s  46388.6  16289.0
 

i3 6100 @ 2.3 GHz 
Doing 256 bits sign ecdsa's for 10s: 274232 256 bits ECDSA signs in 9.98s
Doing 256 bits verify ecdsa's for 10s: 96100 256 bits ECDSA verify in 10.00s
OpenSSL 1.1.1-pre9 (beta) 21 Aug 2018
built on: Tue Aug 21 13:27:43 2018 UTC
options:bn(64,64) rc4(16x,int) des(long) aes(partial) idea(int) blowfish(ptr)
compiler: cl /Zi /Fdossl_static.pdb /Gs0 /GF /Gy /MT /W3 /wd4090 /nologo /O2 -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_CPUID_OBJ -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DKECCAK1600_ASM -DRC4_ASM -DMD5_ASM -DAES_ASM -DVPAES_ASM -DBSAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DX25519_ASM -DPADLOCK_ASM -DPOLY1305_ASM
                              sign    verify    sign/s verify/s
 256 bits ecdsa (nistp256)   0.0000s   0.0001s  27466.1   9610.0
```
