# YogSM 性能测试表

标签（空格分隔）： crypto

---

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

