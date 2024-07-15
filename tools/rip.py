from PIL import Image

# change these once you identify which pixel is mapped to each color
black = (0, 0, 0)
cyan = (156, 252, 240)
green = (76, 220, 72)
orange = (252, 152, 56)


def rip(filename):
    img = Image.open(filename).convert('RGB')
    count = int(img.width / 16)
    colors = [black]

    for c in range(count):
        for y in range(img.height):
            for x in range((c * 16) + 16):
                r, g, b = img.getpixel((x, y))
                colors.append((r, g, b))

    # print initial color list
    # if you get more then four colors (counting black) then something is wrong.
    # this whole program is mainly for NES sprite images.  :-)
    print(list(dict.fromkeys(colors)))

    # make sure the colors above now match the color variables above
    # these will need to change!
    for c in range(count):
        out = "0b"
        for y in range(img.height):
            for x in range((c * 16), (c * 16) + 16):
                r, g, b = img.getpixel((x, y))
                p = (r, g, b)

                if (r, g, b) == black:
                    out += "00"
                elif p == cyan:
                    out += "01"
                elif p == orange:
                    out += "10"
                else:
                    out += "11"
            out += ", \n0b"
        print(out)


rip("../res/sprites/Skree-16x24.png")
