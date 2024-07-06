from PIL import Image

filename = "../res/sprites/Zoomer_16x16.png"
pic_filename = "../res/toucan.png"


def convert_pic(filename):
    with open('../res/toucan.bin', 'w+b') as bf:
        img = Image.open(filename).convert('RGB')
        for y in range(img.height):
            for x in range(img.width):
                r, g, b = img.getpixel((x, y))
                bf.write(r.to_bytes(1, 'little'))


def convert_sprites(path, filename):
    with open(path + "/sprites_c.txt", "w") as f:
        img = Image.open(path + "/" + filename).convert('RGB')

        for y in range(img.height):
            line = "0b"
            for x in range(16):
                r, g, b = img.getpixel((x, y))
                p = (r, g, b)

                if p == (0, 0, 0):
                    line = line + "00"
                elif p == (200, 76, 12):
                    line = line + "01"
                elif p == (252, 188, 176):
                    line = line + "10"
                elif p == (228, 0, 88):
                    line = line + "11"
            f.write(line + ",\n")



def convert_tiles(path, filename):
    with open(path + "/tiles_c.txt", "w") as f:

        img = Image.open(filename).convert('RGB')

        tiles_wide = int(img.width / 16)
        tiles_high = int(img.height / 16)

        f.write("uint32_t tiles[" + str(tiles_wide * tiles_high) + "][16] = {\n")
        for tile_y in range(tiles_high):
            for tile_x in range(tiles_wide):

                f.write("{\n")
                for y in range(16):
                    line = "0b"
                    for x in range(16):
                        r, g, b = img.getpixel((tile_x * 16 + x, tile_y * 16 + y))
                        p = (r, g, b)

                        if p == (0, 0, 0):
                            line = line + "00"
                        elif p == (0, 150, 153):
                            line = line + "01"
                        elif p == (64, 25, 255):
                            line = line + "10"
                        elif p == (125, 218, 255):
                            line = line + "11"

                    line = line + ",\n"
                    f.write(line)
                f.write("\n},\n")
        f.write("};\n")


def rgb_of_pixel(img_path, x, y):
    im = Image.open(img_path).convert('RGB')
    r, g, b = im.getpixel((x, y))
    a = (r, g, b)
    return a


def parse_frame(img_path):
    im = Image.open(img_path).convert('RGB')
    frame = im.crop((0, 0, 16, 16))

    for y in range(frame.height):
        line = "0b"
        for x in range(frame.width):
            r, g, b = frame.getpixel((x, y))
            a = (r, g, b)

            if a == (0, 0, 0):
                line = line + str("00")
            if a == (76, 220, 72):
                line = line + str("01")
            if a == (252, 152, 56):
                line = line + str("10")
            if a == (156, 252, 240):
                line = line + str("11")

        print(line)

    return frame


convert_sprites("../res/sprites/", "Samus_No_Suit_32x48.png")

# convert_pic(pic_filename)

# convert_tiles("../res/tiles", "../res/tiles/brinstar_16.png")
