#!/usr/bin/env python3
"""Build the Mizar S UI icons omitted from the generic Makerbase asset pack.

Source artwork: Geeetech3D/Mizar-S at commit
d3656d6db4bb89f552f9afd3aec013db02a1cff9 (GPL-3.0).
"""

from io import BytesIO
from pathlib import Path
import sys
from urllib.request import Request, urlopen

from PIL import Image

SOURCE_REV = "d3656d6db4bb89f552f9afd3aec013db02a1cff9"
SOURCE_ROOT = f"https://raw.githubusercontent.com/Geeetech3D/Mizar-S/{SOURCE_REV}/Resource/icon"
ICONS = (
    "bmp_clear",
    "bmp_confirm",
    "bmp_confirm_dis",
    "bmp_led_color",
    "bmp_led_off",
    "bmp_led_white",
    "bmp_leveling_auto",
    "bmp_leveling_manual",
    "bmp_machine_para",
    "bmp_machine_para_dis",
    "bmp_motor_off",
    "bmp_mov",
    "bmp_zero",
)


def convert_jpeg_to_lvgl_bin(source: bytes) -> bytes:
    with Image.open(BytesIO(source)) as original:
        image = original.convert("RGB")
    width, height = image.size
    if not width or not height or width > 2047 or height > 2047:
        raise ValueError(f"Unsupported icon dimensions: {width}x{height}")

    # LVGL's true-color image header stores color format, width, and height
    # in a little-endian 32-bit word. Pixel data is RGB565, low byte first.
    header = 4 | (width << 10) | (height << 21)
    pixels = bytearray()
    for red, green, blue in image.getdata():
        rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3)
        pixels.extend((rgb565 & 0xFF, rgb565 >> 8))

    output = header.to_bytes(4, "little") + pixels
    if len(output) != 4 + width * height * 2:
        raise ValueError("Invalid LVGL true-color image size")
    return output


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("Usage: prepare_mizar_s_tft_assets.py <assets-directory>")
    assets_dir = Path(sys.argv[1])
    assets_dir.mkdir(parents=True, exist_ok=True)

    for name in ICONS:
        url = f"{SOURCE_ROOT}/{name}.jpg"
        request = Request(url, headers={"User-Agent": "Mizar-S-Marlin-build"})
        with urlopen(request, timeout=30) as response:
            source = response.read()
        output = convert_jpeg_to_lvgl_bin(source)
        (assets_dir / f"{name}.bin").write_bytes(output)
        print(f"Prepared {name}.bin ({len(output)} bytes)")


if __name__ == "__main__":
    main()
