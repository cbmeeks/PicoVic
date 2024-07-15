from PIL import Image


def grab_palette(filename):
    # go ahead and add black so that it is first in the set
    colors = ['0x0000']

    img = Image.open(filename).convert('RGB')
    for y in range(img.height):
        for x in range(img.width):
            r, g, b = img.getpixel((x, y))
            r = round((r / 255) * 15) if r > 0 else 0
            g = round((g / 255) * 15) if g > 0 else 0
            b = round((b / 255) * 15) if b > 0 else 0

            if r > 15: r = 15
            if g > 15: g = 15
            if b > 15: b = 15

            colors.append(f'0x0{r:x}{g:x}{b:x}')

    out = "{"
    for c in list(dict.fromkeys(colors)):
        out += f'{c},'

    out += "}"
    print(out)


# grab_palette("../res/sprites/Zoomer_16x16.png")
grab_palette("../res/sprites/Samus_No_Suit_32x48.png")
