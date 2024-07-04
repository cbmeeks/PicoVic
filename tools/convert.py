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


def rgb_of_pixel(img_path, x, y):
    im = Image.open(img_path).convert('RGB')
    r, g, b = im.getpixel((x, y))
    a = (r, g, b)
    return a


def parse_frame(img_path):
    im = Image.open(img_path).convert('RGB')
    frame = im.crop((0, 0, 16, 16))

    line = "0b"

    for y in range(frame.height):
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
        line = "0b"

    return frame


# parse_frame(filename)
convert_pic(pic_filename)
