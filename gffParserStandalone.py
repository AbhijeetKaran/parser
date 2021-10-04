import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-f","--file",help="Input GFF/GTF filename")
parameters = parser.parse_args()
print(parameters)