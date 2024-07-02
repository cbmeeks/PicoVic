## RAM Allocated

#### Assuming 320x240 Pixels and 40x30 Characters

#### Does not include internal, small buffers the engine uses

| Name             |  Bytes |    KiB |   Type   |
|:-----------------|-------:|-------:|:--------:|
| PETSCII          |   2048 |      2 | uint8_t  |
| Palette Buffer   |    256 |   0.25 | uint8_t  |
| Text Mode Buffer |   1200 |   1.17 | uint8_t  |
| Text FG CLUT     |   1200 |   1.17 | uint8_t  |
| Text BG CLUT     |   1200 |   1.17 | uint8_t  |
| *Frame Buffer    | 153600 |    150 | uint16_t |
| Total            | 159504 | 155.77 |          |

#### * Could possibly deallocate if not needed

