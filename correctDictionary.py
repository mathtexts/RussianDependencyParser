__author__ = 'steve'
import sys

if len(sys.argv) < 3:
    print "error: the input and output files are missing"
    exit(0)

in_block = False
remember_block = False
block = []
prev_block = []
out_file = open(sys.argv[2], 'w')
for line in open(sys.argv[1]):
    # print line
    line_parts = line.rstrip().split()
    if len(line_parts) == 1:
        in_block = True
    elif len(line_parts) == 0:
        in_block = False
        if remember_block:
            remember_block = False
            prev_block = block
            block = []
        elif block:
            # merge infinitive with following forms to enable correct word normalization
            if (block[-1].find('\tINFN,') > -1):
                prev_block = prev_block[1:]
                block[-1] = block[-1].rstrip()
            print >>out_file, ''.join(block)
            if prev_block:
                print >>out_file, ''.join(prev_block)
                prev_block = []
            block = []
    elif len(line_parts) > 1 and line_parts[1].startswith('VERB,'):
        remember_block = True
    if in_block:
        block.append(line)

out_file.close()
