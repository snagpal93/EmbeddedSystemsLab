#!/usr/bin/python3.4

import os, sys
from PIL import Image


def generateInstance(template_path, instance_params, output_path):
    with open(template_path, 'r') as stream:
        template = stream.read()

    with open(output_path, 'w') as stream:
        stream.write(template.format(params=instance_params))


def main():
    if len(sys.argv)<2:
        print("Example: generateMake \"path_to_img\" DMA[0|1] \"path_to_makefile\" [\"path_to_template\"]")
        return 1;
    if sys.argv[1]=="-h" or sys.argv[1]=="-help":
        print("Example: generateMake \"path_to_img\" DMA[0|1] \"path_to_makefile\" [\"path_to_template\"]")
        return 1;
    if len(sys.argv)<4:
        print("Example: generateMake \"path_to_img\" DMA[0|1] \"path_to_makefile\" [\"path_to_template\"]")
        return 1;
    img_path = sys.argv[1]
    dma = sys.argv[2]
    output_path = sys.argv[3]
    if len(sys.argv) == 5:
        template_path = sys.argv[4]
    else:
        template_path = output_path+".template"
    generateMakefile(img_path, dma, output_path, template_path)


def generateMakefile(img_path, dma, output_path, template_path):
    if not os.path.isabs(img_path):
        img_path=os.path.abspath(img_path)
    im = Image.open(img_path)
    width, height = im.size
    params = {'Width': width,
              'Height': height,
              'DMA': dma,
              'ImgPath': img_path}

    generateInstance(template_path, params, output_path)

if __name__ == "__main__":
    main()
